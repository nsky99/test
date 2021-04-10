#include "ArkCommon.h"


// 获取内核模块基址
PVOID GetNtImageBase(PDRIVER_OBJECT pDriver)
{
  PLDR_DATA_TABLE_ENTRY pLdteHead;	// 内核模块链表头
  PLDR_DATA_TABLE_ENTRY pLdteCur;		// 遍历指针
  UNICODE_STRING usKrnlBaseDllName; // 内核模块名

  // 要获取基址的模块名称
  RtlInitUnicodeString(&usKrnlBaseDllName, L"ntoskrnl.exe");
  pLdteHead = (PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection;
  pLdteCur = pLdteHead;
  do
  {
    // 找到了 - 第三个参数是否忽略大小写
    if (RtlCompareUnicodeString(&pLdteCur->BaseDllName, &usKrnlBaseDllName, TRUE) == 0)
    {
      return pLdteCur->DllBase;
    }
    pLdteCur = (PLDR_DATA_TABLE_ENTRY)pLdteCur->InLoadOrderLinks.Flink;
  } while (pLdteHead != pLdteCur);
  return 0;
}