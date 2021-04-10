#pragma once

#include <ntifs.h>

// �����ļ�������ҵ�һ���ļ�
BOOLEAN ArkFindFirstFile(
  IN HANDLE hFile,   // �ļ����
  IN ULONG ulLen,    // ��Ϣ����
  OUT PFILE_BOTH_DIR_INFORMATION pDir,// �ļ���Ϣ
  IN ULONG uFirstLen,// ��Ϣ����
  OUT PFILE_BOTH_DIR_INFORMATION pFirstDir// ��һ���ļ���Ϣ
  );


// �����ļ����������һ���ļ�
BOOLEAN ArkFindNextFile(
  IN PFILE_BOTH_DIR_INFORMATION pDirList,
  OUT PFILE_BOTH_DIR_INFORMATION pDirInfo,
  IN OUT LONG* Loc
);

// �����ļ�·���������Ӵ����ļ�
HANDLE ArkCreateFileF(
  IN PUNICODE_STRING pstrFile,// �ļ�·����������
  IN BOOLEAN bIsDir            // �Ƿ�Ϊ�ļ���
);

// ��ӦIo ͳ��ָ��Ŀ¼�µ��ļ�����
NTSTATUS OnCountFile(PIRP pIrp);

// ��ӦIo ��ѯָ��Ŀ¼�µ��ļ���Ϣ
NTSTATUS OnQueryFile(PIRP pIrp);


// ��ӦIo ��ѯָ��Ŀ¼�µ��ļ���Ϣ
NTSTATUS OnDelFile(PIRP pIrp);