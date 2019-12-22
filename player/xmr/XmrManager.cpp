#include "XmrManager.hpp"

#include "MainLoop.hpp"

#include "common/Parsing.hpp"
#include "common/crypto/RsaManager.hpp"
#include "common/dt/DateTime.hpp"
#include "common/logger/Logging.hpp"
#include "config/AppConfig.hpp"

const size_t CHANNEL_PART = 0;
const size_t KEY_PART = 1;
const size_t MESSAGE_PART = 2;

const char* const HearbeatChannel = "H";
const Zmq::Channels XmrChannels{AppConfig::mainXmrChannel(), HearbeatChannel};

// TODO: strong type
void XmrManager::connect(const std::string& host)
{
    if (info_.host == host) return;

    info_.host = host;
    subscriber_.messageReceived().connect(
        [this](const Zmq::MultiPartMessage& message) { processMultipartMessage(message); });
    subscriber_.run(host, XmrChannels);
}

void XmrManager::stop()
{
    subscriber_.stop();
}

CollectionIntervalAction& XmrManager::collectionInterval()
{
    return collectionIntervalAction_;
}

ScreenshotAction& XmrManager::screenshot()
{
    return screenshotAction_;
}

XmrStatus XmrManager::status()
{
    return info_;
}

void XmrManager::processMultipartMessage(const Zmq::MultiPartMessage& multipart)
{
    if (multipart[CHANNEL_PART] == AppConfig::mainXmrChannel())
    {
        try
        {
            auto decryptedMessage = decryptMessage(multipart[KEY_PART], multipart[MESSAGE_PART]);
            auto xmrMessage = parseMessage(decryptedMessage);

            processXmrMessage(xmrMessage);

            info_.lastMessageDt = DateTime::now();
        }
        catch (std::exception& e)
        {
            Log::error("[XMR] {}", e.what());
        }
    }
    else
    {
        info_.lastHeartbeatDt = DateTime::now();
    }
}

std::string XmrManager::decryptMessage(const std::string& encryptedBase64Key, const std::string& encryptedBase64Message)
{
    auto privateKey = RsaManager::instance().privateKey();

    auto encryptedKey = CryptoUtils::fromBase64(encryptedBase64Key);
    auto messageKey = CryptoUtils::decryptPrivateKeyPkcs(encryptedKey, privateKey);

    auto encryptedMessage = CryptoUtils::fromBase64(encryptedBase64Message);

    return CryptoUtils::decryptRc4(encryptedMessage, messageKey);
}

XmrMessage XmrManager::parseMessage(const std::string& jsonMessage)
{
    auto tree = Parsing::jsonFromString(jsonMessage);

    XmrMessage message;
    message.action = tree.get<std::string>("action");
    message.createdDt = DateTime::fromIsoExtendedString(tree.get<std::string>("createdDt"));
    message.ttl = tree.get<int>("ttl");

    return message;
}

void XmrManager::processXmrMessage(const XmrMessage& message)
{
    if (isMessageExpired(message)) return;

    if (message.action == "collectNow")
    {
        MainLoop::pushToUiThread([this]() { collectionIntervalAction_(); });
    }
    else if (message.action == "screenShot")
    {
        MainLoop::pushToUiThread([this]() { screenshotAction_(); });
    }
}

bool XmrManager::isMessageExpired(const XmrMessage& message)
{
    auto resultDt = message.createdDt + DateTime::Seconds(message.ttl);
    if (resultDt < DateTime::nowUtc())
    {
        return true;
    }
    return false;
}