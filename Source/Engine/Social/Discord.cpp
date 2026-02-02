#include "Discord.h"

#include <Engine/Engine.h>
#include <Engine/EngineVars.h>
#include <discord.h>

namespace bl {

static discord::Core* core{};

static std::string_view ToString(discord::Result result)
{
    switch(result) {
    case discord::Result::Ok: return "Ok";
    case discord::Result::ServiceUnavailable: return "Service Unavailable";
    case discord::Result::InvalidVersion: return "Invalid Version";
    case discord::Result::LockFailed: return "Lock Failed";
    case discord::Result::InternalError: return "Internal Error";
    case discord::Result::InvalidPayload: return "Invalid Payload";
    case discord::Result::InvalidCommand: return "Invalid Command";
    case discord::Result::InvalidPermissions: return "Invalid Permissions";
    case discord::Result::NotFetched: return "Not Fetched";
    case discord::Result::NotFound: return "Not Found";
    case discord::Result::Conflict: return "Conflict";
    case discord::Result::InvalidSecret: return "Invalid Secret";
    case discord::Result::InvalidJoinSecret: return "Invalid Join Secret";
    case discord::Result::NoEligibleActivity: return "No Eligible Activity";
    case discord::Result::InvalidInvite: return "Invalid Invite";
    case discord::Result::NotAuthenticated: return "Not Authenticated";
    case discord::Result::InvalidAccessToken: return "Invalid Access Token";
    case discord::Result::ApplicationMismatch: return "Application Mismatch";
    case discord::Result::InvalidDataUrl: return "Invalid DataUrl";
    case discord::Result::InvalidBase64: return "Invalid Base64";
    case discord::Result::NotFiltered: return "Not Filtered";
    case discord::Result::LobbyFull: return "Lobby Full";
    case discord::Result::InvalidLobbySecret: return "Invalid Lobby Secret";
    case discord::Result::InvalidFilename: return "Invalid Filename";
    case discord::Result::InvalidFileSize: return "Invalid FileSize";
    case discord::Result::InvalidEntitlement: return "Invalid Entitlement";
    case discord::Result::NotInstalled: return "Not Installed";
    case discord::Result::NotRunning: return "Not Running";
    case discord::Result::InsufficientBuffer: return "Insufficient Buffer";
    case discord::Result::PurchaseCanceled: return "Purchase Canceled";
    case discord::Result::InvalidGuild: return "Invalid Guild";
    case discord::Result::InvalidEvent: return "Invalid Event";
    case discord::Result::InvalidChannel: return "Invalid Channel";
    case discord::Result::InvalidOrigin: return "Invalid Origin";
    case discord::Result::RateLimited: return "Rate Limited";
    case discord::Result::OAuth2Error: return "OAuth2 Error";
    case discord::Result::SelectChannelTimeout: return "Select Channel Timeout";
    case discord::Result::GetGuildTimeout: return "Get Guild Timeout";
    case discord::Result::SelectVoiceForceRequired: return "Select Voice Force Required";
    case discord::Result::CaptureShortcutAlreadyListening: return "Capture Shortcut Already Listening";
    case discord::Result::UnauthorizedForAchievement: return "Unauthorized For Achievement";
    case discord::Result::InvalidGiftCode: return "Invalid Gift Code";
    case discord::Result::PurchaseError: return "Purchase Error";
    case discord::Result::TransactionAborted: return "Transaction Aborted";
    case discord::Result::DrawingInitFailed: return "Drawing Init Failed";
    default: return "Unknown Result";
    }
}

DiscordSystem::DiscordSystem(Engine& engine)
    : System(engine)
{

    // Use engine level variables to get the discord client ID.
    auto&   vars            = engine.GetVars();
    auto    clientID        = vars.Get<uint64_t>("discord.clientID");
    auto    requireDiscord  = vars.Get<bool>("discord.requireDiscord");

    discord::Result result = discord::Core::Create(clientID, requireDiscord ? DiscordCreateFlags_Default : DiscordCreateFlags_NoRequireDiscord, &core);

    if (!core) {
        //engine.LogError("Failed to instantiate discord core! (err {} ({}))", ToString(result), static_cast<int>(result));
    }

    core->SetLogHook(discord::LogLevel::Info, [](discord::LogLevel, const char* message){
        //bl::Print::Info("Discord {}", message);
    });
}

DiscordSystem::~DiscordSystem()
{
    delete core;
}

discord::ActivityType ToDiscord(DiscordActivityType type)
{
    switch (type) {
        case DiscordActivityType::ePlaying: return discord::ActivityType::Playing;
        case DiscordActivityType::eStreaming: return discord::ActivityType::Streaming;
        case DiscordActivityType::eListening: return discord::ActivityType::Listening;
        case DiscordActivityType::eWatching: return discord::ActivityType::Watching;
        default: return discord::ActivityType::Playing;
    }
}

void DiscordSystem::UpdateActivity(DiscordActivity& activity)
{
    discord::Activity da;
    da.SetApplicationId(activity.applicationID);
    da.SetType(ToDiscord(activity.type));
    da.SetName(activity.name.data());
    da.SetState(activity.state.data());
    da.SetDetails(activity.details.data());
    da.GetTimestamps().SetStart(activity.startTime);
    da.GetTimestamps().SetEnd(activity.endTime);
    da.GetAssets().SetSmallImage(activity.art.smallImage.data());
    da.GetAssets().SetSmallText(activity.art.smallImageTooltip.data());
    da.GetAssets().SetLargeImage(activity.art.largeImage.data());
    da.GetAssets().SetLargeText(activity.art.largeImageTooltip.data());
    da.GetParty().SetId(activity.party.id.data());
    da.GetParty().GetSize().SetCurrentSize(activity.party.sizes.currentSize);
    da.GetParty().GetSize().SetMaxSize(activity.party.sizes.maxSize);
    da.GetParty().SetPrivacy(activity.party.isPrivate ? discord::ActivityPartyPrivacy::Private : discord::ActivityPartyPrivacy::Public);
    da.GetSecrets().SetJoin(activity.secrets.joinCode.data());
    da.GetSecrets().SetMatch(activity.secrets.matchCode.data());
    da.GetSecrets().SetSpectate(activity.secrets.spectateCode.data());

    // Log updating discord activity.
    core->ActivityManager().UpdateActivity(da, {});
}

void DiscordSystem::RunCallbacks()
{
    core->RunCallbacks();
}

}