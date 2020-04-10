#include "XmdsRequestSender.hpp"

#include "cms/xmds/Resources.hpp"
#include "cms/xmds/SoapRequestSender.hpp"

const std::string XmdsTarget = "/xmds.php?v=5";

XmdsRequestSender::XmdsRequestSender(const std::string& host,
                                     const std::string& serverKey,
                                     const std::string& hardwareKey) :
    uri_(Uri::fromString(host + XmdsTarget)),
    host_(host),
    serverKey_(serverKey),
    hardwareKey_(hardwareKey)
{
}

FutureResponseResult<GetResource::Result> XmdsRequestSender::getResource(int layoutId, int regionId, int mediaId)
{
    GetResource::Request request;
    request.serverKey = serverKey_;
    request.hardwareKey = hardwareKey_;
    request.layoutId = layoutId;
    request.regionId = std::to_string(regionId);
    request.mediaId = std::to_string(mediaId);

    return SoapRequestHelper::sendRequest<GetResource::Result>(uri_, request);
}

FutureResponseResult<GetFile::Result> XmdsRequestSender::getFile(int fileId,
                                                                 const std::string& fileType,
                                                                 std::size_t chunkOffset,
                                                                 std::size_t chunkSize)
{
    GetFile::Request request;
    request.serverKey = serverKey_;
    request.hardwareKey = hardwareKey_;
    request.fileId = std::to_string(fileId);
    request.fileType = fileType;
    request.chunkOffset = chunkOffset;
    request.chunkSize = chunkSize;

    return SoapRequestHelper::sendRequest<GetFile::Result>(uri_, request);
}

FutureResponseResult<SubmitScreenShot::Result> XmdsRequestSender::submitScreenShot(const std::string& screenShot)
{
    SubmitScreenShot::Request request;
    request.serverKey = serverKey_;
    request.hardwareKey = hardwareKey_;
    request.screenShot = screenShot;

    return SoapRequestHelper::sendRequest<SubmitScreenShot::Result>(uri_, request);
}

std::string XmdsRequestSender::getHost() const
{
    return host_;
}

std::string XmdsRequestSender::getServerKey() const
{
    return serverKey_;
}

std::string XmdsRequestSender::getHardwareKey() const
{
    return hardwareKey_;
}
