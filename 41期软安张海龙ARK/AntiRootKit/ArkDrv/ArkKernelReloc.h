#pragma once
#include <ntifs.h>

// 描述重定位项的结构体
typedef struct _TYPEOFFSET {
	unsigned short Offset : 12;
	unsigned short Type : 4;
} TYPEOFFSET, * PTYPEOFFSET;



// 内核重载
VOID ArkKernelReloc();