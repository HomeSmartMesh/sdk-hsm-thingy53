#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "bsec2.h"
#include "bsec_interface.h"
#include "bsec_datatypes.h"
#include "FieldAir_HandSanitizer/FieldAir_HandSanitizer.h"

LOG_MODULE_REGISTER(bme688_bsec2, LOG_LEVEL_INF);

bsec_bme_settings_t bmeConf;
bsec_output_t outputs[BSEC_NUMBER_OUTPUTS];
uint8_t nOutputs;
//static uint8_t workBuffer[BSEC_MAX_WORKBUFFER_SIZE];

float extTempOffset = 0.0;
uint32_t ovfCounter = 0;
uint32_t lastMillis = 0;

// [{bsec_virtual_sensor_t, DEFINED float}]
bsec_sensor_configuration_t virtualSensors[] = {
    {BSEC_SAMPLE_RATE_LP,BSEC_OUTPUT_IAQ},
    {BSEC_SAMPLE_RATE_LP,BSEC_OUTPUT_CO2_EQUIVALENT},
    {BSEC_SAMPLE_RATE_LP,BSEC_OUTPUT_BREATH_VOC_EQUIVALENT},
    //BSEC_OUTPUT_RAW_TEMPERATURE,
    //BSEC_OUTPUT_RAW_PRESSURE,
    //BSEC_OUTPUT_RAW_HUMIDITY,
    //BSEC_OUTPUT_RAW_GAS,
    {BSEC_SAMPLE_RATE_LP,BSEC_OUTPUT_STABILIZATION_STATUS},
    {BSEC_SAMPLE_RATE_LP,BSEC_OUTPUT_RUN_IN_STATUS}
};

void print_bme_conf(bsec_bme_settings_t &bmeConf){
    LOG_INF("   * next_call: %" PRId64 " process_data: %u",bmeConf.next_call,bmeConf.process_data);
    LOG_INF("   * op_mode : %u, trigger:%u, run_gas: %u",bmeConf.op_mode,bmeConf.trigger_measurement,bmeConf.run_gas);

    LOG_DBG("   * heater_profile_len : %u",bmeConf.heater_profile_len);
    LOG_DBG("   * heater_temperature_profile : ");
    for(int i=0;i<bmeConf.heater_profile_len;i++){
        LOG_DBG("%u ",bmeConf.heater_temperature_profile[i]);
    }
    LOG_DBG("\n");
    LOG_DBG("   * heater_duration_profile : ");
    for(int i=0;i<bmeConf.heater_profile_len;i++){
        LOG_DBG("%u ",bmeConf.heater_duration_profile[i]);
    }
    LOG_DBG("\n");
}

void bsec2_start(){
	LOG_INF("bsec2_start()");

    bsec_version_t ver;
    bsec_get_version(&ver);
    LOG_INF("bsec_get_version() => %u.%u-%u.%u",ver.major,ver.minor,ver.major_bugfix,ver.minor_bugfix);

    bsec_library_return_t res = bsec_init();
	LOG_INF("bsec_init() %d => %s",res,(res == BSEC_OK)?"BSEC_OK":"FAIL");

    //res = bsec_set_configuration(FieldAir_HandSanitizer_config, BSEC_MAX_PROPERTY_BLOB_SIZE, workBuffer, BSEC_MAX_WORKBUFFER_SIZE);
	//LOG_INF("bsec_set_configuration() %d => %s",res,(res == BSEC_OK)?"BSEC_OK":"FAIL");

    bsec_sensor_configuration_t sensorSettings[BSEC_MAX_PHYSICAL_SENSOR];
    uint8_t nSensorSettings = BSEC_MAX_PHYSICAL_SENSOR;
    res = bsec_update_subscription(virtualSensors, ARRAY_LEN(virtualSensors),sensorSettings,&nSensorSettings);
	LOG_INF("bsec_update_subscription() %d => %s",res,(res == BSEC_OK)?"BSEC_OK":"FAIL");
	LOG_INF("nSensorSettings = %u",nSensorSettings);

    memset(&bmeConf, 0, sizeof(bmeConf));

}

void bsec2_get_conf(bsec_bme_settings_t &conf){
    const int64_t currTimeNs = k_ticks_to_ns_near64(k_uptime_ticks());
    bsec_library_return_t res = bsec_sensor_control(currTimeNs, &bmeConf);
	LOG_INF("bsec2_get_conf() bsec_sensor_control() %d => %s",res,(res == BSEC_OK)?"BSEC_OK":"FAIL");
    print_bme_conf(bmeConf);
    conf = bmeConf;
}

