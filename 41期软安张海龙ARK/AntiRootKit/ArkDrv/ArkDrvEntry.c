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


// Arkȫ���豸����ָ��
PDEVICE_OBJECT g_pArkDevObj = NULL;
// Arkȫ����������ָ��
PDRIVER_OBJECT g_pArkDrvObj = NULL;


NTSTATUS DriverEntry(PDRIVER_OBJECT pDrvObj, PUNICODE_STRING pRegPath)
{
  UNREFERENCED_PARAMETER(pRegPath);
  NTSTATUS status = STATUS_SUCCESS;

  // ����Ark����ж�ػص�
  pDrvObj->DriverUnload = ArkDrvUnload;


  // ����ArkIrp�ص�
  for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
  {
    pDrvObj->MajorFunction[i] = ArkIrpControl;
  }


  // �����豸����
  PDEVICE_OBJECT pArkDevObj = NULL;
  // ΪArk�豸����ȡ����
  UNICODE_STRING wZDevName = { 0 };
  RtlInitUnicodeString(&wZDevName, ARK_DEVICE_NAME);
  status = IoCreateDevice(pDrvObj, 0, &wZDevName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pArkDevObj);
  if (!NT_SUCCESS(status))
  {
    return status;
  }

  pDrvObj->Flags |= DO_BUFFERED_IO;
  
  // ������������
  UNICODE_STRING wZSymLinkName = RTL_CONSTANT_STRING(ARK_SYMLINK_NAME);
  status = IoCreateSymbolicLink(&wZSymLinkName, &wZDevName);
  if (!NT_SUCCESS(status))
  {
    IoDeleteDevice(pArkDevObj);
    return status;
  }

  
  // ��ȡArk���������Ark�豸����
  g_pArkDrvObj = pDrvObj;
  g_pArkDevObj = pArkDevObj;


  KernelReload();
  KdPrint(("NtBase: %08X\n", (ULONG)GetNtImageBase(pDrvObj)));
  KdPrint(("�������سɹ�\r\n"));


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
  // ɾ����������
  UNICODE_STRING wZSymLinkName = RTL_CONSTANT_STRING(ARK_SYMLINK_NAME);
  IoDeleteSymbolicLink(&wZSymLinkName);

  // ɾ���豸����
  if (NULL != pDrvObj->DeviceObject)
  {
    IoDeleteDevice(pDrvObj->DeviceObject);
    g_pArkDevObj = NULL;
  }
  KdPrint(("����ж�سɹ�\r\n"));
}


NTSTATUS ArkIrpControl(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
  UNREFERENCED_PARAMETER(pDevObj);
  NTSTATUS           status = STATUS_SUCCESS; // ret
  PIO_STACK_LOCATION pIrpStack = NULL;        // irpջָ��
  ULONG				       ulIoControlCode = 0;     // CTL_CODE - R0-R3 ͨ����

  // ��ȡIrpջ��Ϣ
  pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
  // ��ȡ������
  ulIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;

  switch (ulIoControlCode)
  {
  // ͳ�ƽ��̸���
  case ArkCountProcess:
  {
    status = OnCountProcess(pIrp);
  }
    break;
  // ��ѯ������Ϣ
  case ArkQueryProcess:
  {
    status = OnQueryProcess(pIrp);
  }
    break;
  // ��ͣ����
  case ArkSuspendProcess:
  {
    status = OnSuspendProcess(pIrp);
  }
    break;
  // �ָ�����
  case ArkResumeProcess:
  {
    status = OnResumeProcess(pIrp);
  }
    break;
  // ��������
  case ArkKillProcess:
  {
    status = OnTerminateProcess(pIrp);
  }
    break;
  // ���ؽ���
  case ArkHideProcess:
  {
    status = OnHideProcess(pIrp);
  }
    break;
  // �̼߳��� - �����е��߳�
  case ArkCountThread:
  {
    status = OnCountThread(pIrp);
  }
    break;
  // ��ѯָ�����̵��߳���Ϣ
  case ArkQueryThread:
  {
    status = OnQueryThread(pIrp);
  }
    break;
  // ��ͣ�߳�
  case ArkSuspendThread:
  {
    status = OnSuSpendThread(pIrp);
  }
    break;
  // �ָ��߳�
  case ArkResumeThread:
  {
    status = OnResumeThread(pIrp);
  }
    break;
  // �����߳�
  case ArkKillThread:
  {
    status = OnKillThread(pIrp);
  }
    break;
  // ��ѯָ������ģ������
  case ArkCountModule:
  {
    status = OnCountModule(pIrp);
  }
    break;
  // ��ѯָ������ģ����Ϣ
  case ArkQueryModule:
  {
    status = OnQueryModule(pIrp);
  }
    break;
  // ��ѯ��������
  case ArkCountDriver:
  {
    status = OnCountDriver(pIrp);
  }
    break;
  // ��ѯ������Ϣ
  case ArkQueryDriver:
  {
    status = OnQueryDriver(pIrp);
  }
    break;
  // ��������
  case ArkHideDriver:
  {
    status = OnHideDriver(pIrp);
  }
    break;
  // ͳ��IDT����
  case ArkCountIDT:
  {
    status = OnCountIdt(pIrp);
  }
    break;
  // ��ѯIDT��Ϣ
  case ArkQueryIDT:
  {
    status = OnQueryIdt(pIrp);
  }
    break;
  // ͳ��GDT����
  case ArkCountGDT:
  {
    status = OnCountGDT(pIrp);
  }
    break;
  // ��ѯGDT��Ϣ
  case ArkQueryGDT:
  {
    status = OnQueryGDT(pIrp);
  }
    break;
  // ͳ��ָ��Ŀ¼���ļ�����
  case ArkCountFile:
  {
    status = OnCountFile(pIrp);
  }
    break;
  // ��ѯָ��Ŀ¼���ļ���Ϣ
  case ArkQueryFile:
  {
    status = OnQueryFile(pIrp);
  }
    break;
  // ɾ��ָ��Ŀ¼�µ��ļ�
  case ArkDeleteFile:
  {
    status = OnDelFile(pIrp);
  }
    break;
  // ��ѯSSDT����
  case ArkCountSSDT:
  {
    status = OnCountSSDT(pIrp);
  }
    break;
  // ��ѯSSDT��Ϣ
  case ArkQuerySSDT:
  {
    status = OnQuerySSDT(pIrp);
  }
    break;
  // SysEnter����
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
  // ��������
  default:
  {
    // Ĭ�Ϸ���R3��Ϣ
    pIrp->IoStatus.Information = 0;                       // ��������С
    pIrp->IoStatus.Status = STATUS_SUCCESS;               // R0����״̬
  }
    break;
  }

  // ���IO����
  IoCompleteRequest(pIrp, IO_NO_INCREMENT);
  return status;
}