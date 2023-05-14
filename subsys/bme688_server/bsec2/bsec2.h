
#ifndef __BME688_BSEC2_H__
#define __BME688_BSEC2_H__

#include <stdint.h>
#include <string>

#include "bme68x_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BSEC_CHECK_INPUT(x, shift)		    (x & (1 << (shift-1)))
#define ARRAY_LEN(array)				(sizeof(array)/sizeof(array[0]))

typedef struct{
    float iaq;
    float iaq_accuracy;
    bool stabilization;
    bool runin_status;
}iaq_output_t;

void bsec2_start();

bool processData(struct bme68x_data &data, iaq_output_t &iaq_output);

#ifdef __cplusplus
}
#endif

#endif /*__BME688_BSEC2_H__*/
