#pragma once

#include "Asset/Asset.hpp"

class AssetManager {
public:
    AssetManager();
    ~AssetManager();

    void add(std::shared_ptr<Asset> resource);
private:
    std::unordered_map<std::string, std::shared_ptr<Asset>> m_assets;
};