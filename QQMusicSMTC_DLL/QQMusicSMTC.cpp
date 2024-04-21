#include "pch.h"
#include <windows.h>
#include <vector>
#include <string>
#include <tlhelp32.h>
#include <fstream>
#include <future>
#include <chrono>

#include "Utils.h"
#include "SMTC.h"
#include "Audio.h"
#include "QQMusicHelper.h" 
#include "MemoryUtil.h"

using namespace std;

void btn_callback(int state)
{
	/*{
		6 : "NextSong",
		7 : "PreviousSong",
		1 : "Pause",
		0 : "Play",
	}*/
}

bool isSMTCEnabled = false;

void async_smtc_update() {
	LPCWSTR className = L"QQMusic_Daemon_Wnd";
	HWND hWnd = FindWindowEx(NULL, NULL, className, NULL);

	int last_song_id = 0;

	while (isSMTCEnabled) {
		this_thread::sleep_for(chrono::seconds(3));
		SMTC::updatePlayState(isQQMusicPlaying() ? 3 : 4);
		DWORD dwProcessId = 0;

		if (hWnd == NULL) {
			hWnd = FindWindowEx(NULL, NULL, className, NULL);
		}
		if (hWnd == 0) {
			continue;
		}
		if (dwProcessId == 0) {
			GetWindowThreadProcessId(hWnd, &dwProcessId);
		}

		int length;
		int song_id;

		MemoryUtil::Load(dwProcessId, length, song_id);

		// updateSMTCTimeline(0, length);

		if (song_id == last_song_id) {
			continue;
		}
		last_song_id = song_id;

		string title;
		string artist;
		string pic_url;
		string album_name;
		QQMusicHelper::songInfo(song_id, title, artist, pic_url, album_name);

		char* artist_c = to_cstr_dyn(artist);
		char* title_c = to_cstr_dyn(title);
		char* album_name_c = to_cstr_dyn(album_name);
		char* pic_url_c = to_cstr_dyn(pic_url);

		SMTC::update(title_c, album_name_c, artist_c, pic_url_c);
	}
}

void start_SMTC() {
	SMTC::enable();
	SMTC::registerEventCallback(btn_callback);
	isSMTCEnabled = true;
	auto future = async(launch::async, async_smtc_update);
}

void exit_SMTC() {
	SMTC::disable();
}