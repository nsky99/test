#include "CDbg.h"
#include <stdio.h>
#include <locale>
#include "CBp.h"
#include "Reg.h"
#include <TlHelp32.h>
#include <conio.h>
#include <winternl.h>
#pragma comment(lib, "ntdll.lib")
#include <DbgHelp.h>
#pragma  comment (lib,"dbghelp.lib")
#include "CPlugin.h"  // 插件
#include "CPE.h"// PE

// 反汇编引擎
#include "./capstone/include/capstone.h"
#ifdef _WIN64 // 64位平台编译器会自动定义这个宏
#pragma comment(lib, "capstone/lib/capstone_x64.lib")
#else
#pragma comment(lib,"capstone/lib/capstone_x86.lib")
#endif // _64

//反汇编引擎
//#define BEA_ENGINE_STATIC
//#define BEA_USE_STDCALL
//#include "BeaEngine_4.1\\Win32\\headers\\BeaEngine.h"
//#ifdef _WIN64
//#pragma comment(lib,"BeaEngine_4.1\\Win64\\Win64\\Lib\\BeaEngine.lib")
//#else
//#pragma comment(lib,"BeaEngine_4.1\\Win32\\Win32\\Lib\\BeaEngine.lib")
//#endif // _WIN32
//#pragma comment(linker, "/NODEFAULTLIB:\"crt.lib\"")
//#pragma comment(lib, "legacy_stdio_definitions.lib")

//汇编引擎
#include "XEDParse/XEDParse.h"
#ifdef _WIN64
#pragma comment (lib,"XEDParse/x64/XEDParse_x64.lib")
#else
#pragma comment (lib,"XEDParse/x86/XEDParse_x86.lib")
#endif // _WIN64

// 当前调试的进程和线程句柄
static bool   g_isAttach = false;                // 是否是附加调试
static HANDLE g_dbgProcess;                      // 当前调试进程句柄
static HANDLE g_dbgThread;                       // 当前调试线程句柄
static HANDLE g_dbgSymbol;                       //符号进程句柄
static DWORD  g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED; // 当前调试状态，默认调试器没有处理
static DEBUG_EVENT g_dbgEvent;                   // 当前调试事件
static LPVOID g_lpOep;                           // 调试进程的OEP
static LPVOID g_lpImageBase;                     // 调试进程的镜像基址
static bool   g_bdbgIsFirstBP = true;            // 调试器是否第一次断下-----第一次断于系统领空
static bool   g_bIsInput = false;                // 是否获取用户输入命令
static bool   g_isStepIn = false;                // 是否是单步步入
static bool   g_isCCBP = false;
static LPVOID g_lpCCBPAddr = 0;
static bool   g_isHardBP = false;
static bool   g_isHdPer = false;
static LPVOID g_HDPerAddr = 0;
static BYTE   g_HDType = 0; // 硬件断点的类型
static bool   g_isStepOver = false;// 
static bool   g_isMemBP = false;// 内存断点是否设置
static LPVOID g_lpMemAddr = 0;  // 命中内存断点的地址
static bool   g_isTFMemBP = false;
static bool   g_isCondition = false;// 是否是条件断点
static DWORD  g_condValue = 0;
static DWORD  g_condAddr = 0;
static DWORD  g_DbgPid = 0;
static vector<SYMLOADINFO> g_vecSymBase;
// 调试器主循环
void MainLoop()
{
  char psz[0xFF] = { ".\\ExceptionTest1.exe" };
  //OpenDbgProcess(psz);
  ////typedef struct _DEBUG_EVENT {
  ////  DWORD dwDebugEventCode;
  ////  DWORD dwProcessId;
  ////  DWORD dwThreadId;
  ////  union {
  ////    EXCEPTION_DEBUG_INFO Exception;
  ////    CREATE_THREAD_DEBUG_INFO CreateThread;
  ////    CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
  ////    EXIT_THREAD_DEBUG_INFO ExitThread;
  ////    EXIT_PROCESS_DEBUG_INFO ExitProcess;
  ////    LOAD_DLL_DEBUG_INFO LoadDll;
  ////    UNLOAD_DLL_DEBUG_INFO UnloadDll;
  ////    OUTPUT_DEBUG_STRING_INFO DebugString;
  ////    RIP_INFO RipInfo;
  ////  } u;
  ////} DEBUG_EVENT, * LPDEBUG_EVENT;

  //// CREATE_PROCESS_DEBUG_EVENT
  //// 创建进程之后发送此类调试事件，这是调试器收到的第一个调试事件。
  //// 
  //// CREATE_THREAD_DEBUG_EVENT
  //// 创建一个线程之后发送此类调试事件。
  //// 
  //// EXCEPTION_DEBUG_EVENT
  //// 发生异常时发送此类调试事件。
  //// 
  //// EXIT_PROCESS_DEBUG_EVENT
  //// 进程结束后发送此类调试事件。
  //// 
  //// EXIT_THREAD_DEBUG_EVENT
  //// 一个线程结束后发送此类调试事件。
  //// 
  //// LOAD_DLL_DEBUG_EVENT
  //// 装载一个DLL模块之后发送此类调试事件。
  //// 
  //// OUTPUT_DEBUG_STRING_EVENT
  //// 被调试进程调用OutputDebugString之类的函数时发送此类调试事件。
  //// 
  //// RIP_EVENT
  //// 发生系统调试错误时发送此类调试事件。
  //// 
  //// UNLOAD_DLL_DEBUG_EVENT
  //// 卸载一个DLL模块之后发送此类调试事件。

  //DbgEventLoop();


  bool isRun = true;
  while (isRun)
  {
    g_bdbgIsFirstBP = true;
    system("cls");
    printf("=====Welcome to DbgDebug=====\r\n");
    printf("1.Open\r\n2.Attach\r\n3.exit\r\n");
    switch (_getch())
    {
      // 打开调试进程
    case OPEN:
    {
      printf("请输入要调试的程序路径>>");
      scanf_s("%s", psz, 0xFF);
      g_isAttach = false;
      InitPE(psz);
      OpenDbgProcess(psz);
      DbgEventLoop();
    }
    break;
    // 附加调试进程
    case ATTACH:
    {
      // 获取进程列表
      vector<DBGPROCESSINFO> vecPInfo;
      if (!GetProcessList(vecPInfo))
      {
        printf("获取进程信息失败\r\n");
      }
      // 输出进程列表
      for (size_t i = 0; i < vecPInfo.size(); i++)
      {
        printf("%08d\t%s\r\n", vecPInfo[i].dwPid, vecPInfo[i].strProcessName.c_str());
      }

      // 获取要附加调试的进程
      DWORD dwPid = 0;
      printf("请输入要附加的进程ID: ");
      scanf_s("%d", &dwPid);
      g_isAttach = true;
      AttachDbgProcess(dwPid);
      DbgEventLoop();
    }
    break;

    // 退出调试器
    case EXIT:
      isRun = false;
      break;

    default:
      break;
    }
  }
}

