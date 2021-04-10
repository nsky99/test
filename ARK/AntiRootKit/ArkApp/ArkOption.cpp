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
  // 打开服务管理器 
  m_hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (NULL == m_hSCM)
  {
    OutputDebugStringW(L"服务管理器打开失败\n");
    return FALSE;
  }
  OutputDebugStringW(L"打开服务控制管理器成功\n");

  // 创建ARK服务
  WCHAR szSerPath[MAX_PATH] = { 0 };
  GetFullPathNameW(m_SerPath, MAX_PATH, szSerPath, NULL);
  OutputDebugString(szSerPath);
  m_hSer = CreateServiceW(
    m_hSCM,                   // 服务管理器句柄
    m_SerName,                // 服务名称
    m_SerName,                // 服务显示名
    SERVICE_ALL_ACCESS,       // 创建服务的权限
    SERVICE_KERNEL_DRIVER,    // 服务的类型
    SERVICE_DEMAND_START,     // 服务启动类型
    SERVICE_ERROR_IGNORE,     // 服务错误控制，出错了怎么处理
    szSerPath,                  // 服务程序所在的路径
    NULL,                     // 服务所在组，不关心组启动顺序 - 传NULL
    NULL,                     // 服务分组中的标识，同一分组中驱动顺序，不关心组内启动顺序，传NULL
    NULL,                     // 服务的依赖启动，这里没有依赖服务，传NULL
    NULL,                     // 使用windows哪个用户启动这个服务
    NULL                      // 用户密码
  );
  if (NULL == m_hSer)
  {
    DWORD dwErr = GetLastError();
    CString strErr;
    strErr.Format(L"ARK服务创建失败:%08X\n", dwErr);
    OutputDebugStringW(strErr);
    if (dwErr != ERROR_IO_PENDING && dwErr != ERROR_SERVICE_EXISTS)
    {
      return FALSE;
    }
  }

  m_hSer = OpenServiceW(m_hSCM, m_SerName, SERVICE_ALL_ACCESS);
  DWORD dwErr = GetLastError();
  CString strErr;
  strErr.Format(L"ARK服务句柄打开:%08X\n", dwErr);
  OutputDebugStringW(strErr);
  return TRUE;
}

BOOL ArkOption::StartARK()
{
  BOOL bRet = StartServiceW(m_hSer, 0, NULL);
  if (FALSE == bRet)
  {
    CString strErr;
    strErr.Format(L"ARK服务启动失败:%08X\n", GetLastError());
    OutputDebugStringW(strErr);
    return FALSE;
  }
  OutputDebugStringW(L"ARK服务启动成功\r\n");
  return TRUE;
}

BOOL ArkOption::StopARK()
{
  SERVICE_STATUS serStatus = { 0 };
  BOOL bRet = ControlService(m_hSer, SERVICE_CONTROL_STOP, &serStatus);
  if (FALSE == bRet)
  {
    OutputDebugStringW(L"ARK服务停止失败\r\n");
    return FALSE;
  }
  OutputDebugStringW(L"ARK服务停止成功\r\n");
  return TRUE;
}

BOOL ArkOption::DelARK()
{
  BOOL bRet = DeleteService(m_hSer);
  if (FALSE == bRet)
  {
    OutputDebugStringW(L"ARK服务删除失败\r\n");
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

  OutputDebugStringW(L"ARK服务删除成功\r\n");
  return TRUE;
}
