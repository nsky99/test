#pragma once
#include <ntifs.h>

// ��ӦIo ͳ��IDT
NTSTATUS OnCountIdt(PIRP pIrp);

// ��ӦIo ��ѯIDT
NTSTATUS OnQueryIdt(PIRP pIrp);