// 打开调试进程
bool OpenDbgProcess(char* pszFile)
{
  // 判断进程路径是否为空
  if (nullptr == pszFile)
  {
    return false;
  }

  // 启动信息和进程信息
  STARTUPINFOA si = { sizeof(STARTUPINFOA) };
  PROCESS_INFORMATION pi = { 0 };

  // 创建调试进程
  BOOL bRet = CreateProcessA(
    pszFile,                                      // 可执行程序路径名
    NULL,                                         // 命令行
    NULL,                                         // 进程属性
    NULL,                                         // 线程属性
    FALSE,                                        // 线程是否可以被继承
    DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE, // 以调试方式启动
    NULL,                                         // 新进程环境块
    NULL,                                         // 新进程的工作目录(当前目录)
    &si,                                          // 启动信息
    &pi                                           // 接受新进程的信息
  );

  g_DbgPid = pi.dwProcessId;
  // 关闭不使用的句柄
  g_dbgSymbol = pi.hProcess;
  //if (pi.hProcess)
  //{
  //  CloseHandle(pi.hProcess);
  //}
  if (pi.hThread)
  {
    CloseHandle(pi.hThread);
  }
  return (bool)bRet;
}

// 附加调试进程
bool AttachDbgProcess(DWORD dwPid)
{
  // 调试过程具有授予SE_DEBUG_NAME权限，它可以调试任何进程。
  return DebugActiveProcess(dwPid);
}

// 调试事件循环
DWORD DbgEventLoop()
{
  bool isLoop = true;
  g_dbgEvent = { 0 };

  g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
  // 等待调试事件
  while (isLoop && WaitForDebugEvent(&g_dbgEvent, -1))
  {
    // 打开当前调试的事件句柄
    g_dbgProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_dbgEvent.dwProcessId);
    g_dbgThread = OpenThread(THREAD_ALL_ACCESS, FALSE, g_dbgEvent.dwThreadId);

    // 调试事件分发
    switch (g_dbgEvent.dwDebugEventCode)
    {
    case  EXCEPTION_DEBUG_EVENT:
    {
      OnExceptionDbgEvent(g_dbgEvent.u.Exception);
      // 分发异常事件
      break;
    }

    case CREATE_THREAD_DEBUG_EVENT:
    {
      OnCreateThreadDbgEvent(g_dbgEvent.u.CreateThread);
      break;
    }

    case EXIT_THREAD_DEBUG_EVENT:
    {
      OnExitThreadDbgEvent(g_dbgEvent.u.ExitThread);
      break;
    }

    case  CREATE_PROCESS_DEBUG_EVENT:// 保存OEP和可执行文件映像
    {
      OnCreateProcessDbgEvent(g_dbgEvent.u.CreateProcessInfo);
      break;
    }

    case EXIT_PROCESS_DEBUG_EVENT:
    {
      isLoop = false;
      printf("%08d 进程退出 退出码: %d\n", g_dbgEvent.dwProcessId, g_dbgEvent.u.ExitProcess.dwExitCode);
      break;
    }

    case LOAD_DLL_DEBUG_EVENT:
    {
      OnLoadDllDbgEvent(g_dbgEvent.u.LoadDll);
      break;
    }

    case UNLOAD_DLL_DEBUG_EVENT:
    {
      OnUnLoadDllDbgEvent(g_dbgEvent.u.UnloadDll);
      break;
    }

    case RIP_EVENT:
    {
      printf("“系统错误”或者“内部错误”\n");
      break;
    }


    default:
      break;
    }
    ContinueDebugEvent(g_dbgEvent.dwProcessId, g_dbgEvent.dwThreadId, g_dwContinueStatus);

    // 关闭当前调试的事件句柄
    CloseHandle(g_dbgProcess);
    CloseHandle(g_dbgThread);
  }
  return g_dwContinueStatus;
}

// 响应异常调试事件
void OnExceptionDbgEvent(EXCEPTION_DEBUG_INFO exceptDbgInfo)
{
  /*
  异常分类：
  可修复：错误异常->EIP，陷阱异常->EIP-1，
  不可修复：终止异常
    错误异常恢复执行时，是从引发异常的那条指令开始执行；
    陷阱异常是从引发异常那条指令的下一条指令开始执行
  硬件异常：由CPU引发的异常
  软件异常：程序调用RaiseException函数引发的异常
    硬件异常和软件异常都可以通过Windows提供的结构化异常处理机制来捕捉和处理,这种处理机制可以让程序在发生异常的地方继续执行，或者转到异常处理块内执行。
    C++提供的异常处理机制只能捕捉和处理由throw语句抛出的异常，简单地说，这是通过检查异常代码是否0xE06D7363来决定的。
  另外，C++的异常处理机制只能转到异常处理块中执行，而不能在异常发生的地方继续执行
  */

  /*
  * 异常的分发：
  * 1、.异常未被处理，程序因“应用程序错误”退出。
  * 2、异常被调试器处理了，程序在发生异常的地方继续执行（具体取决于是错误异常还是陷阱异常）。
  * 3、异常被程序内的异常处理器处理了，程序在发生异常的地方继续执行，或者转到异常处理块内继续执行。
  *
  * 异常分发的过程：
  * 1.程序发生了一个异常，Windows捕捉到这个异常，并转入内核态执行。
  * 2.Windows检查发生异常的程序是否正在被调试，如果是，则发送一个EXCEPTION_DEBUG_EVENT调试事件给调试器，这是调试器第一次收到该事件；如果否，则跳到第4步。
  * 3.调试器收到异常调试事件之后，如果在调用ContinueDebugEvent时第三个参数为DBG_CONTINUE，即表示调试器已处理了该异常，程序在发生异常的地方继续执行，
  *   异常分发结束；如果第三个参数为DBG_EXCEPTION_NOT_HANDLED，即表示调试器没有处理该异常，跳到第4步。
  * 4.Windows转回到用户态中执行，寻找可以处理该异常的异常处理器。如果找到，则进入异常处理器中执行，然后根据执行的结果继续程序的执行，异常分发结束；
  *  如果没找到，则跳到第5步。
  * 5.Windows又转回内核态中执行，再次检查发生异常的程序是否正在被调试，如果是，则再次发送一个EXCEPTION_DEBUG_EVENT调试事件给调试器，
  *   这是调试器第二次收到该事件；如果否，跳到第7步。
  * 6.调试器第二次处理该异常，如果调用ContinueDebugEvent时第三个参数为DBG_CONTINUE，程序在发生异常的地方继续执行，异常分发结束；
  *   如果第三个参数为DBG_EXCEPTION_NOT_HANDLED，跳到第7步。
  * 7.异常没有被处理，程序以“应用程序错误”结束。
  * DBG_CONTINUE：调试器处理了异常
  * DBG_EXCEPTION_NOT_HANDLED：调试器没有处理异常 -> 转到用户态寻找异常处理器
  */

  // 判断是否是第一次发生异常。第一次断于系统领空
  exceptDbgInfo.ExceptionRecord.ExceptionFlags;
  exceptDbgInfo.ExceptionRecord.NumberParameters;
  exceptDbgInfo.dwFirstChance; // 同一个异常断下第二次设置为0
  g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;// 默认调试器没有处理异常
  DWORD dwExceptCode = exceptDbgInfo.ExceptionRecord.ExceptionCode;
  PVOID lpExceptAddr = exceptDbgInfo.ExceptionRecord.ExceptionAddress;



  switch (dwExceptCode)
  {
  case EXCEPTION_ACCESS_VIOLATION:// 访问异常 
  {
    LPVOID lpAddr = (LPVOID)exceptDbgInfo.ExceptionRecord.ExceptionInformation[1];
    if (g_isMemBP == false && lpAddr != 0)
    {
      if (g_lpMemAddr != lpAddr)
      {
        DisableMemBP(g_dbgProcess, g_lpMemAddr);
        StepIn();
        g_isTFMemBP = true;
        g_bIsInput = false;
        g_dwContinueStatus = DBG_CONTINUE;
        return;
      }
      else
      {
        g_isTFMemBP = false;
        g_isMemBP = true;
      }
    }
    if (g_isMemBP)           // 内存断点
    {
      g_isMemBP = false;
      g_bIsInput = true;
      DbgDisasm(lpExceptAddr, 5);
      // g_lpMemAddr = lpAddr;
      DisableMemBP(g_dbgProcess, lpAddr);
      StepIn();
      g_isTFMemBP = true;
      g_dwContinueStatus = DBG_CONTINUE;
    }
    else                      // 用户程序异常
    {
      g_bIsInput = false;
      g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
    }
    break;
  }

  case EXCEPTION_BREAKPOINT: // 断点异常
  {
    // 调试器不处理第一次异常
    OnExceptionBreakPoint(lpExceptAddr);
    break;
  }

  case EXCEPTION_SINGLE_STEP:// 单步异常TF == 1 - 硬件异常 
  {
    if (g_isCCBP)
    {
      EnableCCBP(g_dbgProcess, g_lpCCBPAddr);
      g_bIsInput = false;
      g_isCCBP = false;
      g_dwContinueStatus = DBG_CONTINUE;
    }
    else if (g_isStepIn)
    {
      g_bIsInput = true;
      g_isStepIn = false;
      DbgDisasm(lpExceptAddr, 5);
      g_dwContinueStatus = DBG_CONTINUE;
    }
    else if (g_isHardBP)
    {
      DbgDisasm(lpExceptAddr, 5);
      g_isHardBP = false;
      g_bIsInput = true;
      // 取消硬件断点，并设置单步
      FixHWBP(g_dbgThread, lpExceptAddr, g_isHdPer);
      g_dwContinueStatus = DBG_CONTINUE;
    }
    else if (g_isHdPer) // 硬件永久断点
    {
      g_isHdPer = false;
      g_bIsInput = false;
      ReSetBPHW(g_dbgThread, g_HDPerAddr, g_HDType, 0, true);
      g_isHardBP = true;
      g_dwContinueStatus = DBG_CONTINUE;
    }
    else if (g_isTFMemBP)
    {
      EnableMemBP(g_dbgProcess, 0);
      g_bIsInput = false;
      g_dwContinueStatus = DBG_CONTINUE;
    }
    else if (g_isCondition)
    {
      EnableCCBP(g_dbgProcess, (LPVOID)g_condAddr);
      g_bIsInput = false;
      g_dwContinueStatus = DBG_CONTINUE;
    }
    break;
  }

  default:
  {
    g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;// 调试器不处理异常
    break;
  }
  }

  // 如果程序中断就可以输入
  if (g_bIsInput)
  {
    Command(lpExceptAddr);
  }
}

