#pragma once
#include <winioctl.h>

// �豸��������
#define ARK_DEVICE_NAME                  L"\\Device\\ArkDev"
#define ARK_SYMLINK_NAME                 L"\\\\.\\ArkDevLink"

// ��CTL_CODE
#define ARK_IOCTL(Function) CTL_CODE(FILE_DEVICE_UNKNOWN, Function, METHOD_BUFFERED, FILE_ANY_ACCESS)

enum ENUM_ARK_IOCTL
{
  // ������� 0x800 - 0x80F
  ArkQueryProcess = ARK_IOCTL(0x800),		// ��ѯ������Ϣ
  ArkCountProcess = ARK_IOCTL(0x801),		// ���̼���
  ArkSuspendProcess = ARK_IOCTL(0x802), // ��ͣ����
  ArkResumeProcess = ARK_IOCTL(0x803),  // �ָ�����
  ArkKillProcess = ARK_IOCTL(0x804),    // ��������
  ArkHideProcess = ARK_IOCTL(0x805),    // ���ؽ���

  // �����߳���� 0x810 - 0x81F
  ArkQueryThread = ARK_IOCTL(0x810),    // ��ѯ�߳���Ϣ
  ArkCountThread = ARK_IOCTL(0x811),    // �̼߳���
  ArkSuspendThread = ARK_IOCTL(0x812),  // ��ͣ�߳�
  ArkResumeThread = ARK_IOCTL(0x813),   // �ָ��߳�
  ArkKillThread = ARK_IOCTL(0x814),     // �����߳�


  // ����ģ����� 0x820 - 0x82F
  ArkQueryModule = ARK_IOCTL(0x820),    // ��ѯģ����Ϣ
  ArkCountModule = ARK_IOCTL(0x821),    // ģ�����


  // ������� 0x830 - 0x83F
  ArkQueryDriver = ARK_IOCTL(0x830),    // ��ѯ������Ϣ
  ArkCountDriver = ARK_IOCTL(0x831),    // ��������
  ArkHideDriver = ARK_IOCTL(0x832),     // ��������

  // �ļ���� 0x840 - 0x84F
  ArkQueryFile = ARK_IOCTL(0x840),      // ��ѯָ��Ŀ¼�ļ�
  ArkCountFile = ARK_IOCTL(0x841),      // ��ѯָ��Ŀ¼���ļ�����
  ArkDeleteFile = ARK_IOCTL(0x842),     // ��ѯָ���ļ�
  ArkCreateFile = ARK_IOCTL(0x843),     // ��ָ��Ŀ¼�´����ļ�
  ArkDeleteDir = ARK_IOCTL(0x844),      // ɾ��ָ��Ŀ¼

  // IDT��ز��� 0x850 - 0x85F
  ArkCountIDT = ARK_IOCTL(0x850),       // ��ѯidt����
  ArkQueryIDT = ARK_IOCTL(0x851),       // ��ѯidt��Ϣ

  // GDT��ز��� 0x860 - 0x86F
  ArkCountGDT = ARK_IOCTL(0x860),       // ��ѯgdt����
  ArkQueryGDT = ARK_IOCTL(0x861),       // ��ѯgdt��Ϣ


  // SSDT��ز��� 0x870 - 0x87F
  ArkCountSSDT = ARK_IOCTL(0x870),       // ��ѯSSDT����
  ArkQuerySSDT = ARK_IOCTL(0x871),       // ��ѯSSDT��Ϣ

  // SysEnter��ز��� 0x880 - 0x88F
  ArkHookSysEnter = ARK_IOCTL(0x880),   // HookSysEnter
  ArkUnHookSysEnter = ARK_IOCTL(0x881), // UnHookSysEnter

