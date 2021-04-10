#include "ArkDrvEntry.h"
#include "ArkCommon.h"
#include "ArkProcess.h"
#include "ArkThread.h"
#include "ArkModule.h"
#include "ArkDrv.h"
#include "ArkIdt.h"
#include "ArkGdt.h"
#include "ArkFile.h"
#include "ArkReg.h"
#include "ArkSSDT.h"
#include "ArkSysEnterHook.h"
#include "ArkKernelReloc1.h"

#include <ntddk.h>


// Ark全局设备对象指针
PDEVICE_OBJECT g_pArkDevObj = NULL;
// Ark全局驱动对象指针
PDRIVER_OBJECT g_pArkDrvObj = NULL;


NTSTATUS DriverEntry(PDRIVER_OBJECT pDrvObj, PUNICODE_STRING pRegPath)
{
  UNREFERENCED_PARAMETER(pRegPath);
  NTSTATUS status = STATUS_SUCCESS;

  // 设置Ark驱动卸载回调
  pDrvObj->DriverUnload = ArkDrvUnload;


  // 设置ArkIrp回调
  for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
  {
    pDrvObj->MajorFunction[i] = ArkIrpControl;
  }


  // 创建设备对象
  PDEVICE_OBJECT pArkDevObj = NULL;
  // 为Ark设备对象取名称
  UNICODE_STRING wZDevName = { 0 };
  RtlInitUnicodeString(&wZDevName, ARK_DEVICE_NAME);
  status = IoCreateDevice(pDrvObj, 0, &wZDevName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pArkDevObj);
  if (!NT_SUCCESS(status))
  {
    return status;
  }

  pDrvObj->Flags |= DO_BUFFERED_IO;
  
  // 创建符号链接
  UNICODE_STRING wZSymLinkName = RTL_CONSTANT_STRING(ARK_SYMLINK_NAME);
  status = IoCreateSymbolicLink(&wZSymLinkName, &wZDevName);
  if (!NT_SUCCESS(status))
  {
    IoDeleteDevice(pArkDevObj);
    return status;
  }

  
  // 获取Ark驱动对象和Ark设备对象
  g_pArkDrvObj = pDrvObj;
  g_pArkDevObj = pArkDevObj;


  KernelReload();
  KdPrint(("NtBase: %08X\n", (ULONG)GetNtImageBase(pDrvObj)));
  KdPrint(("驱动加载成功\r\n"));


  // reg test
  KdPrint(("reg test\n"));
  ULONG ulMemSize = sizeof(REGINF) * 0xff;
  PREGINF pRegInfo = (PREGINF)ExAllocatePool(PagedPool, ulMemSize);

  GetRegList(0, pRegInfo);

  return status;
}


VOID ArkDrvUnload(PDRIVER_OBJECT pDrvObj)
{
  DbgBreakPoint();
  UnInstallHook();
  UnKernelHook();
  // 删除符号链接
  UNICODE_STRING wZSymLinkName = RTL_CONSTANT_STRING(ARK_SYMLINK_NAME);
  IoDeleteSymbolicLink(&wZSymLinkName);

  // 删除设备对象
  if (NULL != pDrvObj->DeviceObject)
  {
    IoDeleteDevice(pDrvObj->DeviceObject);
    g_pArkDevObj = NULL;
  }
  KdPrint(("驱动卸载成功\r\n"));
}


