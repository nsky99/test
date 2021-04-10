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
#include "CPlugin.h"  // ���
#include "CPE.h"// PE

// ���������
#include "./capstone/include/capstone.h"
#ifdef _WIN64 // 64λƽ̨���������Զ����������
#pragma comment(lib, "capstone/lib/capstone_x64.lib")
#else
#pragma comment(lib,"capstone/lib/capstone_x86.lib")
#endif // _64

//���������
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

//�������
#include "XEDParse/XEDParse.h"
#ifdef _WIN64
#pragma comment (lib,"XEDParse/x64/XEDParse_x64.lib")
#else
#pragma comment (lib,"XEDParse/x86/XEDParse_x86.lib")
#endif // _WIN64

// ��ǰ���ԵĽ��̺��߳̾��
static bool   g_isAttach = false;                // �Ƿ��Ǹ��ӵ���
static HANDLE g_dbgProcess;                      // ��ǰ���Խ��̾��
static HANDLE g_dbgThread;                       // ��ǰ�����߳̾��
static HANDLE g_dbgSymbol;                       //���Ž��̾��
static DWORD  g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED; // ��ǰ����״̬��Ĭ�ϵ�����û�д���
static DEBUG_EVENT g_dbgEvent;                   // ��ǰ�����¼�
static LPVOID g_lpOep;                           // ���Խ��̵�OEP
static LPVOID g_lpImageBase;                     // ���Խ��̵ľ����ַ
static bool   g_bdbgIsFirstBP = true;            // �������Ƿ��һ�ζ���-----��һ�ζ���ϵͳ���
static bool   g_bIsInput = false;                // �Ƿ��ȡ�û���������
static bool   g_isStepIn = false;                // �Ƿ��ǵ�������
static bool   g_isCCBP = false;
static LPVOID g_lpCCBPAddr = 0;
static bool   g_isHardBP = false;
static bool   g_isHdPer = false;
static LPVOID g_HDPerAddr = 0;
static BYTE   g_HDType = 0; // Ӳ���ϵ������
static bool   g_isStepOver = false;// 
static bool   g_isMemBP = false;// �ڴ�ϵ��Ƿ�����
static LPVOID g_lpMemAddr = 0;  // �����ڴ�ϵ�ĵ�ַ
static bool   g_isTFMemBP = false;
static bool   g_isCondition = false;// �Ƿ��������ϵ�
static DWORD  g_condValue = 0;
static DWORD  g_condAddr = 0;
static DWORD  g_DbgPid = 0;
static vector<SYMLOADINFO> g_vecSymBase;
// ��������ѭ��
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
  //// ��������֮���ʹ�������¼������ǵ������յ��ĵ�һ�������¼���
  //// 
  //// CREATE_THREAD_DEBUG_EVENT
  //// ����һ���߳�֮���ʹ�������¼���
  //// 
  //// EXCEPTION_DEBUG_EVENT
  //// �����쳣ʱ���ʹ�������¼���
  //// 
  //// EXIT_PROCESS_DEBUG_EVENT
  //// ���̽������ʹ�������¼���
  //// 
  //// EXIT_THREAD_DEBUG_EVENT
  //// һ���߳̽������ʹ�������¼���
  //// 
  //// LOAD_DLL_DEBUG_EVENT
  //// װ��һ��DLLģ��֮���ʹ�������¼���
  //// 
  //// OUTPUT_DEBUG_STRING_EVENT
  //// �����Խ��̵���OutputDebugString֮��ĺ���ʱ���ʹ�������¼���
  //// 
  //// RIP_EVENT
  //// ����ϵͳ���Դ���ʱ���ʹ�������¼���
  //// 
  //// UNLOAD_DLL_DEBUG_EVENT
  //// ж��һ��DLLģ��֮���ʹ�������¼���

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
      // �򿪵��Խ���
    case OPEN:
    {
      printf("������Ҫ���Եĳ���·��>>");
      scanf_s("%s", psz, 0xFF);
      g_isAttach = false;
      InitPE(psz);
      OpenDbgProcess(psz);
      DbgEventLoop();
    }
    break;
    // ���ӵ��Խ���
    case ATTACH:
    {
      // ��ȡ�����б�
      vector<DBGPROCESSINFO> vecPInfo;
      if (!GetProcessList(vecPInfo))
      {
        printf("��ȡ������Ϣʧ��\r\n");
      }
      // ��������б�
      for (size_t i = 0; i < vecPInfo.size(); i++)
      {
        printf("%08d\t%s\r\n", vecPInfo[i].dwPid, vecPInfo[i].strProcessName.c_str());
      }

      // ��ȡҪ���ӵ��ԵĽ���
      DWORD dwPid = 0;
      printf("������Ҫ���ӵĽ���ID: ");
      scanf_s("%d", &dwPid);
      g_isAttach = true;
      AttachDbgProcess(dwPid);
      DbgEventLoop();
    }
    break;

    // �˳�������
    case EXIT:
      isRun = false;
      break;

    default:
      break;
    }
  }
}

