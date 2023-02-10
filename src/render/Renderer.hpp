#pragma once

#include "config/Config.hpp"
#include "RenderDevice.hpp"

class CRenderer : public IConfigurable
{
public:
    CRenderer(CConfig* pConfig = g_pConfig.get(), CRenderDevice* pRenderDevice = g_pRenderDevice.get());
    ~CRenderer();

    

private:
    VkRenderPass m_pass;
};