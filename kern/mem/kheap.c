#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	cprintf("---------------initialize_kheap_dynamic_allocator called---------------\n");
	//TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator
	// Write your code here, remove the panic and write your code
	//panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");

    //step1 limits of kheap
	start_kernal_heap =  daStart;
	segment_break = start_kernal_heap + initSizeToAllocate;
	hard_limit = daLimit;

	if(daStart + initSizeToAllocate > hard_limit)
	{
		panic("initialize_kheap_dynamic_allocator() exceeds hard limit..!!");
	}

	//step2
	//2.1 create table if   doesn't exist (for sure)
	uint32 *check_page_table = NULL;
	int32 r = get_page_table(ptr_page_directory , start_kernal_heap , &check_page_table);
	if(r!=TABLE_IN_MEMORY)
	{
		create_page_table(ptr_page_directory ,  start_kernal_heap);
		panic("initialize_kheap_dynamic_allocator() tableee ..!!");
	}
	//2.2 allocate all pages and map
	uint32 number_of_pages = ROUNDUP(initSizeToAllocate, PAGE_SIZE) / PAGE_SIZE; // 1007617 = (daStart - KERNEL_HEAP_MAX) ,,, (daStart + initSizeToAllocate )
	//print
	cprintf("dastart : %x \n", daStart);
	cprintf("daLimit : %x \n", hard_limit);
	cprintf("number_of_pages : %d \n", number_of_pages);
	//page address
    uint32 va = daStart;
	for(uint32 i = 0 ; i < number_of_pages ; i++)
	{
		struct FrameInfo *ptr_frame_info;
        int ret = allocate_frame(&ptr_frame_info);
        if(ret == E_NO_MEM){
        	panic("initialize_kheap_dynamic_allocator() no memory ..!!");
        }

        int rrr = map_frame(ptr_page_directory , ptr_frame_info , va , PERM_WRITEABLE);
        if(rrr != 0){
			cprintf("count : %d , rrr: %d", i, ret);
			panic("initialize_kheap_dynamic_allocator() not mapped ..!!");
		}
        va = va + PAGE_SIZE;
	}

	cprintf("va : %x \n", va);
	pgalloc_last = (hard_limit + PAGE_SIZE);

	initialize_dynamic_allocator(start_kernal_heap , initSizeToAllocate);
	//panic("initialize_kheap_dynamic_allocator() no memory ..!!");
	return 0;
}



void* sbrk(int numOfPages)
{
	/* numOfPages > 0: move the segment break of the kernel to increase the size of its heap by the given numOfPages,
	 * 				you should allocate pages and map them into the kernel virtual address space,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, return -1
	 */

	//MS2: COMMENT THIS LINE BEFORE START CODING==========
	//return (void*)-1 ;
	//====================================================

	//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
	// Write your code here, remove the panic and write your code
	//panic("sbrk() is not implemented yet...!!");
	cprintf("---------------sbrk called---------------\n");
	if(numOfPages == 0) return (void*)segment_break; // edge case

	uint32 increasing = numOfPages * PAGE_SIZE; // size to be allocated
	uint32 last_address = segment_break + increasing;
	uint32 old_sbrk = segment_break;

	if(last_address > hard_limit)
	{
		return (void *)E_UNSPECIFIED;
	}

	//page address
	uint32 va = segment_break;

	for(int32 i = 0 ; i < numOfPages ; i++)
	{

		uint32 *ptr_table = NULL;
		struct FrameInfo *ptr_frame_info = get_frame_info(ptr_page_directory , va , &ptr_table);
		if(ptr_frame_info != NULL){
			// this shouldn't execute at all
			continue;
		}

        uint32 ret = allocate_frame(&ptr_frame_info);
        if(ret == E_NO_MEM) {
        	return (void *)E_UNSPECIFIED;
        }
        map_frame(ptr_page_directory , ptr_frame_info , va ,  PERM_WRITEABLE);
        va += PAGE_SIZE;
	}

	struct blockElement * newBlock = (struct blockElement *)(segment_break); // assign old brk to a block
	segment_break = va;
	int32 *end_last_page = (int32 *)(va - sizeof(int));

	*end_last_page = 1;


	set_block_data((void*)newBlock , increasing , 1);

	// you have to set bounds first before calling free as it checks for them
	end_bound = (void*) end_last_page;

	free_block((void*)newBlock);

	return (void*)old_sbrk;
}

//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

