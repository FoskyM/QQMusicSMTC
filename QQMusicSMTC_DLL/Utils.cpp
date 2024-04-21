#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <windows.h>
#include <tlhelp32.h>
#include <fstream>
#include <codecvt>
#include <locale>

using namespace std;

wstring to_wstring(const string& str) {
    int wideStringSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    if (wideStringSize == 0) {
        // Error handling
        return L"";
    }

    wstring wideString(wideStringSize, L'\0');
    if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wideString[0], wideStringSize) == 0) {
        // Error handling
        return L"";
    }

    // Remove the null terminator added by MultiByteToWideChar
    wideString.resize(wideStringSize - 1);

    return wideString;
}

string w_to_string(const wchar_t* wideCharArray) {
    std::wstring wideString(wideCharArray);
    std::string narrowString(wideString.begin(), wideString.end());

    return narrowString;
}

char* to_cstr_dyn(string str) {
    char* cstr = new char[str.length() + 1];
    strcpy_s(cstr, str.length() + 1, str.c_str());
    return cstr;
}

void splitString(const string& s, vector<string>& v, const string& c)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}

string strMiddle(const string& str, const string& start, const string& end) {
    size_t start_pos = str.find(start);
    if (start_pos == string::npos) return "";
    size_t end_pos = str.find(end, start_pos + start.length());
    if (end_pos == string::npos) return "";
    return str.substr(start_pos + start.length(), end_pos - start_pos - start.length());
}

string convertUnicodeSlash(const string& input) {
    string output;
    output.reserve(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '\\' && i + 5 < input.size() && input[i + 1] == 'u') {
            if (input[i + 2] == '0' && input[i + 3] == '0' && input[i + 4] == '2' && input[i + 5] == 'F') {
                output.push_back('/');
                i += 5;
                continue;
            }
        }
        output.push_back(input[i]);
    }

    return output;
}

string Utf8ToAnsi(const string& str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wstr, len);

	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* utf8char = new char[len + 1];
	memset(utf8char, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, utf8char, len, NULL, NULL);

	string strTemp = utf8char;
	if (wstr) delete[] wstr;
	if (utf8char) delete[] utf8char;

	return strTemp;
}

string AnsiToUtf8(const string& str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wstr, len);

	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* utf8char = new char[len + 1];
	memset(utf8char, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8char, len, NULL, NULL);

	string strTemp = utf8char;
	if (wstr) delete[] wstr;
	if (utf8char) delete[] utf8char;

	return strTemp;
}