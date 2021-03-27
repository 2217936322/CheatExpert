#include <ntddk.h>
#include <stdio.h>
#include <stdarg.h>

NTSTATUS WriteLogFile(const char* content) {
	HANDLE hFile = NULL;
	OBJECT_ATTRIBUTES objAttr = { 0 };
	UNICODE_STRING FileName;
	IO_STATUS_BLOCK ioStatus;
	NTSTATUS status = 0;
	FILE_NAME_INFORMATION FileNameInfo = { 0 };
	FILE_STANDARD_INFORMATION  fsi = { 0 };
	RtlInitUnicodeString(&FileName, L"\\??\\c:\\CELog.txt");
	InitializeObjectAttributes(&objAttr,
		&FileName,
		OBJ_CASE_INSENSITIVE,	//比较对象名时不区分大小写
		0,
		0);
	status = ZwCreateFile(&hFile,
		GENERIC_READ | GENERIC_WRITE,
		&objAttr, &ioStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		0,
		FILE_OPEN_IF,	//打开文件失败就报错
		FILE_SYNCHRONOUS_IO_NONALERT,
		0,
		0);
	if (!NT_SUCCESS(status))
	{
		//KdPrint(("[BobHDebug] 文件打开失败 \n"));
		return status;
	}
	ZwQueryInformationFile(hFile, &ioStatus, &fsi, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	FILE_POSITION_INFORMATION fpi;
	fpi.CurrentByteOffset = fsi.EndOfFile;
	ZwSetInformationFile(hFile, &ioStatus, &fpi, sizeof(FILE_POSITION_INFORMATION), FilePositionInformation);
	status = ZwWriteFile(hFile,
		NULL,
		NULL,
		NULL,
		&ioStatus,
		(PVOID)content,
		strnlen_s(content, 3000),			//写入的长度
		0,
		0);
	if (!NT_SUCCESS(status))
	{
		ZwClose(hFile);
		return status;
	}
	ZwClose(hFile);
	return STATUS_SUCCESS;
}

ULONG PrintLog(const char* fmt, ...) {
	ULONG cnt = 0;
	char buf[3000] = { 0 };
	va_list argptr;
	va_start(argptr, fmt);
	cnt = vsprintf_s(buf, 3000, fmt, argptr);
	va_end(argptr);
	WriteLogFile(buf);
	KdPrint(("[CheatExpert] %s", buf));
	return cnt;
}