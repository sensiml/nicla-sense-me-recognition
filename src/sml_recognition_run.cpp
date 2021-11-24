#include "sensor_stream.h"
#include "kb.h"


void sml_recognition_run(signed short *data, int num_sensors)
{

    int ret;
#if ENABLE_ACCEL || ENABLE_GYRO || ENABLE_MAG
    		ret = kb_run_model((SENSOR_DATA_T *)data, num_sensors, 0);
		if (ret >= 0){
			sml_output_results(0, ret);
			kb_reset_model(0);
		};
#endif //ENABLE_ACCEL || ENABLE_GYRO || ENABLE_MAG


#if ENABLE_AUDIO
    //FILL_RUN_MODEL_AUDIO
#endif //ENABLE_AUDIO

}
