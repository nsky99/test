#include "pch.h"
#include "CSnapShot.h"
#include <Psapi.h>
#include <TlHelp32.h>
// 查询所有进程
DWORD CSnapShot::QueryAllProcess()
{
  m_vecPInfo.clear();
  m_dwProcessCount = 0;
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hSnapshot)
    return SNAPSHOT_ERROR;
  PROCESSENTRY32W pew = { sizeof(pew) };

  if (FALSE == Process32FirstW(hSnapshot, &pew))
    return SNAPSHOT_ERROR;  // 没有找到进程

  ProcessInfo tmpPInfo = { 0 };
  do
  {
    // 进程名称
    tmpPInfo.strProcessName.Format(L"%s", pew.szExeFile);

    // PID
    tmpPInfo.strPid.Format(L"%d", pew.th32ProcessID);

    // 正在使用的工作集大小
    tmpPInfo.strWorkingSet = GetProcessMemUsed(pew.th32ProcessID);

    // 优先级
    tmpPInfo.strPriority = GetProcessPriority(pew.th32ProcessID);

    // 句柄数
    tmpPInfo.strHandleNumber = GetProcessHandleNumber(pew.th32ProcessID);

    // 线程数
    tmpPInfo.strThreadNumber.Format(L"%d", pew.cntThreads);

    // 父进程ID
    tmpPInfo.strFatherPid.Format(L"%d", pew.th32ParentProcessID);

    // 路径名称
    tmpPInfo.strProcessPath = GetProcessFullPath(pew.th32ProcessID);

    m_vecPInfo.push_back(tmpPInfo);

    // 进程计数
    m_dwProcessCount++;
  } while (Process32NextW(hSnapshot, &pew));
  CloseHandle(hSnapshot);
  return SNAPSHOT_SUCCESS;
}

// 查询指定进程的线程
DWORD CSnapShot::QueryProcessThread(DWORD dwPid)
{
  m_vecTInfo.clear();
  m_dwThreadCount = 0;
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (INVALID_HANDLE_VALUE == hSnapshot)
    return SNAPSHOT_ERROR;
  THREADENTRY32 te = { sizeof(te) };

  if (FALSE == Thread32First(hSnapshot, &te))
    return SNAPSHOT_ERROR;  // 没有找到线程

  ThreadInfo tmpTInfo = { 0 };
  do
  {
    // 找到了
    if (te.th32OwnerProcessID == dwPid)
    {
      // 线程ID
      tmpTInfo.strTid.Format(L"%d", te.th32ThreadID);

      // 进程ID
      tmpTInfo.strPid.Format(L"%d", te.th32OwnerProcessID);

      // 进程名称
      tmpTInfo.strProcessName = GetProcessName(te.th32OwnerProcessID);

      // 优先级
      tmpTInfo.strPriority.Format(L"%d", te.tpBasePri);

      // 状态
      tmpTInfo.strStaus = GetThreadStaus(te.th32ThreadID);

      m_dwThreadCount++;
      // 保存线程
      m_vecTInfo.push_back(tmpTInfo);
    }
  } while (Thread32Next(hSnapshot, &te));
  CloseHandle(hSnapshot);
  return SNAPSHOT_SUCCESS;
}

// 查询指定进程的模块
DWORD CSnapShot::QueryProcessModel(DWORD dwPid)
{
  // 初始化容器和大小
  m_vecMInfo.clear();
  m_dwModelCount = 0;
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
  if (INVALID_HANDLE_VALUE == hSnapshot)
    return SNAPSHOT_ERROR;

  MODULEENTRY32W mew = { sizeof(mew) };
  ModelInfo tmpMInfo = { 0 };
  Module32FirstW(hSnapshot, &mew);
  do
  {
      //模块名
    tmpMInfo.strModelName = mew.szModule;

      //进程ID
    tmpMInfo.strPid.Format(L"%d", dwPid);

      //起始地址
    tmpMInfo.strStartAddr.Format(L"0x%08X", (DWORD)mew.modBaseAddr);

      //模块大小
    tmpMInfo.strModelSize.Format(L"%d K", mew.modBaseSize / 1024);

      //模块路径
    tmpMInfo.strModelPath = mew.szExePath;

    // 加入vector
    m_dwModelCount++;
    m_vecMInfo.push_back(tmpMInfo);
  } while (Module32NextW(hSnapshot, &mew));

  CloseHandle(hSnapshot);
  return SNAPSHOT_SUCCESS;
}

// 查询堆列表信息
DWORD CSnapShot::QueryProcessHeapList(DWORD dwPid)
{
  // 初始化容器和大小
  m_vecHLInfo.clear();
  m_dwHeapListCount = 0;
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, dwPid);
  if (INVALID_HANDLE_VALUE == hSnapshot)
    return SNAPSHOT_ERROR;


  HEAPLIST32 hl = { sizeof(hl) };
  Heap32ListFirst(hSnapshot, &hl);
  HeapListInfo tmpHLInfo = { 0 };
  do
  {
    // 进程ID
    tmpHLInfo.strPid.Format(L"%d", dwPid);

    // 堆标识
    tmpHLInfo.strHeapID.Format(L"%08X", hl.th32HeapID);

    // 堆标志
    tmpHLInfo.strHeapFlag.Format(L"%s", hl.dwFlags == HF32_DEFAULT ? L"进程默认堆" : L"用户申请堆");

    m_dwHeapListCount++;
    m_vecHLInfo.push_back(tmpHLInfo);
  } while (Heap32ListNext(hSnapshot, &hl));

  CloseHandle(hSnapshot);
  return SNAPSHOT_SUCCESS;
}