// �򿪵��Խ���
bool OpenDbgProcess(char* pszFile)
{
  // �жϽ���·���Ƿ�Ϊ��
  if (nullptr == pszFile)
  {
    return false;
  }

  // ������Ϣ�ͽ�����Ϣ
  STARTUPINFOA si = { sizeof(STARTUPINFOA) };
  PROCESS_INFORMATION pi = { 0 };

  // �������Խ���
  BOOL bRet = CreateProcessA(
    pszFile,                                      // ��ִ�г���·����
    NULL,                                         // ������
    NULL,                                         // ��������
    NULL,                                         // �߳�����
    FALSE,                                        // �߳��Ƿ���Ա��̳�
    DEBUG_ONLY_THIS_PROCESS | CREATE_NEW_CONSOLE, // �Ե��Է�ʽ����
    NULL,                                         // �½��̻�����
    NULL,                                         // �½��̵Ĺ���Ŀ¼(��ǰĿ¼)
    &si,                                          // ������Ϣ
    &pi                                           // �����½��̵���Ϣ
  );

  g_DbgPid = pi.dwProcessId;
  // �رղ�ʹ�õľ��
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

// ���ӵ��Խ���
bool AttachDbgProcess(DWORD dwPid)
{
  // ���Թ��̾�������SE_DEBUG_NAMEȨ�ޣ������Ե����κν��̡�
  return DebugActiveProcess(dwPid);
}

// �����¼�ѭ��
DWORD DbgEventLoop()
{
  bool isLoop = true;
  g_dbgEvent = { 0 };

  g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
  // �ȴ������¼�
  while (isLoop && WaitForDebugEvent(&g_dbgEvent, -1))
  {
    // �򿪵�ǰ���Ե��¼����
    g_dbgProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_dbgEvent.dwProcessId);
    g_dbgThread = OpenThread(THREAD_ALL_ACCESS, FALSE, g_dbgEvent.dwThreadId);

    // �����¼��ַ�
    switch (g_dbgEvent.dwDebugEventCode)
    {
    case  EXCEPTION_DEBUG_EVENT:
    {
      OnExceptionDbgEvent(g_dbgEvent.u.Exception);
      // �ַ��쳣�¼�
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

    case  CREATE_PROCESS_DEBUG_EVENT:// ����OEP�Ϳ�ִ���ļ�ӳ��
    {
      OnCreateProcessDbgEvent(g_dbgEvent.u.CreateProcessInfo);
      break;
    }

    case EXIT_PROCESS_DEBUG_EVENT:
    {
      isLoop = false;
      printf("%08d �����˳� �˳���: %d\n", g_dbgEvent.dwProcessId, g_dbgEvent.u.ExitProcess.dwExitCode);
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
      printf("��ϵͳ���󡱻��ߡ��ڲ�����\n");
      break;
    }


    default:
      break;
    }
    ContinueDebugEvent(g_dbgEvent.dwProcessId, g_dbgEvent.dwThreadId, g_dwContinueStatus);

    // �رյ�ǰ���Ե��¼����
    CloseHandle(g_dbgProcess);
    CloseHandle(g_dbgThread);
  }
  return g_dwContinueStatus;
}

// ��Ӧ�쳣�����¼�
void OnExceptionDbgEvent(EXCEPTION_DEBUG_INFO exceptDbgInfo)
{
  /*
  �쳣���ࣺ
  ���޸��������쳣->EIP�������쳣->EIP-1��
  �����޸�����ֹ�쳣
    �����쳣�ָ�ִ��ʱ���Ǵ������쳣������ָ�ʼִ�У�
    �����쳣�Ǵ������쳣����ָ�����һ��ָ�ʼִ��
  Ӳ���쳣����CPU�������쳣
  ����쳣���������RaiseException�����������쳣
    Ӳ���쳣������쳣������ͨ��Windows�ṩ�Ľṹ���쳣�����������׽�ʹ���,���ִ�����ƿ����ó����ڷ����쳣�ĵط�����ִ�У�����ת���쳣�������ִ�С�
    C++�ṩ���쳣�������ֻ�ܲ�׽�ʹ�����throw����׳����쳣���򵥵�˵������ͨ������쳣�����Ƿ�0xE06D7363�������ġ�
  ���⣬C++���쳣�������ֻ��ת���쳣�������ִ�У����������쳣�����ĵط�����ִ��
  */

  /*
  * �쳣�ķַ���
  * 1��.�쳣δ������������Ӧ�ó�������˳���
  * 2���쳣�������������ˣ������ڷ����쳣�ĵط�����ִ�У�����ȡ�����Ǵ����쳣���������쳣����
  * 3���쳣�������ڵ��쳣�����������ˣ������ڷ����쳣�ĵط�����ִ�У�����ת���쳣������ڼ���ִ�С�
  *
  * �쳣�ַ��Ĺ��̣�
  * 1.��������һ���쳣��Windows��׽������쳣����ת���ں�ִ̬�С�
  * 2.Windows��鷢���쳣�ĳ����Ƿ����ڱ����ԣ�����ǣ�����һ��EXCEPTION_DEBUG_EVENT�����¼��������������ǵ�������һ���յ����¼����������������4����
  * 3.�������յ��쳣�����¼�֮������ڵ���ContinueDebugEventʱ����������ΪDBG_CONTINUE������ʾ�������Ѵ����˸��쳣�������ڷ����쳣�ĵط�����ִ�У�
  *   �쳣�ַ��������������������ΪDBG_EXCEPTION_NOT_HANDLED������ʾ������û�д�����쳣��������4����
  * 4.Windowsת�ص��û�̬��ִ�У�Ѱ�ҿ��Դ�����쳣���쳣������������ҵ���������쳣��������ִ�У�Ȼ�����ִ�еĽ�����������ִ�У��쳣�ַ�������
  *  ���û�ҵ�����������5����
  * 5.Windows��ת���ں�̬��ִ�У��ٴμ�鷢���쳣�ĳ����Ƿ����ڱ����ԣ�����ǣ����ٴη���һ��EXCEPTION_DEBUG_EVENT�����¼�����������
  *   ���ǵ������ڶ����յ����¼��������������7����
  * 6.�������ڶ��δ�����쳣���������ContinueDebugEventʱ����������ΪDBG_CONTINUE�������ڷ����쳣�ĵط�����ִ�У��쳣�ַ�������
  *   �������������ΪDBG_EXCEPTION_NOT_HANDLED��������7����
  * 7.�쳣û�б����������ԡ�Ӧ�ó�����󡱽�����
  * DBG_CONTINUE���������������쳣
  * DBG_EXCEPTION_NOT_HANDLED��������û�д����쳣 -> ת���û�̬Ѱ���쳣������
  */

  // �ж��Ƿ��ǵ�һ�η����쳣����һ�ζ���ϵͳ���
  exceptDbgInfo.ExceptionRecord.ExceptionFlags;
  exceptDbgInfo.ExceptionRecord.NumberParameters;
  exceptDbgInfo.dwFirstChance; // ͬһ���쳣���µڶ�������Ϊ0
  g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;// Ĭ�ϵ�����û�д����쳣
  DWORD dwExceptCode = exceptDbgInfo.ExceptionRecord.ExceptionCode;
  PVOID lpExceptAddr = exceptDbgInfo.ExceptionRecord.ExceptionAddress;



  switch (dwExceptCode)
  {
  case EXCEPTION_ACCESS_VIOLATION:// �����쳣 
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
    if (g_isMemBP)           // �ڴ�ϵ�
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
    else                      // �û������쳣
    {
      g_bIsInput = false;
      g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
    }
    break;
  }

  case EXCEPTION_BREAKPOINT: // �ϵ��쳣
  {
    // �������������һ���쳣
    OnExceptionBreakPoint(lpExceptAddr);
    break;
  }

  case EXCEPTION_SINGLE_STEP:// �����쳣TF == 1 - Ӳ���쳣 
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
      // ȡ��Ӳ���ϵ㣬�����õ���
      FixHWBP(g_dbgThread, lpExceptAddr, g_isHdPer);
      g_dwContinueStatus = DBG_CONTINUE;
    }
    else if (g_isHdPer) // Ӳ�����öϵ�
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
    g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;// �������������쳣
    break;
  }
  }

  // ��������жϾͿ�������
  if (g_bIsInput)
  {
    Command(lpExceptAddr);
  }
}

