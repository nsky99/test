#pragma once

#include <ntifs.h>
// Arkȫ���豸����ָ��
extern PDEVICE_OBJECT g_pArkDevObj;
// Arkȫ����������ָ��
extern PDRIVER_OBJECT g_pArkDrvObj;

// ������ڳ���
NTSTATUS DriverEntry(PDRIVER_OBJECT pDrvObj, PUNICODE_STRING pRegPath);

// ����ж�غ���
VOID ArkDrvUnload(PDRIVER_OBJECT pDrvObj);

// ����ArkIrp - DeviceIoControl�ص�
NTSTATUS ArkIrpControl(PDEVICE_OBJECT pDevObj, PIRP pIrp);