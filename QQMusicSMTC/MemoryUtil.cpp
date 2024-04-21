#include <vector>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include "Utils.h"
#include "MemoryUtil.h"

using namespace std;

void MemoryUtil::Load(DWORD pid, int& length, int& song_id) {
    DWORD _processId;
    HANDLE _entryPoint;
    uintptr_t _baseAddress;
    string _version;

    _entryPoint = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    _baseAddress = NULL;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 moduleEntry;
        moduleEntry.dwSize = sizeof(MODULEENTRY32);

        if (Module32First(hSnapshot, &moduleEntry)) {
            do {
                // cout << moduleEntry.szModule << endl;
                if (wcscmp(moduleEntry.szModule, L"QQMusic.dll") == 0) {
                    _baseAddress = (uintptr_t)moduleEntry.modBaseAddr;
                    break;
                }

            } while (Module32Next(hSnapshot, &moduleEntry));
        }

        CloseHandle(hSnapshot);
    }


    if (_entryPoint == NULL || _baseAddress == NULL) {
        return;
    }

    _processId = pid;

    // 2013.19.03.03
    // MusicSongId -> QQMusic.dll+B06778
    // MusicTotalTime -> QQMusic.dll+B0678C
    // B0678C - B06778 = 0x14

    uintptr_t offset_music_len = 0xB0678C;
    int buffer_music_len;

    uintptr_t offset_music_id = 0xB06778;
    int buffer_music_id;

    if (!ReadProcessMemory(_entryPoint, (LPCVOID)(_baseAddress + offset_music_len), &buffer_music_len, sizeof(buffer_music_len), NULL)) {
        return;
    }

    if (!ReadProcessMemory(_entryPoint, (LPCVOID)(_baseAddress + offset_music_id), &buffer_music_id, sizeof(buffer_music_id), NULL)) {
        return;
    }

    length = buffer_music_len;
    song_id = buffer_music_id;
}
// 取窗口标题获取音乐信息的方法
void MemoryUtil::Title(HWND hWnd, std::string& title, std::string& artist) {
    const int nMaxCount = 256;
    wchar_t szTitle[nMaxCount];
    GetWindowText(hWnd, szTitle, nMaxCount);
    vector<string> arr;
    splitString(w_to_string(szTitle), arr, " - ");
    title = arr[0];
    artist = arr[1];
}