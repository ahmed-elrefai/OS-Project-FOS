/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include <kern/proc/user_environment.h>
#include "kheap.h"
#include "memory_manager.h"
#include <inc/queue.h>

//extern void inctst();

/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va on the given page_directory
//	If the page table at any destination page in the range is not exist, it should create it
//	If ANY of the destination pages exists, deny the entire process and return -1. Otherwise, cut-paste the number of pages and return 0
//	ALL 12 permission bits of the destination should be TYPICAL to those of the source
//	The given addresses may be not aligned on 4 KB
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	//[PROJECT] [CHUNK OPERATIONS] cut_paste_pages
	// Write your code here, remove the panic and write your code
	panic("cut_paste_pages() is not implemented yet...!!");
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va on the given page_directory
//	Ranges DO NOT overlapped.
//	If ANY of the destination pages exists with READ ONLY permission, deny the entire process and return -1.
//	If the page table at any destination page in the range is not exist, it should create it
//	If ANY of the destination pages doesn't exist, create it with the following permissions then copy.
//	Otherwise, just copy!
//		1. WRITABLE permission
//		2. USER/SUPERVISOR permission must be SAME as the one of the source
//	The given range(s) may be not aligned on 4 KB
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	//[PROJECT] [CHUNK OPERATIONS] copy_paste_chunk
	// Write your code here, remove the //panic and write your code
	panic("copy_paste_chunk() is not implemented yet...!!");
}

//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va on the given page_directory
//	Ranges DO NOT overlapped.
//	It should set the permissions of the second range by the given perms
//	If ANY of the destination pages exists, deny the entire process and return -1. Otherwise, share the required range and return 0
//	If the page table at any destination page in the range is not exist, it should create it
//	The given range(s) may be not aligned on 4 KB
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
	//[PROJECT] [CHUNK OPERATIONS] share_chunk
	// Write your code here, remove the //panic and write your code
	panic("share_chunk() is not implemented yet...!!");
}

//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate the given virtual range [<va>, <va> + <size>) in the given address space  <page_directory> with the given permissions <perms>.
//	If ANY of the destination pages exists, deny the entire process and return -1. Otherwise, allocate the required range and return 0
//	If the page table at any destination page in the range is not exist, it should create it
//	Allocation should be aligned on page boundary. However, the given range may be not aligned.
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	//[PROJECT] [CHUNK OPERATIONS] allocate_chunk
	// Write your code here, remove the //panic and write your code
	panic("allocate_chunk() is not implemented yet...!!");
}

//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva, uint32 *num_tables, uint32 *num_pages)
{
	//[PROJECT] [CHUNK OPERATIONS] calculate_allocated_space
	// Write your code here, remove the panic and write your code
	panic("calculate_allocated_space() is not implemented yet...!!");
}

//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
//This function should calculate the required number of pages for allocating and mapping the given range [start va, start va + size) (either for the pages themselves or for the page tables required for mapping)
//	Pages and/or page tables that are already exist in the range SHOULD NOT be counted.
//	The given range(s) may be not aligned on 4 KB
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{
	//[PROJECT] [CHUNK OPERATIONS] calculate_required_frames
	// Write your code here, remove the panic and write your code
	panic("calculate_required_frames() is not implemented yet...!!");
}

//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
/* DYNAMIC ALLOCATOR SYSTEM CALLS */
//=====================================


void mark_page(uint32 va, struct Env* env, uint32 state) {

	// the easy way with array....
	//cprintf(">marking page at %p\n", (void*)va);

	// creating page table if needed
	uint32* ptr_page_table = NULL;
	int status = get_page_table(env->env_page_directory, va, &ptr_page_table);
	if(status == TABLE_NOT_EXIST) {
		create_page_table(env->env_page_directory, va);
	}

	// marking the page in the given environment
	uint32 page_num = (va - USER_HEAP_START)/PAGE_SIZE;
	env->mark_status[page_num] = state;


// change bit of index 9 in the address entry to 1
//	uint32* ptr_page_table;
//	int status = get_page_table(env->env_page_directory, va, &ptr_page_table);
//	if(status == TABLE_NOT_EXIST) {
//		ptr_page_table = create_page_table(env->env_page_directory, va);
//		map_frame()
//	}
//
//	/*
//	 * masking to set the 9th bit to 1
//	 * va   	-> 00000000 00000000 00101100 11011001
//	 * mask 	-> 00000000 00000000 00000010 00000000
//	 * mask&va 	-> 00000000 00000000 00101110 11011001 -> the 9th bit is set to 1
//	 */
//	ptr_page_table[PTX(va)] |= (1 << 10);
}

void unmark_page(uint32 va, struct Env* env) {
	// change bit of index 9 in the address entry to 0

	uint32* ptr_page_table;
	int status = get_page_table(env->env_page_directory, va, &ptr_page_table);
	if(status == TABLE_NOT_EXIST) {
		ptr_page_table = create_page_table(env->env_page_directory, va);
	}


	/*
	 * masking to set the 9th bit to 0
	 * va   	-> 00000000 00000000 00101110 11011001
	 * mask 	-> 11111111 11111111 11111101 11111111
	 * mask&va 	-> 00000000 00000000 00101100 11011001 -> the 9th bit is set to 0
	 */

	ptr_page_table[PTX(va)] &= (~(1 << 10));

}


