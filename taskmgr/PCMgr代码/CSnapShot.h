#pragma once
#include <vector>
using std::vector;

class CSnapShot
{
  // ˽�����ڽṹ��/ö��
private:
  // ������
  enum ERROR_STATUS
  {
    SNAPSHOT_ERROR,
    SNAPSHOT_SUCCESS,
  };

  // ������Ϣ
  typedef struct _ProcessInfo
  {
    CString strProcessName; // ��������
    CString strPid;         // PID
    CString strWorkingSet;  // ����ʹ�õĹ�������С
    CString strPriority;    // ���ȼ�
    CString strHandleNumber;// �����
    CString strThreadNumber;// �߳���
    CString strFatherPid;   // ������ID
    CString strProcessPath; // ·������
  }ProcessInfo, * PProcessInfo;

  // �߳���Ϣ
  typedef struct _ThreadInfo
  {
    CString strTid;        // �߳�ID
    CString strPid;        // ����ID
    CString strProcessName;// ��������
    CString strPriority;   // ���ȼ�
    CString strStaus;      // ״̬
  }ThreadInfo,*PThreadInfo;

  // ģ����Ϣ
  typedef struct _ModelInfo
  {
    CString strModelName; //ģ����
    CString strPid;       //����ID
    CString strStartAddr; //��ʼ��ַ
    CString strModelSize; //ģ���С
    CString strModelPath; //ģ��·��
  }ModelInfo, * PModelInfo;

  // ���б���Ϣ
  typedef struct _HeapListInfo
  {
    CString strPid;     // ����ID
    CString strHeapID;  // �ѱ�ʶ�� 
    CString strHeapFlag;// �ѱ�־
  }HeapListInfo, * PHeapListInfo;

  // ����Ϣ
  typedef struct _HeapInfo
  {
    CString strHeaphandle;   // �ѿ���
    CString strStartAddress; // ����ʼ��ַ
    CString strBlockSize;    // �ѿ�Ĵ�С
    CString strFlag;         // �ѿ�ı�־
    CString strPid;          // ��������ID
  }HeapInfo, * PHeapInfo;

  // �ⲿ�ӿ�
public:
  // ��ѯ���н���
  DWORD QueryAllProcess();
  // ��ѯָ�����̵��߳�
  DWORD QueryProcessThread(DWORD dwPid);
  // ��ѯָ�����̵�ģ��
  DWORD QueryProcessModel(DWORD dwPid);
  // ��ѯ���б���Ϣ
  DWORD QueryProcessHeapList(DWORD dwPid);
  // ��ѯ����Ϣ
  DWORD QueryProcessHeap(DWORD dwPid, DWORD dwHeapID);
public:
  // ���������Ϣ
  vector<ProcessInfo> m_vecPInfo;
  // ���̸���
  DWORD               m_dwProcessCount;


  // �����߳���Ϣ
  vector<ThreadInfo>  m_vecTInfo;
  // �̸߳���
  DWORD               m_dwThreadCount;

  // ����ģ����Ϣ
  vector<ModelInfo>   m_vecMInfo;
  // ģ�����
  DWORD               m_dwModelCount;

  // ������б���Ϣ
  vector<HeapListInfo> m_vecHLInfo;
  // ���б����
  DWORD                m_dwHeapListCount;

  // �������Ϣ
  vector<HeapInfo>     m_vecHInfo;
  // �Ѹ���
  DWORD                m_dwHeapCount;
private:
  // ��ȡ�����ڴ�ʹ����
  CString GetProcessMemUsed(DWORD dwPid);
  // ��ȡ�������ȼ�
  CString GetProcessPriority(DWORD dwPid);
  // ��ȡ���̾������
  CString GetProcessHandleNumber(DWORD dwPid);
  // ��ȡ����ȫ·��
  CString GetProcessFullPath(DWORD dwPid);
  // ��ȡ�߳̽�����
  CString GetProcessName(DWORD dwPid);
  // ��ȡ�߳�״̬
  CString GetThreadStaus(DWORD dwTid);
public:
  // ��ȡ�ѵı���
  CString GetHeapFlag(DWORD dwFlag);
};

