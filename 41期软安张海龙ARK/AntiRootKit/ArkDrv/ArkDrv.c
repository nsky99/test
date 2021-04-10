#include "ArkDrv.h"
#include "ArkCommon.h"
#include "ArkDrvEntry.h"

// 计算驱动个数
ULONG ArkCountDriverNum()
{
  // 获取头节点
  PLDR_DATA_TABLE_ENTRY pSelf = g_pArkDrvObj->DriverSection;      // 自身
  PLDR_DATA_TABLE_ENTRY pHeader = (PLDR_DATA_TABLE_ENTRY)pSelf->InLoadOrderLinks.Flink; // 头节点

  PLDR_DATA_TABLE_ENTRY pCur = pHeader;
  ULONG ulDrvNum = (ULONG)-1; // 头节点不算
  // 获取驱动个数
  do
  {
    ulDrvNum++;
    // KdPrint(("%ws", pCur->BaseDllName.Buffer));
    pCur = (PLDR_DATA_TABLE_ENTRY)pCur->InLoadOrderLinks.Flink;
  } while (pCur != pHeader);

  return ulDrvNum;
}

// 响应Io 查询驱动个数
NTSTATUS OnCountDriver(PIRP pIrp)
{
  // 查询驱动个数
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ArkCountDriverNum();
  KdPrint(("%u", *(PULONG)pIrp->AssociatedIrp.SystemBuffer));
  // 返回
  pIrp->IoStatus.Information = sizeof(ULONG);
  pIrp->IoStatus.Status = STATUS_SUCCESS;
  return STATUS_SUCCESS;
}

// 响应Io 查询驱动信息
NTSTATUS OnQueryDriver(PIRP pIrp)
{
  do
  {
    // count driver
    ULONG ulDrvNum = ArkCountDriverNum();

    // allocate memory
    ULONG ulAllocateMemSize = sizeof(DRIVER_INFO) * ulDrvNum;
    PDRIVER_INFO pDrvInfo = (PDRIVER_INFO)ExAllocatePoolWithTag(PagedPool, ulAllocateMemSize, '.drv');
    if (!pDrvInfo)
    {
      break;
    }
    RtlFillMemory(pDrvInfo, ulAllocateMemSize, 0);
    
    // get drv info writ to memory
    // 获取头节点
    PLDR_DATA_TABLE_ENTRY pSelf = g_pArkDrvObj->DriverSection;      // 自身
    PLDR_DATA_TABLE_ENTRY pHeader = (PLDR_DATA_TABLE_ENTRY)pSelf->InLoadOrderLinks.Flink; // 头节点

    PLDR_DATA_TABLE_ENTRY pCur = pHeader;
    ULONG ulIndex = 0;
    // 获取驱动个数
    pCur = (PLDR_DATA_TABLE_ENTRY)pHeader->InLoadOrderLinks.Flink;


    do
    {

      // name
      RtlCopyMemory((pDrvInfo + ulIndex)->szName, pCur->BaseDllName.Buffer, pCur->BaseDllName.Length);
      // base
      (pDrvInfo + ulIndex)->uBase = (ULONG)pCur->DllBase;
      // size
      (pDrvInfo + ulIndex)->uSize = (ULONG)pCur->SizeOfImage;
      // drv obj
      //(pDrvInfo + ulIndex)->uDrvObj = (ULONG)pCur;       // ObReferenceObjectByName
      PVOID pobj = NULL;
      NTSTATUS status = ObReferenceObjectByName(&pCur->BaseDllName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL,
        FILE_ALL_ACCESS, *IoDriverObjectType, KernelMode, NULL, (PVOID*)&pobj);
      (pDrvInfo + ulIndex)->uDrvObj = (ULONG)pobj;
      status;
      // path
      RtlCopyMemory((pDrvInfo + ulIndex)->szPath, pCur->FullDllName.Buffer, pCur->FullDllName.Length);
      // 顺序
      (pDrvInfo + ulIndex)->uOrder = ulIndex + 1;
      ulIndex++;

      pCur = (PLDR_DATA_TABLE_ENTRY)pCur->InLoadOrderLinks.Flink;
    } while (pCur != pHeader);

    // copy memory to systembuffer
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pDrvInfo, ulAllocateMemSize);


    // free memory
    if (pDrvInfo)
    {
      ExFreePoolWithTag(pDrvInfo, '.drv');
      pDrvInfo = NULL;
    }

    // set ret r3 info
    pIrp->IoStatus.Information = ulAllocateMemSize;
    pIrp->IoStatus.Status = STATUS_SUCCESS;
  } while (FALSE);



  return STATUS_SUCCESS;
}

// 响应Io 隐藏驱动
NTSTATUS OnHideDriver(PIRP pIrp)
{
  // 获取缓冲区地址（输入输出是同一个）
  PVOID pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;


  // 获取需要隐藏的驱动名称
  UNICODE_STRING wZHideDriver = { 0 };
  RtlInitUnicodeString(&wZHideDriver, pIoBuffer);

  // 获取头节点
  PLDR_DATA_TABLE_ENTRY pSelf = g_pArkDrvObj->DriverSection;      // 自身
  PLDR_DATA_TABLE_ENTRY pCur = g_pArkDrvObj->DriverSection;       // 当前
   // 获取驱动个数
  do
  {
    // 找到相等的了
    if (0 == RtlCompareUnicodeString(&pCur->BaseDllName, &wZHideDriver, FALSE))
    {

      LIST_ENTRY pDel = pCur->InLoadOrderLinks;
      pDel.Blink->Flink = pDel.Flink;
      pDel.Flink->Blink = pDel.Blink;
      break;
    }
    pCur = (PLDR_DATA_TABLE_ENTRY)pCur->InLoadOrderLinks.Flink;
  } while (pCur != pSelf);

  pIrp->IoStatus.Information = 0;
  pIrp->IoStatus.Status = STATUS_SUCCESS;

  return STATUS_SUCCESS;
}