// ��Ӧ���̴��������¼�
void OnCreateProcessDbgEvent(CREATE_PROCESS_DEBUG_INFO cpdi)
{
  g_lpOep = cpdi.lpStartAddress;
  g_lpImageBase = cpdi.lpBaseOfImage;

  LoadSymbol(g_dbgSymbol, &g_dbgEvent.u.CreateProcessInfo);

  OnPluginInit();
  printf("%d ���̴���\n", g_dbgEvent.dwProcessId);
}

// ��Ӧdll�������¼�
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
  printf("dll������\n");
}

// ��Ӧdll��ж���¼�
void OnUnLoadDllDbgEvent(UNLOAD_DLL_DEBUG_INFO uddi)
{
  uddi.lpBaseOfDll;
  printf("dll��ж��\n");
}

// ��Ӧ�̴߳��������¼�
void OnCreateThreadDbgEvent(CREATE_THREAD_DEBUG_INFO ctdi)
{
  ctdi.lpThreadLocalBase;
  // CloseHandle(ctdi.hThread);
  printf("%d �̴߳��� 0x%08X��ʼ��ַ\n", g_dbgEvent.dwThreadId, (DWORD)ctdi.lpStartAddress);
}

// ��Ӧ�߳��˳������¼�
void OnExitThreadDbgEvent(EXIT_THREAD_DEBUG_INFO etdi)
{
  printf("%08d �߳��˳� �˳���: %d\n", g_dbgEvent.dwThreadId, etdi.dwExitCode);
}

