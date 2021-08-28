#include <windows.h>
#include <string>
#include <windows.h>
#include <string>
#include <intrin.h>
#include <string.h>
#include "hwid.h"
#include "xor.h"
#include "print.h"

char	m_szHardwareId[33];
char	szRawHardwareId[512];

char Serial::GetSystemDriveLetter(void)
{
	char szSystemDir[MAX_PATH];

	if (!GetSystemDirectoryA(szSystemDir, sizeof(szSystemDir)))
		return '\0';

	//report_info("L: %c", szSystemDir[0]);

	return szSystemDir[0];
}

DWORD Serial::get_system_physical_drive_index(void)
{
	char szDrivePath[MAX_PATH];
	sprintf_s(szDrivePath, xor_a("\\\\.\\%c:"), GetSystemDriveLetter());

	HANDLE hHandle = CreateFile(szDrivePath, 0,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hHandle == INVALID_HANDLE_VALUE)
		return -1;

	VOLUME_DISK_EXTENTS DiskExtents;

	DWORD dwBytesReturned = 0;

	BOOL bResult = DeviceIoControl(hHandle, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
		NULL, 0, &DiskExtents, sizeof(DiskExtents), &dwBytesReturned, NULL);

	CloseHandle(hHandle);

	if (!bResult)
		return -1;

	//report_info("N: %d", DiskExtents.Extents[0].DiskNumber);

	return DiskExtents.Extents[0].DiskNumber;
}

PCHAR Serial::GetPhysicalDriveSN(char* pszSerialNumber, size_t uMaxLength)
{
	PCHAR pszResult = NULL;

	char szPhysicalDrive[MAX_PATH];
	sprintf_s(szPhysicalDrive,
		xor_a("\\\\.\\PhysicalDrive%d"), get_system_physical_drive_index());

	//report_info("PhysDrive: %s", szPhysicalDrive);

	HANDLE hPhysicalDrive = CreateFileA(szPhysicalDrive, 0,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hPhysicalDrive == INVALID_HANDLE_VALUE)
		return pszResult;

	STORAGE_PROPERTY_QUERY Query;
	ZeroMemory(&Query, sizeof(Query));
	Query.QueryType = PropertyStandardQuery;
	Query.PropertyId = StorageDeviceProperty;

	STORAGE_DESCRIPTOR_HEADER DescHeader;
	ZeroMemory(&DescHeader, sizeof(DescHeader));

	DWORD dwBytesReturned = 0;

	BOOL bResult = DeviceIoControl(hPhysicalDrive, IOCTL_STORAGE_QUERY_PROPERTY,
		&Query, sizeof(Query), &DescHeader, sizeof(DescHeader), &dwBytesReturned, NULL);

	if (!bResult)
		goto CleanUp;

	const auto p_device_desc = static_cast<PSTORAGE_DEVICE_DESCRIPTOR>(malloc(DescHeader.Size));

	if (!p_device_desc)
		goto CleanUp;

	bResult = DeviceIoControl(hPhysicalDrive, IOCTL_STORAGE_QUERY_PROPERTY,
		&Query, sizeof(Query), p_device_desc, DescHeader.Size, &dwBytesReturned, NULL);

	if (bResult && p_device_desc->SerialNumberOffset)
	{
		char* psz_device_sn = (reinterpret_cast<char*>(p_device_desc) + p_device_desc->SerialNumberOffset);

		strcpy_s(pszSerialNumber, uMaxLength, psz_device_sn);

		pszResult = pszSerialNumber;
	}

CleanUp:
	if (hPhysicalDrive)
		CloseHandle(hPhysicalDrive);

	//if (p_device_desc)
	//	free(p_device_desc);

	return pszResult;
}

PCHAR Serial::GetHwProfileGuid(char* pszHwProfileGuid)
{
	HW_PROFILE_INFOA HwProfileInfo;

	if (!GetCurrentHwProfileA(&HwProfileInfo))
		return NULL;

	strcpy_s(pszHwProfileGuid, 39, HwProfileInfo.szHwProfileGuid);

	return pszHwProfileGuid;
}

PCHAR Serial::GetCPUVendorId(char* pszCPUVendorId)
{
	int iCPUInfo[4];
	__cpuid(iCPUInfo, 0);
	*((int*)(pszCPUVendorId)) = iCPUInfo[1];
	*((int*)(pszCPUVendorId + 4)) = iCPUInfo[3];
	*((int*)(pszCPUVendorId + 8)) = iCPUInfo[2];

	return pszCPUVendorId;
}

PCHAR Serial::GetHardwareId(std::string id)
{
	// check if hwid already generated
	if (m_szHardwareId[0] && (strlen(m_szHardwareId) == 32))
		return m_szHardwareId;

	// part 1
	char szPhysicalDriveSN[256];
	ZeroMemory(szPhysicalDriveSN, sizeof(szPhysicalDriveSN));

	if (!GetPhysicalDriveSN(szPhysicalDriveSN, sizeof(szPhysicalDriveSN)))
	{
		print::set_error("[ldr_c] HI pt. 1 failed!");
		return NULL;
	}

	// part 2
	char szHwProfileGuid[40];
	ZeroMemory(szHwProfileGuid, sizeof(szHwProfileGuid));

	if (!GetHwProfileGuid(szHwProfileGuid))
	{
		print::set_error("[ldr_c] HI pt. 2 failed!");
		return NULL;
	}

	// part 3
	char szCPUVendorId[13];
	ZeroMemory(szCPUVendorId, sizeof(szCPUVendorId));

	if (!GetCPUVendorId(szCPUVendorId))
	{
		print::set_error("[ldr_c] HI pt. 3 failed!");
		return NULL;
	}

	// part 4

	sprintf_s(szRawHardwareId, "%s@%s_%s_%s", id.c_str(), szCPUVendorId, szPhysicalDriveSN, szHwProfileGuid);
	return szRawHardwareId;
}