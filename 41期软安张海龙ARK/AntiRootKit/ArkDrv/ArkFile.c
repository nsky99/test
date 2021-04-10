#include "ArkFile.h"
#include <ntifs.h>
#include "ArkCommon.h"

// �����ļ�������ҵ�һ���ļ�
BOOLEAN ArkFindFirstFile(
  IN HANDLE hFile, 
  IN ULONG ulLen, 
  OUT PFILE_BOTH_DIR_INFORMATION pDir, 
  IN ULONG uFirstLen,
  OUT PFILE_BOTH_DIR_INFORMATION pFirstDir)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  IO_STATUS_BLOCK statusBlock = { 0 };
  // 1. ��ȡ��һ���ļ���Ϣ
  status = ZwQueryDirectoryFile(
    hFile, NULL, NULL, NULL,
    &statusBlock,// ���պ��������ṹ
    pFirstDir,    // �ļ���Ϣ
    uFirstLen,    // �ļ���Ϣ��ʱ�䳤��
    FileBothDirectoryInformation,// ��ѯģʽ
    TRUE,         // �Ƿ񷵻�һ����ʼ��Ϣ
    NULL,         // �ļ����ָ����ļ���һ��ΪNULL��
    FALSE         // �Ƿ��Ŀ¼��ʼ�ĵ�һ��ɨ��
  );

  // 2. ���ɹ������ȡ�ļ��б�
  if (!NT_SUCCESS(status))
  {
    return FALSE;
  }
  status = ZwQueryDirectoryFile(
    hFile, NULL, NULL, NULL,
    &statusBlock,// ���պ��������ṹ
    pDir,        // �ļ���Ϣ
    ulLen,       // �ļ���Ϣ��ʱ�䳤��
    FileBothDirectoryInformation,// ��ѯģʽ
    FALSE,       // �Ƿ񷵻�һ����ʼ��Ϣ
    NULL,        // �ļ����ָ����ļ���һ��ΪNULL��
    FALSE        // �Ƿ��Ŀ¼��ʼ�ĵ�һ��ɨ��
  );
  return NT_SUCCESS(status);
}


// �����ļ����������һ���ļ�
BOOLEAN ArkFindNextFile(
  IN PFILE_BOTH_DIR_INFORMATION pDirList, 
  OUT PFILE_BOTH_DIR_INFORMATION pDirInfo, 
  IN OUT LONG* Loc)
{
  // �������һ����ƶ�ָ��ָ����һ��
  PFILE_BOTH_DIR_INFORMATION pDir = (PFILE_BOTH_DIR_INFORMATION)((PCHAR)pDirList + *Loc);
  LONG structLen = 0;
  if (pDir->FileName[0] != 0 && pDir->FileNameLength < 0xFF)
  {
    structLen = sizeof(FILE_BOTH_DIR_INFORMATION);
    RtlCopyMemory(pDirInfo, pDir, structLen + pDir->FileNameLength);
    *Loc = *Loc + pDir->NextEntryOffset;
    if (pDir->NextEntryOffset == 0)
    {
      *Loc = *Loc + structLen + pDir->FileNameLength;
    }
    return TRUE;
  }
  return FALSE;
}

// �����ļ�·���������Ӵ����ļ�
HANDLE ArkCreateFileF(
  IN PUNICODE_STRING pstrFile,// �ļ�·����������
  IN BOOLEAN bIsDir            // �Ƿ�Ϊ�ļ���
)
{
  HANDLE hFile = NULL;
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  IO_STATUS_BLOCK statusBlock = { 0 };
  // �ļ���������
  ULONG ulShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
  ULONG ulCreateOpt = FILE_SYNCHRONOUS_IO_NONALERT;
  
  // 1. ��ʼ��OBJ_ATT����
  OBJECT_ATTRIBUTES objAttr = { 0 };
  ULONG ulAttrs = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
  InitializeObjectAttributes(
    &objAttr, // ���س�ʼ����ϵĽṹ��
    pstrFile, // �ļ��������� 
    ulAttrs,  // ��������
    NULL, NULL);

  // 2. �����ļ�����
  ulCreateOpt |= bIsDir ?
    FILE_DIRECTORY_FILE : FILE_NON_DIRECTORY_FILE;
  status = ZwCreateFile(
    &hFile,             // �����ļ����
    GENERIC_ALL,        // �ļ���������
    &objAttr,           // OBJ_ATTR
    &statusBlock,       // ���պ��������ṹ
    0,                  // ��ʼ���ļ���С
    FILE_ATTRIBUTE_NORMAL,// �½��ļ�����
    ulShareAccess,        // �ļ�����ʽ
    FILE_OPEN_IF,         // �ļ����ڴ򿪣��������򴴽�
    ulCreateOpt,          // �򿪲����ĸ��ӱ�־λ
    NULL,// ��չ������
    0// ��չ����������
  );
  KdPrint(("%08X", (ULONG)status));
  if (!NT_SUCCESS(status))
  {
    return (HANDLE)-1;
  }

  return hFile;
}


