#pragma once

#include "Asset/Asset.hpp"

class blAssetManager 
{
public:
    blAssetManager();
    ~blAssetManager();

    void add();

private:
    std::unordered_map<std::string, std::shared_ptr<Asset>> m_assets;
};