#include "pch.h"
#include "ArkOption.h"

#include <Windows.h>
#include <winsvc.h>

ArkOption::ArkOption()
  :m_SerName(L"ArkDrv"),
  m_SerPath(L".\\ArkDrv.sys"),
  m_hSCM(NULL),
  m_hSer(NULL)
{

}

BOOL ArkOption::CreateARK()
{
  // �򿪷�������� 
  m_hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (NULL == m_hSCM)
  {
    OutputDebugStringW(L"�����������ʧ��\n");
    return FALSE;
  }
  OutputDebugStringW(L"�򿪷�����ƹ������ɹ�\n");

  // ����ARK����
  WCHAR szSerPath[MAX_PATH] = { 0 };
  GetFullPathNameW(m_SerPath, MAX_PATH, szSerPath, NULL);
  OutputDebugString(szSerPath);
  m_hSer = CreateServiceW(
    m_hSCM,                   // ������������
    m_SerName,                // ��������
    m_SerName,                // ������ʾ��
    SERVICE_ALL_ACCESS,       // ���������Ȩ��
    SERVICE_KERNEL_DRIVER,    // ���������
    SERVICE_DEMAND_START,     // ������������
    SERVICE_ERROR_IGNORE,     // ���������ƣ���������ô����
    szSerPath,                  // ����������ڵ�·��
    NULL,                     // ���������飬������������˳�� - ��NULL
    NULL,                     // ��������еı�ʶ��ͬһ����������˳�򣬲�������������˳�򣬴�NULL
    NULL,                     // �������������������û���������񣬴�NULL
    NULL,                     // ʹ��windows�ĸ��û������������
    NULL                      // �û�����
  );
  if (NULL == m_hSer)
  {
    DWORD dwErr = GetLastError();
    CString strErr;
    strErr.Format(L"ARK���񴴽�ʧ��:%08X\n", dwErr);
    OutputDebugStringW(strErr);
    if (dwErr != ERROR_IO_PENDING && dwErr != ERROR_SERVICE_EXISTS)
    {
      return FALSE;
    }
  }

  m_hSer = OpenServiceW(m_hSCM, m_SerName, SERVICE_ALL_ACCESS);
  DWORD dwErr = GetLastError();
  CString strErr;
  strErr.Format(L"ARK��������:%08X\n", dwErr);
  OutputDebugStringW(strErr);
  return TRUE;
}

BOOL ArkOption::StartARK()
{
  BOOL bRet = StartServiceW(m_hSer, 0, NULL);
  if (FALSE == bRet)
  {
    CString strErr;
    strErr.Format(L"ARK��������ʧ��:%08X\n", GetLastError());
    OutputDebugStringW(strErr);
    return FALSE;
  }
  OutputDebugStringW(L"ARK���������ɹ�\r\n");
  return TRUE;
}

BOOL ArkOption::StopARK()
{
  SERVICE_STATUS serStatus = { 0 };
  BOOL bRet = ControlService(m_hSer, SERVICE_CONTROL_STOP, &serStatus);
  if (FALSE == bRet)
  {
    OutputDebugStringW(L"ARK����ֹͣʧ��\r\n");
    return FALSE;
  }
  OutputDebugStringW(L"ARK����ֹͣ�ɹ�\r\n");
  return TRUE;
}

BOOL ArkOption::DelARK()
{
  BOOL bRet = DeleteService(m_hSer);
  if (FALSE == bRet)
  {
    OutputDebugStringW(L"ARK����ɾ��ʧ��\r\n");
    return FALSE;
  }

  if (NULL != m_hSer)
  {
    CloseHandle(m_hSer);
    m_hSer = NULL;
  }
  if (NULL != m_hSCM)
  {
    CloseHandle(m_hSCM);
    m_hSCM = NULL;
  }

  OutputDebugStringW(L"ARK����ɾ���ɹ�\r\n");
  return TRUE;
}
