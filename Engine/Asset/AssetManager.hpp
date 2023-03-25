#pragma once

#include "Asset/Asset.hpp"

class AssetManager {

public:
    AssetManager();
    ~AssetManager();

    void add();

private:
    
    struct AssetImpl
    {
        std::filesystem::path path;
        std::vector<uint8_t> data;
    };

    std::unordered_map<std::string, std::shared_ptr<Asset>> m_assets;
};