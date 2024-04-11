#include "RenderDoc.h"

#include "Precompiled.h"
#include "Core/Platform.h"

#ifdef BLUEMETAL_SYSTEM_WINDOWS
#include "Windows.h"
#else

#endif

namespace bl
{

RenderDoc::RenderDoc()
    : _isCapturing(false)
{
    
#ifdef BLUEMETAL_SYSTEM_WINDOWS
    // At init, on windows
    if(HMODULE mod = GetModuleHandleA("renderdoc.dll"))
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
        if (ret != 1) throw std::runtime_error("Could not initialize RenderDoc!");
    }
#else
    #error "DEFINE ME!"
#endif

}

RenderDoc::~RenderDoc()
{
}

void RenderDoc::beginCapture()
{

    // To start a frame capture, call StartFrameCapture.
    // You can specify NULL, NULL for the device to capture on if you have only one device and
    // either no windows at all or only one window, and it will capture from that device.
    // See the documentation below for a longer explanation
    if(rdoc_api) {
        rdoc_api->StartFrameCapture(NULL, NULL);
        _isCapturing = true;
    } 

    // Rendering happens!
}

void RenderDoc::endCapture()
{
    // stop the capture
    if(rdoc_api) { 
        rdoc_api->EndFrameCapture(NULL, NULL);
    }
    _isCapturing = false;
}

}