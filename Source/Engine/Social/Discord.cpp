#include "Discord.h"
#include "Core/Print.h"

#include <Engine/Engine.h>
#include <Engine/EngineVars.h>

#include <discord_game_sdk.h>

namespace bl {

static IDiscordCore* core = nullptr;

static std::string_view ToString(EDiscordResult result)
{
    switch(result) {
    case DiscordResult_Ok: return "Ok";
    case DiscordResult_ServiceUnavailable: return "Service Unavailable";
    case DiscordResult_InvalidVersion: return "Invalid Version";
    case DiscordResult_LockFailed: return "Lock Failed";
    case DiscordResult_InternalError: return "Internal Error";
    case DiscordResult_InvalidPayload: return "Invalid Payload";
    case DiscordResult_InvalidCommand: return "Invalid Command";
    case DiscordResult_InvalidPermissions: return "Invalid Permissions";
    case DiscordResult_NotFetched: return "Not Fetched";
    case DiscordResult_NotFound: return "Not Found";
    case DiscordResult_Conflict: return "Conflict";
    case DiscordResult_InvalidSecret: return "Invalid Secret";
    case DiscordResult_InvalidJoinSecret: return "Invalid Join Secret";
    case DiscordResult_NoEligibleActivity: return "No Eligible Activity";
    case DiscordResult_InvalidInvite: return "Invalid Invite";
    case DiscordResult_NotAuthenticated: return "Not Authenticated";
    case DiscordResult_InvalidAccessToken: return "Invalid Access Token";
    case DiscordResult_ApplicationMismatch: return "Application Mismatch";
    case DiscordResult_InvalidDataUrl: return "Invalid DataUrl";
    case DiscordResult_InvalidBase64: return "Invalid Base64";
    case DiscordResult_NotFiltered: return "Not Filtered";
    case DiscordResult_LobbyFull: return "Lobby Full";
    case DiscordResult_InvalidLobbySecret: return "Invalid Lobby Secret";
    case DiscordResult_InvalidFilename: return "Invalid Filename";
    case DiscordResult_InvalidFileSize: return "Invalid FileSize";
    case DiscordResult_InvalidEntitlement: return "Invalid Entitlement";
    case DiscordResult_NotInstalled: return "Not Installed";
    case DiscordResult_NotRunning: return "Not Running";
    case DiscordResult_InsufficientBuffer: return "Insufficient Buffer";
    case DiscordResult_PurchaseCanceled: return "Purchase Canceled";
    case DiscordResult_InvalidGuild: return "Invalid Guild";
    case DiscordResult_InvalidEvent: return "Invalid Event";
    case DiscordResult_InvalidChannel: return "Invalid Channel";
    case DiscordResult_InvalidOrigin: return "Invalid Origin";
    case DiscordResult_RateLimited: return "Rate Limited";
    case DiscordResult_OAuth2Error: return "OAuth2 Error";
    case DiscordResult_SelectChannelTimeout: return "Select Channel Timeout";
    case DiscordResult_GetGuildTimeout: return "Get Guild Timeout";
    case DiscordResult_SelectVoiceForceRequired: return "Select Voice Force Required";
    case DiscordResult_CaptureShortcutAlreadyListening: return "Capture Shortcut Already Listening";
    case DiscordResult_UnauthorizedForAchievement: return "Unauthorized For Achievement";
    case DiscordResult_InvalidGiftCode: return "Invalid Gift Code";
    case DiscordResult_PurchaseError: return "Purchase Error";
    case DiscordResult_TransactionAborted: return "Transaction Aborted";
    case DiscordResult_DrawingInitFailed: return "Drawing Init Failed";
    default: return "Unknown Result";
    }
}

static void LogCallback(void* hook_data, enum EDiscordLogLevel level, const char* message)
{
    if (level == DiscordLogLevel_Debug) return;
    printf("Discord: %s\n", message);
}

DiscordSystem::DiscordSystem(Engine& engine)
    : System(engine)
{

    // Use engine level variables to get the discord client ID.
    auto&   vars            = engine.GetVars();
    auto    clientID        = vars.Get<uint64_t>("discord.clientID");
    auto    requireDiscord  = vars.Get<bool>("discord.requireDiscord");

    //if (clientID == 0) {
    //    
    //    return;
    //}

    clientID = 763767974469042178;

    DiscordCreateParams params = {};
    DiscordCreateParamsSetDefault(&params);
    params.client_id = clientID;
    params.flags = requireDiscord ? DiscordCreateFlags_Default : DiscordCreateFlags_NoRequireDiscord;

    EDiscordResult result = DiscordCreate(DISCORD_VERSION, &params, &core);

    if (result != DiscordResult_Ok) {
        Print::Error("Could not initialize discord core, error ({}).", ToString(result));
        //engine.LogError("Failed to instantiate discord core! (err {} ({}))", ToString(result), static_cast<int>(result));
    }

    if (core)
        core->set_log_hook(core, DiscordLogLevel_Debug, nullptr, LogCallback);
}

DiscordSystem::~DiscordSystem()
{
    if (core)
        core->destroy(core);
}

static EDiscordActivityType ToDiscord(DiscordActivityType type)
{
    switch (type) {
        case DiscordActivityType::ePlaying: return DiscordActivityType_Playing;
        case DiscordActivityType::eStreaming: return DiscordActivityType_Streaming;
        case DiscordActivityType::eListening: return DiscordActivityType_Listening;
        case DiscordActivityType::eWatching: return DiscordActivityType_Watching;
        default: return DiscordActivityType_Playing;
    }
}

void DiscordSystem::UpdateActivity(DiscordActivity& activity)
{
    if (core == nullptr)
        return;

    ::DiscordActivity raw;
    std::memset(&raw, 0, sizeof(raw));

    raw.type = ToDiscord(activity.type);
    raw.application_id = activity.applicationID;
    std::snprintf(raw.name, sizeof(raw.name), "%s", activity.name.data());
    std::snprintf(raw.state, sizeof(raw.state), "%s", activity.state.data());
    std::snprintf(raw.details, sizeof(raw.details), "%s", activity.details.data());

    raw.timestamps.start = activity.startTime;
    raw.timestamps.end = activity.endTime;

    std::snprintf(raw.assets.large_image, sizeof(raw.assets.large_image), "%s", activity.art.largeImage.data());
    std::snprintf(raw.assets.large_text, sizeof(raw.assets.large_text), "%s", activity.art.largeImageTooltip.data());
    std::snprintf(raw.assets.small_image, sizeof(raw.assets.small_image), "%s", activity.art.smallImage.data());
    std::snprintf(raw.assets.small_text, sizeof(raw.assets.small_text), "%s", activity.art.smallImageTooltip.data());

    std::snprintf(raw.party.id, sizeof(raw.party.id), "%s", activity.party.id.data());
    raw.party.size.current_size = activity.party.sizes.currentSize;
    raw.party.size.max_size = activity.party.sizes.maxSize;
    raw.party.privacy = activity.party.isPrivate ? DiscordActivityPartyPrivacy_Private : DiscordActivityPartyPrivacy_Public;

    std::snprintf(raw.secrets.match, sizeof(raw.secrets.match), "%s", activity.secrets.matchCode.data());
    std::snprintf(raw.secrets.join, sizeof(raw.secrets.join), "%s", activity.secrets.joinCode.data());
    std::snprintf(raw.secrets.spectate, sizeof(raw.secrets.spectate), "%s", activity.secrets.spectateCode.data());

    // Log updating discord activity.
    core->get_activity_manager(core)->update_activity(core->get_activity_manager(core), &raw, nullptr, nullptr);
}

void DiscordSystem::RunCallbacks()
{
    if (core)
        core->run_callbacks(core);
}

}