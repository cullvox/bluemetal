#pragma once

#include "Render/Buffer.hpp"

class blMesh
{

    blRenderDevice* _pRenderDevice;
    blBuffer* _pVertexBuffer;
    blBuffer* _pIndexBuffer;

public:

    blMesh(blRenderDevice* pRenderDevice);

};