
#ifndef __BME688_BSEC2_H__
#define __BME688_BSEC2_H__

#include <stdint.h>
#include <string>

#include "bme68x_defs.h"
#include "bsec_datatypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BSEC_CHECK_INPUT(x, shift)		(x & (1 << (shift-1)))
#define ARRAY_LEN(array)				(sizeof(array)/sizeof(array[0]))

typedef struct{
    float iaq;
    float iaq_accuracy;
    float co2_eq;
    float co2_eq_accuracy;
    float breath_voc;
    float breath_voc_accuracy;
    bool stabilization;
    bool runin_status;
}iaq_output_t;

void bsec2_start();

void bsec2_get_conf(bsec_bme_settings_t &conf);

bool processData(const struct bme68x_data &data, iaq_output_t &iaq_output,int64_t time_stamp);

#ifdef __cplusplus
}
#endif

#endif /*__BME688_BSEC2_H__*/
