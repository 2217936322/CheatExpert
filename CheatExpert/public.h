#pragma once

ULONG PrintLog(const char* fmt, ...);

#define	DEVICE_NAME			L"\\Device\\BobHACE"
#define LINK_NAME			L"\\DosDevices\\BobHACE"
#define LINK_GLOBAL_NAME	L"\\DosDevices\\Global\\BobHACE"

#define IOCTL_GRANTHANDLE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)

struct IoGrantHandlePack
{
	HANDLE handle;
	ACCESS_MASK access;
	int ObjectTableOffset;
}GrandHandlePack;

typedef struct _HANDLE_TABLE_ENTRY
{
	union
	{
		PVOID Object;
		ULONG ObAttributes;
	};
	union
	{
		union
		{
			ACCESS_MASK GrantedAccess;
			struct
			{
				USHORT GrantedAccessIndex;
				USHORT CreatorBackTraceIndex;
			};
		};
		LONG NextFreeTableEntry;
	};
} HANDLE_TABLE_ENTRY, * PHANDLE_TABLE_ENTRY;

typedef struct _HANDLE_TABLE
{
	char padding[100];
} HANDLE_TABLE, * PHANDLE_TABLE;

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

NTSTATUS GrantHandleAccess(IoGrantHandlePack param);