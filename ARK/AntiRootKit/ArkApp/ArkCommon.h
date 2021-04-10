#pragma once
#include <winioctl.h>

// 设备对象名称
#define ARK_DEVICE_NAME                  L"\\Device\\ArkDev"
#define ARK_SYMLINK_NAME                 L"\\\\.\\ArkDevLink"

// 简化CTL_CODE
#define ARK_IOCTL(Function) CTL_CODE(FILE_DEVICE_UNKNOWN, Function, METHOD_BUFFERED, FILE_ANY_ACCESS)

enum ENUM_ARK_IOCTL
{
  // 进程相关 0x800 - 0x80F
  ArkQueryProcess = ARK_IOCTL(0x800),		// 查询进程信息
  ArkCountProcess = ARK_IOCTL(0x801),		// 进程计数
  ArkSuspendProcess = ARK_IOCTL(0x802), // 暂停进程
  ArkResumeProcess = ARK_IOCTL(0x803),  // 恢复进程
  ArkKillProcess = ARK_IOCTL(0x804),    // 结束进程
  ArkHideProcess = ARK_IOCTL(0x805),    // 隐藏进程

  // 进程线程相关 0x810 - 0x81F
  ArkQueryThread = ARK_IOCTL(0x810),    // 查询线程信息
  ArkCountThread = ARK_IOCTL(0x811),    // 线程计数
  ArkSuspendThread = ARK_IOCTL(0x812),  // 暂停线程
  ArkResumeThread = ARK_IOCTL(0x813),   // 恢复线程
  ArkKillThread = ARK_IOCTL(0x814),     // 结束线程


  // 进程模块相关 0x820 - 0x82F
  ArkQueryModule = ARK_IOCTL(0x820),    // 查询模块信息
  ArkCountModule = ARK_IOCTL(0x821),    // 模块计数


  // 驱动相关 0x830 - 0x83F
  ArkQueryDriver = ARK_IOCTL(0x830),    // 查询驱动信息
  ArkCountDriver = ARK_IOCTL(0x831),    // 驱动计数
  ArkHideDriver = ARK_IOCTL(0x832),     // 隐藏驱动

  // 文件相关 0x840 - 0x84F
  ArkQueryFile = ARK_IOCTL(0x840),      // 查询指定目录文件
  ArkCountFile = ARK_IOCTL(0x841),      // 查询指定目录下文件数量
  ArkDeleteFile = ARK_IOCTL(0x842),     // 查询指定文件
  ArkCreateFile = ARK_IOCTL(0x843),     // 在指定目录下创建文件
  ArkDeleteDir = ARK_IOCTL(0x844),      // 删除指定目录

  // IDT相关操作 0x850 - 0x85F
  ArkCountIDT = ARK_IOCTL(0x850),       // 查询idt数量
  ArkQueryIDT = ARK_IOCTL(0x851),       // 查询idt信息

  // GDT相关操作 0x860 - 0x86F
  ArkCountGDT = ARK_IOCTL(0x860),       // 查询gdt数量
  ArkQueryGDT = ARK_IOCTL(0x861),       // 查询gdt信息


  // SSDT相关操作 0x870 - 0x87F
  ArkCountSSDT = ARK_IOCTL(0x870),       // 查询SSDT数量
  ArkQuerySSDT = ARK_IOCTL(0x871),       // 查询SSDT信息

  // SysEnter相关操作 0x880 - 0x88F
  ArkHookSysEnter = ARK_IOCTL(0x880),   // HookSysEnter
  ArkUnHookSysEnter = ARK_IOCTL(0x881), // UnHookSysEnter

  // 注册表相关
  ArkQueryReg = ARK_IOCTL(0x890),       // 获取注册表信息
  ArkDeleteReg = ARK_IOCTL(0x891),      // 删除注册表
  ArkCreateReg = ARK_IOCTL(0x892),      // 添加注册表信息
  ArkCounteReg = ARK_IOCTL(0x893),      // 统计注册表数量
};


// 驱动信息结构体
typedef struct _DRIVER_INFO
{
  // 驱动名
  WCHAR szName[0xFF];
  // 加载基址
  ULONG uBase;
  // 驱动大小
  ULONG uSize;
  // 驱动对象
  ULONG uDrvObj;
  // 驱动路径
  WCHAR szPath[0xFF];
  // 驱动加载顺序
  ULONG uOrder;
}DRIVER_INFO, * PDRIVER_INFO;


// 进程信息结构体
typedef struct _PROCESS_INFO
{
  // 进程名称
  WCHAR szName[0xFF];
  // 进程ID
  ULONG uPid;
  // 父进程ID
  ULONG uParentPid;
  // 进程路径
  WCHAR szPath[0xFF];
  // EPROCESS
  ULONG uEprocess;
}PROCESS_INFO, * PPROCESS_INFO;


// 线程信息结构体
typedef struct _THREAD_INFO
{
  // 线程ID
  ULONG uTid;
  // ETHREAD
  ULONG uEThread;
}THREAD_INFO, * PTHREAD_INFO;


// 模块信息结构体
typedef struct _MODULE_INFO
{
  // 模块名称
  WCHAR szName[0xFF];
  // 基址
  ULONG ulBase;
  // 大小
  ULONG ulSize;
  // 路径
  WCHAR szPath[0xFF];
}MODULE_INFO, * PMODULE_INFO;


// IDT表中描述符结构体
typedef struct _IDT_INFO
{
  // USHORT == UINT16
  USHORT uOffsetLow;      //0x0，低地址偏移
  USHORT uSelector;       //0x2，段选择器

  //USHORT uAccess;       //0x4
  UINT8 uReserved;        // 保留
  UINT8 GateType : 4;     // 中断类型
  UINT8 System : 1; // 0标识描述的是系统段
  UINT8 DPL : 2;          // 特权级
  UINT8 Present : 1;      // 如未使用中断可置为0

  USHORT uOffsetHigh;     // 高地址偏移
}IDT_INFO, * PIDT_INFO;


// GDT描述符信息
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
  UINT8 uiG : 1;// 粒度

  UINT8 uiHighBase;

}GDT_INFO, * PGDT_INFO;

// 文件结构体
typedef struct _FILE_INFO
{
  // 文件名称
  WCHAR szName[0xFF];
  // 文件大小
  ULONG64 ulSize;
  // 文件类型
  WCHAR szType[0xFF];
}FILE_INFO, * PFILE_INFO;



// SSDT 结构体信息
typedef struct _SSDT_INFO
{
  // 服务号
  ULONG ulServerNum;
  // 函数地址
  ULONG ulFunAddr;
  // 参数个数
  UINT8 ulCountArgs;
}SSDT_INFO, * PSSDT_INFO;


// 注册表信息
typedef struct _REGINF
{
  UINT8 RegType; // 子项还是值
  WCHAR KeyName[0xFF];
  WCHAR ValueName[0xFF];
  UINT8 ValueType;
  ULONG Value;
}REGINF, * PREGINF;
