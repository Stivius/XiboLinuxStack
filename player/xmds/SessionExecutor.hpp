#pragma once

#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/asio/connect.hpp>

#include "utils/Utilities.hpp"
#include "SOAP.hpp"
#include "Session.hpp"

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;
namespace ip = boost::asio::ip;

using OnSessionFinished = std::function<void()>;

template<typename Response, typename Request>
class SessionExecutor
{
public:
    SessionExecutor(const std::string& host, int port, std::shared_ptr<Session<Response, Request>> session) :
        m_host(host), m_port(port), m_session(session)
    {
    }

    void exec(OnSessionFinished callback)
    {
        m_callback = callback;

        auto resolve = std::bind(&SessionExecutor::onResolve, this, std::placeholders::_1, std::placeholders::_2);
        m_session->resolver.async_resolve(m_host, std::to_string(m_port), ip::resolver_base::numeric_service, resolve);
    }

private:
    void onResolve(const boost::system::error_code& ec, ip::tcp::resolver::results_type results)
    {
        if(!ec)
        {
            auto connect = std::bind(&SessionExecutor::onConnect, this, std::placeholders::_1, std::placeholders::_2);
            boost::asio::async_connect(m_session->socket, results.begin(), results.end(), connect);
        }
        else
        {
            Utils::logger()->error("SOAP Resolved host with error: {}", ec.message());
        }
    }

    void onConnect(const boost::system::error_code& ec, ip::tcp::resolver::iterator)
    {
        if(!ec)
        {
            SOAP::RequestSerializer<Request> serializer{m_session->soapRequest};

            m_session->httpRequest.method(http::verb::post);
            m_session->httpRequest.target("/xmds.php?v=5");
            m_session->httpRequest.version(11);
            m_session->httpRequest.set(http::field::host, m_host);
            m_session->httpRequest.body() = serializer.string();
            m_session->httpRequest.prepare_payload();

            Utils::logger()->trace("SOAP Request string: {}", serializer.string());

            auto write = std::bind(&SessionExecutor::onWriteSoap, this, std::placeholders::_1, std::placeholders::_2);
            boost::beast::http::async_write(m_session->socket, m_session->httpRequest, write);
        }
        else
        {
            Utils::logger()->error("SOAP Connected to host with error: {}", ec.message());
        }
    }

    void onWriteSoap(const boost::system::error_code& ec, std::size_t)
    {
        if(!ec)
        {
            auto read = std::bind(&SessionExecutor::onReadSoap, this, std::placeholders::_1, std::placeholders::_2);
            boost::beast::http::async_read(m_session->socket, m_session->buffer, m_session->httpResponse, read);
        }
        else
        {
            Utils::logger()->error("Send SOAP request with error: {}", ec.message());
        }
    }

    void onReadSoap(const boost::system::error_code& ec, std::size_t)
    {
        if(!ec)
        {
            if(m_session->httpResponse.result() == http::status::ok)
            {
                SOAP::ResponseParser<Response> parser(m_session->httpResponse.body());
                m_session->responseCallback(parser.get());

                m_callback();
            }
            else
            {
                Utils::logger()->error("Receive SOAP request with HTTP error: {}", m_session->httpResponse.result_int());
            }
        }
        else
        {
            Utils::logger()->error("Receive SOAP request with error: {}", ec.message());
        }
    }

private:
    std::string m_host;
    int m_port;
    std::shared_ptr<Session<Response, Request>> m_session;
    OnSessionFinished m_callback;

};