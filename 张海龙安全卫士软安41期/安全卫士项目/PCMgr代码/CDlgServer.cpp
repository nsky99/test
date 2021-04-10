// CDlgServer.cpp: 实现文件
//

#include "pch.h"
#include "PCMgr.h"
#include "CDlgServer.h"
#include "afxdialogex.h"
#include <winsvc.h>


// CDlgServer 对话框

IMPLEMENT_DYNAMIC(CDlgServer, CDialogEx)

CDlgServer::CDlgServer(CWnd* pParent /*=nullptr*/)
  : CDialogEx(IDD_DIALOG_SERVER, pParent)
{

}

CDlgServer::~CDlgServer()
{
}

void CDlgServer::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_SERVER, m_lsServer);
}


BEGIN_MESSAGE_MAP(CDlgServer, CDialogEx)
  ON_WM_SIZE()
  ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVER, &CDlgServer::OnRclickListServer)
  ON_COMMAND(ID_FLUSHSERVER, &CDlgServer::OnFlushserver)
  ON_COMMAND(ID_STARTSERVER, &CDlgServer::OnStartserver)
  ON_COMMAND(ID_STOPSERVER, &CDlgServer::OnStopserver)
END_MESSAGE_MAP()


// CDlgServer 消息处理程序


BOOL CDlgServer::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  在此添加额外的初始化
  m_lsServer.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
  CRect rc;
  m_lsServer.GetClientRect(rc);
  DWORD dwWidth = rc.right / 6;
  CString strColumnName[] = {
    L"服务名",
    L"全路径",
    L"服务启动用户名",
    L"显示名称",
    L"版本类型",
    L"开始类型",
  };
  for (size_t i = 0; i < _countof(strColumnName); i++)
  {
    m_lsServer.InsertColumn(i, strColumnName[i], 0, dwWidth);
  }


  ShowServerInfo();
  return TRUE;  // return TRUE unless you set the focus to a control
                // 异常: OCX 属性页应返回 FALSE
}


void CDlgServer::OnSize(UINT nType, int cx, int cy)
{
  CDialogEx::OnSize(nType, cx, cy);

  if (m_lsServer.GetSafeHwnd())
  {
    CRect rc;
    GetClientRect(rc);
    m_lsServer.MoveWindow(rc, 1);
  }
}


// 显示服务信息
void CDlgServer::ShowServerInfo()
{
  m_vecServiceInfo.clear();
  m_lsServer.DeleteAllItems();
  GetSericesInfo();
  DWORD dwCount = m_vecServiceInfo.size();
  for (size_t i = 0; i < dwCount; i++)
  {
    // 服务名
    m_lsServer.InsertItem(i, m_vecServiceInfo[i].szServiceName);

    // 全路径
    m_lsServer.SetItemText(i, 1, m_vecServiceInfo[i].szBinaryPathName);

    // 服务启动用户名
    m_lsServer.SetItemText(i, 2, m_vecServiceInfo[i].ServiceStartName);

    // 显示名称
    m_lsServer.SetItemText(i, 3, m_vecServiceInfo[i].szDisplayName);

    // 运行状态
    m_lsServer.SetItemText(i, 4, m_vecServiceInfo[i].ServiceType);

    // 开始类型
    m_lsServer.SetItemText(i, 5, m_vecServiceInfo[i].StartType);
  }
}


