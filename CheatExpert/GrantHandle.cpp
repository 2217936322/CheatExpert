#include <ntddk.h>
#include <windef.h>
#include "public.h"
#include "struct10.h"

PHANDLE_TABLE_ENTRY ExpLookupHandleTableEntry(PHANDLE_TABLE pHandleTable, HANDLE handle)
{
	unsigned __int64 v2; // rdx
	__int64 v3; // r8
	signed __int64 v4; // rax
	__int64 v5; // rax

	v2 = (__int64)handle & 0xFFFFFFFFFFFFFFFCui64;
	if (v2 >= *(DWORD*)pHandleTable)
		return 0i64;
	v3 = *((uintptr_t*)pHandleTable + 1);
	v4 = *((uintptr_t*)pHandleTable + 1) & 3i64;
	if ((uint32_t)v4 == 1)
	{
		v5 = *(uintptr_t*)(v3 + 8 * (v2 >> 10) - 1);
		return (PHANDLE_TABLE_ENTRY)(v5 + 4 * (v2 & 0x3FF));
	}
	if ((uint32_t)v4)
	{
		v5 = *(uintptr_t*)(*(uintptr_t*)(v3 + 8 * (v2 >> 19) - 2) + 8 * ((v2 >> 10) & 0x1FF));
		return (PHANDLE_TABLE_ENTRY)(v5 + 4 * (v2 & 0x3FF));
	}
	return (PHANDLE_TABLE_ENTRY)(v3 + 4 * v2);
}

NTSTATUS GrantHandleAccess(IoGrantHandlePack param) {
	PEPROCESS currentProcess = PsGetCurrentProcess();
	PHANDLE_TABLE pHandleTable = *(PHANDLE_TABLE*)((unsigned char*)currentProcess + param.ObjectTableOffset);
	PHANDLE_TABLE_ENTRY pEntry = ExpLookupHandleTableEntry(pHandleTable, param.handle);
	if (pEntry == 0) return STATUS_UNSUCCESSFUL;
	ACCESS_MASK oldAccess = pEntry->GrantedAccess;
	pEntry->GrantedAccess = param.access;
	PrintLog("Old Access: 0x%llx -> New Access: 0x%llx \r\n", oldAccess, param.access);
	return STATUS_SUCCESS;
}