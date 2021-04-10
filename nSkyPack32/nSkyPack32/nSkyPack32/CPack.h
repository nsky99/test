#pragma once
#include <Windows.h>
#include "../nSkyStub/nSkyStub.h"

enum PackSetting
{
	SUPPORT = 1,   // 支持
	UNSUPPORT = 0 // 不支持
};
class CPack
{
private:
	// 老的buf中的
	PCHAR m_pBuf;
	DWORD m_dwFileSize;
  
	// 新的buf中的
	PCHAR m_pNewBuf;
	DWORD m_dwNewFileSize;
	
	// PE相关结构体
	PIMAGE_DOS_HEADER m_pDos;
	PIMAGE_NT_HEADERS m_pNt;
	PIMAGE_SECTION_HEADER m_pSection;
	DWORD m_OriSectionNumber;					// 原始节区数

	// 代码段所在区段
	DWORD m_codeIndex;

	// 存储资源数据的区段 .rsrc
	DWORD m_pResRva;
	DWORD m_pResSectionRva;
	DWORD m_ResSectionIndex;
	DWORD m_ResPointerToRawData;
	DWORD m_ResSizeOfRawData;

	// 存储tls数据的区段,也就是.tls区段
	DWORD m_pTlsDataRva;
	DWORD m_pTlsSectionRva;
	DWORD m_TlsSectionIndex;
	DWORD m_TlsPointerToRawData;
	DWORD m_TlsSizeOfRawData;

	//tls表中的信息
	DWORD m_StartOfDataAddress;
	DWORD m_EndOfDataAddress;
	DWORD m_CallBackFuncAddress;
private:
	// 读取要加壳的程序到内存中
	// 参数1: 要加壳的程序路径
	VOID ReadPackFile(
		_In_ PCHAR	pPackPath);

	// 将PE文件的rva转为文件偏移(Foa)
	// 参数1: PE文件的RVA
	DWORD RvaToFoa(
		_In_ DWORD Rva);

	// 处理TLS信息
	// 参数1: 用于保存与stub共享的TLS信息
	BOOL DealwithTLS(
		_Out_ PPACKINFO& pPackInfo);

	// 对代码段进行加密
	// 无参
	VOID EnCode(VOID);

	// 对每个区段进行压缩
	// 参数1: 用于保存与stub共享的压缩信息
	VOID CompressPE(
		_In_ PPACKINFO& pPackInfo);

	// 对指定区段进行压缩并返回压缩后的数据首地址
	// 参数1: 指定区段的首地址
	// 参数2: 指定区段的大小
	// 参数3: 压缩后的大小
	PCHAR Compress(
		_In_ PVOID pSource, 
		_In_ long lInLength, 
		_In_ long& lOutLenght);

	// 计算是否对其
	// 参数1: 区段大小
	// 参数2: 对其粒度
	DWORD CalcAlignment(
		_In_ DWORD dwSize, 
		_In_ DWORD dwAlignment);

	// 添加新的区段
	// 参数1: 新区段名字
	// 参数2: 新区段的内容
	// 参数3: 新区段的大小
	// 参数4: 新区段的属性
	DWORD AddSection(
		_In_opt_ PCHAR szName,        
		_In_opt_ PCHAR pSectionBuf,   
		_In_opt_ DWORD dwSectionSize, 
		_In_opt_ DWORD dwAttribute    
	);

	// 获取要跳转的OEP - RVA
	// 无参数
	DWORD GetOepRva(VOID);

	// 获取PE文件加载基址
	// 无参数
	DWORD GetImageBase(VOID);

	// 获取导入表Rva
	// 无参数
	DWORD GetImportTableRva(VOID);

	// 获取重定位表的Rva
	// 无参数
	DWORD GetRelocRva(VOID);

	// 根据新区段的地址修复dll的重定位[dll是加载到内存的,这里根据默认加载基址,新添加的节区的rva以及和原节区开始的差值来重新设置.text的重定位]
	// 参数1: stub内存指针
	// 参数2: stub的局部
	VOID FixDllRloc(
		_In_ PCHAR pBuf, 
		_In_ PCHAR pOri);

	// 如果要添加一个新区段,获得这个新区段的rva
	// 无参数
	DWORD GetNewSectionRva(VOID);

	// 对于动态加载基址,需要将stub的重定位区段(.reloc)修改后保存,将PE重定位信息指针指向该地址
	// 参数1: 修改重定位
	VOID ChangeReloc(
		_In_ PCHAR pBuf);

	// 获取最后一个区段的RVA
	// 无参数
	DWORD GetLastSectionRva(VOID);

	// 设置TLS
	// 参数1: TLS区段的RVA
	// 参数2: 指向Stub区段的指针
	// 参数3: 共享结构体信息
	VOID SetTls(
		_In_ DWORD NewSectionRva, 
		_In_ PCHAR pStubBuf, 
		_In_ PPACKINFO pPackInfo);

	// 设置新的程序入口点
	// 参数1: 新的OEP
	VOID SetNewOep(
		_In_ DWORD dwNewOep);

	// 设置每个区段为可写状态
  // 无参数
	VOID SetMemWritable(VOID);

	// 对导入表进行更改 全部改为0
	// 无参数
	VOID ChangeImportTable(VOID);

	// 将加好壳的程序保存到文件
	// 参数1: 路径\\文件名称
	VOID SavePackFile(
		_In_ PCHAR pNewFileName);
	//------------ 外部接口 - 提供给外部调用的函数 ------------// 
public:
	// 给指定程序加壳
	// 参数1: 是否支持压缩
	// 参数2: 是否支持加密
	// 参数3: 是否支持注册保护
	// 参数4: 是否支持动态加解密
	// 参数5: 是否支持校验总和保护
	// 参数6: 是否支持反调试
	// 参数7: 是否支持API重定向
	// 参数8: 是否支持反dump
	// 参数9: 要加壳程序的路径
	BOOL Pack(
		_In_opt_ BOOL  bIsCompression,
		_In_opt_ BOOL  bIsNormalEncryption,
		_In_opt_ BOOL  bIsRegisteredProtection,
		_In_opt_ BOOL  bIsDynamicEncryption,
		_In_opt_ BOOL  bIsVerificationProtection,
		_In_opt_ BOOL  bIsAntiDebugging,
		_In_opt_ BOOL  bIsApiRedirect,
		_In_opt_ BOOL  bIsAntiDump,
		_In_		 PCHAR pPath);
};

