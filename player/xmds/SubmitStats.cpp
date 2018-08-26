#include "SubmitStats.hpp"
#include "xmds.hpp"

template<>
std::string soap::request_string(const SubmitStats::request& request)
{
    return create_request<SubmitStats::request>(request.server_key, request.hardware_key, request.stat_xml);
}

template<>
SubmitStats::response soap::create_response(const std::string& soap_response)
{
    SubmitStats::response result;
    result.success = xmds::parse_success_response(soap_response);
    return result;
}