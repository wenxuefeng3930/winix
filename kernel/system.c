/**
 * System task for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include "winix.h"

struct message m;
int who_proc_nr;
struct proc *who;

ucontext_t recv_ctx;
struct syscall_ctx syscall_ctx;

void resume_syscall(struct proc *to){
	if(syscall_ctx.who->pid == to->pid && to->flags & RECEIVING && syscall_ctx.interruptted){
		// kprintf("resume syscall %d\n",who->pid);
		winix_send(to->pid,&syscall_ctx.m);
	}
	syscall_ctx.interruptted = 0;
	// setcontext(&syscall_ctx);
}

void intr_syscall(){
	struct syscall_ctx *ctx = &syscall_ctx;
	//if the system is executing a system call, save the system call context
	//and interrupt the System as if it has finished executing the current syscall
	if(!get_proc(SYSTEM_TASK)->flags){
		// kprintf("interrupt syscall %d\n",who->pid);
		ctx->m = m;
		ctx->m.i1 = EINTR;
		ctx->who = who;
		ctx->interruptted = 1;
	}
	setcontext(&recv_ctx);
}

struct message *curr_mesg(){
	return &m;
}


void print_sysinfo(){
	int free_mem_begin;

	free_mem_begin = next_free_page_index() * PAGE_LEN;
	//Print Memory Map
	kprintf("Text Segment: 0x%08x - 0x%08x\r\n", &TEXT_BEGIN, &TEXT_END);
	kprintf("Data Segment: 0x%08x - 0x%08x\r\n", &DATA_BEGIN, &DATA_END);
	kprintf("BSS Segment:  0x%08x - 0x%08x\r\n", &BSS_BEGIN, &BSS_END);
	kprintf("Unallocated:  0x%08x - 0x%08x\r\n", free_mem_begin, FREE_MEM_END);
	kprintf("%d kWords Free\r\n", ((unsigned long)(FREE_MEM_END - free_mem_begin)) / 1024);
}
/**
 * Entry point for system task.
 **/
void system_main() {

	print_sysinfo();
	getcontext(&recv_ctx);

	//Receive message, do work, repeat.
	while(1) {
		
		//get a messa1ge
		winix_receive(&m);
		who_proc_nr = m.src;
		who = get_proc(who_proc_nr);

		//Do the work
		switch(m.type) {
			case SYSCALL_UPTIME:		m.i1 = do_time(who,&m);			break;
			case SYSCALL_EXIT:			m.i1 = do_exit(who,&m);			break;
			case SYSCALL_FORK:			m.i1 = do_fork(who,&m);			break;
			case SYSCALL_EXECVE:		m.i1 = do_exec(who,&m);			break;
			case SYSCALL_SBRK:			m.i1 = do_sbrk(who,&m);			break;
			case SYSCALL_BRK:			m.i1 = do_brk(who,&m);			break;
			case SYSCALL_ALARM:			m.i1 = do_alarm(who,&m);		break;
			case SYSCALL_SIGNAL:		m.i1 = do_sigaction(who,&m);	break;
			case SYSCALL_SIGRET:		m.i1 = do_sigreturn(who,&m);	break;
			case SYSCALL_WAIT:			m.i1 = do_wait(who,&m);			break;
			case SYSCALL_KILL: 			m.i1 = do_kill(who,&m);			break;
			case SYSCALL_GETPID:		m.i1 = do_getpid(who,&m);		break;
			case SYSCALL_GETPPID:		m.i1 = do_getppid(who,&m);		break;
			case SYSCALL_GETC:			m.i1 = do_getc(who,&m);			break;
			case SYSCALL_PS:			m.i1 = do_ps(who,&m);			break;
			case SYSCALL_PRINTF:		m.i1 = do_printf(who,&m);		break;
			default:
				kprintf("\r\n[SYSTEM] Process \"%s (%d)\" performed unknown system call %d\r\n", who->name, who->pid, m.type);
				m.i1 = ENOSYS;
				break;
		}

		if(m.i1 != SUSPEND && m.i1 != DONOTHING){
			winix_send(who_proc_nr,&m);
		}
	}
}

