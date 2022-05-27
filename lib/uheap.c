#include <inc/lib.h>

// malloc()
//	This function use NEXT FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

uint32 start = USER_HEAP_START;
int program_count =  (USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE;

struct Program
{
	uint32 startVA;
	int size;
	int is_allocated ;
}programs[(USER_HEAP_MAX - USER_HEAP_START)/PAGE_SIZE];

void* malloc(uint32 size)
{
	//TODO: [PROJECT 2022 - [9] User Heap malloc()] [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");

	// Steps:
	//	1) Implement NEXT FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	// 	4) Return pointer containing the virtual address of allocated space,
	//

	size = ROUNDUP(size,PAGE_SIZE);
	uint32 framesNo = size / PAGE_SIZE;
	int faild = 0;
	int allocated = 0;
	uint32 address;

	for ( address = start; address <= USER_HEAP_MAX; address+=PAGE_SIZE) {

		if((address + (framesNo*PAGE_SIZE))>=USER_HEAP_MAX && faild==1)
			return NULL;
		if(address>USER_HEAP_MAX - (framesNo*PAGE_SIZE))
		{
			faild=1;
			start=USER_HEAP_START;
			address=start;
		}

		uint32 * ptr_page_table = NULL;
		int frame ;
		for (frame = 0; frame < framesNo; frame++) {

			if (programs[((address - USER_HEAP_START) / PAGE_SIZE) + frame].is_allocated
					== 1) {
				break;
			}
		}

		if(framesNo==frame)
		{
			allocated = 1;
			for (frame = 0; frame < framesNo; frame++) {
				programs[((address - USER_HEAP_START) / PAGE_SIZE) + frame].is_allocated = 1;
			}
			start=address+(frame*PAGE_SIZE);
			sys_allocateMem(address , framesNo);
			break;
		}

	}

	if (allocated == 1) {
		programs[((address - USER_HEAP_START) / PAGE_SIZE)].startVA = address;
		programs[((address - USER_HEAP_START) / PAGE_SIZE)].size = framesNo;
	}

	return (void *)address;
}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("smalloc() is not required ..!!");
	return NULL;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("sget() is not required ..!!");
	return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2022 - [11] User Heap free()] [User Side]
	// Write your code here, remove the panic and write your code

	int i = 0;
	for (; i < program_count; i++) {
		if ((uint32) virtual_address == programs[i].startVA) {
			break;
		}
	}
	//uint32 *ptr_page_table = NULL;
	//sys_freeMem((uint32) virtual_address, programs[i].size);

	for (int NumOfFrames = 0; NumOfFrames < programs[i].size; NumOfFrames++) {
		uint32 VA = (uint32) virtual_address + (NumOfFrames * PAGE_SIZE);
		sys_freeMem((uint32) VA, programs[i].size);
		programs[i+NumOfFrames].is_allocated = 0;
	}
	//programs[i].is_allocated = 0;
	//you shold get the size of the given allocation using its address
	//you need to call sys_freeMem()
	//refer to the project presentation and documentation for details
}


void sfree(void* virtual_address)
{
	panic("sfree() is not requried ..!!");
}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2022 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}