void handle_outputs(bsec_output_t *outputs,uint8_t nOutputs, iaq_output_t &iaq_output){
    for (uint8_t i = 0; i < nOutputs; i++)
    {
        const bsec_output_t &output = outputs[i];
        switch (output.sensor_id)
        {
            case BSEC_OUTPUT_IAQ:
                iaq_output.iaq = output.signal;
                iaq_output.iaq_accuracy = output.accuracy;
                LOG_INF("iaq = %.2f ; iaq accuracy = %u",output.signal,output.accuracy);
                break;
            case BSEC_OUTPUT_CO2_EQUIVALENT:
                iaq_output.co2_eq = output.signal;
                iaq_output.co2_eq_accuracy = output.accuracy;
                LOG_INF("co2 = %.2f ; co2 accuracy = %u",output.signal,output.accuracy);
                break;
            case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
                iaq_output.breath_voc = output.signal;
                iaq_output.breath_voc_accuracy = output.accuracy;
                LOG_INF("breath voc = %.2f ; breath voc accuracy = %u",output.signal,output.accuracy);
                break;
            case BSEC_OUTPUT_RAW_TEMPERATURE:
                LOG_INF("temperature = %.2f",output.signal);
                break;
            case BSEC_OUTPUT_RAW_PRESSURE:
                LOG_INF("pressure = %.2f",output.signal);
                break;
            case BSEC_OUTPUT_RAW_HUMIDITY:
                LOG_INF("humidity = %.2f",output.signal);
                break;
            case BSEC_OUTPUT_RAW_GAS:
                LOG_INF("gas resistance = %.0f",output.signal);
                break;
            case BSEC_OUTPUT_STABILIZATION_STATUS:
                iaq_output.stabilization = output.signal;
                LOG_INF("stabilization status = %f",output.signal);
                break;
            case BSEC_OUTPUT_RUN_IN_STATUS:
                iaq_output.runin_status = output.signal;
                LOG_INF("run in status = %f",output.signal);
                break;
            default:
                break;
        }
    }

}

//called when BME68X_NEW_DATA_MSK && BME68X_GASM_VALID_MSK
bool processData(const struct bme68x_data &data, iaq_output_t &iaq_output,int64_t time_stamp){

    if (bmeConf.trigger_measurement && bmeConf.op_mode != BME68X_SLEEP_MODE){
        LOG_DBG("processData() with trigger_measurement and not in sleep mode");
        //bsec_input_t inputs[nInputs]; /* HeatSrc, Temp, Hum, Press, GasRes, GasIndex */
        bsec_input_t inputs[BSEC_MAX_PHYSICAL_SENSOR]; /* Temp, Pres, Hum & Gas */
        uint8_t nInputs = 0;
        /* Checks all the required sensor inputs, required for the BSEC library for the requested outputs */
        if (BSEC_CHECK_INPUT(bmeConf.process_data, BSEC_INPUT_TEMPERATURE))
        {
            inputs[nInputs].sensor_id = BSEC_INPUT_HEATSOURCE;
            inputs[nInputs].signal = extTempOffset;
            inputs[nInputs].time_stamp = time_stamp;
            nInputs++;
            inputs[nInputs].signal = data.temperature;
            inputs[nInputs].sensor_id = BSEC_INPUT_TEMPERATURE;
            inputs[nInputs].time_stamp = time_stamp;
            nInputs++;
        }
        if (BSEC_CHECK_INPUT(bmeConf.process_data, BSEC_INPUT_HUMIDITY))
        {
            inputs[nInputs].signal = data.humidity;
            inputs[nInputs].sensor_id = BSEC_INPUT_HUMIDITY;
            inputs[nInputs].time_stamp = time_stamp;
            nInputs++;
        }
        if (BSEC_CHECK_INPUT(bmeConf.process_data, BSEC_INPUT_PRESSURE))
        {
            inputs[nInputs].sensor_id = BSEC_INPUT_PRESSURE;
            inputs[nInputs].signal = data.pressure;
            inputs[nInputs].time_stamp = time_stamp;
            nInputs++;
        }
        if (BSEC_CHECK_INPUT(bmeConf.process_data, BSEC_INPUT_GASRESISTOR) &&
                (data.status & BME68X_GASM_VALID_MSK))
        {
            inputs[nInputs].sensor_id = BSEC_INPUT_GASRESISTOR;
            inputs[nInputs].signal = data.gas_resistance;
            inputs[nInputs].time_stamp = time_stamp;
            nInputs++;
        }
        if (BSEC_CHECK_INPUT(bmeConf.process_data, BSEC_INPUT_PROFILE_PART) &&
                (data.status & BME68X_GASM_VALID_MSK))
        {
            inputs[nInputs].sensor_id = BSEC_INPUT_PROFILE_PART;
            inputs[nInputs].signal = data.gas_index;
            inputs[nInputs].time_stamp = time_stamp;
            nInputs++;
        }

        if(nInputs > 0){
            memset(outputs, 0, sizeof(outputs));
            nOutputs = BSEC_NUMBER_OUTPUTS;
            bsec_library_return_t res = bsec_do_steps(inputs, nInputs, outputs, &nOutputs);
            LOG_DBG("bsec_do_steps() nOutputs=%u  res=%d => %s",nOutputs,res,(res == BSEC_OK)?"BSEC_OK":"FAIL");
            if (res != BSEC_OK)
                return false;

            handle_outputs(outputs,nOutputs,iaq_output);
        }
        //else{
        //    LOG_DBG("bsec_do_steps() nInputs=0");
        //}
    }


    return true;
}