NTSTATUS ArkEnumFile(PWCHAR szPath, PFILE_INFO pFileInfo)
{
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  PFILE_BOTH_DIR_INFORMATION pFileTmp = NULL;
  PFILE_BOTH_DIR_INFORMATION pFileList = NULL;
  // �ļ����ַ���
  UNICODE_STRING ustrPath = { 0 };
  RtlInitUnicodeString(&ustrPath, szPath);

  HANDLE hFile = NULL;
  SIZE_T nFileInfoSize = sizeof(FILE_BOTH_DIR_INFORMATION) + 270 * sizeof(WCHAR);

  SIZE_T nSize = nFileInfoSize * 0x256;// �����ļ����0x256
  WCHAR  szFileName[0xFF] = { 0 };
  do
  {
    pFileList = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePoolWithTag(PagedPool, nSize, '.lst');
    if (!pFileList)
    {
      status = STATUS_UNSUCCESSFUL;
      break;
    }
    pFileTmp = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePoolWithTag(PagedPool, nSize, '.tmp');
    if (!pFileTmp)
    {
      status = STATUS_UNSUCCESSFUL;
      break;
    }
    // ��·����װΪ���ӷ������ƣ������ļ�

    hFile = ArkCreateFileF(&ustrPath, TRUE);


    if (ArkFindFirstFile(hFile, nSize, pFileList, nFileInfoSize, pFileTmp))
    {
      LONG loc = 0;
      ULONG ulIndex = 0;
      do
      {
        RtlZeroMemory(szFileName, 0xFF);
        RtlCopyMemory(szFileName, pFileTmp->FileName, pFileTmp->FileNameLength);

        // ��ǰĿ¼����һ��Ŀ¼
        if (0 == wcscmp(szFileName, L"..")
          || 0 == wcscmp(szFileName, L"."))
        {
          continue;
        }

        if (pFileTmp->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
          RtlCopyMemory((pFileInfo + ulIndex)->szName, szFileName, 0xFF);
          RtlCopyMemory((pFileInfo + ulIndex)->szType, L"Ŀ¼", sizeof L"Ŀ¼");
          (pFileInfo + ulIndex)->ulSize = pFileTmp->EndOfFile.QuadPart;
        }
        else
        {
          RtlCopyMemory((pFileInfo + ulIndex)->szName, szFileName, 0xFF);
          RtlCopyMemory((pFileInfo + ulIndex)->szType, L"�ļ�", sizeof L"�ļ�");
          (pFileInfo + ulIndex)->ulSize = pFileTmp->EndOfFile.QuadPart;
        }
        RtlZeroMemory(pFileTmp, nFileInfoSize);
        ulIndex++;
      } while (ArkFindNextFile(pFileList, pFileTmp, &loc));

    }
  } while (FALSE);

  if (hFile)
  {
    ZwClose(hFile);
    hFile = NULL;
  }

  // �ͷ��ڴ�
  if (pFileTmp)
  {
    ExFreePoolWithTag(pFileTmp, '.tmp');
    pFileTmp = NULL;
  }
  if (pFileList)
  {
    ExFreePoolWithTag(pFileList, '.lst');
    pFileList = NULL;
  }

  return status;
}

