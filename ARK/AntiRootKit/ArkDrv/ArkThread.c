#include "ArkThread.h"
#include "ArkCommon.h"    // Ark通用
#include "ArkDrvEntry.h"



// 获取指定进程的线程数量
ULONG ArkCountThreadByEProcess(PEPROCESS pEProcess)
{
  NTSTATUS status = STATUS_SUCCESS;
  PEPROCESS pCurEProc = NULL;
  PETHREAD pEThrd = NULL;

  ULONG ulThreadNum = 0;// 线程个数
  for (ULONG i = 25600; i > 0; i -= 4)
  {
    // 根据Tid获取ETHREAD
    status = PsLookupThreadByThreadId((HANDLE)i, &pEThrd);
    if (!NT_SUCCESS(status))
    {
      continue;
    }
    // 获取线程所属进程，相等则计数
    pCurEProc = IoThreadToProcess(pEThrd);
    if (pCurEProc == pEProcess)
    {
      ulThreadNum++;
      // KdPrint(("Tid = %u\n", i));
    }
    // 线程引用计数--
    ObDereferenceObject(pEThrd);
  }
  return ulThreadNum;
}

// 响应Io计数线程
NTSTATUS OnCountThread(PIRP pIrp)
{

  // 指定的进程 - R3 - in
  PEPROCESS pTheEproc = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;

  // 获取指定进程的线程数量
  ULONG ulThreadNum = ArkCountThreadByEProcess(pTheEproc);

  // R0 - out
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ulThreadNum;
  // KdPrint(("线程进程数量: %u\n", ulThreadNum));
  // 请求完成
  pIrp->IoStatus.Information = sizeof(ULONG);        // 返回内存大小
  pIrp->IoStatus.Status = STATUS_SUCCESS;            // 返回处理状态

  return STATUS_SUCCESS;
}

// 响应Io查询指定进程线程信息
NTSTATUS OnQueryThread(PIRP pIrp)
{
  do
  {
    // 1、获取进程中线程数量
    PEPROCESS pTheEproc = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;
    ULONG ulThreadNum = ArkCountThreadByEProcess(pTheEproc);

    // 2、为存储这些线程信息开辟空间
    ULONG ulAllocateMemSize = sizeof(THREAD_INFO) * ulThreadNum;
    PTHREAD_INFO pTrdInfo = (PTHREAD_INFO)ExAllocatePoolWithTag(PagedPool, ulAllocateMemSize, '.ark');
    if (NULL == pTrdInfo) // 申请内存失败
    {
      break;
    }
    RtlFillMemory(pTrdInfo, ulAllocateMemSize, 0);

    PEPROCESS pCurEProc = NULL;
    PETHREAD pEThrd = NULL;
    ULONG ulIndex = 0;
    for (ULONG i = 25600; i > 0; i -= 4)
    {
      // 根据Tid获取ETHREAD
      NTSTATUS status = PsLookupThreadByThreadId((HANDLE)i, &pEThrd);
      if (!NT_SUCCESS(status))
      {
        continue;
      }

      // 获取线程所属进程，相等则计数
      pCurEProc = IoThreadToProcess(pEThrd);
      if (pCurEProc == pTheEproc)
      {
        // Tid
        (pTrdInfo + ulIndex)->uTid = (ULONG)i;

        // EThread
        (pTrdInfo + ulIndex)->uEThread = (ULONG)pEThrd;

        ulIndex++;
      }
      // 线程引用计数--
      ObDereferenceObject(pEThrd);
    }

    // R0 out
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pTrdInfo, ulAllocateMemSize);

    // 释放内存
    if (pTrdInfo)
    {
      ExFreePoolWithTag(pTrdInfo, '.ark');
      pTrdInfo = NULL;
    }

    // 请求完成
    pIrp->IoStatus.Information = ulAllocateMemSize;    // 返回内存大小
    pIrp->IoStatus.Status = STATUS_SUCCESS;            // 返回处理状态
  } while (FALSE);


  return STATUS_SUCCESS;
}


// 响应Io暂停指定线程
NTSTATUS OnSuSpendThread(PIRP pIrp)
{
  // 1、获取进程中线程数量
  PETHREAD pETrd = *(PETHREAD*)pIrp->AssociatedIrp.SystemBuffer;

  NTSTATUS status = STATUS_SUCCESS;
  do
  {
    // 获取为导出函数地址
    FPsSuspendThread  PsSuspendThread = (FPsSuspendThread)((ULONG)GetNtImageBase(g_pArkDrvObj) + 0x296EFD);
    ULONG ulSuspendCount = 0;
    KdPrint(("PsSuspendThread:%08X\n", (ULONG)PsSuspendThread));
    status = PsSuspendThread(pETrd, &ulSuspendCount);
    // 请求完成
    pIrp->IoStatus.Information = 0;    // 返回内存大小
    pIrp->IoStatus.Status = status;    // 返回处理状态
  } while (FALSE);

  return status;
}

// 响应Io恢复指定线程
NTSTATUS OnResumeThread(PIRP pIrp)
{
  // 1、获取进程中线程数量
  PETHREAD pETrd = *(PETHREAD*)pIrp->AssociatedIrp.SystemBuffer;

  NTSTATUS status = STATUS_SUCCESS;
  do
  {
    // 获取为导出函数地址
    FZwResumeThread  ZwResumeThread = (FZwResumeThread)((ULONG)GetNtImageBase(g_pArkDrvObj) + 0x3CF14);
    FZwOpenThread ZwOpenThread = (FZwOpenThread)((ULONG)GetNtImageBase(g_pArkDrvObj) + 0x3C6CC);
    KdPrint(("FZwResumeThread:%08X\n", (ULONG)ZwResumeThread));
    KdPrint(("ZwOpenThread:%08X\n", (ULONG)ZwOpenThread));

    // 恢复线程
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

    // 请求完成
    pIrp->IoStatus.Information = 0;    // 返回内存大小
    pIrp->IoStatus.Status = status;    // 返回处理状态
  } while (FALSE);

  return status;
}

// 响应Io结束指定线程
NTSTATUS OnKillThread(PIRP pIrp)
{
  // 1、获取进程中线程数量
  PETHREAD pETrd = *(PETHREAD*)pIrp->AssociatedIrp.SystemBuffer;

  NTSTATUS status = STATUS_SUCCESS;
  
  do
  {
    // 获取为导出函数地址
    FZwTerminateThread  ZwTerminateThread = (FZwTerminateThread)((ULONG)GetNtImageBase(g_pArkDrvObj) + 0x3D450);
    FZwOpenThread ZwOpenThread = (FZwOpenThread)((ULONG)GetNtImageBase(g_pArkDrvObj) + 0x3C6CC);
    KdPrint(("ZwTerminateThread:%08X\n", (ULONG)ZwTerminateThread));
    KdPrint(("ZwOpenThread:%08X\n", (ULONG)ZwOpenThread));

    // 结束线程
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

    // 请求完成
    pIrp->IoStatus.Information = 0;    // 返回内存大小
    pIrp->IoStatus.Status = status;    // 返回处理状态
  } while (FALSE);

  return status;
}


