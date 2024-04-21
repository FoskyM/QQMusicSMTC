#include <string>

class QQMusicHelper {
public:
    static std::string sign(const std::string& data);
    static std::string songInfo(int song_id, std::string& title, std::string& artist, std::string& pic_url, std::string& album_name);
};