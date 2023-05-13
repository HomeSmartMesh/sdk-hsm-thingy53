
#ifndef __BME688_SERVER_H__
#define __BME688_SERVER_H__

#include <stdint.h>
#include <string>

#ifdef __cplusplus

#include <json.hpp>
using json = nlohmann::json;

extern "C" {
#endif

namespace hsm
{

}

typedef void (*bme688_handler_t)(json &data);

void set_bme688_handler(bme688_handler_t handler);

void set_bme688_config(json &config);

#ifdef __cplusplus
}
#endif

#endif /*__BME688_SERVER_H__*/
