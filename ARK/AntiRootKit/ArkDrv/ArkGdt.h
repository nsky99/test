#pragma once
#include <ntifs.h>

// ��ӦIO ͳ��GDT����
NTSTATUS OnCountGDT(PIRP pIrp);

// ��ӦIO ��ѯGDT��Ϣ
NTSTATUS OnQueryGDT(PIRP pIrp);