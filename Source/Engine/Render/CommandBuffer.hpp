#pragma once

enum class blCommand
{
    eDrawIndexed,

};

struct blSubmit
{
    blSubmit(blCommand)

    blCommand command;
    std::shared_ptr<blBuffer> vertexBuffer;
    std::shared_ptr<blBuffer> indexBuffer;
    
};

class blCommandBuffer
{
public:
    blCommandBuffer(std::shared_ptr<blRenderDevice> renderDevice);
    blCommandBuffer() = default;

    void begin();
    void end();
}