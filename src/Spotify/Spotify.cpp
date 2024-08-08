#include "AuthWebserver.hpp"
#include <Geode/Geode.hpp>
#include "Spotify.hpp"

#include "../lib/base64.hpp"

void Spotify::init() {
    if (!this->m_isWebserverCreated) {
        this->m_webserverThread = std::thread([=, this] {
            Webserver* server = new Webserver(this);
            server->createServer();
        });
        log::info("created webserver successfully");
        this->m_webserverThread.detach();
    }

    auto refreshToken = Mod::get()->getSavedValue<std::string>("refresh_token");

    this->m_refreshTokenListener.bind([=, this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->ok()) {
                auto resJson = res->json().unwrap();
                Mod::get()->setSavedValue<std::string>("access_token", resJson["access_token"].as_string());
                Mod::get()->setSavedValue<std::string>("refresh_token", resJson["refresh_token"].as_string());
            }
        } else if (e->isCancelled()) {
            log::info("The request was cancelled... So sad :(");
        }
    });

    auto req = web::WebRequest();
    req.header("content-type", "application/x-www-form-urlencoded");
    req.bodyString(fmt::format("grant_type=refresh_token&refresh_token={}&client_id={}&client_secret={}", 
        refreshToken,
        this->clientID,
        base64::from_base64(this->spotifyToken)
    ));
    this->m_refreshTokenListener.setFilter(req.post("https://accounts.spotify.com/api/token"));
}

void Spotify::getAccessToken(std::string code) {
    this->m_webListener2.bind([=, this] (web::WebTask::Event* e) {
        if (web::WebResponse* res = e->getValue()) {
            if (res->ok()) {
                auto resJson = res->json().unwrap();
                Mod::get()->setSavedValue<std::string>("access_token", resJson["access_token"].as_string());
                Mod::get()->setSavedValue<std::string>("refresh_token", resJson["refresh_token"].as_string());
                log::info("saved refresh token");
            }
        } else if (e->isCancelled()) {
            log::info("The request was cancelled... So sad :(");
        }
    });

    auto req = web::WebRequest();
    req.bodyString(fmt::format("code={}&redirect_uri=http%3A%2F%2Flocalhost%3A18080%2Fcallback&grant_type=authorization_code&client_id={}&client_secret={}", 
        code, 
        this->clientID,
        base64::from_base64(this->spotifyToken)
    ));

    req.header("content-type", "application/x-www-form-urlencoded");
    this->m_webListener2.setFilter(req.post("https://accounts.spotify.com/api/token"));
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