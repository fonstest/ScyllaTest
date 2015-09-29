// ScyllaTest.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"



typedef const WCHAR * (WINAPI * def_ScyllaVersionInformationW)();
typedef const char * (WINAPI * def_ScyllaVersionInformationA)();
typedef DWORD (WINAPI * def_ScyllaVersionInformationDword)();
typedef int (WINAPI * def_ScyllaIatSearch)(DWORD dwProcessId, DWORD_PTR * iatStart, DWORD * iatSize, DWORD_PTR searchStart, BOOL advancedSearch);
typedef int (WINAPI * def_ScyllaStartGui)(DWORD dwProcessId, HINSTANCE mod);
//added by me 
typedef int  (WINAPI * def_ScyllaIatFixAutoW)(DWORD_PTR iatAddr, DWORD iatSize, DWORD dwProcessId, const WCHAR * dumpFile, const WCHAR * iatFixFile);


	 
def_ScyllaIatSearch ScyllaIatSearch = 0;
def_ScyllaStartGui ScyllaStartGui = 0;
def_ScyllaVersionInformationW ScyllaVersionInformationW = 0;
def_ScyllaVersionInformationA ScyllaVersionInformationA = 0;
def_ScyllaVersionInformationDword ScyllaVersionInformationDword = 0;
def_ScyllaIatFixAutoW  ScyllaIatFixAutoW=0;

//void testGui();
void testIatSearch();
DWORD_PTR GetExeModuleBase(DWORD dwProcessId);


STARTUPINFOW si = { 0 };
PROCESS_INFORMATION pi = { 0 };
WCHAR target[] = L"../compiled.exe";
HMODULE hScylla = 0;

void LoadScyllaLibrary(){
	#ifdef _WIN64
		hScylla = LoadLibraryW(L"../ScyllaDLLx86.dll");
	#else
		hScylla = LoadLibraryW(L"../ScyllaDLLx86.dll");
	#endif
		printf("Loading scylla\n ");
		if (hScylla)
		{
			ScyllaIatSearch = (def_ScyllaIatSearch)GetProcAddress(hScylla, "ScyllaIatSearch");
		
			ScyllaVersionInformationW = (def_ScyllaVersionInformationW)GetProcAddress(hScylla, "ScyllaVersionInformationW");
			ScyllaVersionInformationA = (def_ScyllaVersionInformationA)GetProcAddress(hScylla, "ScyllaVersionInformationA");
			ScyllaVersionInformationDword = (def_ScyllaVersionInformationDword)GetProcAddress(hScylla, "ScyllaVersionInformationDword");

			printf("Scylla DLL: %s - %08X\n", ScyllaVersionInformationA(), ScyllaVersionInformationDword());

			IATAutoFix();
			//testGui();
		}

}

int main(int argc, char *argv[])
{


	getchar();
	return 0;
}



void IATAutoFix()
{
	printf("----------------\nIAT Search Test\n----------------\n");

	si.cb = sizeof(STARTUPINFOW);
	printf("launchig %s",target);
	if (CreateProcessW(0, target, 0, 0, TRUE, 0, 0, 0, &si, &pi))
	{
		Sleep(1000);

		DWORD_PTR iatStart = 0;
		DWORD iatSize = 0;

		DWORD_PTR hMod = GetExeModuleBase(pi.dwProcessId);
		printf("GetExeModuleBase %X\n", hMod);

		//
		int error = ScyllaIatSearch(pi.dwProcessId, &iatStart, &iatSize, hMod + 0x00001028, TRUE);

		printf("error %d iatStart %X iatSize %X\n", error, iatStart, iatSize);

		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}



DWORD_PTR GetExeModuleBase(DWORD dwProcessId)
{
	MODULEENTRY32 lpModuleEntry = { 0 };
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	lpModuleEntry.dwSize = sizeof(lpModuleEntry);
	Module32First(hSnapShot, &lpModuleEntry);

	CloseHandle(hSnapShot);

	return (DWORD_PTR)lpModuleEntry.modBaseAddr;
}


/*Testing code
void testGui()
{
	printf("----------------\nGUI TEST\n----------------\n");

	si.cb = sizeof(STARTUPINFOW);

	if (CreateProcessW(0, target, 0, 0, TRUE, 0, 0, 0, &si, &pi))
	{
		Sleep(1000);


		DWORD_PTR hMod = GetExeModuleBase(pi.dwProcessId);
		printf("GetExeModuleBase %X\n", hMod);

		ScyllaStartGui(pi.dwProcessId, 0);

		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}
*/
