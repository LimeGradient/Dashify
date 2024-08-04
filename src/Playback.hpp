#include <Geode/Geode.hpp>

class Artist {
public:
    Artist(std::string name) {
        this->name = name;
    }
    std::string name;
};

class Playback {
public:
    Playback() {}

    std::string songName;
    std::string albumName;
    std::vector<Artist*> artists;
    std::string albumCoverURL;
};