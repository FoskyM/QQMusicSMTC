#include <string>
#include <windows.h>

using namespace std;

wstring to_wstring(const string& str);
string w_to_string(const wchar_t* wideCharArray);
char* to_cstr_dyn(string str);

void splitString(const string& s, vector<string>& v, const string& c);
string strMiddle(const string& str, const string& start, const string& end);

string Utf8ToAnsi(const string& str);
string AnsiToUtf8(const string& str);
string convertUnicodeSlash(const string& input);