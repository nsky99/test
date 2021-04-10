#pragma once
#include <vector>
using std::vector;

class CSnapShot
{
  // 私有类内结构体/枚举
private:
  // 错误码
  enum ERROR_STATUS
  {
    SNAPSHOT_ERROR,
    SNAPSHOT_SUCCESS,
  };

  // 进程信息
  typedef struct _ProcessInfo
  {
    CString strProcessName; // 进程名称
    CString strPid;         // PID
    CString strWorkingSet;  // 正在使用的工作集大小
    CString strPriority;    // 优先级
    CString strHandleNumber;// 句柄数
    CString strThreadNumber;// 线程数
    CString strFatherPid;   // 父进程ID
    CString strProcessPath; // 路径名称
  }ProcessInfo, * PProcessInfo;

  // 线程信息
  typedef struct _ThreadInfo
  {
    CString strTid;        // 线程ID
    CString strPid;        // 进程ID
    CString strProcessName;// 进程名称
    CString strPriority;   // 优先级
    CString strStaus;      // 状态
  }ThreadInfo,*PThreadInfo;

  // 模块信息
  typedef struct _ModelInfo
  {
    CString strModelName; //模块名
    CString strPid;       //进程ID
    CString strStartAddr; //起始地址
    CString strModelSize; //模块大小
    CString strModelPath; //模块路径
  }ModelInfo, * PModelInfo;

  // 堆列表信息
  typedef struct _HeapListInfo
  {
    CString strPid;     // 进程ID
    CString strHeapID;  // 堆标识符 
    CString strHeapFlag;// 堆标志
  }HeapListInfo, * PHeapListInfo;

  // 堆信息
  typedef struct _HeapInfo
  {
    CString strHeaphandle;   // 堆块句柄
    CString strStartAddress; // 堆起始地址
    CString strBlockSize;    // 堆块的大小
    CString strFlag;         // 堆块的标志
    CString strPid;          // 所属进程ID
  }HeapInfo, * PHeapInfo;

  // 外部接口
public:
  // 查询所有进程
  DWORD QueryAllProcess();
  // 查询指定进程的线程
  DWORD QueryProcessThread(DWORD dwPid);
  // 查询指定进程的模块
  DWORD QueryProcessModel(DWORD dwPid);
  // 查询堆列表信息
  DWORD QueryProcessHeapList(DWORD dwPid);
  // 查询堆信息
  DWORD QueryProcessHeap(DWORD dwPid, DWORD dwHeapID);
public:
  // 保存进程信息
  vector<ProcessInfo> m_vecPInfo;
  // 进程个数
  DWORD               m_dwProcessCount;


  // 保存线程信息
  vector<ThreadInfo>  m_vecTInfo;
  // 线程个数
  DWORD               m_dwThreadCount;

  // 保存模块信息
  vector<ModelInfo>   m_vecMInfo;
  // 模块个数
  DWORD               m_dwModelCount;

  // 保存堆列表信息
  vector<HeapListInfo> m_vecHLInfo;
  // 堆列表个数
  DWORD                m_dwHeapListCount;

  // 保存堆信息
  vector<HeapInfo>     m_vecHInfo;
  // 堆个数
  DWORD                m_dwHeapCount;
private:
  // 获取进程内存使用量
  CString GetProcessMemUsed(DWORD dwPid);
  // 获取进程优先级
  CString GetProcessPriority(DWORD dwPid);
  // 获取进程句柄个数
  CString GetProcessHandleNumber(DWORD dwPid);
  // 获取进程全路径
  CString GetProcessFullPath(DWORD dwPid);
  // 获取线程进程名
  CString GetProcessName(DWORD dwPid);
  // 获取线程状态
  CString GetThreadStaus(DWORD dwTid);
public:
  // 获取堆的标致
  CString GetHeapFlag(DWORD dwFlag);
};

