#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <tlhelp32.h>
#include <map>
#include <string>
#include "kernelOffset.h"

using namespace std;

std::map<std::string, DWORD> GetProcessList()
{
	std::map<std::string, DWORD> processList;
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE || hSnapshot == 0)
		return processList;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe32))
	{
		CloseHandle(hSnapshot);
		return processList;
	}

	do
	{
		processList[pe32.szExeFile] = pe32.th32ProcessID;
	} while (Process32Next(hSnapshot, &pe32));

	CloseHandle(hSnapshot);
	return processList;
}

HANDLE DriverHandle = 0;

#define IOCTL_GRANTHANDLE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)

struct IoGrantHandlePack
{
	HANDLE handle;
	ACCESS_MASK access;
	int ObjectTableOffset;
}GrandHandlePack;

BOOL KernelGrantAccess(HANDLE handle, ACCESS_MASK access_desired) {
	GrandHandlePack.ObjectTableOffset = kernel_offsets::objecttable;
	GrandHandlePack.handle = handle;
	GrandHandlePack.access = access_desired;
	DWORD dwoutput;
	return DeviceIoControl(DriverHandle, IOCTL_GRANTHANDLE, &GrandHandlePack, sizeof(GrandHandlePack), NULL, 0, &dwoutput, NULL);
}

int main()
{
	kernel_offsets::init();
	printf("HandleTableOffset = 0x%X \n", kernel_offsets::objecttable);
	DriverHandle = CreateFile("\\\\.\\BobHACE", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (DriverHandle == INVALID_HANDLE_VALUE) {
		printf("Cannot communicate with Driver! \n");
		system("pause");
		return 0;
	}
	printf("Open Successfully!");
	DWORD explorerPid = GetProcessList()["explorer.exe"];
	if (!explorerPid) {
		printf("Cannot get pid of expolorer.exe\n");
		system("pause");
		return 0;
	}
	HANDLE myProHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, explorerPid);
	int res = KernelGrantAccess(myProHandle, PROCESS_ALL_ACCESS) ? 1 : 0;
	printf("Grant handle finished(res = %d), check the handle! \n", res);
	int wait;
	scanf_s("%d", &wait);

    return 0;
}
