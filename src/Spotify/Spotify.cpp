#include <Geode/Geode.hpp>
#include "Spotify.hpp"

#include "../lib/base64.hpp"
#include "AuthWebserver.hpp"

void Spotify::init() {
    if (Mod::get()->getSavedValue<std::string>("spotify-code").empty()) {
        Mod::get()->setSavedValue<std::string>("spotify-code", "MDRkNTcxMWFjZWRkNGE2NWE1N2YzYjI5YmYwZWNmNjU=");
    }

    this->spotifyToken = Mod::get()->getSavedValue<std::string>("spotify-code");
    Webserver* server = new Webserver();
    server->createServer();
/*     this->m_webListener.bind([=, this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->ok()) {
                auto jsonRes = res->json().unwrap();
                this->accessToken = jsonRes["access_token"].as_string();
                log::info("access token for this session: {}", this->accessToken);
            }
        } else if (e->isCancelled()) {
            log::info("The request was cancelled... So sad :(");
        }
    });

    auto req = web::WebRequest();
    req.param("grant_type","client_credentials");
    req.param("client_id", "32c1a06b2faa4414aa19f3b4caa06a47");
    req.param("client_secret", base64::from_base64(this->spotifyToken));
    req.header("Content-Type", "application/x-www-form-urlencoded");
    this->m_webListener.setFilter(req.post("https://accounts.spotify.com/api/token")); */

    //web::openLinkInBrowser("https://accounts.spotify.com/en/authorize?response_type=code&client_id=32c1a06b2faa4414aa19f3b4caa06a47&scope=user-read-private%20user-read-email&redirect_uri=https%3A%2F%2Fexample.org%2Fcallback");
}

Playback* Spotify::getCurrentPlayback() {
    Playback* playback = new Playback();

    this->m_webListener.bind([=, this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->ok()) {
                auto jsonRes = res->json().unwrap();
                playback->songName = jsonRes["item"]["name"].as_string();
                playback->albumName = jsonRes["item"]["album"]["name"].as_string();
                playback->albumCoverURL = jsonRes["item"]["album"]["images"][0]["url"].as_string();

                auto artists = jsonRes["item"]["artists"].as_array();
                for (auto artist : artists) {
                    playback->artists.push_back(new Artist(artist["name"].as_string()));
                }
            }
        } else if (e->isCancelled()) {
            log::info("The request was cancelled... So sad :(");
        }
    });

    auto req = web::WebRequest();
    req.header("Authorization", fmt::format("Bearer {}", this->accessToken));
    this->m_webListener.setFilter(req.get("https://api.spotify.com/v1/me/player"));

    return playback;
}