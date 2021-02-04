#pragma  once

#include <iostream>
#include <string>
#include <memory>
#include "nlohmann/json.hpp"
#include <fstream>
#include <mutex>


class CConfig {

private:
    std::mutex m_ConfigLock;

    nlohmann::json m_ConfigJson;


public:

    explicit CConfig(const std::string& configpath){
        try {
            std::ifstream i(configpath);
            i >> m_ConfigJson;
        }
        catch (std::exception &e) {
            std::cerr << "Failed when load config file: " << configpath << " reason: " << e.what() << std::endl;
        }

    }

    template<typename  T>
    T LookupConfig(const std::string& configName, const T& defaultValue) {
        std::lock_guard<std::mutex> guard(m_ConfigLock);
        if (m_ConfigJson.contains(configName)) {
            return m_ConfigJson[configName].get<T>();
        }
        return defaultValue;
    }

};