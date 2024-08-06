#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DataManager {
public:
    DataManager() {}

    void saveBool(std::string name, bool value) {
        Mod::get()->setSavedValue<bool>(name, value);
    }

    bool getBool(std::string name) {
        return Mod::get()->getSavedValue<bool>(name);
    }
};