// 响应进程创建调试事件
void OnCreateProcessDbgEvent(CREATE_PROCESS_DEBUG_INFO cpdi)
{
  g_lpOep = cpdi.lpStartAddress;
  g_lpImageBase = cpdi.lpBaseOfImage;

  LoadSymbol(g_dbgSymbol, &g_dbgEvent.u.CreateProcessInfo);

  OnPluginInit();
  printf("%d 进程创建\n", g_dbgEvent.dwProcessId);
}

// 响应dll被加载事件
void OnLoadDllDbgEvent(LOAD_DLL_DEBUG_INFO lddi)
{
  char buf[0xFF] = { 0 };
  GetFinalPathNameByHandleA(lddi.hFile, buf, 0xFF, 0);
  IMAGEHLP_MODULE64 imageHlp{ 0 };
  imageHlp.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
  SymGetModuleInfo64(g_dbgSymbol, (DWORD64)lddi.lpBaseOfDll, &imageHlp);

  //SYMLOADINFO tmp;
  //tmp.base = (DWORD64)lddi.lpBaseOfDll;
  //tmp.hFile = lddi.hFile;
  //g_vecSymBase.push_back(tmp);

  SymLoadModule64(g_dbgSymbol, lddi.hFile, buf + 4, NULL, (DWORD64)lddi.lpBaseOfDll, 0);
  printf("dll被加载\n");
}

// 响应dll被卸载事件
void OnUnLoadDllDbgEvent(UNLOAD_DLL_DEBUG_INFO uddi)
{
  uddi.lpBaseOfDll;
  printf("dll被卸载\n");
}

// 响应线程创建调试事件
void OnCreateThreadDbgEvent(CREATE_THREAD_DEBUG_INFO ctdi)
{
  ctdi.lpThreadLocalBase;
  // CloseHandle(ctdi.hThread);
  printf("%d 线程创建 0x%08X起始地址\n", g_dbgEvent.dwThreadId, (DWORD)ctdi.lpStartAddress);
}

// 响应线程退出调试事件
void OnExitThreadDbgEvent(EXIT_THREAD_DEBUG_INFO etdi)
{
  printf("%08d 线程退出 退出码: %d\n", g_dbgEvent.dwThreadId, etdi.dwExitCode);
}

