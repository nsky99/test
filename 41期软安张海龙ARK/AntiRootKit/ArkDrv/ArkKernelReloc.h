#pragma once
#include <ntifs.h>

// �����ض�λ��Ľṹ��
typedef struct _TYPEOFFSET {
	unsigned short Offset : 12;
	unsigned short Type : 4;
} TYPEOFFSET, * PTYPEOFFSET;



// �ں�����
VOID ArkKernelReloc();