// 获取服务信息
void CDlgServer::GetSericesInfo()
{
  //1.打开服务管理器，本地服务，默认数据库，权限所有
  SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);

  //2.第一次调用 需要内存大小
  DWORD dwServiceNum = 0;
  DWORD dwSize = 0;
  EnumServicesStatusExW(
    hSCM, //服务句柄
    SC_ENUM_PROCESS_INFO, // 返回的属性 默认的都是这个值
    SERVICE_WIN32,        // 服务类型 应用程序的服务
    SERVICE_STATE_ALL,    // 服务状态，所有
    0,                    // 缓冲区
    0,                    // 缓冲区大小
    &dwSize,              // 需要的大小
    &dwServiceNum,        // 缓冲区的服务个数
    NULL,                 // 默认0
    NULL                  // 默认0
  );

  //3 申请需要的内存大小
  LPENUM_SERVICE_STATUS_PROCESS pEnumSerice = new ENUM_SERVICE_STATUS_PROCESS[dwSize];

  //4. 第二次枚举
  BOOL bstatus = FALSE;
  bstatus = EnumServicesStatusExW(
    hSCM, //服务句柄
    SC_ENUM_PROCESS_INFO, //返回的属性 默认的都是这个值
    SERVICE_WIN32, //服务类型 应用程序的服务
    SERVICE_STATE_ALL, //服务状态，所有
    (PBYTE)pEnumSerice, //缓冲区
    dwSize, //缓冲区大小
    &dwSize,
    &dwServiceNum,
    NULL, NULL
  );


  // 5遍历信息
  CString strTmp;
  SERVICEINFO ServiceInfo;
  for (DWORD i = 0; i < dwServiceNum; i++)
  {

    //获取基础信息
    // 6.服务详细信息
    // 打开服务
    SC_HANDLE hService = OpenServiceW(hSCM,
      pEnumSerice[i].lpServiceName, //服务名
      SERVICE_QUERY_CONFIG //打开权限
    );
    //第一次1获取缓冲区大小
    QueryServiceConfigW(hService, NULL, 0, &dwSize);
    //分配内存
    LPQUERY_SERVICE_CONFIG pServiceConfig =
      new QUERY_SERVICE_CONFIG[dwSize];
    //第二次调用
    QueryServiceConfigW(hService, pServiceConfig, dwSize, &dwSize);
    
    
    //运行状态类型
    switch (pEnumSerice[i].ServiceStatusProcess.dwCurrentState)
    {
    case SERVICE_CONTINUE_PENDING:
      strTmp = L"该服务即将继续";
      break;
    case SERVICE_PAUSE_PENDING:
      strTmp = L"服务正在暂停";
      break;
    case SERVICE_PAUSED:
      strTmp = L"服务已暂停";
      break;
    case SERVICE_RUNNING:
      strTmp = L"服务正在运行";
      break;
    case SERVICE_START_PENDING:
      strTmp = L"服务正在启动";
      break;
    case SERVICE_STOP_PENDING:
      strTmp = L"服务正在停止";
      break;
    case SERVICE_STOPPED:
      strTmp = L"服务已经停止";
      break;
    default:
      break;
    }
    ServiceInfo.ServiceType.Format(L"%s", strTmp.GetBuffer());



    // 服务类型类型
    switch (pServiceConfig->dwStartType)
    {
    case SERVICE_AUTO_START:
      strTmp = L"系统启动期间自动启动的服务";
      break;
    case SERVICE_BOOT_START:
      strTmp = L"系统加载器启动的设备驱动程序";
      break;
    case SERVICE_DEMAND_START:
      strTmp = L"当进程调用 StartService 函数时，服务控制管理器启动的服务";
      break;
    case SERVICE_DISABLED:
      strTmp = L"无法启动的服务";
      break;
    case SERVICE_SYSTEM_START:
      strTmp = L"由IoInitSystem 函数启动的设备驱动程序";
      break;

    }
    ServiceInfo.StartType.Format(L"%s", strTmp.GetBuffer());

    // 服务全路经
    ServiceInfo.szBinaryPathName.Format(L"%s", pServiceConfig->lpBinaryPathName);
    // 服务显示名
    ServiceInfo.szDisplayName.Format(L"%s", pServiceConfig->lpDisplayName); 
    // 服务启动名
    ServiceInfo.ServiceStartName.Format(L"%s", pServiceConfig->lpServiceStartName);
    // 创建的服务名
    ServiceInfo.szServiceName.Format(L"%s", pEnumSerice[i].lpServiceName); 


    m_vecServiceInfo.push_back(ServiceInfo);
    CloseServiceHandle(hService);
    delete[] pServiceConfig;
  }
  //关闭服务进程
  CloseServiceHandle(hSCM);
  delete[] pEnumSerice;
}


void CDlgServer::OnRclickListServer(NMHDR* pNMHDR, LRESULT* pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

  POINT point;
  GetCursorPos(&point);

  // 加载主菜单
  CMenu popMainMenu;
  popMainMenu.LoadMenuW(IDR_MENU_POP);

  CMenu* pSubMenu = popMainMenu.GetSubMenu(5);
  pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

  *pResult = 0;
}


void CDlgServer::OnFlushserver()
{
  ShowServerInfo();
}


void CDlgServer::OnStartserver()
{
  DWORD dwCurSel = m_lsServer.GetSelectionMark();
  CString strServerName = m_lsServer.GetItemText(dwCurSel, 0);
  if (StartThisVerices(strServerName))
  {
    MessageBoxW(L"服务打开成功", L"Success", MB_OK);
    m_lsServer.SetItemText(dwCurSel, 4, L"服务正在运行");
  }
  else
  {
    MessageBoxW(L"服务打开失败", L"Error", MB_ICONERROR);
  }

}


void CDlgServer::OnStopserver()
{
  DWORD dwCurSel = m_lsServer.GetSelectionMark();
  CString strServerName = m_lsServer.GetItemText(dwCurSel, 0);
  if (CloseThisVerices(strServerName))
  {
    MessageBoxW(L"服务关闭成功", L"Success", MB_OK);
    m_lsServer.SetItemText(dwCurSel, 4, L"服务已停止");
  }
  else
  {
    MessageBoxW(L"服务关闭失败", L"Error", MB_ICONERROR);
  }
}


// 关闭服务
BOOL CDlgServer::CloseThisVerices(CString serverName)
{
  SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (hSCManager != NULL) {
    SC_HANDLE hService = OpenService(hSCManager, serverName, SERVICE_STOP);
    if (hService != NULL)
    {
      SERVICE_STATUS status;
      if (ControlService(hService, SERVICE_CONTROL_STOP, &status))
      {
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return TRUE;
      }
      CloseServiceHandle(hService);
      CloseServiceHandle(hSCManager);
      return FALSE;
    }
    CloseServiceHandle(hSCManager);
    return FALSE;
  }
  else return FALSE;
}

// 启动服务
BOOL CDlgServer::StartThisVerices(CString serverName)
{
  SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
  if (hSCManager != NULL) {
    SC_HANDLE hService = OpenService(hSCManager, serverName, SERVICE_START);
    if (hService != NULL) {
      if (StartService(hService, NULL, NULL))
      {
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return TRUE;
      }
      CloseServiceHandle(hService);
      CloseServiceHandle(hSCManager);
      return FALSE;
    }
    CloseServiceHandle(hSCManager);
    return FALSE;
  }
  else return FALSE;
}
