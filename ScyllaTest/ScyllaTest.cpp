// ScyllaTest.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include "debug.h"



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
def_ScyllaIatFixAutoW  ScyllaIatFixAutoW = 0;

//void testGui();
void IATAutoFix(DWORD pid, WCHAR *dump, WCHAR *outputFile);
DWORD_PTR GetExeModuleBase(DWORD dwProcessId);


WCHAR target[] = L"../compiled.exe";
HMODULE hScylla = 0;

/**
Load Scylla library from dll and importing usefull functions
**/
void LoadScyllaLibrary(){
	#ifdef _WIN64
		hScylla = LoadLibraryW(L"../ScyllaDLLx64.dll");
	#else
		hScylla = LoadLibraryW(L"../ScyllaDLLx86.dll");
	#endif
		INFO("Loading scylla\n ");
		if (hScylla)
		{
			ScyllaIatSearch = (def_ScyllaIatSearch)GetProcAddress(hScylla, "ScyllaIatSearch");
			ScyllaIatFixAutoW = (def_ScyllaIatFixAutoW)GetProcAddress(hScylla, "ScyllaIatFixAutoW");
		}

}

int wmain(int argc, wchar_t *argv[])
{	
	if(argc < 4){
		INFO("ScyllaTest.exe <pid> <dump_file> <output_file>");
		return -1;
	}
	DWORD pid = _wtoi(argv[1]);
	WCHAR *dump = argv[2];
	WCHAR *outputFile = argv[3];
	//Loading Scylla Functions
	LoadScyllaLibrary();
	IATAutoFix(pid, dump, outputFile);
	return 0;
}



void IATAutoFix(DWORD pid, WCHAR *dump, WCHAR *outputFile)
{
	INFO("----------------\nIAT Fixing Test\n----------------\n");

	DWORD_PTR iatStart = 0;
	DWORD iatSize = 0;

	DWORD_PTR hMod = GetExeModuleBase(pid);
	if(!hMod){
		DEBUG("Can't find PID");
	}
	INFO("GetExeModuleBase %X\n", hMod);

	//
	int error = ScyllaIatSearch(pid, &iatStart, &iatSize, hMod + 0x00001028, TRUE);
	if(error){
		ERROR("[IAT SEARCH] error %d",error);
		return;
	}
	printf("[IAT SEARCH] iatStart %X iatSize %X\n",iatStart, iatSize);
	
	error = ScyllaIatFixAutoW(iatStart,iatSize,pid,dump,outputFile);
	if(error){
		ERROR("[IAT FIX]error %d",error);
		return;
	}
	INFO("[IAT FIX] Success");

	
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


