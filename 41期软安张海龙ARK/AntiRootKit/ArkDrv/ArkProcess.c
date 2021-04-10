#include "ArkProcess.h"
#include "ArkCommon.h"

// 获取进程数量
ULONG ArkGetProcessNum()
{
  NTSTATUS status = STATUS_SUCCESS;
  // 进程块
  PEPROCESS pProc = NULL;
  // 获取进程个数
  ULONG uProcessNum = 0;       // 进程个数
  for (ULONG i = 90000; i > 0; i -= 4)
  {
    // 尝试获取进程EPROCESS
    status = PsLookupProcessByProcessId((HANDLE)i, &pProc);
    if (!NT_SUCCESS(status))
    {
      continue;
    }

    uProcessNum++;
    // 使用了对象就会引用计数+1,
    ObDereferenceObject(pProc);
  }
  return uProcessNum;
}

// 结束进程
NTSTATUS ArkTerminateProcess(ULONG ulPid)
{
  HANDLE hProcess = NULL;
  CLIENT_ID ClientId = { 0 };
  OBJECT_ATTRIBUTES objAttr = { sizeof(OBJECT_ATTRIBUTES) };
  ClientId.UniqueProcess = (HANDLE)ulPid;
  ClientId.UniqueThread = 0;

  // 打开进程，如果句柄有效，则结束进程
  NTSTATUS status = ZwOpenProcess(&hProcess, 1, &objAttr, &ClientId);
  if (hProcess)
  {
    ZwTerminateProcess(hProcess, 0);
    ZwClose(hProcess);
  }
  return status;
}

// 响应统计进程Io
NTSTATUS OnCountProcess(PIRP pIrp)
{
  // 获取进程个数
  ULONG uProcessNum = ArkGetProcessNum();

  // 返回进程个数
  RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, &uProcessNum, sizeof(ULONG));

  // 请求完成
  pIrp->IoStatus.Information = sizeof(ULONG);        // 返回内存大小
  pIrp->IoStatus.Status = STATUS_SUCCESS;            // 返回处理状态

  return STATUS_SUCCESS;
}

// 响应查询进程Io
NTSTATUS OnQueryProcess(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;
  // 进程块
  PEPROCESS pProc = NULL;

  // 获取进程个数
  ULONG ulProcessNum = ArkGetProcessNum();
  
  // 为存储信息申请内存
  ULONG ulAllocateMemSize = sizeof(PROCESS_INFO) * ulProcessNum;
  PPROCESS_INFO pProcInfo = ExAllocatePoolWithTag(PagedPool, ulAllocateMemSize, '.ark');
  if (NULL == pProcInfo)
  {
    return STATUS_SUCCESS;// 申请内存失败
  }
  RtlFillMemory(pProcInfo, ulAllocateMemSize, 0);

  // 查询并存储进程信息
  ULONG ulIndex = 0;
  for (ULONG i = 90000; i > 0; i -= 4)
  {
    // 尝试获取进程EPROCESS
    status = PsLookupProcessByProcessId((HANDLE)i, &pProc);
    if (!NT_SUCCESS(status))
    {
      continue;
    }

    // name
    CANSI_STRING aZProcName = { 0 };
    UNICODE_STRING wZProcName = { 0 };
    RtlInitAnsiString(&aZProcName, (PCSZ)PsGetProcessImageFileName(pProc));
    RtlAnsiStringToUnicodeString(&wZProcName, &aZProcName, TRUE);

    RtlCopyMemory((pProcInfo + ulIndex)->szName, wZProcName.Buffer, wZProcName.MaximumLength);

    // pid
    (pProcInfo + ulIndex)->uPid = (ULONG)PsGetProcessId(pProc);

    // Ppid
    (pProcInfo + ulIndex)->uParentPid = (ULONG)PsGetProcessInheritedFromUniqueProcessId(pProc);

    // 进程路径


    // EPROCESS
    (pProcInfo + ulIndex)->uEprocess = (ULONG)pProc;

    // 遍历到进程, 下标++
    ulIndex++;
    // 使用了对象就会引用计数+1,
    ObDereferenceObject(pProc);
  }


  // 将查询好的R0数据拷贝到R3缓冲区中
  RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pProcInfo, ulAllocateMemSize);

  // 释放内存
  if (pProcInfo)
  {
    ExFreePoolWithTag(pProcInfo, '.ark');
    pProcInfo = NULL;
  }

  // 请求完成设置返回信息
  pIrp->IoStatus.Information = ulAllocateMemSize;
  pIrp->IoStatus.Status = STATUS_SUCCESS;


  return STATUS_SUCCESS;
}

// 响应暂停进程Io
NTSTATUS OnSuspendProcess(PIRP pIrp)
{
  PEPROCESS pProc = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;
  
  // 暂停进程
  NTSTATUS status = PsSuspendProcess(pProc);

  // 请求完成设置返回信息
  pIrp->IoStatus.Information = 0;
  pIrp->IoStatus.Status = status;
  return status;
}

// 响应恢复进程Io
NTSTATUS OnResumeProcess(PIRP pIrp)
{
  PEPROCESS pProc = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;

  NTSTATUS status = PsResumeProcess(pProc);

  // 请求完成设置返回信息
  pIrp->IoStatus.Information = 0;
  pIrp->IoStatus.Status = status;
  return status;
}

// 响应结束进程Io
NTSTATUS OnTerminateProcess(PIRP pIrp)
{
  ULONG ulPid = *(PULONG)pIrp->AssociatedIrp.SystemBuffer;

  NTSTATUS status = ArkTerminateProcess(ulPid);

  // 请求完成设置返回信息
  pIrp->IoStatus.Information = 0;
  pIrp->IoStatus.Status = status;
  return status;
}

// 响应隐藏进程Io
NTSTATUS OnHideProcess(PIRP pIrp)
{
  // 要隐藏的进程EPROCESS
  PEPROCESS pHideProc = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;
  
  // 获取当前进程EPROCESS
  PEPROCESS pCurProc = PsGetCurrentProcess();
  
  // 获取进程对象内的当前活动进程链表pCurProc->ActiveProcessLinks 
  LIST_ENTRY* pNowList = (LIST_ENTRY*)((ULONG)pCurProc + 0xb8);

  // 临时链表
  LIST_ENTRY* pTempList = pNowList;
  // 遍历链表
  while (pNowList != pTempList->Flink)
  {
    // 当前相对进程对象偏移是0xb8，需要减去
    // 对比是否为目标进程			
    // EPROCESS相等
    if (pHideProc == (PEPROCESS)((ULONG)pTempList - 0xb8))
    {
      //把找到的进程从链表中删除
      (pTempList->Blink)->Flink = pTempList->Flink;
      (pTempList->Flink)->Blink = pTempList->Blink;

      pTempList->Flink = pTempList->Blink = NULL;
      return STATUS_SUCCESS;
      break;
    }
    pTempList = pTempList->Flink;
  }
  return 1; // 没有找到
}