// ��Ӧ�쳣�ϵ� -- int3�ϵ�
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
      // ����ϵͳ��գ�������
      // ������ֻ����յ�һ�γ�ʼ�ϵ㣬������DBG_CONTINUE����DBG_EXCEPTION_NOT_HANDLED����ContinueDebugEvent���������ٽ��յ���ʼ�ϵ㡣
      // ������һЩ��ʼ�����������ص��Է��ţ�����OEP�ϵ㣬���ز��������
      g_bdbgIsFirstBP = false;
      SetCCBP(g_dbgProcess, g_lpOep);// ����OEP�ϵ�
      g_dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED; // �������������һ��ϵͳ���int3�쳣
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

  // �ϵ���OEP
  if (lpAddr == g_lpOep)
  {
    // ɾ���ϵ�
    CleanCCBP(g_dbgProcess, lpAddr);
    HidePEB();
    DbgDisasm(lpAddr, 5);

    // �ָ�EIP
    CONTEXT context = { CONTEXT_CONTROL };
    GetThreadContext(g_dbgThread, &context);
    context.Eip--;
    SetThreadContext(g_dbgThread, &context);

    // ������������int3�쳣 EIP--
    g_dwContinueStatus = DBG_CONTINUE;
    g_bIsInput = true;   // �ȴ��û�����
    return;
  }

  // ��stepover

  if (g_isStepOver == true)
  {
    g_isStepOver = false;
    DisableCCBP(g_dbgProcess, lpAddr);
    CleanCCBP(g_dbgProcess, lpAddr);
    DbgDisasm(lpAddr, 5);
    // int3 �ϵ���º�EIP���Լ���1������Ҫ��ԭ��
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
    // int3 �ϵ���º�EIP���Լ���1������Ҫ��ԭ��
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
  // �������������쳣 --- int3 �ϵ㽫����
  // ��ʾ�����
  // 1����������ϵ�
  DisableCCBP(g_dbgProcess, lpAddr);

  // 2�������
  DbgDisasm(lpAddr, 5);

  // int3 �ϵ���º�EIP���Լ���1������Ҫ��ԭ��
  CONTEXT context = { CONTEXT_CONTROL };
  GetThreadContext(g_dbgThread, &context);
  context.Eip--;
  SetThreadContext(g_dbgThread, &context);
  // ����ϵ��λ�ò���ʶCC�ϵ��Ѿ�����  === Ϊ���ڵ����ϵ��ʱ���жϺû�ԭ�ϵ�

  // ���õ����ϵ�
  g_isCCBP = true;       // ��ʶ��������ΪCC�ϵ���µ�
  g_lpCCBPAddr = lpAddr; // ��ʶ����CC�ϵ���µĵ�ַ
  StepIn();              // Ϊ�����´�ִ�еİ�CC�ϵ���������ȥ

  // ������������int3�쳣 EIP--
  g_dwContinueStatus = DBG_CONTINUE;
  g_bIsInput = true; // �ȴ��û�����
}