// ͳ���ļ�����
ULONG ArkCountFileF(PWCHAR szPath)
{
  ULONG ulFileNum = 0;
  NTSTATUS status = STATUS_UNSUCCESSFUL;
  PFILE_BOTH_DIR_INFORMATION pFileTmp = NULL;
  PFILE_BOTH_DIR_INFORMATION pFileList = NULL;
  // �ļ����ַ���
  UNICODE_STRING ustrPath = { 0 };
  RtlInitUnicodeString(&ustrPath, szPath);
  HANDLE hFile = NULL;
  SIZE_T nFileInfoSize = sizeof(FILE_BOTH_DIR_INFORMATION) + 270 * sizeof(WCHAR);

  SIZE_T nSize = nFileInfoSize * 0x256;// �����ļ����0x256
  WCHAR  szFileName[0xFF] = { 0 };
  do
  {
    pFileList = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePoolWithTag(PagedPool, nSize, '.lst');
    if (!pFileList)
    {
      status = STATUS_UNSUCCESSFUL;
      break;
    }
    pFileTmp = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePoolWithTag(PagedPool, nSize, '.tmp');
    if (!pFileTmp)
    {
      status = STATUS_UNSUCCESSFUL;
      break;
    }
    // ��·����װΪ���ӷ������ƣ������ļ�
    hFile = ArkCreateFileF(&ustrPath, TRUE);
    if (ArkFindFirstFile(hFile, nSize, pFileList, nFileInfoSize, pFileTmp))
    {
      LONG loc = 0;
      do
      {
        RtlZeroMemory(szFileName, 0xFF);
        RtlCopyMemory(szFileName, pFileTmp->FileName, pFileTmp->FileNameLength);

        // ��ǰĿ¼����һ��Ŀ¼
        if (0 == wcscmp(szFileName, L"..")
          || 0 == wcscmp(szFileName, L"."))
        {
          continue;
        }
        ulFileNum++;
      } while (ArkFindNextFile(pFileList, pFileTmp, &loc));

    }
  } while (FALSE);

  if (hFile)
  {
    ZwClose(hFile);
    hFile = NULL;
  }

  // �ͷ��ڴ�
  if (pFileTmp)
  {
    ExFreePoolWithTag(pFileTmp, '.tmp');
    pFileTmp = NULL;
  }
  if (pFileList)
  {
    ExFreePoolWithTag(pFileList, '.lst');
    pFileList = NULL;
  }
  return ulFileNum;
}

// ��ӦIo ͳ��ָ��Ŀ¼�µ��ļ�����
NTSTATUS OnCountFile(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;
  // ��ȡҪͳ�Ƶķ�������Ŀ¼
  WCHAR szPath[0xff] = { 0 };
  RtlCopyMemory(szPath, pIrp->AssociatedIrp.SystemBuffer, 0xFF);
  // ͳ���ļ�����
  *(PULONG)pIrp->AssociatedIrp.SystemBuffer = ArkCountFileF(szPath);
  // ����
  pIrp->IoStatus.Status = status;
  pIrp->IoStatus.Information = sizeof(ULONG);
  return status;
}


// ��ӦIo ��ѯָ��Ŀ¼�µ��ļ���Ϣ
NTSTATUS OnQueryFile(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;
  PFILE_INFO pFileInfo = NULL;
  // ��ȡҪͳ�Ƶķ�������Ŀ¼
  WCHAR szPath[0xff] = { 0 };
  RtlCopyMemory(szPath, pIrp->AssociatedIrp.SystemBuffer, 0xFF);
  // ͳ���ļ�����
  ULONG ulNum = ArkCountFileF(szPath);
  ULONG ulMemSize = sizeof(FILE_INFO) * ulNum;
  do
  {
    // Ϊ�洢�ļ���Ϣ�����ڴ�
    pFileInfo = (PFILE_INFO)ExAllocatePoolWithTag(PagedPool, ulMemSize, '.fil');
    if (!pFileInfo)
    {
      // status = STATUS_UNSUCCESSFUL;
      pFileInfo = NULL;
      break;
    }

    ArkEnumFile(szPath, pFileInfo);

    RtlCopyMemory(pIrp->AssociatedIrp.SystemBuffer, pFileInfo, ulMemSize);
  } while (FALSE);

  // �ͷ��ڴ�
  if (pFileInfo)
  {
    ExFreePoolWithTag(pFileInfo, '.fil');
    pFileInfo = NULL;
  }

  // ����
  pIrp->IoStatus.Status = status;
  pIrp->IoStatus.Information = ulMemSize;
  return status;
}


// ��ӦIo ��ѯָ��Ŀ¼�µ��ļ���Ϣ
NTSTATUS OnDelFile(PIRP pIrp)
{
  NTSTATUS status = STATUS_SUCCESS;
  // ��ȡҪͳ�Ƶķ�������Ŀ¼
  WCHAR szPath[0xff] = { 0 };
  RtlCopyMemory(szPath, pIrp->AssociatedIrp.SystemBuffer, 0xFF);

  // Ҫɾ�����ļ��������� unicode string
  UNICODE_STRING StrDelFile = { 0 };
  RtlInitUnicodeString(&StrDelFile, szPath);

  OBJECT_ATTRIBUTES objAttr = { 0 };
  ULONG ulAttrs = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
  InitializeObjectAttributes(&objAttr, &StrDelFile, ulAttrs, NULL, NULL);

  ZwDeleteFile(&objAttr);

  // ����
  pIrp->IoStatus.Status = status;
  pIrp->IoStatus.Information = 0;
  return status;
}

