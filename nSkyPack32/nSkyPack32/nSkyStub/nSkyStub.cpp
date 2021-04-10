#include "stdafx.h"
#include "string"
#include <windows.h>
#include <direct.h>
#include <Shlobj.h>
#include "nSkyStub.h"
#include "../aplib/aplib.h"
#pragma comment(lib, "..\\aplib\\aplib.lib")
//�ϲ���
#pragma comment(linker, "/merge:.data=.text") 
#pragma comment(linker, "/merge:.rdata=.text")
#pragma comment(linker, "/section:.text,RWE")
typedef VOID(WINAPI* EXITPROCESS)(
  _In_ UINT uExitCode
);//ExitProcess
typedef int (WINAPI* LPMESSAGEBOX)(
  HWND, LPCTSTR,
  LPCTSTR, UINT
  ); //MessageBoxW
typedef DWORD(WINAPI* LPGETPROCADDRESS)(
  HMODULE,
  LPCSTR
  );         // GetProcAddress
typedef HMODULE(WINAPI* LPLOADLIBRARYEX)(
  LPCTSTR, HANDLE, DWORD
  ); // LoadLibaryEx
typedef HMODULE(WINAPI* GETModuleHandle)(
  _In_opt_ LPCTSTR lpModuleName
  );
typedef BOOL(WINAPI* SHOWWINDOW)(
  _In_ HWND hWnd,
  _In_ int  nCmdShow
  );
typedef BOOL(WINAPI* GteMessage)(
  _Out_    LPMSG lpMsg,
  _In_opt_ HWND  hWnd,
  _In_     UINT  wMsgFilterMin,
  _In_     UINT  wMsgFilterMax
  );
typedef LRESULT(WINAPI* DISpatchMessage)(
  _In_ const MSG* lpmsg
  );
typedef ATOM(WINAPI* REGisterClass)(
  _In_ const WNDCLASS* lpWndClass
  );
typedef HWND(WINAPI* CREateWindowEx)(
  _In_     DWORD     dwExStyle,
  _In_opt_ LPCTSTR   lpClassName,
  _In_opt_ LPCTSTR   lpWindowName,
  _In_     DWORD     dwStyle,
  _In_     int       x,
  _In_     int       y,
  _In_     int       nWidth,
  _In_     int       nHeight,
  _In_opt_ HWND      hWndParent,
  _In_opt_ HMENU     hMenu,
  _In_opt_ HINSTANCE hInstance,
  _In_opt_ LPVOID    lpParam
  );
typedef VOID(WINAPI* POSTQuitMessage)(
  _In_ int nExitCode
  );
typedef LRESULT(WINAPI* DEFWindowProc)(
  _In_ HWND   hWnd,
  _In_ UINT   Msg,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
  );
typedef BOOL(*UPDateWindow)(
  _In_ HWND hWnd
  );
typedef int (WINAPI* GETWindowText)(
  _In_  HWND   hWnd,
  _Out_ LPTSTR lpString,
  _In_  int    nMaxCount
  );
typedef int (WINAPI* GETWindowTextLength)(
  _In_ HWND hWnd
  );
typedef HWND(WINAPI* GETDlgItem)(
  _In_opt_ HWND hDlg,
  _In_     int  nIDDlgItem
  );
typedef BOOL(WINAPI* SETWindowText)(
  _In_     HWND    hWnd,
  _In_opt_ LPCTSTR lpString
  );
typedef BOOL(WINAPI* TRanslateMessage)(
  _In_ const MSG* lpMsg
  );
typedef LPVOID(WINAPI* MYVIRTUALALLOC)(
  _In_opt_ LPVOID lpAddress,
  _In_     SIZE_T dwSize,
  _In_     DWORD  flAllocationType,
  _In_     DWORD  flProtect
  );
typedef BOOL(WINAPI* MYVIRTUALFREE)(
  _In_ LPVOID lpAddress,
  _In_ SIZE_T dwSize,
  _In_ DWORD  dwFreeType
  );
typedef HMODULE(WINAPI* MYLOADLIBRARY)(
  _In_ LPCSTR lpLibFileName
  );
