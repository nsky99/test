#pragma once

#include <ntifs.h>
// ��װ����
void InstallHook();


// ж�ع���
void UnInstallHook();



// ��ӦIo SysHook
NTSTATUS OnHookSysEnter(PIRP pIrp);


// ��ӦIo SysUnHook
NTSTATUS OnUnHookSysEnter(PIRP pIrp);