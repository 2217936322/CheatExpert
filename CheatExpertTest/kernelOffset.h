#pragma once

#include <string>
#include <Windows.h>

enum windows_version
{
	WINDOWS7,
	WINDOWS8,
	WINDOWS81,
	WINDOWS10,
	UNSUPPORTED
};

extern windows_version get_windows_version();

//extern "C" NTSTATUS RtlGetVersion(PRTL_OSVERSIONINFOW lpVersionInformation);

NTSTATUS RtlGetVersion(PRTL_OSVERSIONINFOW lpVersionInformation);

class kernel_offsets
{
public:
	static windows_version version;

	// Eprocess Offsets
	static uint64_t name;
	static uint64_t pid;
	static uint64_t base;
	static uint64_t link;
	static uint64_t protection;
	static uint64_t flags2;
	static uint64_t objecttable;
	static uint64_t vadroot;

	static void init()
	{
		windows_version win_ver = get_windows_version();

		version = win_ver;
		switch (win_ver)
		{
		case WINDOWS7:
			init_win7();
			break;
		case WINDOWS8:
			init_win8();
			break;
		case WINDOWS81:
			init_win81();
			break;
		case WINDOWS10:
			init_win10();
			break;
		}
	}

private:
	static void init_win7()
	{
		name = 0x2D8;
		pid = 0x180;
		base = 0x270;
		link = 0x188;
		protection = 0x43C;
		flags2 = 0;
		objecttable = 0x200;
		vadroot = 0x448;
	}

	static void init_win8()
	{
		name = 0x438;
		pid = 0x2E0;
		base = 0x3B0;
		link = 0x2E8;
		protection = 0x648;
		flags2 = 0;
		objecttable = 0x408;
		vadroot = 0x590;
	}

	static void init_win81()
	{
		name = 0x438;
		pid = 0x2E0;
		base = 0x3B0;
		link = 0x2E8;
		protection = 0x67A;
		flags2 = 0x2F8;
		objecttable = 0x408;
		vadroot = 0x5D8;
	}

	static void init_win10()
	{
		name = 0x450;
		pid = 0x2E0;
		base = 0x3C0;
		link = 0x2E8;
		protection = 0x6B2;
		flags2 = 0x300;
		objecttable = 0x418;
		vadroot = 0x610;

		RTL_OSVERSIONINFOW osVersion;
		RtlGetVersion(&osVersion);
		if (osVersion.dwBuildNumber == 10586)
		{
			protection = 0x6B2;
			flags2 = 0x300;
			objecttable = 0x418;
			vadroot = 0x610;
		}
		else if (osVersion.dwBuildNumber == 14393)
		{
			protection = 0x6C2;
			flags2 = 0x300;
			objecttable = 0x418;
			vadroot = 0x620;
		}
		else if (osVersion.dwBuildNumber == 15063)
		{
			protection = 0x6CA;
			flags2 = 0x300;
			objecttable = 0x418;
			vadroot = 0x628;
		}
		else if (osVersion.dwBuildNumber == 16299)
		{
			protection = 0x6CA;
			flags2 = 0x828;
			objecttable = 0x418;
			vadroot = 0x628;
		}
		else if (osVersion.dwBuildNumber == 17134)
		{
			protection = 0x6CA;
			flags2 = 0x828;
			objecttable = 0x418;
			vadroot = 0x628;
		}
		else if (osVersion.dwBuildNumber == 17763) {
			protection = 0x6CA;
			vadroot = 0x628;
			flags2 = 0x820;
			objecttable = 0x418;
		}
		else if (osVersion.dwBuildNumber == 18362 || osVersion.dwBuildNumber == 18363) {
			protection = 0x6FA;
			flags2 = 0x850;
			objecttable = 0x418;
			vadroot = 0x658;
		}
		else if (osVersion.dwBuildNumber == 19041) {
			protection = 0x87A;
			flags2 = 0x9D4;
			objecttable = 0x570;
			vadroot = 0x7D8;
		}
	}
};