#pragma once

#include <ntifs.h>

// ��ӦIoͳ��ָ������ģ������
NTSTATUS OnCountModule(PIRP pIrp);

// ��ӦIo��ѯ����ģ����Ϣ
NTSTATUS OnQueryModule(PIRP pIrp);