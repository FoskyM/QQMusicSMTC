
//
// created by AheadLib
// github:https://github.com/strivexjun/AheadLib-x86-x64
//
#include "pch.h"
#include "QQMusicSMTC.h"
#include <windows.h>
#include <Shlwapi.h>

#pragma comment( lib, "Shlwapi.lib")

#pragma comment(linker, "/EXPORT:arkCreateStub=_AheadLib_arkCreateStub,@1")
#pragma comment(linker, "/EXPORT:arkDeleteStub=_AheadLib_arkDeleteStub,@2")
#pragma comment(linker, "/EXPORT:arkRegisterFS=_AheadLib_arkRegisterFS,@3")
#pragma comment(linker, "/EXPORT:cfileStubCreate=_AheadLib_cfileStubCreate,@4")
#pragma comment(linker, "/EXPORT:cfileStubDelete=_AheadLib_cfileStubDelete,@5")


PVOID pfnAheadLib_arkCreateStub;
PVOID pfnAheadLib_arkDeleteStub;
PVOID pfnAheadLib_arkRegisterFS;
PVOID pfnAheadLib_cfileStubCreate;
PVOID pfnAheadLib_cfileStubDelete;


static
HMODULE g_OldModule = NULL;

VOID WINAPI Free()
{
	if (g_OldModule)
	{
		FreeLibrary(g_OldModule);
	}
}


BOOL WINAPI Load()
{
	TCHAR tzPath[MAX_PATH];
	TCHAR tzTemp[MAX_PATH * 2];

	//
	// �����Ƿ��ϵͳĿ¼��ǰĿ¼����ԭʼDLL
	//
	GetModuleFileName(NULL,tzPath,MAX_PATH); //��ȡ��Ŀ¼�µ�
	PathRemoveFileSpec(tzPath);

	//GetSystemDirectory(tzPath, MAX_PATH); //Ĭ�ϻ�ȡϵͳĿ¼��

	lstrcat(tzPath, TEXT("\\arkIOStubOrg.dll"));

	g_OldModule = LoadLibrary(tzPath);
	if (g_OldModule == NULL)
	{
		wsprintf(tzTemp, TEXT("�޷��ҵ�ģ�� %s,�����޷���������"), tzPath);
		MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
	}

	return (g_OldModule != NULL);

}


FARPROC WINAPI GetAddress(PCSTR pszProcName)
{
	FARPROC fpAddress;
	CHAR szProcName[64];
	TCHAR tzTemp[MAX_PATH];

	fpAddress = GetProcAddress(g_OldModule, pszProcName);
	if (fpAddress == NULL)
	{
		if (HIWORD(pszProcName) == 0)
		{
			wsprintfA(szProcName, "#%d", pszProcName);
			pszProcName = szProcName;
		}

		wsprintf(tzTemp, TEXT("�޷��ҵ����� %hs,�����޷���������"), pszProcName);
		MessageBox(NULL, tzTemp, TEXT("AheadLib"), MB_ICONSTOP);
		ExitProcess(-2);
	}
	return fpAddress;
}

BOOL WINAPI Init()
{
	pfnAheadLib_arkCreateStub = GetAddress("arkCreateStub");
	pfnAheadLib_arkDeleteStub = GetAddress("arkDeleteStub");
	pfnAheadLib_arkRegisterFS = GetAddress("arkRegisterFS");
	pfnAheadLib_cfileStubCreate = GetAddress("cfileStubCreate");
	pfnAheadLib_cfileStubDelete = GetAddress("cfileStubDelete");
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		if (Load() && Init())
		{
			TCHAR szAppName[MAX_PATH] = TEXT("QQMusic.exe");//���޸�����������
			TCHAR szCurName[MAX_PATH];

			GetModuleFileName(NULL, szCurName, MAX_PATH);
			PathStripPath(szCurName);

			//�Ƿ��ж�����������
			if (StrCmpI(szCurName, szAppName) == 0)
			{
				start_SMTC();
			}
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		exit_SMTC();
		Free();
	}

	return TRUE;
}

EXTERN_C __declspec(naked) void __cdecl AheadLib_arkCreateStub(void)
{
	__asm jmp pfnAheadLib_arkCreateStub;
}

EXTERN_C __declspec(naked) void __cdecl AheadLib_arkDeleteStub(void)
{
	__asm jmp pfnAheadLib_arkDeleteStub;
}

EXTERN_C __declspec(naked) void __cdecl AheadLib_arkRegisterFS(void)
{
	__asm jmp pfnAheadLib_arkRegisterFS;
}

EXTERN_C __declspec(naked) void __cdecl AheadLib_cfileStubCreate(void)
{
	__asm jmp pfnAheadLib_cfileStubCreate;
}

EXTERN_C __declspec(naked) void __cdecl AheadLib_cfileStubDelete(void)
{
	__asm jmp pfnAheadLib_cfileStubDelete;
}