// 响应异常断点 -- int3断点
void OnExceptionBreakPoint(LPVOID lpAddr)
{
  if (g_bdbgIsFirstBP)
  {
    if (g_isAttach == false)
    {
      //for (size_t i = 0; i < g_vecSymBase.size(); i++)
      //{
      //  SymLoadModule64(g_dbgSymbol, g_vecSymBase[i].hFile, NULL, NULL, g_vecSymBase[i].base, 0);
      //}
      // 断在系统领空，不处理
      // 调试器只会接收到一次初始断点，无论以DBG_CONTINUE还是DBG_EXCEPTION_NOT_HANDLED调用ContinueDebugEvent，都不会再接收到初始断点。
      // 可以做一些初始化工作，加载调试符号，设置OEP断点，加载插件。。。
      g_bdbgIsFirstBP = false;
      SetCCBP(g_dbgProcess, g_lpOep);// 设置OEP断点
      g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED; // 调试器不处理第一次系统领空int3异常
      return;
    }
    else
    {
      g_bdbgIsFirstBP = false;
      DbgDisasm(lpAddr, 5);
      g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
      g_bIsInput = true;
      return;
    }
  }

  // 断点是OEP
  if (lpAddr == g_lpOep)
  {
    // 删除断点
    CleanCCBP(g_dbgProcess, lpAddr);
    HidePEB();
    DbgDisasm(lpAddr, 5);

    // 恢复EIP
    CONTEXT context = { CONTEXT_CONTROL };
    GetThreadContext(g_dbgThread, &context);
    context.Eip--;
    SetThreadContext(g_dbgThread, &context);

    // 调试器处理了int3异常 EIP--
    g_dwContinueStatus = DBG_CONTINUE;
    g_bIsInput = true;   // 等待用户输入
    return;
  }

  // 是stepover

  if (g_isStepOver == true)
  {
    g_isStepOver = false;
    DisableCCBP(g_dbgProcess, lpAddr);
    CleanCCBP(g_dbgProcess, lpAddr);
    DbgDisasm(lpAddr, 5);
    // int3 断点断下后EIP会自己加1，我们要还原他
    CONTEXT context = { CONTEXT_CONTROL };
    GetThreadContext(g_dbgThread, &context);
    context.Eip--;
    SetThreadContext(g_dbgThread, &context);
    g_bIsInput = true;
    g_dwContinueStatus = DBG_CONTINUE;
    return;
  }


  CONTEXT context1 = { CONTEXT_ALL };
  GetThreadContext(g_dbgThread, &context1);
  BYTE buf = 0;
  ReadProcessMemory(g_dbgProcess, (LPVOID)(context1.Ebp + 0x8), &buf, 1, 0);
  if (g_isCondition && buf == g_condValue)
  {
    g_isCondition = false;
    CleanCCBP(g_dbgProcess, lpAddr);
    DbgDisasm(lpAddr, 5);
    // int3 断点断下后EIP会自己加1，我们要还原他
    CONTEXT context = { CONTEXT_CONTROL };
    GetThreadContext(g_dbgThread, &context);
    context.Eip--;
    SetThreadContext(g_dbgThread, &context);
    g_bIsInput = true;
    g_dwContinueStatus = DBG_CONTINUE;
    return;
  }
  else if (g_isCondition)
  {
    StepIn();
    CONTEXT context = { CONTEXT_CONTROL };
    GetThreadContext(g_dbgThread, &context);
    context.Eip--;
    SetThreadContext(g_dbgThread, &context);
    DisableCCBP(g_dbgProcess, lpAddr);
    g_dwContinueStatus = DBG_CONTINUE;
    g_bIsInput = false;
    return;
  }
  // 调试器不处理异常 --- int3 断点将断下
  // 显示反汇编
  // 1、禁用软件断点
  DisableCCBP(g_dbgProcess, lpAddr);

  // 2、反汇编
  DbgDisasm(lpAddr, 5);

  // int3 断点断下后EIP会自己加1，我们要还原他
  CONTEXT context = { CONTEXT_CONTROL };
  GetThreadContext(g_dbgThread, &context);
  context.Eip--;
  SetThreadContext(g_dbgThread, &context);
  // 保存断点的位置并标识CC断点已经命中  === 为了在单步断点的时候判断好还原断点

  // 设置单步断点
  g_isCCBP = true;       // 标识程序是因为CC断点断下的
  g_lpCCBPAddr = lpAddr; // 标识程序CC断点断下的地址
  StepIn();              // 为了在下次执行的把CC断点再设置上去

  // 调试器处理了int3异常 EIP--
  g_dwContinueStatus = DBG_CONTINUE;
  g_bIsInput = true; // 等待用户输入
}

// 反汇编
void DbgDisasm(LPVOID pAddr, size_t nLen)
{
  unsigned char szOpcode[256] = {}; // 目标进程中的OPCODE
  csh handle;					    // 反汇编引擎句柄
  cs_insn* pInsn;			    // 保存反汇编得到的指令的缓冲区首地址
  size_t count = 0;       // 保存得到的反汇编的指令条数
  DWORD  dwReadSize = 0;	// 实际读取长度

  //初始化反汇编器句柄,(x86_64架构,32位模式,句柄)
  cs_open(
    CS_ARCH_X86,    /*x86指令集*/
    CS_MODE_32,     /*使用32位模式解析opcode*/
    &handle         /*输出的反汇编句柄*/
  );

  DWORD dwOld = 0;
  VirtualProtectEx(g_dbgProcess, (LPVOID)pAddr, 256, PAGE_READWRITE, &dwOld);
  // 获取机器码
  ReadProcessMemory(g_dbgProcess, (LPVOID)pAddr, szOpcode, 256, &dwReadSize);
  VirtualProtectEx(g_dbgProcess, (LPVOID)pAddr, 256, dwOld, &dwOld);

  // 开始反汇编.
  // 函数会返回总共得到了几条汇编指令
  count = cs_disasm(handle,		/*反汇编器句柄,从cs_open函数得到*/
    szOpcode,					/*需要反汇编的opcode的缓冲区首地址*/
    sizeof(szOpcode),			/*opcode的字节数*/
    (uint64_t)pAddr,			/*opcode的所在的内存地址*/
    nLen,				/*需要反汇编的指令条数,如果是0,则反汇编出全部*/
    &pInsn/*反汇编输出*/
  );

  printf("\r\n--------------------------------------------------------------\r\n");
  // 输出反汇编
  for (size_t j = 0; j < nLen; j++)
  {

    // 判断是否显示源码
    DWORD dwLen = GetSymLine((DWORD)pInsn[j].address, dwLen);
    if (dwLen == 0)
    {
      printf("\n");
    }
    else
    {
      printf("0x%08X | %s\n",
        (int)pInsn[j].address,
        ShowSrcCode(dwLen));
    }

    // 拼接opcode
    string buf;
    for (size_t i = 0; i < pInsn[j].size; i++)
    {
      char tmp[6] = { 0 };
      sprintf_s(tmp, 6, "%02X ", pInsn[j].bytes[i]);
      buf += tmp;
    }

    // 显示地址|opcode|汇编指令
    printf("0x%08X | %-*s %-*s %-*s ",
      (int)pInsn[j].address, /*指令地址*/
      30, buf.c_str(),
      8, pInsn[j].mnemonic,/*指令操作码*/
      10, pInsn[j].op_str /*指令操作数*/

    );
    DWORD dwTagAddr = 0;
    sscanf_s(pInsn[j].op_str, "%x", &dwTagAddr);

    // 显示符号
    if (!strcmp("call", pInsn[j].mnemonic) || !strcmp("jmp", pInsn[j].mnemonic))
    {
      printf("%-*s\n", 20, GetFunSymbolName(dwTagAddr));
    }
    else
    {
      printf("\n");
    }
  }
  printf("--------------------------------------------------------------\r\n");

  // 释放保存指令的空间
  cs_free(pInsn, count);
  // 关闭句柄
  cs_close(&handle);
}