uint32 get_pgallocation_address(uint32 size) {
	uint32 start = hard_limit + PAGE_SIZE;
	uint32 total_pages = (pgalloc_last - start) / PAGE_SIZE;

	uint32 it = start;
	uint32 curSize = 0;
	uint32 pgalloc_ptr = 0;


	for (; it < pgalloc_last; it += PAGE_SIZE) {

		uint32 *ptr_table = NULL;
		struct FrameInfo *ptr_frame_info = get_frame_info(ptr_page_directory, it, &ptr_table);

		if (ptr_frame_info == NULL) { // free frame
			if(curSize == 0) {
				pgalloc_ptr = it;
			}
			curSize += PAGE_SIZE;

		}else {
			curSize = 0;
			pgalloc_ptr = 0;
		}

		if (curSize >= size) {
			break;
		}
	}

	// if exist some free pages before pgalloc_last
	if(pgalloc_ptr != 0 && curSize >= size)
	{
		return (pgalloc_ptr);
	}
	return pgalloc_last;

}

void* kmalloc(unsigned int size)
{
    //TODO: [PROJECT'24.MS2 - #03] [1] KERNEL HEAP - kmalloc
    // Write your code here, remove the panic and write your code
	//panic("kmalloc() is not implemented yet...!!");

//	  1. validate size
//    2. create vp
//    3. allocate physical frame
//    4. map virtual to physical

//	SIZE VALIDATION:

		int total_size = size;
		if(total_size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
			return alloc_block_FF((uint32)total_size);
		}

		if (size > (KERNEL_HEAP_MAX - pgalloc_last)){
			//cprintf("trying to allocate greater than limit\n");
			return NULL;
		}


		// making total size multiple of page size
		if (size % PAGE_SIZE != 0) {
			int remain = PAGE_SIZE - (size % PAGE_SIZE);
			total_size += remain;
		}


//		ALLOCATE  & MAP
		uint32 it = get_pgallocation_address((uint32)total_size);

		if (it == pgalloc_last) {
			if(pgalloc_last + total_size > KERNEL_HEAP_MAX) {
				return NULL;
			}

			pgalloc_last += total_size;
		}

		uint32 result = it;
		uint32 rett = it;
		uint32 num_pages = total_size / PAGE_SIZE;
		for (int i = 1; i <= num_pages; i++, it += PAGE_SIZE) {

			struct FrameInfo *newFrame = NULL;
			int state = allocate_frame(&newFrame);

			if (state == E_NO_MEM) {
				return NULL;
			}

			state = map_frame(ptr_page_directory, newFrame, it, PERM_WRITEABLE);
			if (state == E_NO_MEM) {		// just to make sure.
				return NULL;
			}

		}

		*((uint32*) rett) = num_pages;

		return (void*)rett;
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
//	panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

//	1. validate arguments
//	2. if Blk allocator | Call free
//	3. if Pg allocator  | unmap & remove all the related frames.

////	validate:
	if (virtual_address == NULL || (uint32)virtual_address > KERNEL_HEAP_MAX|| (uint32)virtual_address < KERNEL_HEAP_START)
	{return;}
//
//	if (virtual_address > (void*)segment_break && virtual_address <= (void*)hard_limit+PAGE_SIZE)
//	{return;}

//	free: whether Blk or Pg allocator
	if (virtual_address <= (void*)segment_break && virtual_address >=(void*)start_kernal_heap){
//		Blk allocator: call free;
		cprintf("free using the dynalloc.\n");
		free_block(virtual_address);
		return;
	}

	uint32 num_pages = *((uint32*)virtual_address);
	if (num_pages==0)
		return;

	void *it = virtual_address;
	for(uint32 i = 0; i < num_pages; i++){
		uint32 *ptr_page_table = NULL;
		uint32 table_status =  get_page_table(ptr_page_directory,(uint32)it,&ptr_page_table);
		struct FrameInfo *frame = get_frame_info(ptr_page_directory, (uint32)it, &ptr_page_table);
		if (frame == NULL){return;}
		free_frame(frame);
		unmap_frame(ptr_page_directory, (uint32)it);
		it+= PAGE_SIZE;
	}



}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");
	uint32 *ptr_table = NULL;
	struct FrameInfo *ptr_frame_info = get_frame_info(ptr_page_directory, (uint32)virtual_address, &ptr_table);
	if(ptr_frame_info == NULL) {
		return (unsigned int) 0;
	}

	//current_ptr - frames_info --> size
	// size / size of struct
	//

	/*
	 * masking to get offset
	 * va   	-> 00000000 00000000 00101100 11011001
	 * mask 	-> 00000000 00000000 00001111 11111111
	 * mask&va 	-> 00000000 00000000 00001100 11011001 -> the offset to be added to the address
	 */

	uint32 mask = (1 << 12)-1;
	uint32 offset = (mask&(uint32)virtual_address);

	return (unsigned int)to_physical_address(ptr_frame_info) + offset;
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}
//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");
}
