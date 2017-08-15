#ifndef	_W_MEM_MAP_H_
#define _W_MEM_MAP_H_

#include <kernel/proc.h>
#include <winix/page.h>
#include <winix/gfp.h>

void init_mem_table();

ptr_t* get_free_pages(int length, int flags);
int free_pages(ptr_t* page, int num);
int user_free_pages(struct proc* who, ptr_t* page, int len);
ptr_t* user_get_free_pages(struct proc* who, int length, int flags);

#define free_page(page)         (free_pages((page),1))
#define get_free_page(flags)    (get_free_pages(1,(flags)))
#define user_get_free_page(who,flags)   (user_get_free_pages((who),1,(flags)))
#define user_free_page(who,page)        (user_free_pages((who),(page),1))

int next_free_page_index();

void* dup_vm(struct proc* parent, struct proc* child, struct bit_pattern* ptn);
void print_ptable(struct proc* who);
void print_sysmap();

#define MEM_MAP_LEN	4

#endif
