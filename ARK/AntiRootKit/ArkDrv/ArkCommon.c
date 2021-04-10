#include "ArkCommon.h"


// ��ȡ�ں�ģ���ַ
PVOID GetNtImageBase(PDRIVER_OBJECT pDriver)
{
  PLDR_DATA_TABLE_ENTRY pLdteHead;	// �ں�ģ������ͷ
  PLDR_DATA_TABLE_ENTRY pLdteCur;		// ����ָ��
  UNICODE_STRING usKrnlBaseDllName; // �ں�ģ����

  // Ҫ��ȡ��ַ��ģ������
  RtlInitUnicodeString(&usKrnlBaseDllName, L"ntoskrnl.exe");
  pLdteHead = (PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection;
  pLdteCur = pLdteHead;
  do
  {
    // �ҵ��� - �����������Ƿ���Դ�Сд
    if (RtlCompareUnicodeString(&pLdteCur->BaseDllName, &usKrnlBaseDllName, TRUE) == 0)
    {
      return pLdteCur->DllBase;
    }
    pLdteCur = (PLDR_DATA_TABLE_ENTRY)pLdteCur->InLoadOrderLinks.Flink;
  } while (pLdteHead != pLdteCur);
  return 0;
}