#include <string>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Foundation.h>
#include <windows.h>
#include "QQMusicHelper.h"
#include "MD5.h"
#include "Utils.h"

using namespace winrt;
using namespace Windows::Web::Http;
using namespace Windows::Foundation;
using namespace std;

string QQMusicHelper::sign(const string& data) {
    unordered_map<char, int> k1 = { {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
                                        {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15} };
    vector<int> l1 = { 212, 45, 80, 68, 195, 163, 163, 203, 157, 220, 254, 91, 204, 79, 104, 6 };
    string t = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

    string md5 = MD5(data).toStr();
    transform(md5.begin(), md5.end(), md5.begin(), ::toupper);

    string t1 = string(1, md5[21]) + string(1, md5[4]) + string(1, md5[9]) + string(1, md5[26]) +
        string(1, md5[16]) + string(1, md5[20]) + string(1, md5[27]) + string(1, md5[30]);
    string t3 = string(1, md5[18]) + string(1, md5[11]) + string(1, md5[3]) + string(1, md5[2]) +
        string(1, md5[1]) + string(1, md5[7]) + string(1, md5[6]) + string(1, md5[25]);

    vector<int> ls2;
    for (size_t i = 0; i < 16; i++) {
        int x1 = k1[md5[i * 2]];
        int x2 = k1[md5[i * 2 + 1]];
        int x3 = ((x1 * 16) ^ x2) ^ l1[i];
        ls2.push_back(x3);
    }

    vector<char> ls3;
    for (size_t i = 0; i < 6; i++) {
        if (i == 5) {
            ls3.push_back(t[ls2.back() >> 2]);
            ls3.push_back(t[(ls2.back() & 3) << 4]);
        }
        else {
            int x4 = ls2[i * 3] >> 2;
            int x5 = (ls2[i * 3 + 1] >> 4) ^ ((ls2[i * 3] & 3) << 4);
            int x6 = (ls2[i * 3 + 2] >> 6) ^ ((ls2[i * 3 + 1] & 15) << 2);
            int x7 = 63 & ls2[i * 3 + 2];
            ls3.push_back(t[x4]);
            ls3.push_back(t[x5]);
            ls3.push_back(t[x6]);
            ls3.push_back(t[x7]);
        }
    }
    string t2;
    for (char c : ls3) {
        if (c != '+' && c != '/') {
            t2 += c;
        }
    }
    string sign = "zzb" + t1 + t2 + t3;

    transform(sign.begin(), sign.end(), sign.begin(), ::tolower);

    return sign;
}

string QQMusicHelper::songInfo(int song_id, string& title, string& artist, string& pic_url, string& album_name) {
    winrt::hstring url = L"https://y.qq.com/n/ryqq/songDetail/";
    url = url + to_hstring(song_id);

    auto httpClient = HttpClient();
    auto httpRequest = HttpRequestMessage(HttpMethod::Get(), Uri(url));

    auto httpResponse = httpClient.SendRequestAsync(httpRequest).get();
    auto responseData = to_string(httpResponse.Content().ReadAsStringAsync().get());

    string json = strMiddle(responseData, "window.__INITIAL_DATA__ =", "</script><script");
    // json = Utf8ToAnsi(json);
    json = strMiddle(json, "{\"detail\":{", ",\"songList\"");
    title = strMiddle(json, "\"title\":\"", "\"");

    pic_url = "https:" + convertUnicodeSlash(strMiddle(json, "\"picurl\":\"", "\""));
    album_name = strMiddle(json, "\"albumName\":\"", "\"");
    json = strMiddle(json, "\"singer\":[", "\],\"albumName");

    vector<string> artists_info;
    if (json.find("},{") != string::npos) {
        splitString(json, artists_info, "},{");
    }
    else {
        artists_info.push_back(json);
    }

    for (string& artist_info : artists_info) {
        artist += strMiddle(artist_info, "\"name\":\"", "\"") + " / ";
    }

    artist = artist.substr(0, artist.length() - 3);

    return responseData;
}