#include "Discord.h"
#include "Core/Print.h"

#include <Engine/Engine.h>
#include <Engine/EngineVars.h>

#define DISCORDPP_IMPLEMENTATION
#include <discordpp.h>

namespace bl {

static auto client = std::make_shared<discordpp::Client>();

DiscordSystem::DiscordSystem()
    : System()
{

    // Use engine level variables to get the discord client ID.
    auto   vars            = GetEngine()->GetVars();
    auto    clientID        = vars->Get<uint64_t>("discord.clientID");
    auto    requireDiscord  = vars->Get<bool>("discord.requireDiscord");

    //if (clientID == 0) {
    //    
    //    return;
    //}

    clientID = 763767974469042178;
  
    client->AddLogCallback([](auto message, auto severity){
        switch (severity) {
        case discordpp::LoggingSeverity::Verbose:
            Print::Verbose("{}\n", message);
            break;
        default:
        case discordpp::LoggingSeverity::None:
        case discordpp::LoggingSeverity::Info:
            Print::Info("{}\n", message);
            break;
        case discordpp::LoggingSeverity::Warning:
            Print::Warn("{}\n", message);
            break;
        case discordpp::LoggingSeverity::Error:
            Print::Error("{}\n", message);
            break;
        }
    }, discordpp::LoggingSeverity::Info);

    client->SetStatusChangedCallback([](auto status, auto error, auto details){
        Print::Info("Discord client status changed to {}\n", discordpp::Client::StatusToString(status));

        if (status == discordpp::Client::Status::Ready) {
            Print::Info("Discord client ready.");
        } else if (error != discordpp::Client::Error::None) {
            Print::Error("Discord client error connecting: {} {}\n", discordpp::Client::ErrorToString(error), details);
        }
    });

    // auto codeVerifier = client->CreateAuthorizationCodeVerifier();
    // discordpp::AuthorizationArgs args{};
    // args.SetClientId(clientID);
    // args.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
    // args.SetCodeChallenge(codeVerifier.Challenge());

    // client->Authorize(args, [clientID, codeVerifier](auto result, auto code, auto redirectUri){
    //     if (!result.Successful()) {
    //         Print::Error("Discord client authentication error: {}\n", result.ToString());
    //     } else {
    //         Print::Info("Discord client authentication code recieved, exchanging for access token.\n");
    //         client->GetToken(clientID, code, codeVerifier.Verifier(), redirectUri, 
    //         [](auto result, auto accessToken, auto refreshToken, auto, auto, auto){
    //             Print::Info("Discord client authentication access token recieved, connecting to discord.\n");
    //             client->UpdateToken(discordpp::AuthorizationTokenType::Bearer, accessToken, [](auto result){
    //                 client->Connect();
    //             });
    //         });
    //     }
    // });

    client->SetApplicationId(clientID);
}

DiscordSystem::~DiscordSystem()
{
}

DiscordSystem* DiscordSystem::Get()
{
    static DiscordSystem system;
    return &system;
}

static inline discordpp::ActivityTypes DiscordActivityType_ToDiscord(DiscordActivityType type)
{
    switch (type) {
    case DiscordActivityType::ePlaying: return discordpp::ActivityTypes::Playing;
    case DiscordActivityType::eStreaming: return discordpp::ActivityTypes::Streaming;
    case DiscordActivityType::eListening: return discordpp::ActivityTypes::Listening;
    case DiscordActivityType::eWatching: return discordpp::ActivityTypes::Watching;
    case DiscordActivityType::eCustom: return discordpp::ActivityTypes::CustomStatus;
    case DiscordActivityType::eCompeting: return discordpp::ActivityTypes::Competing;
    case DiscordActivityType::eHanging: return discordpp::ActivityTypes::HangStatus;
    default: return discordpp::ActivityTypes::Playing;
    }
}

static inline discordpp::ActivityPartyPrivacy DiscordActivityPartyPrivacyType_ToDiscord(bool isPrivate)
{
    return isPrivate ? discordpp::ActivityPartyPrivacy::Private : discordpp::ActivityPartyPrivacy::Public;
}

void DiscordSystem::UpdateActivity(DiscordActivity& activity)
{
    discordpp::Activity newActivity;

    discordpp::ActivityTimestamps timestamps;
    timestamps.SetStart(activity.startTime);
    timestamps.SetEnd(activity.endTime);

    discordpp::ActivityAssets assets;
    assets.SetSmallImage(std::string{activity.art.smallImage});
    assets.SetSmallText(std::string{activity.art.smallImageTooltip});
    assets.SetLargeImage(std::string{activity.art.largeImage});
    assets.SetLargeText(std::string{activity.art.largeImage});

    discordpp::ActivitySecrets secrets;
    secrets.SetJoin(std::string{activity.secrets.joinCode});

    discordpp::ActivityParty party;
    party.SetId(std::string{activity.party.id});
    party.SetCurrentSize(activity.party.sizes.currentSize);
    party.SetMaxSize(activity.party.sizes.maxSize);
    party.SetPrivacy(DiscordActivityPartyPrivacyType_ToDiscord(activity.party.isPrivate));

    newActivity.SetApplicationId(activity.applicationID);
    newActivity.SetName(std::string{activity.name});
    newActivity.SetDetails(std::string{activity.details});
    newActivity.SetState(std::string{activity.state});
    newActivity.SetType(DiscordActivityType_ToDiscord(activity.type));
    newActivity.SetTimestamps(timestamps);
    newActivity.SetAssets(assets);
    newActivity.SetSecrets(secrets);
    newActivity.SetParty(party);

    client->UpdateRichPresence(newActivity, [](auto result){
        if (!result.Successful()) {
            Print::Error("Discord client rich presense error: {}\n", result.ToString());
        } else {
            Print::Info("Discord rich presense updated.\n");
        }
    });
}

void DiscordSystem::RunCallbacks()
{
    discordpp::RunCallbacks();
}

}