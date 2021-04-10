#pragma once
#include <ntifs.h>

typedef struct _ServiceDescriptorTable
{
  //System Service Dispatch Table�Ļ���ַ
  PVOID ServiceTableBase;
  //SSDT��ÿ�����񱻵��ô����ļ����������������һ����sysenter ���¡�
  PVOID ServiceCounterTable;
  //�� ServiceTableBase �����ķ������Ŀ��
  unsigned int NumberOfServices;
  //ÿ��ϵͳ��������ֽ�����Ļ���ַ-ϵͳ���������SSPT 
  PVOID ParamTableBase;
}ServiceDescriptorTable,*PServiceDescriptorTable;

// ��ӦIo �鿴SSDT����
NTSTATUS OnCountSSDT(PIRP pIrp);

// ��ӦIo �鿴SSDT��Ϣ
NTSTATUS OnQuerySSDT(PIRP pIrp);

