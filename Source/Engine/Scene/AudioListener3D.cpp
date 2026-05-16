#include "AudioListener3D.h"
#include "Audio/AudioSystem.h"
#include "Engine/Engine.h"
#include "Core/ClassDB.h"
#include "Audio/FMOD.h"

namespace bl {

AudioListener3D::AudioListener3D(Engine& engine)
    : Node3D(engine)
{
}

AudioListener3D::~AudioListener3D()
{
}

void AudioListener3D::Update(float dt)
{
    Node3D::Update(dt);

    // Update the listener's 3D attributes.
    glm::vec3 pos = GetWorldPosition();
    glm::vec3 forward = glm::normalize(GetWorldRotation() * glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 up = glm::normalize(GetWorldRotation() * glm::vec3(0.0f, 1.0f, 0.0f));

    FMOD_VECTOR fmodPosition { pos.x, pos.y, pos.z };
    FMOD_VECTOR fmodForward { forward.x, forward.y, forward.z };
    FMOD_VECTOR fmodUp { up.x, up.y, up.z };

    auto audioSystem = GetEngine().GetAudio();
    FMOD_CHECK(audioSystem->GetFMOD()->set3DListenerAttributes(0, &fmodPosition, nullptr, &fmodForward, &fmodUp))
}

void AudioListener3D::RegisterClass(ClassDB& db)
{
    db.RegisterClass("AudioListener3D", "Node3D", &AudioListener3D::Create);
}

} // namespace bl