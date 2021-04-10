#include "ArkSSDT.h"
#include "ArkCommon.h"


//导出系统服务描述符表SSDT的指针
extern  PServiceDescriptorTable  KeServiceDescriptorTable;


// 响应Io 查看SSDT数量
NTSTATUS OnCountSSDT(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;

  ULONG ulNum = (ULONG)KeServiceDescriptorTable->NumberOfServices;
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ulNum;
  // 返回
  pIrp->IoStatus.Status = status;
  pIrp->IoStatus.Information = sizeof(ULONG);
  return status;
}

// 响应Io 查看SSDT信息
NTSTATUS OnQuerySSDT(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;
  ULONG ulNum = (ULONG)KeServiceDescriptorTable->NumberOfServices;


  // 申请内存
  ULONG ulMemSize = sizeof(SSDT_INFO) * ulNum;
  PSSDT_INFO pSSDTInfo = (PSSDT_INFO)ExAllocatePoolWithTag(PagedPool, ulMemSize, 'ssdt');
  do
  {
    if (!pSSDTInfo)
    {
      status = STATUS_UNSUCCESSFUL;
      break;
    }
    RtlZeroMemory(pSSDTInfo, ulMemSize);
    PULONG pFunAddr = (PULONG)KeServiceDescriptorTable->ServiceTableBase;
    PUINT8 pArgsNum = (PUINT8)KeServiceDescriptorTable->ParamTableBase;

    for (ULONG i = 0; i < ulNum; i++)
    {
      (pSSDTInfo + i)->ulServerNum = i;
      (pSSDTInfo + i)->ulFunAddr = pFunAddr[i];
      (pSSDTInfo + i)->ulCountArgs = pArgsNum[i] / 4;
    }

    // 拷贝内存到systembuf
    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pSSDTInfo, ulMemSize);

    // 释放内存
    if (pSSDTInfo)
    {
      ExFreePoolWithTag(pSSDTInfo, 'ssdt');
      pSSDTInfo = NULL;
    }

  } while (FALSE);
  

  // 返回
  pIrp->IoStatus.Status = status;
  pIrp->IoStatus.Information = ulMemSize;
  return status;
}