// ȥ�����������к��������Ʒ�����ƣ���������ĵ��ú��޸�
extern "C"
{

  wchar_t g_wcbuf100[100] = { 0 };
  wchar_t g_MIMA100[100] = L"123";
  wchar_t wStrtext[100] = L"����������";
  _declspec(thread) int g_num;
  /////////////////////////////////////////////////////////////
  //��ʼ��
  LPGETPROCADDRESS    g_funGetProcAddress = nullptr;
  LPLOADLIBRARYEX     g_funLoadLibraryEx = nullptr;
  HMODULE             hModuleKernel32 = nullptr;
  HMODULE             hModuleUser32 = nullptr;
  GETModuleHandle     g_funGetModuleHandle = nullptr;
  LPMESSAGEBOX        g_funMessageBox = nullptr;
  CREateWindowEx      g_funCreateWindowEx = nullptr;
  POSTQuitMessage     g_funPostQuitMessage = nullptr;
  DEFWindowProc       g_funDefWindowProc = nullptr;
  GteMessage          g_funGetMessage = nullptr;
  REGisterClass       g_funRegisterClass = nullptr;
  SHOWWINDOW          g_funShowWindow = nullptr;
  UPDateWindow        g_funUpdateWindow = nullptr;
  DISpatchMessage     g_funDispatchMessage = nullptr;
  GETWindowText       g_funGetWindowText = nullptr;
  GETWindowTextLength g_funGetWindowTextLength = nullptr;
  GETDlgItem          g_funGetDlgItem = nullptr;
  SETWindowText       g_funSetWindowText = nullptr;
  TRanslateMessage    g_funTranslateMessage = nullptr;
  MYVIRTUALALLOC      g_VirtualAlloc = nullptr;
  MYVIRTUALFREE       g_VirtualFree = nullptr;
  MYLOADLIBRARY    g_LoadLibraryA = nullptr;
  EXITPROCESS      g_funExitProcess = nullptr;
  DWORD g_dwImageBase;
  DWORD g_oep;
  void start();
  PACKINFO g_PackInfo = { (DWORD)start };
  //��ȡkernel32ģ����ػ�ַ
  DWORD GetKernel32Base()
  {
    DWORD dwKernel32Addr = 0;
    __asm
    {
      push eax
      mov eax, dword ptr fs : [0x30] // eax = PEB�ĵ�ַ
      mov eax, [eax + 0x0C]          // eax = ָ��PEB_LDR_DATA�ṹ��ָ��
      mov eax, [eax + 0x1C]          // eax = ģ���ʼ�������ͷָ��InInitializationOrderModuleList
      mov eax, [eax]                 // eax = �б��еĵڶ�����Ŀ
      mov eax, [eax + 0x08]          // eax = ��ȡ����Kernel32.dll��ַ��Win7�»�ȡ����KernelBase.dll�Ļ�ַ��
      mov dwKernel32Addr, eax
      pop eax
    }

    return dwKernel32Addr;
  }

  //��ȡGetProcAddress�Ļ�ַ
  DWORD GetGPAFunAddr()
  {
    DWORD dwAddrBase = GetKernel32Base();

    // 1. ��ȡDOSͷ��NTͷ
    PIMAGE_DOS_HEADER pDos_Header;
    PIMAGE_NT_HEADERS pNt_Header;
    pDos_Header = (PIMAGE_DOS_HEADER)dwAddrBase;
    pNt_Header = (PIMAGE_NT_HEADERS)(dwAddrBase + pDos_Header->e_lfanew);

    // 2. ��ȡ��������
    PIMAGE_DATA_DIRECTORY   pDataDir;
    PIMAGE_EXPORT_DIRECTORY pExport;
    pDataDir = pNt_Header->OptionalHeader.DataDirectory;
    pDataDir = &pDataDir[IMAGE_DIRECTORY_ENTRY_EXPORT];
    pExport = (PIMAGE_EXPORT_DIRECTORY)(dwAddrBase + pDataDir->VirtualAddress);

    // 3����ȡ������ı�Ҫ��Ϣ
    DWORD dwModOffset = pExport->Name;                                  // ģ�������
    DWORD dwFunCount = pExport->NumberOfFunctions;                      // ��������������
    DWORD dwNameCount = pExport->NumberOfNames;                         // �������Ƶ�����

    PDWORD pEAT = (PDWORD)(dwAddrBase + pExport->AddressOfFunctions);   // ��ȡ��ַ���RVA
    PDWORD pENT = (PDWORD)(dwAddrBase + pExport->AddressOfNames);       // ��ȡ���Ʊ��RVA
    PWORD pEIT = (PWORD)(dwAddrBase + pExport->AddressOfNameOrdinals);  //��ȡ�������RVA
    // 4����ȡGetProAddress�����ĵ�ַ
    for (DWORD i = 0; i < dwFunCount; i++)
    {
      if (!pEAT[i])
      {
        continue;
      }

      // 4.1 ��ȡ���
      DWORD dwID = pExport->Base + i;

      // 4.2 ����EIT ���л�ȡ�� GetProcAddress�ĵ�ַ
      for (DWORD dwIdx = 0; dwIdx < dwNameCount; dwIdx++)
      {
        // ��ű��е�Ԫ�ص�ֵ ��Ӧ�ź�����ַ���λ��
        if (pEIT[dwIdx] == i)
        {
          //������Ż�ȡ�����Ʊ��е�����
          DWORD dwNameOffset = pENT[dwIdx];
          char* pFunName = (char*)(dwAddrBase + dwNameOffset);

          //�ж��Ƿ���GetProcAddress����
          if (!strcmp(pFunName, "GetProcAddress"))
          {
            // ��ȡEAT�ĵ�ַ ����GetProcAddress��ַ����
            DWORD dwFunAddrOffset = pEAT[i];
            return dwAddrBase + dwFunAddrOffset;
          }
        }
      }
    }
    return -1;
  }

  //��ʼ��API
  bool InitializationAPI()
  {
    g_num;//ʹ��tls����,����tls�ڱ�
    //��ʼ��
    g_funGetProcAddress = (LPGETPROCADDRESS)GetGPAFunAddr();
    g_funLoadLibraryEx = (LPLOADLIBRARYEX)g_funGetProcAddress((HMODULE)GetKernel32Base(), "LoadLibraryExW");
    hModuleKernel32 = g_funLoadLibraryEx(L"Kernel32.dll", NULL, NULL);
    hModuleUser32 = g_funLoadLibraryEx(L"user32.dll", NULL, NULL);
    g_funExitProcess = (EXITPROCESS)g_funGetProcAddress(hModuleKernel32, "ExitProcess");
    g_LoadLibraryA = (MYLOADLIBRARY)g_funGetProcAddress(hModuleKernel32, "LoadLibraryA");
    g_funGetModuleHandle = (GETModuleHandle)g_funGetProcAddress(hModuleKernel32, "GetModuleHandleW");
    g_VirtualAlloc = (MYVIRTUALALLOC)g_funGetProcAddress(hModuleKernel32, "VirtualAlloc");
    g_VirtualFree = (MYVIRTUALFREE)g_funGetProcAddress(hModuleKernel32, "VirtualFree");
    g_funMessageBox = (LPMESSAGEBOX)g_funGetProcAddress(hModuleUser32, "MessageBoxW");
    g_funCreateWindowEx = (CREateWindowEx)g_funGetProcAddress(hModuleUser32, "CreateWindowExW");
    g_funPostQuitMessage = (POSTQuitMessage)g_funGetProcAddress(hModuleUser32, "PostQuitMessage");
    g_funDefWindowProc = (DEFWindowProc)g_funGetProcAddress(hModuleUser32, "DefWindowProcW");
    g_funGetMessage = (GteMessage)g_funGetProcAddress(hModuleUser32, "GetMessageW");
    g_funRegisterClass = (REGisterClass)g_funGetProcAddress(hModuleUser32, "RegisterClassW");
    g_funShowWindow = (SHOWWINDOW)g_funGetProcAddress(hModuleUser32, "ShowWindow");
    g_funUpdateWindow = (UPDateWindow)g_funGetProcAddress(hModuleUser32, "UpdateWindow");
    g_funDispatchMessage = (DISpatchMessage)g_funGetProcAddress(hModuleUser32, "DispatchMessageW");
    g_funGetWindowText = (GETWindowText)g_funGetProcAddress(hModuleUser32, "GetWindowTextW");
    g_funGetWindowTextLength = (GETWindowTextLength)g_funGetProcAddress(hModuleUser32, "GetWindowTextLengthW");
    g_funGetDlgItem = (GETDlgItem)g_funGetProcAddress(hModuleUser32, "GetDlgItem");
    g_funSetWindowText = (SETWindowText)g_funGetProcAddress(hModuleUser32, "SetWindowTextW");
    g_funTranslateMessage = (TRanslateMessage)g_funGetProcAddress(hModuleUser32, "TranslateMessage");

    g_dwImageBase = (DWORD)g_funGetModuleHandle(NULL);
    g_oep = g_PackInfo.TargetOepRva + g_dwImageBase;
    return true;
  }

  // ����IAT
  void DealwithIAT()
  {

    // 1.��ȡ��һ��iat��
    PIMAGE_IMPORT_DESCRIPTOR pImportTable =
      (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)g_PackInfo.ImportTableRva + g_dwImageBase);
    if (g_PackInfo.ImportTableRva) //���û�õ����������
    {
      HMODULE lib;
      IMAGE_THUNK_DATA* IAT, * INTable;
      IMAGE_IMPORT_BY_NAME* IatByName;

      while (pImportTable->Name)//(pImportTable->FirstThunk)
      {
        lib = g_LoadLibraryA((char*)(pImportTable->Name + (DWORD)g_dwImageBase));

        IAT = (IMAGE_THUNK_DATA*)(pImportTable->FirstThunk + (DWORD)g_dwImageBase);
        INTable = (IMAGE_THUNK_DATA*)((pImportTable->OriginalFirstThunk ? pImportTable->OriginalFirstThunk : pImportTable->FirstThunk) + (DWORD)g_dwImageBase);
        while (INTable->u1.AddressOfData)
        {
          if ((((DWORD)INTable->u1.Function) & 0x80000000) == 0)
          {
            IatByName = (IMAGE_IMPORT_BY_NAME*)((DWORD)INTable->u1.AddressOfData + (DWORD)g_dwImageBase);
            IAT->u1.Function = (DWORD)g_funGetProcAddress(lib, (char*)(IatByName->Name));
          }
          else
          {
            IAT->u1.Function = (DWORD)g_funGetProcAddress(lib, (LPCSTR)(INTable->u1.Ordinal & 0xFFFF));
          }
          INTable++;
          IAT++;
        }
        pImportTable++;
      }
    }
  }

  // �޸��ض�λ
  void FixReloc()
  {

    //�������ض�λ
    DWORD* tmp;
    if (g_PackInfo.RelocRva)  //���û���ض�λ���ʾ�����ض�λ�������ض�λ����
    {
      DWORD relocation = (DWORD)g_dwImageBase - g_PackInfo.ImageBase;
      IMAGE_BASE_RELOCATION* relocationAddress = (IMAGE_BASE_RELOCATION*)(g_PackInfo.RelocRva + (DWORD)g_dwImageBase);

      while (relocationAddress->VirtualAddress != 0)
      {
        LPVOID rva = (LPVOID)((DWORD)g_dwImageBase + relocationAddress->VirtualAddress);
        DWORD BlockNum = (relocationAddress->SizeOfBlock - 8) / 2;
        if (BlockNum == 0) break;
        WORD* Offset = (WORD*)((DWORD)relocationAddress + 8);
        for (int i = 0; i < (int)BlockNum; i++)
        {
          if ((Offset[i] & 0xF000) != 0x3000) continue;
          tmp = (DWORD*)((Offset[i] & 0xFFF) + (DWORD)rva);
          *tmp = (*tmp) + relocation;
        }
        relocationAddress = (IMAGE_BASE_RELOCATION*)((DWORD)relocationAddress + relocationAddress->SizeOfBlock);
      }
    }
  }

  // ����
  void Decode()
  {

    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)g_dwImageBase;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + g_dwImageBase);
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);

    // �ҵ�.text��,������
    while (TRUE)
    {
      if (!strcmp((char*)pSection->Name, ".text"))
      {
        PCHAR pStart = pSection->VirtualAddress + (PCHAR)g_dwImageBase;
        for (size_t i = 0; i < pSection->Misc.VirtualSize; i++)
        {
          pStart[i] ^= 0x20;
        }
        break;
      }
      pSection = pSection + 1;
    }
  }

  // ��ѹ��
  void Decompress()
  {
    // 1.��ȡ����ͷ�׵�ַ

    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)g_dwImageBase;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pDosHeader->e_lfanew + g_dwImageBase);
    PIMAGE_SECTION_HEADER pSecHeader = IMAGE_FIRST_SECTION(pNtHeader);

    // 2.��ѹѹ������
    PCHAR lpPacked = ((PCHAR)g_dwImageBase + g_PackInfo.packSectionRva);// �ڴ��ַ
    DWORD dwPackedSize = aPsafe_get_orig_size(lpPacked);// ��ȡ��ѹ��Ĵ�С
    PCHAR lpBuffer = (PCHAR)g_VirtualAlloc(NULL, dwPackedSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);//�����ڴ�
    aPsafe_depack(lpPacked, g_PackInfo.packSectionSize, lpBuffer, dwPackedSize);// ��ѹ
    // 3.�������λ�ԭ��ȥ
    DWORD offset = 0;
    for (size_t i = 0; i < g_PackInfo.PackSectionNumber; i++)
    {
      // ���εı��
      int index = g_PackInfo.PackInfomation[i][0];
      // ������ε�SizeOfRawData
      int size = g_PackInfo.PackInfomation[i][1];
      int* pint = &size;
      PCHAR destionVA = (PCHAR)g_dwImageBase + pSecHeader[index].VirtualAddress;
      PCHAR srcVA = lpBuffer + offset;
      _asm 
      {
        mov eax, eax
        mov eax, eax
        mov eax, eax
        mov eax, eax
        mov eax, eax
        mov eax, eax
      }
      //memcpy(destionVA, srcVA, size);
      _asm 
      {
        mov esi, srcVA
        mov edi, destionVA
        mov ebx, pint
        mov ecx, [ebx]
        cld; ��ַ��������
        rep movsb; repִ��һ�δ�ָ���ecx��һ
      }
      offset += size;
    }
    g_VirtualFree(lpBuffer, dwPackedSize, MEM_DECOMMIT);

  }

  // ��������
  void MachineCheck()
  {


  }

  // ����ԭ����TLS 
  void CallTls()
  {
    IMAGE_DOS_HEADER* lpDosHeader = (IMAGE_DOS_HEADER*)g_dwImageBase;
    IMAGE_NT_HEADERS* lpNtHeader = (IMAGE_NT_HEADERS*)(lpDosHeader->e_lfanew + g_dwImageBase);

    // ���tls����,����tls
    if (g_PackInfo.bIsTlsUseful == TRUE)
    {
      // ��tls�ص�������ָ�����û�ȥ
      PIMAGE_TLS_DIRECTORY pTlsDir =
        (PIMAGE_TLS_DIRECTORY)(lpNtHeader->OptionalHeader.DataDirectory[9].VirtualAddress + g_dwImageBase);
      pTlsDir->AddressOfCallBacks = g_PackInfo.TlsCallbackFuncRva;

      PIMAGE_TLS_CALLBACK* lptlsFun =
        (PIMAGE_TLS_CALLBACK*)(g_PackInfo.TlsCallbackFuncRva - lpNtHeader->OptionalHeader.ImageBase + g_dwImageBase);
      while ((*lptlsFun) != NULL)
      {
        (*lptlsFun)((PVOID)g_dwImageBase, DLL_PROCESS_ATTACH, NULL);
        lptlsFun++;
      }
    }

  }

  // IAT�޸�
  void IATReloc()
  {
    //��ȡIAT��dll , ���dll���ػ�ַ; ��ȡIAT�еĺ����� , ��ú�����ַ; ����ָ����С�Ŀռ�

    // 1.��ȡ��һ��iat��
    PIMAGE_IMPORT_DESCRIPTOR pImportTable =
      (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)g_PackInfo.ImportTableRva + g_dwImageBase);
    if (g_PackInfo.ImportTableRva) //���û�õ����������
    {
      HMODULE lib;
      IMAGE_THUNK_DATA* IAT, * INTable;
      IMAGE_IMPORT_BY_NAME* IatByName;

      while (pImportTable->Name)//(pImportTable->FirstThunk)
      {
        lib = g_LoadLibraryA((char*)(pImportTable->Name + (DWORD)g_dwImageBase));

        IAT = (IMAGE_THUNK_DATA*)(pImportTable->FirstThunk + (DWORD)g_dwImageBase);
        INTable = (IMAGE_THUNK_DATA*)((pImportTable->OriginalFirstThunk ? pImportTable->OriginalFirstThunk : pImportTable->FirstThunk) + (DWORD)g_dwImageBase);
        while (INTable->u1.AddressOfData)
        {
          DWORD dwAddress;
          if ((((DWORD)INTable->u1.Function) & 0x80000000) == 0)
          {
            IatByName = (IMAGE_IMPORT_BY_NAME*)((DWORD)INTable->u1.AddressOfData + (DWORD)g_dwImageBase);
            dwAddress = (DWORD)g_funGetProcAddress(lib, (char*)(IatByName->Name));
          }
          else
          {
            dwAddress = (DWORD)g_funGetProcAddress(lib, (LPCSTR)(INTable->u1.Ordinal & 0xFFFF));
          }
          char* dllName = (char*)(pImportTable->Name + (DWORD)g_dwImageBase);

          // ֻ�ض����⼸��dll,������еĶ��ض�������
          if ((!strcmp(dllName, "kernel32.dll"))
            || (!strcmp(dllName, "user32.dll"))
            || (!strcmp(dllName, "advapi32.dll"))
            || (!strcmp(dllName, "gdi32.dll")))
          {
            // ���������ڴ�
            PCHAR virBuf = (PCHAR)g_VirtualAlloc(NULL, 7, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

            // ��ֵ������     
            // mov ebx,address ;jmp address   0xbb 00 00 00 00 ff e3
            virBuf[0] = (CHAR)0xBB;
            *(DWORD*)(virBuf + 1) = dwAddress;
            virBuf[5] = (CHAR)0xFF;
            virBuf[6] = (CHAR)0xE3;

            // ��iat�����Ϊ���
            IAT->u1.Function = (DWORD)virBuf;
          }
          else
          {
            IAT->u1.Function = dwAddress;
          }



          INTable++;
          IAT++;
        }
        pImportTable++;
      }
    }
  }

  // ���ú��� - ��ӵ���Ҫ���õĺ��� - �������ͣ�
  void _stdcall FusedFunc(DWORD funcAddress)
  {
    _asm
    {
      jmp label1
      label2 :
      _emit 0xeb; //���������call
      _emit 0x04;
      CALL DWORD PTR DS : [EAX + EBX * 2 + 0x123402EB] ;
      // ִ��EB 02  Ҳ����������һ��
      // call Init;// ��ȡһЩ���������ĵ�ַ
      // call��һ��,���ڻ��eip
      _emit 0xE8;
      _emit 0x00;
      _emit 0x00;
      _emit 0x00;
      _emit 0x00;
      //-------���������call
      _emit 0xEB;
      _emit 0x0E;

      //-------��ָ��
      PUSH 0x0;
      PUSH 0x0;
      MOV EAX, DWORD PTR FS : [0] ;
      PUSH EAX;
      //-------��ָ��

      CALL DWORD PTR DS : [EAX + EBX * 2 + 0x5019C083] ;

      push funcAddress; // ��������ǲ����������Ҫע�������add eax,??��??
      retn;

      jmp label3

      // ��ָ��
      _emit 0xE8;
      _emit 0x00;
      _emit 0x00;
      _emit 0x00;
      _emit 0x00;
      // ��ָ��
    label1:
      jmp label2
        label3 :
    }
  }

  // �ǳ������ִ�д���
  int g_RunCount = 10;
  
  // ��dump
  void AntidumpFunc1() 
  {

  }

  // ��ִ��
  void AllFunc()
  {
    // �ݹ�ִ��10�κ�ִ�пǳ��� - ���ڻ���
    if (!g_RunCount)
    {
      _asm
      {
        nop
        mov   ebp, esp
        push - 1
        push   0
        push   0
        mov   eax, fs: [0]
        push   eax
        mov   fs : [0] , esp
        sub   esp, 0x68
        push   ebx
        push   esi
        push   edi
        pop   eax
        pop   eax
        pop   eax
        add   esp, 0x68
        pop   eax
        mov   fs : [0] , eax
        pop   eax

        sub g_RunCount, 1

        pop   eax
        pop   eax
        pop   eax
        mov   ebp, eax

        push AllFunc
        call FusedFunc
      }
    }


    g_PackInfo.bIsDynamicEncryption;     //��̬����
    g_PackInfo.bIsVerificationProtection;// У��
    g_PackInfo.bIsAntiDebugging;         // ������
    g_PackInfo.bIsAntiDump;              // ��dump


    // ��dump1
    AntidumpFunc1();


    // �������
    if (g_PackInfo.bIsRegisteredProtection)
    {
      FusedFunc((DWORD)MachineCheck);
    }
    // ��ѹ��
    if (g_PackInfo.bIsCompression)
    {
      FusedFunc((DWORD)Decompress);
    }
    // ����μ���
    if (g_PackInfo.bIsNormalEncryption)
    {
      FusedFunc((DWORD)Decode);
    }
    // �޸��ض�λ
    FusedFunc((DWORD)FixReloc);
    // �Ƿ�IAT�ض���
    if (g_PackInfo.bIsApiRedirect)
    {
      FusedFunc((DWORD)IATReloc);
    }
    else
    {
      FusedFunc((DWORD)DealwithIAT);// �����,����ͨ�޸�IAT
    }
    // ����tls
    FusedFunc((DWORD)CallTls);
  }

  //�ж�����
  int decide()
  {
    int a = 0;
    __asm
    {
      push eax
      push ebx
      push ecx
      push edi
      push esi
      ////////////////////////////////////////////////////////////
      mov ecx, 18
      mov edi, offset g_MIMA100;//��������
      mov esi, offset g_wcbuf100
        repz cmpsb
        je  T
        jmp F
        T :
      mov a, 1
        F :
        ////////////////////////////////////////////////////////////
        pop esi
        pop edi
        pop ecx
        pop ebx
        pop eax
    }
    return a;
  }

  LRESULT CALLBACK WindowProc(
    _In_ HWND   hwnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
  ) {


    switch (uMsg)
    {
      // ���ڴ����ĳ�ʼ�� - ���������ִ��ڽ�����
    case WM_CREATE: 
    {
      // �༭��
      DWORD dwStyle = ES_LEFT | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE;
      DWORD dwExStyle = WS_EX_CLIENTEDGE | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
      HWND hWnd = g_funCreateWindowEx(
        dwExStyle,      //dwExStyle ��չ��ʽ
        L"Edit",        //lpClassName ��������
        wStrtext,       //lpWindowName ���ڱ���
        dwStyle,        //dwStyle ������ʽ
        150,            //x ���λ��
        100,            //y ����λ��
        200,            //nWidth ���
        20,             //nHeight �߶�
        hwnd,           //hWndParent �����ھ��
        (HMENU)0x1002,  //ID
        g_funGetModuleHandle(0), //hInstance Ӧ�ó�����
        NULL //lpParam ���Ӳ���
      );

      return 0;
      /////////////////////////////////////////////////////////////////////////////////
    }

    case WM_COMMAND: 
    {
      WORD wId = LOWORD(wParam);
      WORD wCode = HIWORD(wParam);
      HANDLE hChild = (HANDLE)lParam;
      if (wId == 0x1001 && wCode == BN_CLICKED)
      {
        HWND hwndCombo = g_funGetDlgItem(hwnd, 0x1002);
        int cTxtLen = g_funGetWindowTextLength(hwndCombo);
        g_funGetWindowText(hwndCombo, g_wcbuf100, 100);

        wchar_t wStr3[20] = L"";
        // �ж������Ƿ���ȷ
        if (decide() == 1) 
        {
          // g_funPostQuitMessage(0);
          g_funShowWindow(hwnd, SW_HIDE);
          // FusedFunc���пǴ���AllFunc - ��ӵ���
          FusedFunc((DWORD)AllFunc);
          //_asm jmp g_PackInfo.TargetOep;
          wchar_t wStr[20] = L"������ȷ������";
          wchar_t wStr2[20] = L"success";
          g_funMessageBox(NULL, wStr, wStr2, NULL);
          _asm jmp g_oep;
        }
        else 
        {
          wchar_t wStr[20] = L"����������������룡����";
          wchar_t wStr2[20] = L"Warrning";
          g_funMessageBox(NULL, wStr, wStr2, NULL);
        }
        g_funSetWindowText(hwndCombo, wStr3);
        return 1;
      }
      break;
    }
    case WM_CLOSE:
    {
      g_funPostQuitMessage(0);
      break;
    }

    }
    // ����Ĭ�ϵĴ��ڴ������
    return g_funDefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  // �Ǵ��ڵĴ��� - Ҳ�ǿǵ����
  void CreatePackWin() 
  {

    MSG msg = { 0 };
    // ��ע�ᴰ����
    WNDCLASS wcs = {};
    wcs.lpszClassName = L"nskypack";
    wcs.lpfnWndProc = WindowProc;
    wcs.hbrBackground = (HBRUSH)(COLOR_CAPTIONTEXT + 1);
    g_funRegisterClass(&wcs);

    //ע�ᴰ��
    HWND hWnd = g_funCreateWindowEx(0L, L"nskypack", L"_nskypack", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      500, 200, 500, 500,
      NULL, NULL, NULL, NULL);
    // ��ť
    g_funCreateWindowEx(0L, L"BUTTON", L"����", WS_CHILD | WS_VISIBLE,
      200, 150,// �ڸ����ڵĿͻ�����λ�ã�
      100, 50,// �� ��
      hWnd,// ������
      (HMENU)0x1001,// ����Ƕ��㴰�� ���ǲ˵���� �Ӵ��ھ��Ǳ����ID
      g_funGetModuleHandle(0), NULL);

    // ��ʾ�͸��´���
    g_funShowWindow(hWnd, SW_SHOW);
    g_funUpdateWindow(hWnd);

    // �ַ���Ϣ
    while (g_funGetMessage(&msg, 0, 0, 0))
    {
      g_funTranslateMessage(&msg);
      g_funDispatchMessage(&msg);
    }
  }

  // ������
  bool IsDebugger()
  {
    int isdbg = false;
    _asm
    {
      mov eax, dword ptr fs : [0x30] ;
      movzx eax, byte ptr ds : [eax + 0x2] ;
      mov [isdbg], eax;
    }
    if (isdbg)
    {
      g_funMessageBox(0, L"�е�����", L"ERROR", MB_ICONWARNING);
      g_funExitProcess(-1);
    }
    return isdbg;
  }

  //������start()���� ���ڸ��Ƶ����ӵĽ�
  _declspec(naked) void start()
  {
    InitializationAPI();
    _asm
    {
      jmp lab1
      _emit 0xe9;
    lab1:
    }
    IsDebugger();
    // MachineCheck();
    CreatePackWin();
    _asm
    {
      call lab2
      _emit 0xe9;
      _emit 0x00;
    lab2:
    }
  }
}