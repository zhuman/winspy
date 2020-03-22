//
//	DisplayProcessInfo.c
//  Copyright (c) 2002 by J Brown 
//	Freeware
//
//	void SetProcesInfo(HWND hwnd)
//
//	Fill the process-tab-pane with proces info for the
//  specified window. 
//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <psapi.h>

#include "WinSpy.h"
#include "resource.h"

//
// This uses PSAPI.DLL, which is only available under NT/2000/XP I think,
// so we dynamically load this library, so that we can still run under 9x.
//
//	dwProcessId  [in]
//  szName       [out]
//  nNameSize    [in]
//  szPath       [out]
//  nPathSize    [in]	
//
BOOL GetProcessNameByPid(DWORD dwProcessId, TCHAR szName[], DWORD nNameSize, TCHAR szPath[], DWORD nPathSize)
{
	HANDLE hProcess;

	HMODULE hModule;
	DWORD   dwNumModules;
	
	// OK, we have access to the PSAPI functions, so open the process
	hProcess = OpenProcess(
		//PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
		PROCESS_QUERY_LIMITED_INFORMATION,
		FALSE, dwProcessId);

	if(!hProcess) 
	{
		return FALSE;
	}

	szName[0] = _T('\0');
	szPath[0] = _T('\0');

	TCHAR TempPath[MAX_PATH];
	DWORD TempPathSize = ARRAYSIZE(TempPath);

	if (QueryFullProcessImageName(hProcess, 0, TempPath, &TempPathSize))
	{
		if (szPath)
			wcsncpy_s(szPath, nPathSize, TempPath, ARRAYSIZE(TempPath));

		TCHAR* szNameString = wcsrchr(TempPath, L'\\');
		if (szNameString != NULL)
			szNameString++;

		if (szName)
			wcsncpy_s(szName, nNameSize, szNameString, ARRAYSIZE(TempPath));
	}
	
	CloseHandle(hProcess);

	return TRUE;
}

//
//	Update the Process tab for the specified window
//
void SetProcessInfo(HWND hwnd)
{
	DWORD dwProcessId;
	DWORD dwThreadId;
	TCHAR ach[32];
	TCHAR szPath[MAX_PATH];

	HWND  hwndDlg = WinSpyTab[PROCESS_TAB].hwnd; 

	dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId);

	// Process Id
	wsprintf(ach, _T("%08X  (%u)"), dwProcessId, dwProcessId);
	SetDlgItemText(hwndDlg, IDC_PID, ach);

	// Thread Id
	wsprintf(ach, _T("%08X  (%u)"), dwThreadId, dwThreadId);
	SetDlgItemText(hwndDlg, IDC_TID, ach);

	// Try to get process name and path
	if(GetProcessNameByPid(dwProcessId, ach,    sizeof(ach)    / sizeof(TCHAR),
										szPath, sizeof(szPath) / sizeof(TCHAR)))
	{
		SetDlgItemText(hwndDlg, IDC_PROCESSNAME, ach);
		SetDlgItemText(hwndDlg, IDC_PROCESSPATH, szPath);
	}
	else
	{
		SetDlgItemText(hwndDlg, IDC_PROCESSNAME, _T("N/A"));
		SetDlgItemText(hwndDlg, IDC_PROCESSNAME, _T("N/A"));
	}


}
