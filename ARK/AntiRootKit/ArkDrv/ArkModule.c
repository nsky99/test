#include "ArkModule.h"
#include "ArkCommon.h"

#include <ntifs.h>


// 计数模块数量
ULONG ArkCountModuleByEProcess(PEPROCESS pEProcess)
{

  ULONG ulNum = 0;
  // 获取指定进程PEB
  ULONG Peb = *(PULONG)((ULONG)pEProcess + 0x1a8);
  // 当前线程切换到新的进程对象
  KeAttachProcess(pEProcess);


  // 获取LDR链
  LIST_ENTRY pLdrHead = ((PEB_LDR_DATA*)(*(PULONG)(Peb + 0xC)))->InLoadOrderModuleList;

  // 获取链表开始
  PLIST_ENTRY pTmp = pLdrHead.Flink;
  PLIST_ENTRY pNext = pLdrHead.Flink;


  do
  {
    // 获取模块信息
    LDR_DATA_TABLE_ENTRY pLdrTable = *(PLDR_DATA_TABLE_ENTRY)pNext;
    if (pLdrTable.DllBase)
    {
      // KdPrint(("Name:%ws\n", pLdrTable.BaseDllName.Buffer));
      // 统计
      ulNum++;
    }
    pNext = pNext->Flink;
  } while (pTmp != pNext);
  
  // 将线程转回原来的进程
  KeDetachProcess();
  // 引用计数--
  ObDereferenceObject(pEProcess);
  return ulNum;
}

// 响应Io统计指定进程模块数量
NTSTATUS OnCountModule(PIRP pIrp)
{
  // 取出EPROCESS
  PEPROCESS pEProcess = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;

  // 获取指定进程的线程数量
  ULONG ulModuleNum = ArkCountModuleByEProcess(pEProcess);

  // R0 - out
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ulModuleNum;
  // KdPrint(("线程模块数量: %u\n", ulThreadNum));
  // 请求完成
  pIrp->IoStatus.Information = sizeof(ULONG);        // 返回内存大小
  pIrp->IoStatus.Status = STATUS_SUCCESS;            // 返回处理状态

  return STATUS_SUCCESS;
}

// 响应Io查询指定进程模块信息
NTSTATUS OnQueryModule(PIRP pIrp)
{
  PEPROCESS pEProcess = *(PEPROCESS*)pIrp->AssociatedIrp.SystemBuffer;
  
  do
  {
    // 计算模块数量
    ULONG ulModuleNum = ArkCountModuleByEProcess(pEProcess);

    // 为存储信息分配内存
    ULONG ulAllocateMemSize = ulModuleNum * sizeof(MODULE_INFO);
    PMODULE_INFO pModuleInfo = ExAllocatePoolWithTag(PagedPool, ulAllocateMemSize, '.ark');
    if (!pModuleInfo)
    {
      break;
    }

    // 获取指定进程PEB
    ULONG Peb = *(PULONG)((ULONG)pEProcess + 0x1a8);
    // 当前线程切换到新的进程对象
    KeAttachProcess(pEProcess);


    // 获取LDR链
    LIST_ENTRY pLdrHead = ((PEB_LDR_DATA*)(*(PULONG)(Peb + 0xC)))->InLoadOrderModuleList;

    // 获取链表开始
    PLIST_ENTRY pTmp = pLdrHead.Flink;
    PLIST_ENTRY pNext = pLdrHead.Flink;

    ULONG ulIndex = 0;
    do
    {
      // 获取模块信息
      LDR_DATA_TABLE_ENTRY pLdrTable = *(PLDR_DATA_TABLE_ENTRY)pNext;
      if (pLdrTable.DllBase)
      {
        // KdPrint(("Name:%ws\n", pLdrTable.BaseDllName.Buffer));
        // 查询模块信息
        RtlCopyMemory((pModuleInfo + ulIndex)->szName, 
          pLdrTable.BaseDllName.Buffer, pLdrTable.BaseDllName.MaximumLength + 1);
        (pModuleInfo + ulIndex)->ulBase = (ULONG)pLdrTable.DllBase;
        (pModuleInfo + ulIndex)->ulSize = (ULONG)pLdrTable.SizeOfImage;
        RtlCopyMemory((pModuleInfo + ulIndex)->szPath, 
          pLdrTable.FullDllName.Buffer, pLdrTable.FullDllName.MaximumLength + 1);


        // 查询下一个
        ulIndex++;
      }
      pNext = pNext->Flink;
    } while (pTmp != pNext);

    // 将线程转回原来的进程
    KeDetachProcess();
    // 引用计数--
    ObDereferenceObject(pEProcess);

    // 拷贝内存到irp缓冲区
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pModuleInfo, ulAllocateMemSize);

    // 设置返回信息
    pIrp->IoStatus.Information = ulAllocateMemSize;
    pIrp->IoStatus.Status = STATUS_SUCCESS;

    // 释放内存
    if (pModuleInfo)
    {
      ExFreePoolWithTag(pModuleInfo, '.ark');
      pModuleInfo = NULL;
    }
  } while (FALSE);

  return STATUS_SUCCESS;
}
