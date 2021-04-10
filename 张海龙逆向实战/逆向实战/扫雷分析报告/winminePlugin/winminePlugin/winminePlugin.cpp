// winminePlugin.cpp: 定义 DLL 的初始化例程。
//

#include "pch.h"
#include "framework.h"
#include "winminePlugin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为以下项中的第一个语句:
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CwinminePluginApp

BEGIN_MESSAGE_MAP(CwinminePluginApp, CWinApp)
END_MESSAGE_MAP()


// CwinminePluginApp 构造

CwinminePluginApp::CwinminePluginApp()
{
  // TODO:  在此处添加构造代码，
  // 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CwinminePluginApp 对象

CwinminePluginApp theApp;
typedef BYTE(*Base)[0x20];
PDWORD g_pMineCount = (PDWORD)0x01005330;    // 雷的数量
PDWORD g_pWidth = (PDWORD)0x01005334;        // 地图的宽
PDWORD g_pHight = (PDWORD)0x01005338;        // 地图的高
Base  g_pBase =   (Base)0x01005340;
HWND g_hwinmineWnd = 0;											// 扫雷窗口
WNDPROC g_OldWndProc = (WNDPROC)0x1001BC9;  // 扫雷原窗口回调
#define MINE 0x8F
LRESULT CALLBACK NewWndProc(
  _In_ HWND   hwnd,
  _In_ UINT   uMsg,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
)
{

  CString StrDbg;
  if (uMsg == WM_KEYDOWN && wParam == VK_F5)
  {
    
    for (size_t y = 1; y < (*g_pHight) + 1; y++)
    {
      StrDbg = _T("");
      for (size_t x = 1; x < (*g_pWidth) + 1; x++)
      {
        DWORD wXPos = (x << 4) - 0x4;
        DWORD wYPos = (y << 4) + 0x27;
        BYTE bCode = g_pBase[y][x];
        if (MINE == bCode)
        {
          SendMessage(hwnd, WM_RBUTTONDOWN, 0, MAKELPARAM(wXPos, wYPos));
          SendMessage(hwnd, WM_RBUTTONUP, 0, MAKELPARAM(wXPos, wYPos));
        }
        else
        {
          SendMessage(hwnd, WM_LBUTTONDOWN, 0, MAKELPARAM(wXPos, wYPos));
          SendMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(wXPos, wYPos));
        }
        CString strTmp;
        strTmp.Format(_T("%02X "), bCode);
        StrDbg += strTmp;
      }
      ::OutputDebugString(StrDbg);
    }
  }
  else if (uMsg == WM_MOUSEMOVE)
  {
    CString strTitle;

    WORD wX = GET_X_LPARAM(lParam);
    WORD wY = GET_Y_LPARAM(lParam);
    WORD wPosY = (wY - 0x27) >> 0x4;
    WORD wPosX = (wX + 0x4) >> 0x4;
    if (0 < wPosX && wPosX <= *g_pWidth 
      &&0 < wPosY && wPosY <= *g_pHight)
    {
      BYTE bCode = g_pBase[wPosY][wPosX];
      if (MINE == bCode)
      {
        strTitle.Format(_T("扫雪 %02d %02d %02X"), wPosX, wPosY, bCode);
      }
      else
      {
        strTitle.Format(_T("扫雷 %02d %02d %02X"), wPosX, wPosY, bCode);
      }
      ::SetWindowText(hwnd, strTitle);
    }
    else
    {
      ::SetWindowText(hwnd, _T("扫雷"));
    }
  }
  // 调用扫雷原来的回调函数
  return CallWindowProc(g_OldWndProc, hwnd, uMsg, wParam, lParam);
}
// CwinminePluginApp 初始化

BOOL CwinminePluginApp::InitInstance()
{
  CWinApp::InitInstance();
  // 设置新的窗口回调
  g_hwinmineWnd = ::FindWindow(_T("扫雷"), _T("扫雷"));
  ::SetWindowLong(g_hwinmineWnd, GWL_WNDPROC, (LONG)NewWndProc);
  return TRUE;
}
