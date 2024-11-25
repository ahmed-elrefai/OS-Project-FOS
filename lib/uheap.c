#include <inc/lib.h>


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}


//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================

int32 uhis_free_page(uint32 page_va) {
	// page_va is the virtual address within some page , could be with offset doesn't matter.
	uint32 page_num = page_va / PAGE_SIZE;
	return (myEnv->mark_status[page_num] == PAGE_FREE);
}

uint32 uhget_pgallocation_address(uint32 size) {
	//cprintf("%d pages needed\n", pages_needed);

	uint32 start = myEnv->start;
	uint32* page_directory = myEnv->env_page_directory;
	uint32 pg_alloc_last = myEnv->pgalloc_last;

	uint32 it = start;
	uint32 curSize = 0;
	uint32 pgalloc_ptr = 0;

	for (; curSize < size && it < pg_alloc_last; it += PAGE_SIZE) {

		if (uhis_free_page(it)) { // if free page
			if(curSize == 0) {
				pgalloc_ptr = it;
			}
			curSize += PAGE_SIZE;

		}else {	// if marked for another space
			curSize = 0;
			pgalloc_ptr = 0;
		}
	}



	// if exist some free pages before pgalloc_last which could be used.
	if(pgalloc_ptr != 0 && curSize >= size) {
		return pgalloc_ptr;
	}

	return pg_alloc_last;

}


void* malloc(uint32 size)
{

	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	//return NULL;
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy

	uint32 va = myEnv->start;
	uint32 total_size = ((size+PAGE_SIZE-1)/PAGE_SIZE)*PAGE_SIZE;
	uint32 size_counter = 0;
	if (sys_isUHeapPlacementStrategyFIRSTFIT()) {
		if (size <= DYN_ALLOC_MAX_SIZE) {
			return alloc_block_FF(size);
		}else {

			uint32 result = uhget_pgallocation_address(total_size);
			if (result == myEnv->pgalloc_last) {

				if((myEnv->pgalloc_last + total_size) > (uint32)USER_HEAP_MAX) {
					return NULL;
				}

				myEnv->pgalloc_last += total_size;
			}
			sys_allocate_user_mem(result, total_size);
			return (void*)result;
		}
	} else if(sys_isUHeapPlacementStrategyBESTFIT()) { // best fit strategy

	}


	return NULL;


}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
	panic("free() is not implemented yet...!!");

	if (virtual_address == NULL) {
			panic("ufree() : the provided address is NULL..!!");
			return;
	}

	//	free: whether Blk or Pg allocator
	if ((char*)virtual_address < (char*)(myEnv->sbreak - sizeof(int)) && (char*)virtual_address >= (char*)(myEnv->start + sizeof(int))) {
		// block allocator: call free;
		cprintf("(user free)free using the dynalloc.\n");
		free_block(virtual_address);
		return;
	} else if((char*)virtual_address >= (char*)(myEnv->hlimit + sizeof(int)) && (char*)virtual_address < (char*)KERNEL_HEAP_MAX) {
		// unmark pages and free them

		// get the size of the current allocated pages
		uint32 page_num = (((uint32)virtual_address) - USER_HEAP_START)/PAGE_SIZE;
		uint32 pages = 0;
		// delete the allocated range from the list


		// unmark the allocated pages
		uint32 va = (uint32)virtual_address;
		//		for(uint32 i = 0 ; i < pages ; i++, va += PAGE_SIZE) {
		//			unmark_page(va);
		//		}

		sys_free_user_mem((uint32)virtual_address, (pages * PAGE_SIZE));
	} else {
		panic("(user free) the provided address is invalid!\n");
	}
}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
	panic("smalloc() is not implemented yet...!!");
	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
	// Write your code here, remove the panic and write your code
	panic("sget() is not implemented yet...!!");
	return NULL;
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [USER SIDE] - sfree()
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}

//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//[PROJECT]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}
