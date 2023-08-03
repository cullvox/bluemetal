#include "GraphicsSubsystem.h"


namespace bl
{

GraphicsSubsystem::GraphicsSubsystem(Engine& engine)
    : m_engine(engine)
{
}

GraphicsSubsystem::~GraphicsSubsystem()
{
}

void GraphicsSubsystem::init(const GraphicsSubsystemInitInfo* pInfo)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error(SDL_GetError());
    }

    m_instance = std::make_shared<Instance>();
}

} // namespace bl