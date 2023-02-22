#pragma once

//===========================//
#include "config/Config.hpp"
#include "RenderDevice.hpp"
//===========================//

class CRenderer : public IConfigurable
{
public:
    CRenderer(CConfig* pConfig = g_pConfig.get(), CRenderDevice* pRenderDevice = g_pRenderDevice.get());
    ~CRenderer();

    void RenderFrame();
    void PresentFrame();    
private:
    CRenderDevice* m_pRenderDevice;
    CConfig* m_pConfig;
    VkRenderPass m_pass;
};