#pragma once

#include <Engine/System.h>

namespace bl {

enum class DiscordActivityType
{
    ePlaying,
    eStreaming,
    eListening,
    eWatching,
    eCustom,
    eCompeting,
    eHanging,
};

// Representation of Discord statuses and their UI.
// ┌───────────────────────────────────────┐
// │                                       │
// │  ┌────────┐  APPLICATION NAME         │
// │  │        │                           │
// │  │        │  DETAILS ABOUT GAME       │
// │  │    ┌────┐                          │
// │  │    │    │ 1:40                     │
// │  └────│    │                          │
// │   ▲   └────┘ STATUS OF GAME           │
// │   │    ▲                              │
// │   │    │                              │
// └───┼────┼──────────────────────────────┘
//     │    │
//     └────┼──Large Image
//          │
//          └──Small Image


struct DiscordActivity
{
    DiscordActivity() = default;
    DiscordActivity& operator=(DiscordActivity& rhs) = default;
    DiscordActivity& operator=(DiscordActivity&& rhs) = default;

    std::uint64_t applicationID;
    std::string_view name = "";
    std::string_view details = "";
    std::string_view state = "";
    DiscordActivityType type = DiscordActivityType::ePlaying;
    uint64_t startTime = std::time(nullptr);
    uint64_t endTime = 0;

    struct Art
    {
        std::string_view smallImage = "";
        std::string_view smallImageTooltip = "";
        std::string_view largeImage = "";
        std::string_view largeImageTooltip = "";
    } art;

    struct Secrets
    {
        std::string_view matchCode = "example";
        std::string_view joinCode = "example";
        std::string_view spectateCode = "example";
    } secrets;

    struct Party
    {
        std::string_view id = "example";
        struct PartySize
        {
            int32_t currentSize = 0;
            int32_t maxSize = 0;
        } sizes;
        bool isPrivate = true;
    } party;
};

class DiscordSystem : public System {
    DiscordSystem();
    ~DiscordSystem();
public:
    static DiscordSystem* Get();

    /// @brief Returns true if the Discord Game SDK was successfully initialized.
    /// If the Discord Game SDK was not successfully initialzed or disabled 
    /// all other methods will be noop for the session.
    bool IsInitialized();

    /// @brief Updates the users in Discord activity.
    /// @param[in] activity The current status to set the players Discord to.
    void UpdateActivity(DiscordActivity& activity);

    /// @brief Runs discord callbacks, call this either every frame or on a timer.
    void RunCallbacks();
};

}