NTSTATUS ArkIrpControl(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
  UNREFERENCED_PARAMETER(pDevObj);
  NTSTATUS           status = STATUS_SUCCESS; // ret
  PIO_STACK_LOCATION pIrpStack = NULL;        // irp栈指针
  ULONG				       ulIoControlCode = 0;     // CTL_CODE - R0-R3 通信码

  // 获取Irp栈信息
  pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
  // 获取控制码
  ulIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;

  switch (ulIoControlCode)
  {
  // 统计进程个数
  case ArkCountProcess:
  {
    status = OnCountProcess(pIrp);
  }
    break;
  // 查询进程信息
  case ArkQueryProcess:
  {
    status = OnQueryProcess(pIrp);
  }
    break;
  // 暂停进程
  case ArkSuspendProcess:
  {
    status = OnSuspendProcess(pIrp);
  }
    break;
  // 恢复进程
  case ArkResumeProcess:
  {
    status = OnResumeProcess(pIrp);
  }
    break;
  // 结束进程
  case ArkKillProcess:
  {
    status = OnTerminateProcess(pIrp);
  }
    break;
  // 隐藏进程
  case ArkHideProcess:
  {
    status = OnHideProcess(pIrp);
  }
    break;
  // 线程计数 - 进程中的线程
  case ArkCountThread:
  {
    status = OnCountThread(pIrp);
  }
    break;
  // 查询指定进程的线程信息
  case ArkQueryThread:
  {
    status = OnQueryThread(pIrp);
  }
    break;
  // 暂停线程
  case ArkSuspendThread:
  {
    status = OnSuSpendThread(pIrp);
  }
    break;
  // 恢复线程
  case ArkResumeThread:
  {
    status = OnResumeThread(pIrp);
  }
    break;
  // 结束线程
  case ArkKillThread:
  {
    status = OnKillThread(pIrp);
  }
    break;
  // 查询指定进程模块数量
  case ArkCountModule:
  {
    status = OnCountModule(pIrp);
  }
    break;
  // 查询指定进程模块信息
  case ArkQueryModule:
  {
    status = OnQueryModule(pIrp);
  }
    break;
  // 查询驱动数量
  case ArkCountDriver:
  {
    status = OnCountDriver(pIrp);
  }
    break;
  // 查询驱动信息
  case ArkQueryDriver:
  {
    status = OnQueryDriver(pIrp);
  }
    break;
  // 隐藏驱动
  case ArkHideDriver:
  {
    status = OnHideDriver(pIrp);
  }
    break;
  // 统计IDT数量
  case ArkCountIDT:
  {
    status = OnCountIdt(pIrp);
  }
    break;
  // 查询IDT信息
  case ArkQueryIDT:
  {
    status = OnQueryIdt(pIrp);
  }
    break;
  // 统计GDT数量
  case ArkCountGDT:
  {
    status = OnCountGDT(pIrp);
  }
    break;
  // 查询GDT信息
  case ArkQueryGDT:
  {
    status = OnQueryGDT(pIrp);
  }
    break;
  // 统计指定目录的文件数量
  case ArkCountFile:
  {
    status = OnCountFile(pIrp);
  }
    break;
  // 查询指定目录的文件信息
  case ArkQueryFile:
  {
    status = OnQueryFile(pIrp);
  }
    break;
  // 删除指定目录下的文件
  case ArkDeleteFile:
  {
    status = OnDelFile(pIrp);
  }
    break;
  // 查询SSDT数量
  case ArkCountSSDT:
  {
    status = OnCountSSDT(pIrp);
  }
    break;
  // 查询SSDT信息
  case ArkQuerySSDT:
  {
    status = OnQuerySSDT(pIrp);
  }
    break;
  // SysEnter操作
  case ArkHookSysEnter:
  {
    status = OnHookSysEnter(pIrp);
  }
    break;
  case ArkUnHookSysEnter:
  {
    status = OnUnHookSysEnter(pIrp);
  }
    break;
  case ArkQueryReg:
  {
    status = OnArkQueryReg(pIrp);
  }
    break;
  case ArkCounteReg:
  {
    status = OnArkCountReg(pIrp);
  }
    break;
  // 其他操作
  default:
  {
    // 默认返回R3信息
    pIrp->IoStatus.Information = 0;                       // 缓冲区大小
    pIrp->IoStatus.Status = STATUS_SUCCESS;               // R0处理状态
  }
    break;
  }

  // 完成IO请求
  IoCompleteRequest(pIrp, IO_NO_INCREMENT);
  return status;
}