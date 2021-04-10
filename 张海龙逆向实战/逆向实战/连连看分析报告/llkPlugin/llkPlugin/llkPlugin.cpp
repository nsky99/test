// llkPlugin.cpp: 定义 DLL 的初始化例程。
//

#include "pch.h"
#include "framework.h"
#include "llkPlugin.h"
#include "CDlgPlugin.h"
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

// CllkPluginApp

BEGIN_MESSAGE_MAP(CllkPluginApp, CWinApp)
END_MESSAGE_MAP()


// CllkPluginApp 构造

CllkPluginApp::CllkPluginApp()
{
  // TODO:  在此处添加构造代码，
  // 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CllkPluginApp 对象

CllkPluginApp theApp;

WNDPROC g_OldProc = 0;
LRESULT CALLBACK NewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  // 使用道具
  if (WM_USEPROP == uMsg)
  {
    DWORD dj = wParam;
    __asm
    {
      pushad
      mov     esi, 0x19A1D8
      mov     eax, dword ptr[esi + 0x494]
      lea     ecx, dword ptr[esi + 0x494]
      push    [dj]
      push    0
      push    0
      call    dword ptr[eax + 0x28]
      popad
    }
    ::DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  else if (WM_PLUGIN2 == uMsg)
  {
    POINT p1 = { 0 };
    POINT p2 = { 0 };
    __asm
    {
      pushad
      mov  esi, 0x019A66C
      mov  ecx, dword ptr[esi + 0x19F0]
      lea  eax, dword ptr[p2];
      push eax
      lea  eax, dword ptr[p1];
      push eax
      mov  eax, 0x042923F
      call eax
      popad
    }


    CString str;
    str.Format(_T("A:%d %d B:%d %d"), p1.x, p1.y, p2.x, p2.y);
    OutputDebugString(str);
    __asm
    {
      pushad
      push  2 

      push  0x23D5848
      
      lea   eax, dword ptr[p2];
      push  eax
      
      lea   eax, dword ptr[p1];
      mov   esi, 0x0019A1D8
      mov   ecx, esi
      push  eax
      push  0x19BB34
      push  0


      mov   eax,0x041C68E
      call eax
      popad
    }
    ::DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  return ::CallWindowProc(g_OldProc, hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI ThreadProc(
  LPVOID lpThreadParameter
)
{
  CDlgPlugin dlg;
  dlg.DoModal();
  return 0;
}

// CllkPluginApp 初始化

BOOL CllkPluginApp::InitInstance()
{
  CWinApp::InitInstance();
  // 1、查找目标窗口句柄
  m_llkhWnd = ::FindWindow(NULL, _T("QQ连连看"));
  if (NULL == m_llkhWnd)
  {
    return FALSE;
  }

  // 2、设置窗口回调
  g_OldProc = (WNDPROC)SetWindowLong(m_llkhWnd, GWLP_WNDPROC, (LONG)NewWndProc);
  if (g_OldProc == NULL)
  {
    return FALSE;
  }
  m_OldProc = g_OldProc;
  // 3、创建对话框
  HANDLE hThread = ::CreateThread(0, 0, ThreadProc, 0, 0, 0);
  if (hThread == 0)
  {
    return FALSE;
  }
  CloseHandle(hThread);
  return TRUE;
}