// 查询堆信息
DWORD CSnapShot::QueryProcessHeap(DWORD dwPid,DWORD dwHeapID)
{
  m_dwHeapCount = 0;
  m_vecHInfo.clear();
  HEAPENTRY32 he = { };
  he.dwSize = sizeof(he);

  if (Heap32First(&he, dwPid, dwHeapID))
  {
    HeapInfo tmpHInfo = { 0 };
    do
    {
      // 堆块句柄
      tmpHInfo.strHeaphandle.Format(L"%08X", (DWORD)he.hHandle);

      // 堆起始地址
      tmpHInfo.strStartAddress.Format(L"%08X", he.dwAddress);

      // 堆块的大小
      tmpHInfo.strBlockSize.Format(L"%08X", he.dwBlockSize);

      // 堆块的标志
      tmpHInfo.strFlag = GetHeapFlag(he.dwFlags);

      // 所属进程ID
      tmpHInfo.strPid.Format(L"%d", he.th32ProcessID);

      m_dwHeapCount++;
      m_vecHInfo.push_back(tmpHInfo);
    } while (Heap32Next(&he));
  }
  DWORD dw = GetLastError();
  return SNAPSHOT_SUCCESS;
}

// 获取进程内存使用量
CString CSnapShot::GetProcessMemUsed(DWORD dwPid)
{
  CString strUsed;
  PROCESS_MEMORY_COUNTERS Mem = { sizeof(Mem) };
  HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);
  if (0 == hP)
    return L"";
  BOOL bRet = K32GetProcessMemoryInfo(hP, &Mem, sizeof(Mem));
  if (!bRet)
    return L"";
  CloseHandle(hP);
  strUsed.Format(L"%d K", Mem.WorkingSetSize / 1024);
  return strUsed;
}


// 获取进程优先级
CString CSnapShot::GetProcessPriority(DWORD dwPid)
{
  CString strPriority;
  DWORD dwPriority = 0;
  HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);
  if (0 == hP)
    return L"";
  dwPriority = GetPriorityClass(hP);
  CloseHandle(hP);
  switch (dwPriority)
  {
  case ABOVE_NORMAL_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"高于正常");
    break;
  case BELOW_NORMAL_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"低于正常");
    break;
  case HIGH_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"高");
    break;
  case IDLE_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"低");
    break;
  case NORMAL_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"正常");
    break;
  case REALTIME_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"实时");
    break;
  default:
    strPriority.Format(L"%s", L"无");
    break;
  }
  return strPriority;
}


// 获取进程句柄个数
CString CSnapShot::GetProcessHandleNumber(DWORD dwPid)
{
  CString strHandleNumber;
  DWORD dwHandleNumber = 0;
  HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);
  if (0 == hP)
    return L"";
  BOOL bRet = GetProcessHandleCount(hP, &dwHandleNumber);
  if (!bRet)
    return L"";
  strHandleNumber.Format(L"%d", dwHandleNumber);
  CloseHandle(hP);
  return strHandleNumber;
}


// 获取进程全路径
CString CSnapShot::GetProcessFullPath(DWORD dwPid)
{
  CString strFullPath;
  DWORD dwPathLen = MAX_PATH;
  HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);
  if (0 == hP)
    return L"";
  BOOL bRet = QueryFullProcessImageNameW(hP, 0, strFullPath.GetBuffer(MAX_PATH), &dwPathLen);
  if(0 == bRet)
    return L"";
  CloseHandle(hP);
  return strFullPath;
}


// 获取线程的进程名称
CString CSnapShot::GetProcessName(DWORD dwPid)
{
  CString strProcessName;
  PROCESSENTRY32W pew;
  pew.dwSize = sizeof(pew);
  HANDLE nSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == nSnapShot)
    return L"";
  BOOL nRet = Process32FirstW(nSnapShot, &pew);
  if (0 == nRet)
  {
    CloseHandle(nSnapShot);
    return L"";
  }

  do
  {
    if (dwPid == pew.th32ProcessID)
    {
      strProcessName = pew.szExeFile;
      CloseHandle(nSnapShot);
      return strProcessName;
    }
  } while (Process32NextW(nSnapShot, &pew));

  CloseHandle(nSnapShot);
  return L"";
}

// 获取线程状态
CString CSnapShot::GetThreadStaus(DWORD dwTid)
{
  CString strThreadStaus;

  HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, 0, dwTid);
  if (0 == hThread)
    return L"";
  DWORD dwSuspend = ResumeThread(hThread);
  if (dwSuspend == 0)
  {
    CloseHandle(hThread);
    strThreadStaus = L"运行";
    return strThreadStaus;
  }

  SuspendThread(hThread);
  CloseHandle(hThread);
  return L"暂停";
}


// 获取堆的标致
CString CSnapShot::GetHeapFlag(DWORD dwFlag)
{
  CString strFlag;
  switch (dwFlag)
  {
  case LF32_FIXED:
    strFlag = L"固定内存块";
    break;
  case LF32_FREE:
    strFlag = L"未使用内存块";
    break;
  case LF32_MOVEABLE:
    strFlag = L"可移动内存块";
    break;
  default:
    strFlag = L"";
    break;
  }

  return strFlag;
}
