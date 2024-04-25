#include <iostream>
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

void btnCallback(int state)
{
	/*{
		6 : "NextSong",
		7 : "PreviousSong",
		1 : "Pause",
		0 : "Play",
	}*/
}
bool isSMTCEnabled = false;

void exit_SMTC() {
	SMTC::disable();
}

void asyncSMTCUpdate() {
	LPCWSTR className = L"QQMusic_Daemon_Wnd";
	HWND hWnd = FindWindowEx(NULL, NULL, className, NULL);

	int last_song_id = 0;

	while (isSMTCEnabled) {
		this_thread::sleep_for(chrono::seconds(3));
		SMTC::updatePlayState(isQQMusicPlaying() ? 3 : 4);
		DWORD dwProcessId = 0;

		hWnd = FindWindowEx(NULL, NULL, className, NULL);

		if (hWnd == NULL) {
			exit_SMTC();
			exit(0);
		}
		if (dwProcessId == 0) {
			GetWindowThreadProcessId(hWnd, &dwProcessId);
		}

		int length;
		int song_id;

		MemoryUtil::Load(dwProcessId, length, song_id);

		// updateSMTCTimeline(0, length);

		if (song_id == last_song_id || song_id == 0) {
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
		cout << title_c << album_name_c << artist_c << pic_url_c << endl;
		cout << "Updated SMTC" << endl;
	}
}

void start_SMTC() {
	SMTC::enable();
	SMTC::registerEventCallback(btnCallback);
	isSMTCEnabled = true;
	auto future = async(launch::async, asyncSMTCUpdate);
}

int main() {
	SetConsoleTitle(L"QQMusicSMTC");
	SetConsoleOutputCP(CP_UTF8);
	start_SMTC();
	return 0;
}
