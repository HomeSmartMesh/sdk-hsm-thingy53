
#ifndef __JSON_ENDPOINT_H__
#define __JSON_ENDPOINT_H__

#include <stdint.h>

#ifdef __cplusplus

#include <json.hpp>
using json = nlohmann::json;

extern "C" {
#endif

namespace hsm
{

}

typedef void (*json_endpoint_handler_t)(std::string &client, std::string &topic, json &request, json &response);

void set_endpoint_handler(json_endpoint_handler_t handler);

#ifdef __cplusplus
}
#endif

#endif /*__JSON_ENDPOINT_H__*/
