#pragma  once

#include <iostream>
#include <string>
#include <memory>
#include "nlohmann/json.hpp"
#include <fstream>
#include <mutex>


class Config {
public:

    explicit Config(const std::string& configpath){
        try {
            std::ifstream i(configpath);
            i >> json_;
        }
        catch (std::exception &e) {
            std::cerr << "Failed when load config file: " << configpath << " reason: " << e.what() << std::endl;
        }

    }

    template<typename  T>
    T LookupConfig(const std::string& configName, const T& defaultValue) {
        std::lock_guard<std::mutex> guard(config_lock_);
        if (json_.contains(configName)) {
            return json_[configName].get<T>();
        }
        return defaultValue;
    }


    std::string GetAuthToken() {
        std::lock_guard<std::mutex> guard(config_lock_);
        return auth_token_;
    }

    void SetAuthToken(const std::string& token ) {
        std::lock_guard<std::mutex> guard(config_lock_);
        auth_token_ = token;
    }

private:
    std::mutex config_lock_;
    std::string auth_token_;

    nlohmann::json json_;
};