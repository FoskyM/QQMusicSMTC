#include "pch.h"

#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#pragma comment(lib, "Winmm.lib")


void GetActiveAudioApplications(std::vector<std::wstring>& activeApps) {
	HRESULT hr;
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDevice* pDevice = NULL;
	IAudioSessionManager2* pManager = NULL;
	IAudioSessionEnumerator* pSessionEnumerator = NULL;
	IAudioSessionControl* pSessionControl = NULL;

	CoInitialize(NULL);

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
		(void**)&pEnumerator);
	if (FAILED(hr)) {
		CoUninitialize();
		return;
	}

	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	if (FAILED(hr)) {
		pEnumerator->Release();
		CoUninitialize();
		return;
	}

	hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
		NULL, (void**)&pManager);
	if (FAILED(hr)) {
		pDevice->Release();
		pEnumerator->Release();
		CoUninitialize();
		return;
	}

	hr = pManager->GetSessionEnumerator(&pSessionEnumerator);
	if (FAILED(hr)) {
		pManager->Release();
		pDevice->Release();
		pEnumerator->Release();
		CoUninitialize();
		return;
	}

	int count;
	hr = pSessionEnumerator->GetCount(&count);
	if (FAILED(hr)) {
		pSessionEnumerator->Release();
		pManager->Release();
		pDevice->Release();
		pEnumerator->Release();
		CoUninitialize();
		return;
	}

	for (int i = 0; i < count; ++i) {
		hr = pSessionEnumerator->GetSession(i, &pSessionControl);
		if (FAILED(hr)) {
			continue;
		}

		AudioSessionState state;
		hr = pSessionControl->GetState(&state);
		if (FAILED(hr)) {
			pSessionControl->Release();
			continue;
		}

		if (state == AudioSessionStateActive) {
			IAudioSessionControl2* pSessionControl2 = NULL;
			hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2);
			if (SUCCEEDED(hr)) {
				DWORD processId;
				hr = pSessionControl2->GetProcessId(&processId);
				if (SUCCEEDED(hr)) {
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
					if (hProcess != NULL) {
						WCHAR szProcessName[MAX_PATH] = L"";
						DWORD dwSize = MAX_PATH;
						QueryFullProcessImageName(hProcess, 0, szProcessName, &dwSize);
						activeApps.push_back(szProcessName);
						CloseHandle(hProcess);
					}
				}
				pSessionControl2->Release();
			}
		}

		// 释放会话控制
		pSessionControl->Release();
	}

	// 释放资源
	pSessionEnumerator->Release();
	pManager->Release();
	pDevice->Release();
	pEnumerator->Release();
	CoUninitialize();
}
// 判断 QQ 音乐是否正在播放，播放时无延迟，暂停时会有延迟（微软搞什么）
bool isQQMusicPlaying() {
	std::vector<std::wstring> activeApps;
	GetActiveAudioApplications(activeApps);
	for (const auto& app : activeApps) {
		if (app.find(L"QQMusic.exe") != std::wstring::npos) {
			return true;
		}
	}
	return false;
}