void* sys_sbrk(int numOfPages)
{
	/* numOfPages > 0: move the segment break of the current user program to increase the size of its heap
	 * 				by the given number of pages. You should allocate NOTHING,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) As in real OS, allocate pages lazily. While sbrk moves the segment break, pages are not allocated
	 * 		until the user program actually tries to access data in its heap (i.e. will be allocated via the fault handler).
	 * 	2) Allocating additional pages for a process’ heap will fail if, for example, the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sys_sbrk fails, the net effect should
	 * 		be that sys_sbrk returns (void*) -1 and that the segment break and the process heap are unaffected.
	 * 		You might have to undo any operations you have done so far in this case.
	 */

	//TODO: [PROJECT'24.MS2 - #11] [3] USER HEAP - sys_sbrk
	/*====================================*/
	/*Remove this line before start coding*/
	//return (void*)-1 ;
	/*====================================*/
	struct Env* env = get_cpu_proc(); //the current running Environment to adjust its break limit

	/////////
	if(numOfPages == 0) return (void*)env->sbreak; // edge case

	uint32 increasing = numOfPages * PAGE_SIZE; // size to be allocated
	uint32 last_address = env->sbreak + increasing;
	uint32 old_sbrk = env->sbreak;

	if(last_address > env->hlimit)
	{
		return (void*)E_UNSPECIFIED;
	}


	//page address
	uint32 it = env->sbreak;

	// mark the range
	for(int32 i = 0 ; i < numOfPages ; i++, it += PAGE_SIZE) {
		mark_page(it, env, PAGE_MARKED);
	}

	// update sbreak pointer
	env->sbreak = last_address;

	// adjusting end block
	int32 *new_end_block = (int32 *)(last_address - sizeof(int));
	*new_end_block = 1;

	// updating end block in env
	env->end_bound = (uint32)new_end_block;
	return (void*)old_sbrk;


}

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	/*====================================*/
	/*Remove this line before start coding*/
	//	inctst();
	//	return;
	/*====================================*/

	//TODO: [PROJECT'24.MS2 - #13] [3] USER HEAP [KERNEL SIDE] - allocate_user_mem()
	// Write your code here, remove the panic and write your code
	//panic("allocate_user_mem() is not implemented yet...!!");


	uint32 it = virtual_address;
	uint32 num_of_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE; // rounding up the pages

	for (int i = 0; i < num_of_pages; i++, it += PAGE_SIZE) {
		//mark the range
		if(i == 0) mark_page(it, e, PAGE_MARK_START);
		else mark_page(it, e, PAGE_MARKED);

	}
}

//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	/*====================================*/
	/*Remove this line before start coding*/
//	inctst();
//	return;
	/*====================================*/


	//TODO: [PROJECT'24.MS2 - #15] [3] USER HEAP [KERNEL SIDE] - free_user_mem
	// Write your code here, remove the panic and write your code
	panic("free_user_mem() is not implemented yet...!!");


	uint32 num_of_pages = size/PAGE_SIZE;
	uint32 va = virtual_address;

	uint32 start_page = virtual_address/PAGE_SIZE;
	// the page num of the last page in the range
	uint32 end_page = (virtual_address + size - PAGE_SIZE)/PAGE_SIZE;

	// 1. unmark and free the processes
	for(int i = 0; i<num_of_pages; i++, va+=PAGE_SIZE) {
		// unmark page and update the size array
		unmark_page(va, e);

		// free the page
		uint32 *page_table_ptr;
		struct FrameInfo* frame_info_ptr = get_frame_info(e->env_page_directory, va, &page_table_ptr);
		kfree((void*)va);
//		unmap_frame(e->env_page_directory, va);
//		free_frame(frame_info_ptr);

		// remove the page from page file
		pf_remove_env_page(e, va);
	}

	// 2. remove page from the page file (done in the above loop for each page)

	// 3. remove pages from the process working set elements
	struct WorkingSetElement* ws_element;
	struct WorkingSetElement* held_ws_element = NULL;
	LIST_FOREACH(ws_element, &(e->page_WS_list)) {

		// remove the hold element
		if(held_ws_element != NULL) {
			LIST_REMOVE(&(e->page_WS_list), held_ws_element);
			held_ws_element = NULL;
		}

		uint32 page_num = ws_element->virtual_address /PAGE_SIZE;
		if((char*)(ws_element->virtual_address) >= (char*)virtual_address &&
				(char*)(ws_element->virtual_address) < (char*)(virtual_address + size)) {
			// if the current working set element within the given range
			// i will hold it and remove it in the next iteration
			// can't remove it in the current iteration as it will make the iterator pointer lost
			held_ws_element = ws_element;
		}
	}

	// if the last element is to be removed
	if(held_ws_element != NULL) {
		LIST_REMOVE(&(e->page_WS_list), held_ws_element);
	}



	//TODO: [PROJECT'24.MS2 - BONUS#3] [3] USER HEAP [KERNEL SIDE] - O(1) free_user_mem
}

//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//[PROJECT] [USER HEAP - KERNEL SIDE] move_user_mem
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//

