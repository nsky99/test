#pragma once

#include <ntifs.h>
// Ark全局设备对象指针
extern PDEVICE_OBJECT g_pArkDevObj;
// Ark全局驱动对象指针
extern PDRIVER_OBJECT g_pArkDrvObj;

// 驱动入口程序
NTSTATUS DriverEntry(PDRIVER_OBJECT pDrvObj, PUNICODE_STRING pRegPath);

// 驱动卸载函数
VOID ArkDrvUnload(PDRIVER_OBJECT pDrvObj);

// 设置ArkIrp - DeviceIoControl回调
NTSTATUS ArkIrpControl(PDEVICE_OBJECT pDevObj, PIRP pIrp);