#include "stdafx.h"
#include "string.h"
#include "assert.h"
#include <iostream>

typedef struct tagManageData{
	void* data_ptr;
	void* next;
	size_t block_size;
	bool isbusy;
}ManageData, *PManageData;



class SmallAllocator {
private:
	char Memory[1048576];
	size_t total_size;
public:
	SmallAllocator() {
		
		total_size = 1048576;
		memset(Memory, 0, total_size);
		PManageData p_manage_data = (PManageData)Memory;
		p_manage_data->next = NULL;
		p_manage_data->block_size = total_size-sizeof(ManageData);
	}

	void *Alloc(unsigned int Size) {
		
		size_t manage_size = sizeof(ManageData);
		PManageData p_new_manage_data;
		PManageData p_manage_data;
		if (Size == NULL) {
			std::cout << "Null ptr";
			return NULL;
		}

		if (Size+manage_size > total_size) {
			std::cout << "Out of memory";
			return NULL;
		}
		
		p_manage_data = (PManageData)Memory;
		while (1) {
			if (!p_manage_data->isbusy &&p_manage_data->block_size >= Size) {
				break;
			}
				p_manage_data = (PManageData)p_manage_data->next;
		}
		p_manage_data->isbusy = true;

		p_manage_data->data_ptr = (char*)p_manage_data + sizeof(ManageData);
		char*ptr = (char*)p_manage_data->data_ptr + Size;
		p_new_manage_data = (PManageData)ptr;
		if (!p_new_manage_data->isbusy) {
			p_new_manage_data->block_size = p_manage_data->block_size - Size;
			p_new_manage_data->data_ptr = (char*)p_new_manage_data + manage_size;
		}
		p_manage_data->block_size = Size;
		p_manage_data->next = p_new_manage_data;
		return p_manage_data->data_ptr;
	}
	void *ReAlloc(void *Pointer, unsigned int Size) {
		if (NULL == Pointer){
			std::cout << "Out of memory";
			return NULL;
	}
		size_t manage_size = sizeof(ManageData);
		PManageData p_new_manage_data=NULL;
		PManageData p_manage_data=NULL;

		char* old_data = (char*)Pointer-manage_size;
		p_manage_data = (PManageData)old_data;
 		p_new_manage_data = (PManageData)p_manage_data->next;
		
		
			void* ptr = Alloc(Size);
			ptr = (char*)ptr - manage_size;
			p_new_manage_data = (PManageData)ptr;
			p_new_manage_data->data_ptr = (char*)p_new_manage_data + manage_size;
			if (p_manage_data->block_size > Size)
				memcpy(p_new_manage_data->data_ptr, p_manage_data->data_ptr, Size);
			else
				memcpy(p_new_manage_data->data_ptr, p_manage_data->data_ptr, p_manage_data->block_size);
		Free(p_manage_data->data_ptr);
		return p_new_manage_data->data_ptr;
	};
	void Free(void *Pointer) {
		if (NULL == Pointer) {
			std::cerr << "Out of memory";
			return;
		}
		size_t manage_size = sizeof(ManageData);
		char* old_data = (char*)Pointer - manage_size;
		PManageData p_manage_data=(PManageData)old_data;
		p_manage_data->isbusy = false;
		memset(p_manage_data->data_ptr, 0, p_manage_data->block_size);
	};
};

int main() {
	SmallAllocator A2;

	int * in1 = (int *)A2.Alloc(1 * sizeof(int));
	*in1 = 56;
	int * in2 = (int *)A2.Alloc(1 * sizeof(int));
	*in2 = 63;
	int * in3 = (int *)A2.Alloc(1 * sizeof(int));
	*in3 = 58;
	int * in4 = (int *)A2.Alloc(1 * sizeof(int));
	*in4 = 23;
	int * in5 = (int *)A2.Alloc(1 * sizeof(int));
	*in5 = 29;

	assert(*in1 == 56);
	assert(*in2 == 63);
	assert(*in3 == 58);
	assert(*in4 == 23);
	assert(*in5 == 29);

	A2.Free(in2);
	A2.Free(in3);
	A2.Free(in4);

	in2 = (int *)A2.Alloc(1 * sizeof(int));
	*in2 = 163;
	in3 = (int *)A2.Alloc(1 * sizeof(int));
	*in3 = 158;
	in4 = (int *)A2.Alloc(1 * sizeof(int));
	*in4 = 123;

	assert(*in2 == 163);
	assert(*in3 == 158);
	assert(*in4 == 123);


	in5 = (int *)A2.ReAlloc(in5, 3 * sizeof(int));

	in5[1] = 25;
	in5[2] = 21;

	assert(in5[0] == 29);
	assert(in5[1] == 25);
	assert(in5[2] == 21);
	assert(*in1 == 56);
	assert(*in5 == 29);

	A2.Free(in1);
	A2.Free(in5);

	int * A2_P3 = (int *)A2.Alloc(10 * sizeof(int));
	for (unsigned int i = 0; i < 10; i++) A2_P3[i] = i;
	for (unsigned int i = 0; i < 10; i++) assert(A2_P3[i] == i);

	int * A2_P4 = (int *)A2.Alloc(10 * sizeof(int));
	for (unsigned int i = 0; i < 10; i++) A2_P4[i] = i;
	A2_P4 = (int *)A2.ReAlloc(A2_P4, 20 * sizeof(int));
	for (unsigned int i = 10; i < 20; i++) A2_P4[i] = i;
	for (unsigned int i = 0; i < 10; i++) assert(A2_P3[i] == i);
	for (unsigned int i = 0; i < 20; i++) assert(A2_P4[i] == i);

	A2.Free(A2_P3);

	int * A2_P1 = (int *)A2.Alloc(10 * sizeof(int));
	for (unsigned int i = 0; i < 10; i++) A2_P1[i] = i;
	for (unsigned int i = 0; i < 10; i++) assert(A2_P1[i] == i);

	int * A2_P2 = (int *)A2.Alloc(10 * sizeof(int));
	for (unsigned int i = 0; i < 10; i++) A2_P2[i] = i;
	A2_P2 = (int *)A2.ReAlloc(A2_P2, 20 * sizeof(int));
	for (unsigned int i = 10; i < 20; i++) A2_P2[i] = i;
	for (unsigned int i = 0; i < 10; i++) assert(A2_P1[i] == i);
	for (unsigned int i = 0; i < 20; i++) assert(A2_P2[i] == i);
	for (unsigned int i = 0; i < 20; i++) assert(A2_P4[i] == i);

	assert(*in2 == 163);
	assert(*in3 == 158);
	assert(*in4 == 123);


	return 0;

}