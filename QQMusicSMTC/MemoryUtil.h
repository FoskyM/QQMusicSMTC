#include <windows.h>

class MemoryUtil {
public:
	static void Load(DWORD pid, int& length, int& song_id);
	static void Title(HWND hWnd, std::string& title, std::string& artist);
};