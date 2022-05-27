#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
uint32 start = KERNEL_HEAP_START;
int program_count = (USER_HEAP_MAX - USER_HEAP_START) / PAGE_SIZE;

struct Program {
	uint32 startVA;
	int size;
} programs[(KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE];

struct BESTADD {
	uint32 startVA;
	int size;
} BESTADDs[(KERNEL_HEAP_MAX - KERNEL_HEAP_START) / PAGE_SIZE];

void* kmalloc(unsigned int size) {
	//TODO: [PROJECT 2022 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	//NOTE: Allocation using NEXTFIT strategy
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details
	size = ROUNDUP(size, PAGE_SIZE);
	uint32 framesNo = size / PAGE_SIZE;

	uint32 address;

	if (isKHeapPlacementStrategyNEXTFIT()) {
		int faild = 0;

		uint32 * ptr_page_table = NULL;
		uint32 frame = 0;

		for (address = start; address <= KERNEL_HEAP_MAX; address +=
				PAGE_SIZE) {

			if ((address + (framesNo * PAGE_SIZE)) >= KERNEL_HEAP_MAX
					&& faild == 1)
				return NULL;
			if (address > KERNEL_HEAP_MAX - (framesNo * PAGE_SIZE)) {
				faild = 1;
				start = KERNEL_HEAP_START;
				address = start;
			}

			uint32 * ptr_page_table = NULL;
			uint32 frame = 0;

			for (frame = 0; frame < framesNo; frame++) {
				get_page_table(ptr_page_directory,
						(void*) address + (frame * PAGE_SIZE), &ptr_page_table);

				if ((ptr_page_table[PTX(address + (frame * PAGE_SIZE))]
						& PERM_PRESENT) != 0) {
					break;
				}
			}
			if (frame == framesNo)
				break;
		}
	} else if (isKHeapPlacementStrategyBESTFIT()) {
		uint32 Baddress = -1;
		int freeFRAME = 1;
		int frame = 0;
		int firstTime = 1;
		int MAKEBEST = -1;
		int OldBEST;
		uint32 * ptr_page_table;
		int BestPROGRAMS = 0;

		for (int i = KERNEL_HEAP_START; i < KERNEL_HEAP_MAX; i += PAGE_SIZE) {

			ptr_page_table = NULL;
			get_page_table(ptr_page_directory, (void*) i, &ptr_page_table);

			if ((ptr_page_table[PTX(i)] & PERM_PRESENT) == 0
					&& Baddress == -1) {
				Baddress = i;
				frame++;
			}

			else if ((ptr_page_table[PTX(i)] & PERM_PRESENT) == 0) {
				frame++;
			}

			else if ((ptr_page_table[PTX(i)] & PERM_PRESENT) != 0) {
				Baddress = -1;
				frame = 0;
				continue;
			}

			if ((frame >= framesNo) && firstTime == 1) {
				BESTADDs[BestPROGRAMS].startVA = Baddress;
				BESTADDs[BestPROGRAMS].size = frame;
				firstTime = 0;
				MAKEBEST = 1;
			} else if ((frame >= framesNo)) {
				if (Baddress == BESTADDs[BestPROGRAMS].startVA) {
					BESTADDs[BestPROGRAMS].size = frame;
				} else {
					BestPROGRAMS++;
					BESTADDs[BestPROGRAMS].startVA = Baddress;
					BESTADDs[BestPROGRAMS].size = frame;
				}

			}
		}

		if (BestPROGRAMS >= 0) {
			if (MAKEBEST == -1)
				return NULL;
			address = BESTADDs[0].startVA;
			OldBEST = BESTADDs[0].size;
			for (int j = 1; j < BestPROGRAMS; j++) {
				if (BESTADDs[j].size < OldBEST) {
					OldBEST = BESTADDs[j].size;
					address = BESTADDs[j].startVA;
				}
			}
		} else {
			return NULL;
		}

	}

	//TODO: [PROJECT 2022 - BONUS1] Implement a Kernel allocation strategy
	// Instead of the Next allocation/deallocation, implement
	// BEST FIT strategy
	// use "isKHeapPlacementStrategyBESTFIT() ..."
	// and "isKHeapPlacementStrategyNEXTFIT() ..."
	//functions to check the current strategy
	//change this "return" according to your answer

	uint32 add2 = address;

	struct Frame_Info* ptr_free_frame = NULL;

	for (int frame = 0; frame < framesNo; frame++) {
		ptr_free_frame = NULL;
		allocate_frame(&ptr_free_frame);
		map_frame(ptr_page_directory, ptr_free_frame, (void*) add2,
				(PERM_PRESENT | PERM_WRITEABLE));
		add2 += PAGE_SIZE;
	}

	if (isKHeapPlacementStrategyNEXTFIT())
		start = address + (framesNo * PAGE_SIZE);

	programs[((address - KERNEL_HEAP_START) / PAGE_SIZE)].startVA = address;
	programs[((address - KERNEL_HEAP_START) / PAGE_SIZE)].size = framesNo;

	return (void *) address;

}

void kfree(void* virtual_address) {

	//TODO: [PROJECT 2022 - [2] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");

	int i = 0;
	for (; i < program_count; i++) {
		if ((uint32) virtual_address == programs[i].startVA) {
			break;
		}
	}

	uint32 *ptr_page_table = NULL;
	for (int NumOfFrames = 0; NumOfFrames < programs[i].size; NumOfFrames++) {
		uint32 VA = (uint32) virtual_address + (NumOfFrames * PAGE_SIZE);
		unmap_frame(ptr_page_directory, (uint32*) VA);

	}

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

}

unsigned int kheap_virtual_address(unsigned int physical_address) {
	//TODO: [PROJECT 2022 - [3] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
//	uint32 virtual_address;
//	uint32 framNo = physical_address/PAGE_SIZE;
//	for(int i = KERNEL_HEAP_START)
//	{
//
//	}

//	uint32 FramNumber = physical_address/PAGE_SIZE;
//
//	for(int i = KERNEL_HEAP_START ; i < KERNEL_HEAP_MAX; i++ )
//	{
//		uint32* pPT_1 = NULL;
//		uint32 VADD = i * PAGE_SIZE;
//
//		get_page_table(ptr_page_directory,(int*)VADD,&pPT_1);
//
//		//chick PT not = NULL and chick PRESENT BIT
//		if((pPT_1 != NULL) && (((pPT_1[PTX(VADD)]) & PERM_PRESENT) != 0))
//		{
//			//compare two frames
//			if(FramNumber == (pPT_1[PTX(VADD)]>>12))
//			{
//				return VADD;
//			}
//		}
//	}

	for (uint32 i = KERNEL_HEAP_START; i < KERNEL_HEAP_MAX; i += PAGE_SIZE) {

		uint32* pagetable_ptr = NULL;

		get_page_table(ptr_page_directory, (void*) i, &pagetable_ptr);

		if (pagetable_ptr != NULL) {
			int entry = pagetable_ptr[PTX(i)];
			int frame_num = entry >> 12;
			if (frame_num == physical_address / PAGE_SIZE) {

				if ((entry & PERM_PRESENT) == PERM_PRESENT)
					return i;
				else
					return 0;
			}
		}
	}
	return 0;

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer

//	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address) {
	uint32* ptr_table = NULL;
	get_page_table(ptr_page_directory, (uint32*) virtual_address, &ptr_table);
	uint32 pa = (ptr_table[PTX(virtual_address)] >> 12) * PAGE_SIZE;
	return pa;

	//TODO: [PROJECT 2022 - [4] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer
	return 0;
}
