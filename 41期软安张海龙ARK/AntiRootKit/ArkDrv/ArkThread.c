#include "ArkThread.h"
#include "ArkCommon.h"    // Arkͨ��
#include "ArkDrvEntry.h"



// ��ȡָ�����̵��߳�����
ULONG ArkCountThreadByEProcess(PEPROCESS pEProcess)
{
  NTSTATUS status = STATUS_SUCCESS;
  PEPROCESS pCurEProc = NULL;
  PETHREAD pEThrd = NULL;

  ULONG ulThreadNum = 0;// �̸߳���
  for (ULONG i = 25600; i > 0; i -= 4)
  {
    // ����Tid��ȡETHREAD
    status = PsLookupThreadByThreadId((HANDLE)i, &pEThrd);
    if (!NT_SUCCESS(status))
    {
      continue;
    }
    // ��ȡ�߳��������̣���������
    pCurEProc = IoThreadToProcess(pEThrd);
    if (pCurEProc == pEProcess)
    {
      ulThreadNum++;
      // KdPrint(("Tid = %u\n", i));
    }
    // �߳����ü���--
    ObDereferenceObject(pEThrd);
  }
  return ulThreadNum;
}

// ��ӦIo�����߳�
NTSTATUS OnCountThread(PIRP pIrp)
{

  // ָ���Ľ��� - R3 - in
  PEPROCESS pTheEproc = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;

  // ��ȡָ�����̵��߳�����
  ULONG ulThreadNum = ArkCountThreadByEProcess(pTheEproc);

  // R0 - out
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ulThreadNum;
  // KdPrint(("�߳̽�������: %u\n", ulThreadNum));
  // �������
  pIrp->IoStatus.Information = sizeof(ULONG);        // �����ڴ��С
  pIrp->IoStatus.Status = STATUS_SUCCESS;            // ���ش���״̬

  return STATUS_SUCCESS;
}

// ��ӦIo��ѯָ�������߳���Ϣ
NTSTATUS OnQueryThread(PIRP pIrp)
{
  do
  {
    // 1����ȡ�������߳�����
    PEPROCESS pTheEproc = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;
    ULONG ulThreadNum = ArkCountThreadByEProcess(pTheEproc);

    // 2��Ϊ�洢��Щ�߳���Ϣ���ٿռ�
    ULONG ulAllocateMemSize = sizeof(THREAD_INFO) * ulThreadNum;
    PTHREAD_INFO pTrdInfo = (PTHREAD_INFO)ExAllocatePoolWithTag(PagedPool, ulAllocateMemSize, '.ark');
    if (NULL == pTrdInfo) // �����ڴ�ʧ��
    {
      break;
    }
    RtlFillMemory(pTrdInfo, ulAllocateMemSize, 0);

    PEPROCESS pCurEProc = NULL;
    PETHREAD pEThrd = NULL;
    ULONG ulIndex = 0;
    for (ULONG i = 25600; i > 0; i -= 4)
    {
      // ����Tid��ȡETHREAD
      NTSTATUS status = PsLookupThreadByThreadId((HANDLE)i, &pEThrd);
      if (!NT_SUCCESS(status))
      {
        continue;
      }

      // ��ȡ�߳��������̣���������
      pCurEProc = IoThreadToProcess(pEThrd);
      if (pCurEProc == pTheEproc)
      {
        // Tid
        (pTrdInfo + ulIndex)->uTid = (ULONG)i;

        // EThread
        (pTrdInfo + ulIndex)->uEThread = (ULONG)pEThrd;

        ulIndex++;
      }
      // �߳����ü���--
      ObDereferenceObject(pEThrd);
    }

    // R0 out
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pTrdInfo, ulAllocateMemSize);

    // �ͷ��ڴ�
    if (pTrdInfo)
    {
      ExFreePoolWithTag(pTrdInfo, '.ark');
      pTrdInfo = NULL;
    }

    // �������
    pIrp->IoStatus.Information = ulAllocateMemSize;    // �����ڴ��С
    pIrp->IoStatus.Status = STATUS_SUCCESS;            // ���ش���״̬
  } while (FALSE);


  return STATUS_SUCCESS;
}