  // ע������
  ArkQueryReg = ARK_IOCTL(0x890),       // ��ȡע�����Ϣ
  ArkDeleteReg = ARK_IOCTL(0x891),      // ɾ��ע���
  ArkCreateReg = ARK_IOCTL(0x892),      // ���ע�����Ϣ
  ArkCounteReg = ARK_IOCTL(0x893),      // ͳ��ע�������
};


// ������Ϣ�ṹ��
typedef struct _DRIVER_INFO
{
  // ������
  WCHAR szName[0xFF];
  // ���ػ�ַ
  ULONG uBase;
  // ������С
  ULONG uSize;
  // ��������
  ULONG uDrvObj;
  // ����·��
  WCHAR szPath[0xFF];
  // ��������˳��
  ULONG uOrder;
}DRIVER_INFO, * PDRIVER_INFO;


// ������Ϣ�ṹ��
typedef struct _PROCESS_INFO
{
  // ��������
  WCHAR szName[0xFF];
  // ����ID
  ULONG uPid;
  // ������ID
  ULONG uParentPid;
  // ����·��
  WCHAR szPath[0xFF];
  // EPROCESS
  ULONG uEprocess;
}PROCESS_INFO, * PPROCESS_INFO;


// �߳���Ϣ�ṹ��
typedef struct _THREAD_INFO
{
  // �߳�ID
  ULONG uTid;
  // ETHREAD
  ULONG uEThread;
}THREAD_INFO, * PTHREAD_INFO;


// ģ����Ϣ�ṹ��
typedef struct _MODULE_INFO
{
  // ģ������
  WCHAR szName[0xFF];
  // ��ַ
  ULONG ulBase;
  // ��С
  ULONG ulSize;
  // ·��
  WCHAR szPath[0xFF];
}MODULE_INFO, * PMODULE_INFO;


// IDT�����������ṹ��
typedef struct _IDT_INFO
{
  // USHORT == UINT16
  USHORT uOffsetLow;      //0x0���͵�ַƫ��
  USHORT uSelector;       //0x2����ѡ����

  //USHORT uAccess;       //0x4
  UINT8 uReserved;        // ����
  UINT8 GateType : 4;     // �ж�����
  UINT8 System : 1; // 0��ʶ��������ϵͳ��
  UINT8 DPL : 2;          // ��Ȩ��
  UINT8 Present : 1;      // ��δʹ���жϿ���Ϊ0

  USHORT uOffsetHigh;     // �ߵ�ַƫ��
}IDT_INFO, * PIDT_INFO;


// GDT��������Ϣ
typedef struct _GDT_INFO
{
  UINT16 uiLowLimit;
  UINT16 uiLowBase1;
  UINT8  uiLowBase2;

  UINT8 uiType : 4;// type
  UINT8 uiS : 1; // system
  UINT8 uiDPL : 2;
  UINT8 uiP : 1; // Present

  UINT8 ulHighLimit : 4;
  UINT8 uiAVL : 1;
  UINT8 uiL : 1;
  UINT8 uiD_B : 1;
  UINT8 uiG : 1;// ����

  UINT8 uiHighBase;

}GDT_INFO, * PGDT_INFO;

// �ļ��ṹ��
typedef struct _FILE_INFO
{
  // �ļ�����
  WCHAR szName[0xFF];
  // �ļ���С
  ULONG64 ulSize;
  // �ļ�����
  WCHAR szType[0xFF];
}FILE_INFO, * PFILE_INFO;



// SSDT �ṹ����Ϣ
typedef struct _SSDT_INFO
{
  // �����
  ULONG ulServerNum;
  // ������ַ
  ULONG ulFunAddr;
  // ��������
  UINT8 ulCountArgs;
}SSDT_INFO, * PSSDT_INFO;


// ע�����Ϣ
typedef struct _REGINF
{
  UINT8 RegType; // �����ֵ
  WCHAR KeyName[0xFF];
  WCHAR ValueName[0xFF];
  UINT8 ValueType;
  ULONG Value;
}REGINF, * PREGINF;