// �����
void DbgDisasm(LPVOID pAddr, size_t nLen)
{
  unsigned char szOpcode[256] = {}; // Ŀ������е�OPCODE
  csh handle;					    // �����������
  cs_insn* pInsn;			    // ���淴���õ���ָ��Ļ������׵�ַ
  size_t count = 0;       // ����õ��ķ�����ָ������
  DWORD  dwReadSize = 0;	// ʵ�ʶ�ȡ����

  //��ʼ������������,(x86_64�ܹ�,32λģʽ,���)
  cs_open(
    CS_ARCH_X86,    /*x86ָ�*/
    CS_MODE_32,     /*ʹ��32λģʽ����opcode*/
    &handle         /*����ķ������*/
  );

  DWORD dwOld = 0;
  VirtualProtectEx(g_dbgProcess, (LPVOID)pAddr, 256, PAGE_READWRITE, &dwOld);
  // ��ȡ������
  ReadProcessMemory(g_dbgProcess, (LPVOID)pAddr, szOpcode, 256, &dwReadSize);
  VirtualProtectEx(g_dbgProcess, (LPVOID)pAddr, 256, dwOld, &dwOld);

  // ��ʼ�����.
  // �����᷵���ܹ��õ��˼������ָ��
  count = cs_disasm(handle,		/*����������,��cs_open�����õ�*/
    szOpcode,					/*��Ҫ������opcode�Ļ������׵�ַ*/
    sizeof(szOpcode),			/*opcode���ֽ���*/
    (uint64_t)pAddr,			/*opcode�����ڵ��ڴ��ַ*/
    nLen,				/*��Ҫ������ָ������,�����0,�򷴻���ȫ��*/
    &pInsn/*��������*/
  );

  printf("\r\n--------------------------------------------------------------\r\n");
  // ��������
  for (size_t j = 0; j < nLen; j++)
  {

    // �ж��Ƿ���ʾԴ��
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

    // ƴ��opcode
    string buf;
    for (size_t i = 0; i < pInsn[j].size; i++)
    {
      char tmp[6] = { 0 };
      sprintf_s(tmp, 6, "%02X ", pInsn[j].bytes[i]);
      buf += tmp;
    }

    // ��ʾ��ַ|opcode|���ָ��
    printf("0x%08X | %-*s %-*s %-*s ",
      (int)pInsn[j].address, /*ָ���ַ*/
      30, buf.c_str(),
      8, pInsn[j].mnemonic,/*ָ�������*/
      10, pInsn[j].op_str /*ָ�������*/

    );
    DWORD dwTagAddr = 0;
    sscanf_s(pInsn[j].op_str, "%x", &dwTagAddr);

    // ��ʾ����
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

  // �ͷű���ָ��Ŀռ�
  cs_free(pInsn, count);
  // �رվ��
  cs_close(&handle);
}

// �û���������
void Command(LPVOID lpAddr)
{
  // int3 �ϵ���º�EIP���Լ���1������Ҫ��ԭ��
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

    // ����ϵ����
    else if (!strcmp("bp", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      SetCCBP(g_dbgProcess, (LPVOID)dwBPAddr);
      printf("����ϵ� 0x%08X ���óɹ�\n", dwBPAddr);
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
      printf("����ϵ� 0x%08X ɾ���ɹ�\n", dwCleanBPAddr);
    }
    else if (!strcmp("cbp", cmd))// �����ϵ�
    {
      SetConditionBp(g_dbgProcess);
    }
    // ��������
    else if (!strcmp("t", cmd))
    {
      StepIn();
      g_isStepIn = true;
      break;
    }
    // ��������
    else if (!strcmp("p", cmd))
    {
      StepOver();
      break;
    }
    // ִ�е�����
    else if (!strcmp("ret", cmd))
    {
      // StepOut();
      break;
    }

    // Ӳ���ϵ����
    else if (!strcmp("bhr", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      if (SetBPHW(g_dbgThread, (LPVOID)dwBPAddr, 3, 0, false))
      {
        printf("Ӳ����д�ϵ� 0x%08X ���óɹ�\n", dwBPAddr);
        g_HDPerAddr = (LPVOID)dwBPAddr;
        g_HDType = 3;
        g_isHardBP = true;
      }
      else
      {
        printf("Ӳ����д�ϵ� 0x%08X ����ʧ��\n", dwBPAddr);
      }
    }
    else if (!strcmp("bhw", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      if (SetBPHW(g_dbgThread, (LPVOID)dwBPAddr, 1, 0, false))
      {
        printf("Ӳ��д��ϵ� 0x%08X ���óɹ�\n", dwBPAddr);
        g_HDType = 1;
        g_HDPerAddr = (LPVOID)dwBPAddr;
        g_isHardBP = true;
      }
      else
      {
        printf("Ӳ��д��ϵ� 0x%08X ����ʧ��\n", dwBPAddr);
      }
    }
    else if (!strcmp("bhe", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      if (SetBPHW(g_dbgThread, (LPVOID)dwBPAddr, 0, 0, false))
      {
        printf("Ӳ��ִ�жϵ� 0x%08X ���óɹ�\n", dwBPAddr);
        g_HDType = 0;
        g_HDPerAddr = (LPVOID)dwBPAddr;
        g_isHardBP = true;
      }
      else
      {
        printf("Ӳ��ִ�жϵ� 0x%08X ����ʧ��\n", dwBPAddr);
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
      printf("Ӳ���ϵ� 0x%08X ɾ���ɹ�\n", dwCleanBPAddr);
    }

    // �ڴ�ϵ����
    else if (!strcmp("bmr", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      SetMemBP(g_dbgThread, g_dbgProcess, (LPVOID)dwBPAddr, 0);
      printf("�ڴ��д�ϵ� 0x%08X ���óɹ�\n", dwBPAddr);
      g_isMemBP = true;
      g_lpMemAddr = (LPVOID)dwBPAddr;

    }
    else if (!strcmp("bmw", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      SetMemBP(g_dbgThread, g_dbgProcess, (LPVOID)dwBPAddr, 1);
      printf("�ڴ��д�ϵ� 0x%08X ���óɹ�\n", dwBPAddr);
      g_isMemBP = true;
      g_lpMemAddr = (LPVOID)dwBPAddr;

    }
    else if (!strcmp("bme", cmd))
    {
      DWORD dwBPAddr = 0;
      scanf_s("%X", &dwBPAddr);
      SetMemBP(g_dbgThread, g_dbgProcess, (LPVOID)dwBPAddr, 2);
      printf("�ڴ��д�ϵ� 0x%08X ���óɹ�\n", dwBPAddr);
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
      printf("Ӳ���ϵ� 0x%08X ɾ���ɹ�\n", dwCleanBPAddr);
    }

    // ����
    else if (!strcmp("h", cmd))
    {

    }

    // ��ʾ�����
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



    // �鿴�Ĵ���
    else if (!strcmp("r", cmd))
    {
      QueryReg();
    }
    // �޸ļĴ���
    else if (!strcmp("er", cmd))
    {
      DWORD dwVar = 0;
      scanf_s("%X", &dwVar);
      ModifyReg(dwVar);
    }

    // �༭�ڴ�
    else if (!strcmp("em", cmd))
    {
      DWORD dwAddr = 0;
      scanf_s("%X", &dwAddr);
      DWORD bytes = 0;

      scanf_s("%X", &bytes);
      ModifyMemory(dwAddr, bytes);
    }

    // ��ʾ�ڴ�
    else if (!strcmp("d", cmd))
    {
      DWORD dwVar = 0;
      scanf_s("%X", &dwVar);
      ShowMemory(dwVar);
    }

    // ��ʾģ����Ϣ
    else if (!strcmp("lm", cmd))
    {

      vector<MMODULEINFO> mModule;
      ListModule(mModule);
    }

    else if (!strcmp("k", cmd))//�鿴ջ
    {
      ShowStack();
    }

    // dump
    else if (!strcmp("dump", cmd))
    {
      DumpExe();
    }

    // ����������    
    else if (!strcmp("lexp", cmd))
    {
      ShowExportInfo();
    }
    // ���������
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

// ��������
void StepIn()
{
  CONTEXT ct = { CONTEXT_ALL };
  GetThreadContext(g_dbgThread, &ct);
  ((PEFLAGS)&ct.EFlags)->TF = 1;
  SetThreadContext(g_dbgThread, &ct);
}

// ��������
void StepOver()
{
  //��ǰָ��ִ�е��ĵ�ַ
  CONTEXT ct = { CONTEXT_CONTROL };
  if (!GetThreadContext(g_dbgThread, &ct))
  {
    printf("��ȡ�߳�������ʧ��");
    exit(0);
  }

  unsigned char szOpcode[256] = {}; // Ŀ������е�OPCODE
  csh handle;					    // �����������
  cs_insn* pInsn;			    // ���淴���õ���ָ��Ļ������׵�ַ
  size_t count = 0;       // ����õ��ķ�����ָ������
  DWORD  dwReadSize = 0;	// ʵ�ʶ�ȡ����

    //��ʼ������������,(x86_64�ܹ�,32λģʽ,���)
  cs_open(
    CS_ARCH_X86,    /*x86ָ�*/
    CS_MODE_32,     /*ʹ��32λģʽ����opcode*/
    &handle         /*����ķ������*/
  );

  // ��ȡ������
  ReadProcessMemory(g_dbgProcess, (LPVOID)ct.Eip, szOpcode, 256, &dwReadSize);

  // ��ʼ�����.
// �����᷵���ܹ��õ��˼������ָ��
  count = cs_disasm(handle,		/*����������,��cs_open�����õ�*/
    szOpcode,					/*��Ҫ������opcode�Ļ������׵�ַ*/
    sizeof(szOpcode),			/*opcode���ֽ���*/
    (uint64_t)ct.Eip,			/*opcode�����ڵ��ڴ��ַ*/
    1,				/*��Ҫ������ָ������,�����0,�򷴻���ȫ��*/
    &pInsn/*��������*/
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

// �鿴�Ĵ���
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

// �޸ļĴ���
void ModifyReg(DWORD dwVar)
{
  CONTEXT ct = { CONTEXT_ALL };
  GetThreadContext(g_dbgThread, &ct);
  ct.Eax = dwVar;
  SetThreadContext(g_dbgThread, &ct);
}

// ��ʾ�ڴ���Ϣ
void ShowMemory(DWORD dwVar)
{
  // 5*16���ֽ�
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

// �޸��ڴ�
void ModifyMemory(DWORD dwAddr, DWORD bytes)
{
  DWORD dwWriten = 0;
  WriteProcessMemory(g_dbgProcess, (LPVOID)dwAddr, (LPVOID)&bytes, 4, &dwWriten);
}

// �鿴ģ����Ϣ
void ListModule(std::vector<MMODULEINFO>& mModule)
{
  //1.����ģ�����
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, g_dbgEvent.dwProcessId);
  if (hSnap == INVALID_HANDLE_VALUE) {
    return;
  }

  //2.��һ�α���ģ��
  MODULEENTRY32 stcMd = { sizeof(stcMd) };

  printf("start\t\tsize\tname\n");
  if (Module32First(hSnap, &stcMd))
  {
    //3.ѭ������ģ��Next
    do
    {
      printf("%08X\t%d\t%s\n", (DWORD)stcMd.modBaseAddr, stcMd.modBaseSize, stcMd.szModule);
    } while (Module32Next(hSnap, &stcMd));
  }
  CloseHandle(hSnap);
}

// �鿴��ջ
void ShowStack()
{
  CONTEXT ct = { CONTEXT_ALL };
  //��ȡ�߳�������
  if (!GetThreadContext(g_dbgThread, &ct)) {
    printf("��ȡ�߳�������ʧ��");
    exit(0);
  }

  LPBYTE pByte = new BYTE[100];
  SIZE_T read = 0;
  //��ȡ������
  if (!ReadProcessMemory(g_dbgProcess, (LPCVOID)ct.Esp, pByte, 100, &read))
  {
    printf("��ȡ�ڴ�ʧ��");
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

// �޸Ļ�����
void ModifyDisasm(LPVOID addr)
{
  XEDPARSE xed = { 0 };
  xed.cip = (ULONGLONG)addr;

  // ����ָ��
  printf("ָ�");
  getchar();
  gets_s(xed.instr, XEDPARSE_MAXBUFSIZE);

  // xed.cip, ��������תƫ�Ƶ�ָ��ʱ,��Ҫ��������ֶ�
  if (XEDPARSE_OK != XEDParseAssemble(&xed))
  {
    printf("ָ�����%s\n", xed.error);
    return;
  }


  unsigned char szOpcode[256] = {}; // Ŀ������е�OPCODE
  csh handle;					    // �����������
  cs_insn* pInsn;			    // ���淴���õ���ָ��Ļ������׵�ַ
  size_t count = 0;       // ����õ��ķ�����ָ������
  DWORD  dwReadSize = 0;	// ʵ�ʶ�ȡ����

  //��ʼ������������,(x86_64�ܹ�,32λģʽ,���)
  cs_open(
    CS_ARCH_X86,    /*x86ָ�*/
    CS_MODE_32,     /*ʹ��32λģʽ����opcode*/
    &handle         /*����ķ������*/
  );

  // ��ȡ������
  ReadProcessMemory(g_dbgProcess, (LPVOID)addr, szOpcode, 256, &dwReadSize);

  // ��ʼ�����.
  // �����᷵���ܹ��õ��˼������ָ��
  count = cs_disasm(handle,		/*����������,��cs_open�����õ�*/
    szOpcode,					/*��Ҫ������opcode�Ļ������׵�ַ*/
    sizeof(szOpcode),			/*opcode���ֽ���*/
    (uint64_t)addr,			/*opcode�����ڵ��ڴ��ַ*/
    1,				/*��Ҫ������ָ������,�����0,�򷴻���ȫ��*/
    &pInsn/*��������*/
  );

  //printf("\r\n-------------------------------\r\n");
  //// ��������
  //for (size_t j = 0; j < nLen; j++)
  //{
  //  printf("0x%08X | %-*s %-*s\n",
  //    (int)pInsn[j].address, /*ָ���ַ*/
  //    10, pInsn[j].mnemonic,/*ָ�������*/
  //    15, pInsn[j].op_str/*ָ�������*/
  //  );
  //}
  //printf("-------------------------------\r\n");

  // �ͷű���ָ��Ŀռ�
  cs_free(pInsn, count);
  // �رվ��
  cs_close(&handle);

  size_t nopCount = pInsn->size - xed.dest_size;
  //��OPCODEд���ڴ�
  SIZE_T write1 = 0;
  if (!WriteProcessMemory(g_dbgProcess, addr, xed.dest, xed.dest_size, &write1))
  {
    printf("д������ڴ�ʧ��");
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
  //ɾ�����е��ڴ�ϵ�
  DeleteAllCCBP(g_dbgProcess);

  DWORD nPeSize = 0;				//PEͷ
  DWORD nImageSize = 0;			//�ڴ��д�С
  DWORD nFileSize = 0;			//�ļ���С
  DWORD nSectionNum = 0;			//��������
  PBYTE nPeHeadData = nullptr;	//PE����
  PBYTE nImageBuf = nullptr;		//�ļ�����
  FILE* pFile = nullptr;			//�ļ�ָ��

  nPeHeadData = new BYTE[4096]{};

  //��ȡ�ļ�ͷ��Ϣ

  ReadProcessMemory(g_dbgProcess, g_lpImageBase, nPeHeadData, 4096, NULL);
  //��ȡPE��Ϣ
  PIMAGE_DOS_HEADER nDosHead = (PIMAGE_DOS_HEADER)nPeHeadData;
  PIMAGE_NT_HEADERS nNtHead = (PIMAGE_NT_HEADERS)(nPeHeadData + nDosHead->e_lfanew);
  PIMAGE_SECTION_HEADER nSecetionHead = IMAGE_FIRST_SECTION(nNtHead);

  //PEͷ��С
  nPeSize = nNtHead->OptionalHeader.SizeOfHeaders;
  //�ļ��ĳߴ�
  nImageSize = nNtHead->OptionalHeader.SizeOfImage;
  //��������	
  nSectionNum = nNtHead->FileHeader.NumberOfSections;


  //����exe����Ķѿռ�
  nImageBuf = new BYTE[nImageSize]{};

  //��ȡPE����
  ReadProcessMemory(g_dbgProcess, g_lpImageBase, nImageBuf, nPeSize, NULL);

  nFileSize += nPeSize;
  // ��ȡÿ�����ε�����
  for (DWORD i = 0; i < nSectionNum; i++)
  {
    ReadProcessMemory(g_dbgProcess, (LPVOID)((DWORD)g_lpImageBase + nSecetionHead[i].VirtualAddress),
      nImageBuf + nSecetionHead[i].PointerToRawData, nSecetionHead[i].SizeOfRawData, NULL);

    nFileSize += nSecetionHead[i].SizeOfRawData;
  }

  // �޸��ļ�����
  nDosHead = (PIMAGE_DOS_HEADER)nImageBuf;
  nNtHead = (PIMAGE_NT_HEADERS)((DWORD)nImageBuf + nDosHead->e_lfanew);
  nNtHead->OptionalHeader.FileAlignment = nNtHead->OptionalHeader.SectionAlignment;

  fopen_s(&pFile, ".\\dump.exe", "wb");
  fwrite(nImageBuf, nFileSize, 1, pFile);
  fclose(pFile);

  delete[] nPeHeadData;
  delete[] nImageBuf;

  printf("�ɹ�����.\\mdump.exe\n");
}

// ���ط���
void LoadSymbol(HANDLE hProcess, CREATE_PROCESS_DEBUG_INFO* pInfo)
{
  // ��ʼ�����Ŵ�����
  SymInitialize(hProcess, "C:\\Users\\nSky\\Desktop\\sym", FALSE);
  //SYMLOADINFO tmp;
  //tmp.base = (DWORD64)pInfo->lpBaseOfImage;
  //tmp.hFile = pInfo->hFile;
  //g_vecSymBase.push_back(tmp);

  IMAGEHLP_MODULE64 imageHlp{ 0 };
  imageHlp.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
  SymGetModuleInfo64(hProcess, (DWORD64)pInfo->lpBaseOfImage, &imageHlp);
  // ��������ļ�
  SymLoadModule64(hProcess, pInfo->hFile, NULL, NULL, (DWORD64)pInfo->lpBaseOfImage, 0);
}


long MyExceptFilter()
{

  return EXCEPTION_EXECUTE_HANDLER;// ִ���쳣�����
}


// ��ȡ���Է��ź�������
const char* GetFunSymbolName(SIZE_T nAddress)
{
  __try
  {
    DWORD64  dwDisplacement = 0;
    char* buffer = new char[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]{ 0 };
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;
    //���ݵ�ַ��ȡ������Ϣ



    if (!SymFromAddr(g_dbgSymbol, nAddress, &dwDisplacement, pSymbol))
      return 0;
    return pSymbol->Name;
  }
  __except (MyExceptFilter())
  {

  }
  return 0;
}


// ����PEB
void HidePEB()
{

  // �����ѯ���Ļ�����Ϣ
  struct PROCESS_BASIC_INFORMATION {
    ULONG ExitStatus;		// ���̷�����
    DWORD  PebBaseAddress;  // PEB��ַ
    ULONG AffinityMask;		// CPU�׺�������
    LONG  BasePriority;		// �������ȼ�
    ULONG UniqueProcessId;  // ������PID
    ULONG InheritedFromUniqueProcessId; // ������PID
  }stcProcInfo;
  // Ŀ����̵ľ��
  // ����ͨ��������ȡ��Ŀ����̵� PEB
  NtQueryInformationProcess(
    g_dbgProcess,
    ProcessBasicInformation,
    &stcProcInfo,
    sizeof(stcProcInfo),
    NULL);

  WriteProcessMemory(g_dbgProcess, (LPVOID)(stcProcInfo.PebBaseAddress + 0x02), "", 1, NULL);
  printf("BegingDebugged�Ѿ�����\n");
  // �޸�Ŀ�� PEB.NtGlobalFlag ��ֵΪ 0
  WriteProcessMemory(g_dbgProcess,
    (LPVOID)(stcProcInfo.PebBaseAddress + 0x68),
    "", 1, NULL);
  printf("NtGlobalFlag�Ѿ�����\n");
  //3 PEB.ProcessHeap �ֶ�ָ��� _HEAP �ṹ�е�
  // Flags 0x40�� ForceFlags 0x44����ȷ���Ƿ񱻵��ԣ�
  // ���û�б����ԣ����б����ֵ�ֱ��� 2 �� 0
  LPVOID addr = 0;
  // ��ȡPEB
  ReadProcessMemory(g_dbgProcess, (LPVOID)(stcProcInfo.PebBaseAddress + 0x18), &addr, 4, NULL);
  WriteProcessMemory(g_dbgProcess, (LPVOID)((DWORD)addr + 0x40), "\x02", 1, NULL);
  WriteProcessMemory(g_dbgProcess, (LPVOID)((DWORD)addr + 0x44), "\x00", 1, NULL);
  printf("Flags��ForceFlags�Ѿ�����\n");
  return;
}


// �����ϵ�
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
    //��CC�ϵ�
    g_isCondition = true;
    SetCCBP(hProcess, (LPVOID)addr);
  }
  g_condAddr = addr;
  printf("�����ϵ����óɹ�\n");
}


// ��ʾ�����б�
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


// ����API���Ʋ��ҵ�ַ
DWORD FindApiAddress(HANDLE hProcess, const char* pszName)
{
  DWORD64  dwDisplacement = 0;
  char* buffer = new char[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]{ 0 };
  PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
  pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
  pSymbol->MaxNameLen = MAX_SYM_NAME;
  //���ݵ�ַ��ȡ������Ϣ
  if (!SymFromName(g_dbgSymbol, pszName, pSymbol))
    return 0;
  return (DWORD)pSymbol->Address;
}


// Դ�����
DWORD GetSymLine(DWORD dwAddr, DWORD& dwLine)
{
  //��ȡԴ�ļ��Լ�����Ϣ
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

// ����Դ����
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


  char filename[] = "./CheckDebug.cpp"; //�ļ���
  FILE* fp;
  int WhichLine = dwLine-1;             //ָ��Ҫ��ȡ��һ��
  int CurrentIndex = 0;             //��ǰ��ȡ����
  char StrLine[1024];             //ÿ������ȡ���ַ���,�ɸ���ʵ���������
  fopen_s(&fp, filename, "r");

  while (!feof(fp))
  {

    if (CurrentIndex == WhichLine)
    {
      fgets(StrLine, 1024, fp);  //��ȡһ��

      return StrLine;
    }
    fgets(StrLine, 1024, fp);  //��ȡһ��,����λ����һ��
    CurrentIndex++;

    //printf("%s", StrLine); //���
  }
  fclose(fp);                     //�ر��ļ�
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
//  //1. ��ȡ�߳�������
//
//  GetThreadContext(g_dbgThread, &context);
//  //��ȡԴ�ļ��Լ�����Ϣ
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
////��ʾԴ�ļ���ָ������
//void DisplaySourceLines(PCHAR sourceFile, int lineNum, unsigned int address, int after, int before)
//{
//  //����ӵڼ��п�ʼ���
//  int startLineNumber = lineNum - before;
//  int curLineNumber = 1;
//  //��������Ҫ��ʾ����
//  while (curLineNumber < startLineNumber)
//  {
//    ++curLineNumber;
//  }
//  //�����ǰ�еĵ�ַ
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
//  //������ļ��еĺ�������ѭ���������ǵ�ǰ�����ϵ��λ��
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