// ��ӦIo��ָͣ���߳�
NTSTATUS OnSuSpendThread(PIRP pIrp)
{
  // 1����ȡ�������߳�����
  PETHREAD pETrd = *(PETHREAD*)pIrp->AssociatedIrp.SystemBuffer;

  NTSTATUS status = STATUS_SUCCESS;
  do
  {
    // ��ȡΪ����������ַ
    FPsSuspendThread  PsSuspendThread = (FPsSuspendThread)((ULONG)GetNtImageBase(g_pArkDrvObj) + 0x296EFD);
    ULONG ulSuspendCount = 0;
    KdPrint(("PsSuspendThread:%08X\n", (ULONG)PsSuspendThread));
    status = PsSuspendThread(pETrd, &ulSuspendCount);
    // �������
    pIrp->IoStatus.Information = 0;    // �����ڴ��С
    pIrp->IoStatus.Status = status;    // ���ش���״̬
  } while (FALSE);

  return status;
}

// ��ӦIo�ָ�ָ���߳�
NTSTATUS OnResumeThread(PIRP pIrp)
{
  // 1����ȡ�������߳�����
  PETHREAD pETrd = *(PETHREAD*)pIrp->AssociatedIrp.SystemBuffer;

  NTSTATUS status = STATUS_SUCCESS;
  do
  {
    // ��ȡΪ����������ַ
    FZwResumeThread  ZwResumeThread = (FZwResumeThread)((ULONG)GetNtImageBase(g_pArkDrvObj) + 0x3CF14);
    FZwOpenThread ZwOpenThread = (FZwOpenThread)((ULONG)GetNtImageBase(g_pArkDrvObj) + 0x3C6CC);
    KdPrint(("FZwResumeThread:%08X\n", (ULONG)ZwResumeThread));
    KdPrint(("ZwOpenThread:%08X\n", (ULONG)ZwOpenThread));

    // �ָ��߳�
    HANDLE hTrd = NULL;
    CLIENT_ID clientId = { 0 };
    OBJECT_ATTRIBUTES objAttr = { sizeof(OBJECT_ATTRIBUTES) };
    clientId.UniqueProcess = 0;
    clientId.UniqueThread = PsGetThreadId(pETrd);
    ZwOpenThread(&hTrd, 1, &objAttr, &clientId);
    ULONG ulSuspendCount = 0;
    if (hTrd)
    {
      while (TRUE)
      {
        ZwResumeThread(hTrd, &ulSuspendCount);
        if (0 == ulSuspendCount)
        {
          break;
        }
      }
      ZwClose(hTrd);
    }

    // �������
    pIrp->IoStatus.Information = 0;    // �����ڴ��С
    pIrp->IoStatus.Status = status;    // ���ش���״̬
  } while (FALSE);

  return status;
}

// ��ӦIo����ָ���߳�
NTSTATUS OnKillThread(PIRP pIrp)
{
  // 1����ȡ�������߳�����
  PETHREAD pETrd = *(PETHREAD*)pIrp->AssociatedIrp.SystemBuffer;

  NTSTATUS status = STATUS_SUCCESS;
  
  do
  {
    // ��ȡΪ����������ַ
    FZwTerminateThread  ZwTerminateThread = (FZwTerminateThread)((ULONG)GetNtImageBase(g_pArkDrvObj) + 0x3D450);
    FZwOpenThread ZwOpenThread = (FZwOpenThread)((ULONG)GetNtImageBase(g_pArkDrvObj) + 0x3C6CC);
    KdPrint(("ZwTerminateThread:%08X\n", (ULONG)ZwTerminateThread));
    KdPrint(("ZwOpenThread:%08X\n", (ULONG)ZwOpenThread));

    // �����߳�
    HANDLE hTrd = NULL;
    CLIENT_ID clientId = { 0 };
    OBJECT_ATTRIBUTES objAttr = { sizeof(OBJECT_ATTRIBUTES) };
    clientId.UniqueProcess = 0;
    clientId.UniqueThread = PsGetThreadId(pETrd);
    ZwOpenThread(&hTrd, 1, &objAttr, &clientId);
    if (hTrd)
    {
      ZwTerminateThread(hTrd, 0);
      ZwClose(hTrd);
    }

    // �������
    pIrp->IoStatus.Information = 0;    // �����ڴ��С
    pIrp->IoStatus.Status = status;    // ���ش���״̬
  } while (FALSE);

  return status;
}


