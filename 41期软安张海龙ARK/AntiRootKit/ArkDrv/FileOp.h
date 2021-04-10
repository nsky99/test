#pragma once
#include <ntifs.h>

/// <summary>
/// ��������ļ�
/// </summary>
/// <param name="pustrPath">�ļ�·��</param>
/// <param name="access">��������</param>
/// <param name="bIsFile">�Ƿ����ļ�</param>
/// <returns>�ļ����������-1ʧ��</returns>
HANDLE CreateFile(PUNICODE_STRING pustrPath, ACCESS_MASK access, BOOLEAN bIsFile)
{
  DbgBreakPoint();
  HANDLE hFile = NULL;

  // Ŀ���ļ���Ϣ
  OBJECT_ATTRIBUTES objAttr = { 0 };
  InitializeObjectAttributes(
    &objAttr, // �ļ����Զ���
    pustrPath,// �ļ�·��
    OBJ_CASE_INSENSITIVE,// �ļ����ԣ����ִ�Сд
    NULL, NULL           // �����Ŀ¼�Ͱ�ȫ�����ֶ�
  );

  // ����ZwCreateFile������Ϣ
  IO_STATUS_BLOCK statusBlock = { 0 };

  // �����û�����Ĳ���ֵ���� CreateOption ��ֵ��Ŀ¼\�ļ�
  ULONG CreateOption = bIsFile ? FILE_NON_DIRECTORY_FILE
    : FILE_DIRECTORY_FILE;

  // ��������ļ�
  NTSTATUS status = ZwCreateFile(
    &hFile, // �����ļ����
    access,// ��������
    &objAttr,     // Ŀ���ļ�����
    &statusBlock, // ���շ���״̬
    0,            // ����Ǵ����µ��ļ� - �ļ���С
    FILE_ATTRIBUTE_NORMAL,// �������򿪵��ļ�����ʲô��������
    FILE_SHARE_VALID_FLAGS,// ���������ļ�����Ĺ���ʽ���� R3 ����һ��
    FILE_OPEN_IF,          // ���������򿪣��������򴴽�
    CreateOption,          // ������򿪵���������
    NULL, 0                // ��չ������
  );
  if (!NT_SUCCESS(status))
  {
    return (HANDLE)-1;
  }
  return hFile;
}


/// <summary>
/// ��ȡ�ļ���С
/// </summary>
/// <param name="hFile">�ļ����</param>
/// <returns>�ļ���С�������-1��ʶ��ȡʧ��</returns>
ULONG GetFileSize(HANDLE hFile)
{
  // Ҫ��ȡ���ļ���Ϣ
  FILE_STANDARD_INFORMATION FileInfo = { 0 };

  // ����ZwQueryInformationFile������Ϣ
  IO_STATUS_BLOCK statusBlock = { 0 };

  // ִ�в�ѯ�ļ���Ϣ - ��ȡ�ļ���С
  NTSTATUS status = ZwQueryInformationFile(
    hFile,    // Ҫ��ѯ��Ϣ���ļ����
    &statusBlock,   // ���պ���ִ����Ϣ
    &FileInfo,      // �����ļ���Ϣ
    sizeof(FileInfo),// �����ļ���Ϣ�Ľṹ���С
    FileStandardInformation // Ҫ��ѯʲô��Ϣ
  );
  if (!NT_SUCCESS(status))
  {
    return (ULONG)-1;
  }
  return FileInfo.EndOfFile.LowPart;
}


/// <summary>
/// ��ȡ�ļ�
/// </summary>
/// <param name="hFile">�ļ����</param>
/// <param name="pBuffer">���浽��</param>
/// <param name="ulReadSize">������</param>
/// <param name="pOffset">��ʼƫ��</param>
/// <returns></returns>
NTSTATUS ReadFile(HANDLE hFile ,PVOID pBuffer,ULONG ulReadSize, ULONG pOffset)
{
  // ���պ���ִ�н��
  IO_STATUS_BLOCK statusBlock = { 0 };
  LARGE_INTEGER Offset = { pOffset, 0 };

  // ִ�ж�ȡ
  NTSTATUS status = ZwReadFile(
    hFile,                // Ҫ��ȡ˭���ļ�
    NULL, NULL, NULL,			// �첽 IO ʹ�õ�������������������
    &statusBlock,         // ���պ���ִ�н��
    pBuffer, ulReadSize,  // �������������
    &Offset, NULL         // �ñ���ָ���˽���ʼ��ȡ�������ļ��е���ʼƫ��
  );
  return status;
}