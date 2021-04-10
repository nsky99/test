#include "pch.h"
#include "CSnapShot.h"
#include <Psapi.h>
#include <TlHelp32.h>
// ��ѯ���н���
DWORD CSnapShot::QueryAllProcess()
{
  m_vecPInfo.clear();
  m_dwProcessCount = 0;
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (INVALID_HANDLE_VALUE == hSnapshot)
    return SNAPSHOT_ERROR;
  PROCESSENTRY32W pew = { sizeof(pew) };

  if (FALSE == Process32FirstW(hSnapshot, &pew))
    return SNAPSHOT_ERROR;  // û���ҵ�����

  ProcessInfo tmpPInfo = { 0 };
  do
  {
    // ��������
    tmpPInfo.strProcessName.Format(L"%s", pew.szExeFile);

    // PID
    tmpPInfo.strPid.Format(L"%d", pew.th32ProcessID);

    // ����ʹ�õĹ�������С
    tmpPInfo.strWorkingSet = GetProcessMemUsed(pew.th32ProcessID);

    // ���ȼ�
    tmpPInfo.strPriority = GetProcessPriority(pew.th32ProcessID);

    // �����
    tmpPInfo.strHandleNumber = GetProcessHandleNumber(pew.th32ProcessID);

    // �߳���
    tmpPInfo.strThreadNumber.Format(L"%d", pew.cntThreads);

    // ������ID
    tmpPInfo.strFatherPid.Format(L"%d", pew.th32ParentProcessID);

    // ·������
    tmpPInfo.strProcessPath = GetProcessFullPath(pew.th32ProcessID);

    m_vecPInfo.push_back(tmpPInfo);

    // ���̼���
    m_dwProcessCount++;
  } while (Process32NextW(hSnapshot, &pew));
  CloseHandle(hSnapshot);
  return SNAPSHOT_SUCCESS;
}

// ��ѯָ�����̵��߳�
DWORD CSnapShot::QueryProcessThread(DWORD dwPid)
{
  m_vecTInfo.clear();
  m_dwThreadCount = 0;
  HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if (INVALID_HANDLE_VALUE == hSnapshot)
    return SNAPSHOT_ERROR;
  THREADENTRY32 te = { sizeof(te) };

  if (FALSE == Thread32First(hSnapshot, &te))
    return SNAPSHOT_ERROR;  // û���ҵ��߳�

  ThreadInfo tmpTInfo = { 0 };
  do
  {
    // �ҵ���
    if (te.th32OwnerProcessID == dwPid)
    {
      // �߳�ID
      tmpTInfo.strTid.Format(L"%d", te.th32ThreadID);

      // ����ID
      tmpTInfo.strPid.Format(L"%d", te.th32OwnerProcessID);

      // ��������
      tmpTInfo.strProcessName = GetProcessName(te.th32OwnerProcessID);

      // ���ȼ�
      tmpTInfo.strPriority.Format(L"%d", te.tpBasePri);

      // ״̬
      tmpTInfo.strStaus = GetThreadStaus(te.th32ThreadID);

      m_dwThreadCount++;
      // �����߳�
      m_vecTInfo.push_back(tmpTInfo);
    }
  } while (Thread32Next(hSnapshot, &te));
  CloseHandle(hSnapshot);
  return SNAPSHOT_SUCCESS;
}

// ��ѯָ�����̵�ģ��
DWORD CSnapShot::QueryProcessModel(DWORD dwPid)
{
  // ��ʼ�������ʹ�С
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
      //ģ����
    tmpMInfo.strModelName = mew.szModule;

      //����ID
    tmpMInfo.strPid.Format(L"%d", dwPid);

      //��ʼ��ַ
    tmpMInfo.strStartAddr.Format(L"0x%08X", (DWORD)mew.modBaseAddr);

      //ģ���С
    tmpMInfo.strModelSize.Format(L"%d K", mew.modBaseSize / 1024);

      //ģ��·��
    tmpMInfo.strModelPath = mew.szExePath;

    // ����vector
    m_dwModelCount++;
    m_vecMInfo.push_back(tmpMInfo);
  } while (Module32NextW(hSnapshot, &mew));

  CloseHandle(hSnapshot);
  return SNAPSHOT_SUCCESS;
}

// ��ѯ���б���Ϣ
DWORD CSnapShot::QueryProcessHeapList(DWORD dwPid)
{
  // ��ʼ�������ʹ�С
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
    // ����ID
    tmpHLInfo.strPid.Format(L"%d", dwPid);

    // �ѱ�ʶ
    tmpHLInfo.strHeapID.Format(L"%08X", hl.th32HeapID);

    // �ѱ�־
    tmpHLInfo.strHeapFlag.Format(L"%s", hl.dwFlags == HF32_DEFAULT ? L"����Ĭ�϶�" : L"�û������");

    m_dwHeapListCount++;
    m_vecHLInfo.push_back(tmpHLInfo);
  } while (Heap32ListNext(hSnapshot, &hl));

  CloseHandle(hSnapshot);
  return SNAPSHOT_SUCCESS;
}

// ��ѯ����Ϣ
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
      // �ѿ���
      tmpHInfo.strHeaphandle.Format(L"%08X", (DWORD)he.hHandle);

      // ����ʼ��ַ
      tmpHInfo.strStartAddress.Format(L"%08X", he.dwAddress);

      // �ѿ�Ĵ�С
      tmpHInfo.strBlockSize.Format(L"%08X", he.dwBlockSize);

      // �ѿ�ı�־
      tmpHInfo.strFlag = GetHeapFlag(he.dwFlags);

      // ��������ID
      tmpHInfo.strPid.Format(L"%d", he.th32ProcessID);

      m_dwHeapCount++;
      m_vecHInfo.push_back(tmpHInfo);
    } while (Heap32Next(&he));
  }
  DWORD dw = GetLastError();
  return SNAPSHOT_SUCCESS;
}

// ��ȡ�����ڴ�ʹ����
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


// ��ȡ�������ȼ�
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
    strPriority.Format(L"%s", L"��������");
    break;
  case BELOW_NORMAL_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"��������");
    break;
  case HIGH_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"��");
    break;
  case IDLE_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"��");
    break;
  case NORMAL_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"����");
    break;
  case REALTIME_PRIORITY_CLASS:
    strPriority.Format(L"%s", L"ʵʱ");
    break;
  default:
    strPriority.Format(L"%s", L"��");
    break;
  }
  return strPriority;
}


// ��ȡ���̾������
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


// ��ȡ����ȫ·��
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


// ��ȡ�̵߳Ľ�������
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

// ��ȡ�߳�״̬
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
    strThreadStaus = L"����";
    return strThreadStaus;
  }

  SuspendThread(hThread);
  CloseHandle(hThread);
  return L"��ͣ";
}


// ��ȡ�ѵı���
CString CSnapShot::GetHeapFlag(DWORD dwFlag)
{
  CString strFlag;
  switch (dwFlag)
  {
  case LF32_FIXED:
    strFlag = L"�̶��ڴ��";
    break;
  case LF32_FREE:
    strFlag = L"δʹ���ڴ��";
    break;
  case LF32_MOVEABLE:
    strFlag = L"���ƶ��ڴ��";
    break;
  default:
    strFlag = L"";
    break;
  }

  return strFlag;
}