// 用户接收命令
void Command(LPVOID lpAddr)
{
  // int3 断点断下后EIP会自己加1，我们要还原他
  // CONTEXT context = { CONTEXT_CONTROL };
  // GetThreadContext(g_dbgThread, &context);
  // printf(">>EIP:%08X", context.Eip);
  char cmd[0xFF] = { 0 };
  while (true)
  {
    printf(">>");
    scanf_s("%s", cmd, 0xFF);


    if (!strcmp("g", cmd))
    {
      break;
    }

    // 软件断点操作
    else if (!strcmp("bp", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      SetCCBP(g_dbgProcess, (LPVOID)dwBPAddr);
      printf("软件断点 0x%08X 设置成功\n", dwBPAddr);
    }
    else if (!strcmp("bl", cmd))
    {
      ShowAllCCBP();
    }
    else if (!strcmp("bc", cmd))
    {
      DWORD dwCleanBPAddr = 0;
      scanf_s("%X", &dwCleanBPAddr);
      CleanCCBP(g_dbgProcess, (LPVOID)dwCleanBPAddr);
      printf("软件断点 0x%08X 删除成功\n", dwCleanBPAddr);
    }
    else if (!strcmp("cbp", cmd))// 条件断点
    {
      SetConditionBp(g_dbgProcess);
    }
    // 单步步入
    else if (!strcmp("t", cmd))
    {
      StepIn();
      g_isStepIn = true;
      break;
    }
    // 单步步过
    else if (!strcmp("p", cmd))
    {
      StepOver();
      break;
    }
    // 执行到返回
    else if (!strcmp("ret", cmd))
    {
      // StepOut();
      break;
    }

    // 硬件断点操作
    else if (!strcmp("bhr", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      if (SetBPHW(g_dbgThread, (LPVOID)dwBPAddr, 3, 0, false))
      {
        printf("硬件读写断点 0x%08X 设置成功\n", dwBPAddr);
        g_HDPerAddr = (LPVOID)dwBPAddr;
        g_HDType = 3;
        g_isHardBP = true;
      }
      else
      {
        printf("硬件读写断点 0x%08X 设置失败\n", dwBPAddr);
      }
    }
    else if (!strcmp("bhw", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      if (SetBPHW(g_dbgThread, (LPVOID)dwBPAddr, 1, 0, false))
      {
        printf("硬件写入断点 0x%08X 设置成功\n", dwBPAddr);
        g_HDType = 1;
        g_HDPerAddr = (LPVOID)dwBPAddr;
        g_isHardBP = true;
      }
      else
      {
        printf("硬件写入断点 0x%08X 设置失败\n", dwBPAddr);
      }
    }
    else if (!strcmp("bhe", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      if (SetBPHW(g_dbgThread, (LPVOID)dwBPAddr, 0, 0, false))
      {
        printf("硬件执行断点 0x%08X 设置成功\n", dwBPAddr);
        g_HDType = 0;
        g_HDPerAddr = (LPVOID)dwBPAddr;
        g_isHardBP = true;
      }
      else
      {
        printf("硬件执行断点 0x%08X 设置失败\n", dwBPAddr);
      }
    }
    else if (!strcmp("bhl", cmd))
    {
      ShowHWBP();
    }
    else if (!strcmp("bhc", cmd))
    {
      DWORD dwCleanBPAddr = 0;
      scanf_s("%X", &dwCleanBPAddr);
      CleanHWBP(g_dbgThread, (LPVOID)dwCleanBPAddr);
      printf("硬件断点 0x%08X 删除成功\n", dwCleanBPAddr);
    }

    // 内存断点操作
    else if (!strcmp("bmr", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      SetMemBP(g_dbgThread, g_dbgProcess, (LPVOID)dwBPAddr, 0);
      printf("内存读写断点 0x%08X 设置成功\n", dwBPAddr);
      g_isMemBP = true;
      g_lpMemAddr = (LPVOID)dwBPAddr;

    }
    else if (!strcmp("bmw", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      SetMemBP(g_dbgThread, g_dbgProcess, (LPVOID)dwBPAddr, 1);
      printf("内存读写断点 0x%08X 设置成功\n", dwBPAddr);
      g_isMemBP = true;
      g_lpMemAddr = (LPVOID)dwBPAddr;

    }
    else if (!strcmp("bme", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      SetMemBP(g_dbgThread, g_dbgProcess, (LPVOID)dwBPAddr, 2);
      printf("内存读写断点 0x%08X 设置成功\n", dwBPAddr);
      g_isMemBP = true;
      g_lpMemAddr = (LPVOID)dwBPAddr;
    }
    else if (!strcmp("bml", cmd))
    {

    }
    else if (!strcmp("bmc", cmd))
    {
      DWORD dwCleanBPAddr = 0;
      scanf_s("%X", &dwCleanBPAddr);
      CleanMemBP(g_dbgProcess, (LPVOID)dwCleanBPAddr);
      printf("硬件断点 0x%08X 删除成功\n", dwCleanBPAddr);
    }

    // 帮助
    else if (!strcmp("h", cmd))
    {

    }

    // 显示反汇编
    else if (!strcmp("u", cmd))
    {
      DWORD dwLen = 0;
      scanf_s("%d", &dwLen);
      DbgDisasm(lpAddr, dwLen);
    }
    else if (!strcmp("ua", cmd))
    {
      DWORD dwLen = 0;
      DWORD dwAddr = 0;
      scanf_s("%d", &dwLen);
      scanf_s("%X", &dwAddr);
      DbgDisasm((LPVOID)dwAddr, dwLen);
    }
    else if (!strcmp("easm", cmd))
    {
      LPVOID lpAddr = 0;
      scanf_s("%X", (DWORD*)&lpAddr);
      ModifyDisasm(lpAddr);
    }



    // 查看寄存器
    else if (!strcmp("r", cmd))
    {
      QueryReg();
    }
    // 修改寄存器
    else if (!strcmp("er", cmd))
    {
      DWORD dwVar = 0;
      scanf_s("%X", &dwVar);
      ModifyReg(dwVar);
    }

    // 编辑内存
    else if (!strcmp("em", cmd))
    {
      DWORD dwAddr = 0;
      scanf_s("%X", &dwAddr);
      DWORD bytes = 0;

      scanf_s("%X", &bytes);
      ModifyMemory(dwAddr, bytes);
    }

    // 显示内存
    else if (!strcmp("d", cmd))
    {
      DWORD dwVar = 0;
      scanf_s("%X", &dwVar);
      ShowMemory(dwVar);
    }

    // 显示模块信息
    else if (!strcmp("lm", cmd))
    {

      vector<MMODULEINFO> mModule;
      ListModule(mModule);
    }

    else if (!strcmp("k", cmd))//查看栈
    {
      ShowStack();
    }

    // dump
    else if (!strcmp("dump", cmd))
    {
      DumpExe();
    }

    // 解析导出表    
    else if (!strcmp("lexp", cmd))
    {
      ShowExportInfo();
    }
    // 解析导入表
    else if (!strcmp("limp", cmd))
    {
      ShowImportInfo();
    }

    else if (!strcmp("bpapi", cmd))
    {
      char buf[0xFF] = { 0 };
      scanf_s("%s", buf, 0xFF);
      SetCCBP(g_dbgProcess, (LPVOID)FindApiAddress(g_dbgProcess, buf));
    }
  }
}

// 单步步入
void StepIn()
{
  CONTEXT ct = { CONTEXT_ALL };
  GetThreadContext(g_dbgThread, &ct);
  ((PEFLAGS)&ct.EFlags)->TF = 1;
  SetThreadContext(g_dbgThread, &ct);
}

// 单步步过
void StepOver()
{
  //当前指令执行到的地址
  CONTEXT ct = { CONTEXT_CONTROL };
  if (!GetThreadContext(g_dbgThread, &ct))
  {
    printf("获取线程上下文失败");
    exit(0);
  }

  unsigned char szOpcode[256] = {}; // 目标进程中的OPCODE
  csh handle;					    // 反汇编引擎句柄
  cs_insn* pInsn;			    // 保存反汇编得到的指令的缓冲区首地址
  size_t count = 0;       // 保存得到的反汇编的指令条数
  DWORD  dwReadSize = 0;	// 实际读取长度

    //初始化反汇编器句柄,(x86_64架构,32位模式,句柄)
  cs_open(
    CS_ARCH_X86,    /*x86指令集*/
    CS_MODE_32,     /*使用32位模式解析opcode*/
    &handle         /*输出的反汇编句柄*/
  );

  // 获取机器码
  ReadProcessMemory(g_dbgProcess, (LPVOID)ct.Eip, szOpcode, 256, &dwReadSize);

  // 开始反汇编.
// 函数会返回总共得到了几条汇编指令
  count = cs_disasm(handle,		/*反汇编器句柄,从cs_open函数得到*/
    szOpcode,					/*需要反汇编的opcode的缓冲区首地址*/
    sizeof(szOpcode),			/*opcode的字节数*/
    (uint64_t)ct.Eip,			/*opcode的所在的内存地址*/
    1,				/*需要反汇编的指令条数,如果是0,则反汇编出全部*/
    &pInsn/*反汇编输出*/
  );
  if (!strcmp(pInsn->mnemonic, "call"))
  {
    LPVOID lpStepOverAddr = (LPVOID)((ct.Eip) + (DWORD)pInsn->size);
    SetCCBP(g_dbgProcess, lpStepOverAddr);
    g_isStepOver = true;
  }
  else
  {
    StepIn();
    g_isStepIn = true;
  }
}

// 查看寄存器
void QueryReg()
{
  CONTEXT ct = { CONTEXT_ALL };
  GetThreadContext(g_dbgThread, &ct);
  printf("=====================\n");
  printf("eax:%08X\n", ct.Eax);
  printf("ebx:%08X\n", ct.Ebx);
  printf("ecx:%08X\n", ct.Ecx);
  printf("edx:%08X\n", ct.Edx);
  printf("esi:%08X\n", ct.Esi);
  printf("edi:%08X\n", ct.Edi);
  printf("esp:%08X\n", ct.Esp);
  printf("ebp:%08X\n", ct.Ebp);
  printf("eip:%08X\n", ct.Eip);
  printf("ss:%04X\n", ct.SegSs);
  printf("cs:%04X\n", ct.SegCs);
  printf("ds:%04X\n", ct.SegDs);
  printf("es:%04X\n", ct.SegEs);
  printf("fs:%04X\n", ct.SegFs);
  printf("gs:%04X\n", ct.SegGs);
  printf("=====================\n");
}

// 修改寄存器
void ModifyReg(DWORD dwVar)
{
  CONTEXT ct = { CONTEXT_ALL };
  GetThreadContext(g_dbgThread, &ct);
  ct.Eax = dwVar;
  SetThreadContext(g_dbgThread, &ct);
}

// 显示内存信息
void ShowMemory(DWORD dwVar)
{
  // 5*16个字节
  BYTE lpMem[5][0x10] = { 0 };
  ReadProcessMemory(g_dbgProcess, (LPVOID)dwVar, lpMem, 0x10 * 5, 0);
  for (size_t i = 0; i < 5; i++)
  {
    for (size_t j = 0; j < 0x10; j++)
    {
      printf("%02X ", lpMem[i][j]);
    }
    printf("\n");
  }
}

// 修改内存
void ModifyMemory(DWORD dwAddr, DWORD bytes)
{
  DWORD dwWriten = 0;
  WriteProcessMemory(g_dbgProcess, (LPVOID)dwAddr, (LPVOID)&bytes, 4, &dwWriten);
}

// 查看模块信息
void ListModule(std::vector<MMODULEINFO>& mModule)
{
  //1.创建模块快照
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, g_dbgEvent.dwProcessId);
  if (hSnap == INVALID_HANDLE_VALUE) {
    return;
  }

  //2.第一次遍历模块
  MODULEENTRY32 stcMd = { sizeof(stcMd) };

  printf("start\t\tsize\tname\n");
  if (Module32First(hSnap, &stcMd))
  {
    //3.循环遍历模块Next
    do
    {
      printf("%08X\t%d\t%s\n", (DWORD)stcMd.modBaseAddr, stcMd.modBaseSize, stcMd.szModule);
    } while (Module32Next(hSnap, &stcMd));
  }
  CloseHandle(hSnap);
}

// 查看堆栈
void ShowStack()
{
  CONTEXT ct = { CONTEXT_ALL };
  //获取线程上下文
  if (!GetThreadContext(g_dbgThread, &ct)) {
    printf("获取线程上下文失败");
    exit(0);
  }

  LPBYTE pByte = new BYTE[100];
  SIZE_T read = 0;
  //获取机器码
  if (!ReadProcessMemory(g_dbgProcess, (LPCVOID)ct.Esp, pByte, 100, &read))
  {
    printf("读取内存失败");
    exit(0);
  }
  int j = 0;
  for (int i = 0; i < 40; ++i)
  {

    if (i % 4 == 0)
    {
      printf("%08X | ", ct.Esp + 4 * j);
      j++;
    }
    DWORD tmp = pByte[i];
    printf("%02X ", tmp);
    if ((i + 1) % 4 == 0)
    {
      printf("\n");
    }
  }
}

// 修改汇编代码
void ModifyDisasm(LPVOID addr)
{
  XEDPARSE xed = { 0 };
  xed.cip = (ULONGLONG)addr;

  // 接收指令
  printf("指令：");
  getchar();
  gets_s(xed.instr, XEDPARSE_MAXBUFSIZE);

  // xed.cip, 汇编带有跳转偏移的指令时,需要配置这个字段
  if (XEDPARSE_OK != XEDParseAssemble(&xed))
  {
    printf("指令错误：%s\n", xed.error);
    return;
  }


  unsigned char szOpcode[256] = {}; // 目标进程中的OPCODE
  csh handle;					    // 反汇编引擎句柄
  cs_insn* pInsn;			    // 保存反汇编得到的指令的缓冲区首地址
  size_t count = 0;       // 保存得到的反汇编的指令条数
  DWORD  dwReadSize = 0;	// 实际读取长度

  //初始化反汇编器句柄,(x86_64架构,32位模式,句柄)
  cs_open(
    CS_ARCH_X86,    /*x86指令集*/
    CS_MODE_32,     /*使用32位模式解析opcode*/
    &handle         /*输出的反汇编句柄*/
  );

  // 获取机器码
  ReadProcessMemory(g_dbgProcess, (LPVOID)addr, szOpcode, 256, &dwReadSize);

  // 开始反汇编.
  // 函数会返回总共得到了几条汇编指令
  count = cs_disasm(handle,		/*反汇编器句柄,从cs_open函数得到*/
    szOpcode,					/*需要反汇编的opcode的缓冲区首地址*/
    sizeof(szOpcode),			/*opcode的字节数*/
    (uint64_t)addr,			/*opcode的所在的内存地址*/
    1,				/*需要反汇编的指令条数,如果是0,则反汇编出全部*/
    &pInsn/*反汇编输出*/
  );

  //printf("\r\n-------------------------------\r\n");
  //// 输出反汇编
  //for (size_t j = 0; j < nLen; j++)
  //{
  //  printf("0x%08X | %-*s %-*s\n",
  //    (int)pInsn[j].address, /*指令地址*/
  //    10, pInsn[j].mnemonic,/*指令操作码*/
  //    15, pInsn[j].op_str/*指令操作数*/
  //  );
  //}
  //printf("-------------------------------\r\n");

  // 释放保存指令的空间
  cs_free(pInsn, count);
  // 关闭句柄
  cs_close(&handle);

  size_t nopCount = pInsn->size - xed.dest_size;
  //将OPCODE写入内存
  SIZE_T write1 = 0;
  if (!WriteProcessMemory(g_dbgProcess, addr, xed.dest, xed.dest_size, &write1))
  {
    printf("写入进程内存失败");
    exit(0);
  }
  DWORD nopaddr = (DWORD)addr + xed.dest_size;

  for (size_t i = 0; i < nopCount; i++)
  {
    WriteProcessMemory(g_dbgProcess, (LPVOID)(nopaddr + i), "\x90", 1, &write1);
  }
}

// dump
void DumpExe()
{
  //删除所有的内存断点
  DeleteAllCCBP(g_dbgProcess);

  DWORD nPeSize = 0;				//PE头
  DWORD nImageSize = 0;			//内存中大小
  DWORD nFileSize = 0;			//文件大小
  DWORD nSectionNum = 0;			//区段数量
  PBYTE nPeHeadData = nullptr;	//PE缓存
  PBYTE nImageBuf = nullptr;		//文件缓存
  FILE* pFile = nullptr;			//文件指针

  nPeHeadData = new BYTE[4096]{};

  //读取文件头信息

  ReadProcessMemory(g_dbgProcess, g_lpImageBase, nPeHeadData, 4096, NULL);
  //获取PE信息
  PIMAGE_DOS_HEADER nDosHead = (PIMAGE_DOS_HEADER)nPeHeadData;
  PIMAGE_NT_HEADERS nNtHead = (PIMAGE_NT_HEADERS)(nPeHeadData + nDosHead->e_lfanew);
  PIMAGE_SECTION_HEADER nSecetionHead = IMAGE_FIRST_SECTION(nNtHead);

  //PE头大小
  nPeSize = nNtHead->OptionalHeader.SizeOfHeaders;
  //文件的尺寸
  nImageSize = nNtHead->OptionalHeader.SizeOfImage;
  //区段数量	
  nSectionNum = nNtHead->FileHeader.NumberOfSections;


  //申请exe所需的堆空间
  nImageBuf = new BYTE[nImageSize]{};

  //读取PE数据
  ReadProcessMemory(g_dbgProcess, g_lpImageBase, nImageBuf, nPeSize, NULL);

  nFileSize += nPeSize;
  // 读取每个区段的数据
  for (DWORD i = 0; i < nSectionNum; i++)
  {
    ReadProcessMemory(g_dbgProcess, (LPVOID)((DWORD)g_lpImageBase + nSecetionHead[i].VirtualAddress),
      nImageBuf + nSecetionHead[i].PointerToRawData, nSecetionHead[i].SizeOfRawData, NULL);

    nFileSize += nSecetionHead[i].SizeOfRawData;
  }

  // 修改文件对齐
  nDosHead = (PIMAGE_DOS_HEADER)nImageBuf;
  nNtHead = (PIMAGE_NT_HEADERS)((DWORD)nImageBuf + nDosHead->e_lfanew);
  nNtHead->OptionalHeader.FileAlignment = nNtHead->OptionalHeader.SectionAlignment;

  fopen_s(&pFile, ".\\dump.exe", "wb");
  fwrite(nImageBuf, nFileSize, 1, pFile);
  fclose(pFile);

  delete[] nPeHeadData;
  delete[] nImageBuf;

  printf("成功保存.\\mdump.exe\n");
}

// 加载符号
void LoadSymbol(HANDLE hProcess, CREATE_PROCESS_DEBUG_INFO* pInfo)
{
  // 初始化符号处理器
  SymInitialize(hProcess, "C:\\Users\\nSky\\Desktop\\sym", FALSE);
  //SYMLOADINFO tmp;
  //tmp.base = (DWORD64)pInfo->lpBaseOfImage;
  //tmp.hFile = pInfo->hFile;
  //g_vecSymBase.push_back(tmp);

  IMAGEHLP_MODULE64 imageHlp{ 0 };
  imageHlp.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
  SymGetModuleInfo64(hProcess, (DWORD64)pInfo->lpBaseOfImage, &imageHlp);
  // 载入符号文件
  SymLoadModule64(hProcess, pInfo->hFile, NULL, NULL, (DWORD64)pInfo->lpBaseOfImage, 0);
}


long MyExceptFilter()
{

  return EXCEPTION_EXECUTE_HANDLER;// 执行异常代码块
}


// 获取调试符号函数名称
const char* GetFunSymbolName(SIZE_T nAddress)
{
  __try
  {
    DWORD64  dwDisplacement = 0;
    char* buffer = new char[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]{ 0 };
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;
    //根据地址获取符号信息



    if (!SymFromAddr(g_dbgSymbol, nAddress, &dwDisplacement, pSymbol))
      return 0;
    return pSymbol->Name;
  }
  __except (MyExceptFilter())
  {

  }
  return 0;
}


// 隐藏PEB
void HidePEB()
{

  // 保存查询到的基本信息
  struct PROCESS_BASIC_INFORMATION {
    ULONG ExitStatus;		// 进程返回码
    DWORD  PebBaseAddress;  // PEB地址
    ULONG AffinityMask;		// CPU亲和性掩码
    LONG  BasePriority;		// 基本优先级
    ULONG UniqueProcessId;  // 本进程PID
    ULONG InheritedFromUniqueProcessId; // 父进程PID
  }stcProcInfo;
  // 目标进程的句柄
  // 可以通过函数获取到目标进程的 PEB
  NtQueryInformationProcess(
    g_dbgProcess,
    ProcessBasicInformation,
    &stcProcInfo,
    sizeof(stcProcInfo),
    NULL);

  WriteProcessMemory(g_dbgProcess, (LPVOID)(stcProcInfo.PebBaseAddress + 0x02), "", 1, NULL);
  printf("BegingDebugged已经隐藏\n");
  // 修改目标 PEB.NtGlobalFlag 的值为 0
  WriteProcessMemory(g_dbgProcess,
    (LPVOID)(stcProcInfo.PebBaseAddress + 0x68),
    "", 1, NULL);
  printf("NtGlobalFlag已经隐藏\n");
  //3 PEB.ProcessHeap 字段指向的 _HEAP 结构中的
  // Flags 0x40和 ForceFlags 0x44可以确定是否被调试，
  // 如果没有被调试，其中保存的值分别是 2 和 0
  LPVOID addr = 0;
  // 读取PEB
  ReadProcessMemory(g_dbgProcess, (LPVOID)(stcProcInfo.PebBaseAddress + 0x18), &addr, 4, NULL);
  WriteProcessMemory(g_dbgProcess, (LPVOID)((DWORD)addr + 0x40), "\x02", 1, NULL);
  WriteProcessMemory(g_dbgProcess, (LPVOID)((DWORD)addr + 0x44), "\x00", 1, NULL);
  printf("Flags和ForceFlags已经隐藏\n");
  return;
}


// 条件断点
void SetConditionBp(HANDLE hProcess)
{
  DWORD addr = 0;
  scanf_s("%X", &addr);
  string condition;
  condition.resize(30);
  scanf_s("%s", &condition[0], 30);
  int idx = condition.find("==");
  string regi;

  if (idx >= 0)
  {
    string v = condition.substr(idx + 2);
    g_condValue = atoi(v.c_str());
    regi = condition.substr(0, idx);
    //下CC断点
    g_isCondition = true;
    SetCCBP(hProcess, (LPVOID)addr);
  }
  g_condAddr = addr;
  printf("条件断点设置成功\n");
}


// 显示进程列表
bool GetProcessList(vector<DBGPROCESSINFO>& vecProcessInfo)
{
  HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hSnapShot)
  {
    return false;
  }

  PROCESSENTRY32 pe = { 0 };
  pe.dwSize = sizeof(pe);
  Process32First(hSnapShot, &pe);
  do
  {
    DBGPROCESSINFO tmp = { 0 };
    tmp.dwPid = pe.th32ProcessID;
    tmp.strProcessName = pe.szExeFile;
    vecProcessInfo.push_back(tmp);
  } while (Process32Next(hSnapShot, &pe));
  return true;
}


// 根据API名称查找地址
DWORD FindApiAddress(HANDLE hProcess, const char* pszName)
{
  DWORD64  dwDisplacement = 0;
  char* buffer = new char[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]{ 0 };
  PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
  pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  pSymbol->MaxNameLen = MAX_SYM_NAME;
  //根据地址获取符号信息
  if (!SymFromName(g_dbgSymbol, pszName, pSymbol))
    return 0;
  return (DWORD)pSymbol->Address;
}


// 源码调试
DWORD GetSymLine(DWORD dwAddr, DWORD& dwLine)
{
  //获取源文件以及行信息
  IMAGEHLP_LINE64 lineInfo = { 0 };
  lineInfo.SizeOfStruct = sizeof(lineInfo);
  DWORD displacement = 0;
  BOOL bret = SymGetLineFromAddr64(
    g_dbgSymbol,
    dwAddr,
    &displacement,
    &lineInfo);

  if (bret == 0)
  {
    return 0;
  }
  return lineInfo.LineNumber;
}

// 返回源代码
const char* ShowSrcCode(DWORD dwLine)
{
  //char buf[0xFF] = { 0 };
  //FILE* pf = nullptr;
  //fopen_s(&pf,"./CheckDebug.cpp", "rb+");
  //char StrLen[0xFF] = { 0 };
  //int l = 0;
  //DWORD dwCurLine = 0;
  //while (true)
  //{
  //  if (NULL == fgets(buf, 0xFF, pf))
  //    break;
  //  l = strlen(buf);
  //  if ('\r\n' == buf[l - 1])
  //  {
  //    dwCurLine++;
  //  }
  //  if (dwCurLine == dwLine)
  //  {
  //    break;
  //  }
  //}

  //fclose(pf);
  //return (const char*)StrLen;


  char filename[] = "./CheckDebug.cpp"; //文件名
  FILE* fp;
  int WhichLine = dwLine-1;             //指定要读取哪一行
  int CurrentIndex = 0;             //当前读取的行
  char StrLine[1024];             //每行最大读取的字符数,可根据实际情况扩大
  fopen_s(&fp, filename, "r");

  while (!feof(fp))
  {

    if (CurrentIndex == WhichLine)
    {
      fgets(StrLine, 1024, fp);  //读取一行

      return StrLine;
    }
    fgets(StrLine, 1024, fp);  //读取一行,并定位到下一行
    CurrentIndex++;

    //printf("%s", StrLine); //输出
  }
  fclose(fp);                     //关闭文件
  return NULL;
}
//
//int n = 0;
//void OnShowSourceLines()
//{
//
//
//  CONTEXT context = { CONTEXT_ALL };
//
//  //1. 获取线程上下文
//
//  GetThreadContext(g_dbgThread, &context);
//  //获取源文件以及行信息
//  IMAGEHLP_LINE64 lineInfo = { 0 };
//  lineInfo.SizeOfStruct = sizeof(lineInfo);
//  DWORD displacement = 0;
//  SymGetLineFromAddr64(
//    g_dbgSymbol,
//    context.Eip,
//    &displacement,
//    &lineInfo);
//  if (n < 2)
//  {
//    n++;
//    return;
//  }
//  else
//  {
//    DisplaySourceLines(
//      lineInfo.FileName,
//      lineInfo.LineNumber,
//      (unsigned int)lineInfo.Address,
//      10,10);
//  }
//}
////显示源文件中指定的行
//void DisplaySourceLines(PCHAR sourceFile, int lineNum, unsigned int address, int after, int before)
//{
//  //计算从第几行开始输出
//  int startLineNumber = lineNum - before;
//  int curLineNumber = 1;
//  //跳过不需要显示的行
//  while (curLineNumber < startLineNumber)
//  {
//    ++curLineNumber;
//  }
//  //输出当前行的地址
//  DisplayLine(sourceFile, 1, curLineNumber, TRUE);
//}
//
//void DisplayLine(PCHAR sourceFile, int line, int lineNumber, BOOL isCurLine)
//{
//  LONG displacement;
//  IMAGEHLP_LINE64 lineInfo = { 0 };
//  lineInfo.SizeOfStruct = sizeof(lineInfo);
//  SymGetLineFromName64(
//    g_dbgSymbol,
//    NULL,
//    sourceFile,
//    lineNumber,
//    &displacement,
//    &lineInfo);
//
//  int type = 0;
//  int nPrint = 0;
//  FILE* fCheckSum;
//  fopen_s(&fCheckSum, lineInfo.FileName, "r+");
//
//  int sum = 0;
//  char bufSum[MAX_PATH] = {};
//
//  while (fgets(bufSum, MAX_PATH, fCheckSum))
//  {
//    sum++;
//  }
//  fclose(fCheckSum);
//
//
//  FILE* fSource;
//  fopen_s(&fSource, lineInfo.FileName, "r+");
//  //如果从文件中的函数等于循环次数就是当前触发断点的位置
//  for (int i = 0; i <= sum; i++)
//  {
//    if (nPrint == i)
//    {
//      printf("=>   ");
//    }
//    char bufSource[MAX_PATH] = {};
//    fgets(bufSource, MAX_PATH, fSource);
//    if (strcmp(bufSource, "int main(int argc, char* argv[])\n") == 0)
//    {
//      nPrint = lineNumber + type;
//    }
//    else
//    {
//      type++;
//    }
//    printf("%s", bufSource);
//  }
//}