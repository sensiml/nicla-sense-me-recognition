#ifndef __MODEL_JSON_H__
#define __MODEL_JSON_H__

const char recognition_model_string_json[] = {"{\"NumModels\":1,\"ModelIndexes\":{\"0\":\"DEFAULT_MODEL_RANK_4\"},\"ModelDescriptions\":[{\"Name\":\"DEFAULT_MODEL_RANK_4\",\"ClassMaps\":{\"1\":\"Blade_Interference\",\"2\":\"Fan_Off\",\"3\":\"Fan_On\",\"4\":\"Flow_Blocked\",\"5\":\"GuardTamper\",\"6\":\"Mount_Fault\",\"7\":\"Shock_Fault\",\"0\":\"Unknown\"},\"ModelType\":\"PME\",\"FeatureFunctions\":[\"InterquartileRange\",\"InterquartileRange\",\"Variance\",\"Variance\",\"Variance\",\"Variance\",\"Variance\",\"Kurtosis\",\"Kurtosis\",\"Kurtosis\",\"Kurtosis\",\"Kurtosis\",\"Minimum\",\"Minimum\",\"Minimum\",\"Minimum\",\"AbsoluteMean\",\"AbsoluteMean\",\"AbsoluteMean\",\"PositiveZeroCrossings\",\"NegativeZeroCrossings\",\"NegativeZeroCrossings\",\"75thPercentile\",\"75thPercentile\",\"75thPercentile\",\"Mean\",\"Median\",\"Median\",\"StandardDeviation\",\"StandardDeviation\",\"StandardDeviation\",\"100thPercentile\",\"100thPercentile\",\"100thPercentile\",\"25thPercentile\",\"25thPercentile\",\"25thPercentile\"]}]}"};

int32_t recognition_model_string_json_len = sizeof(recognition_model_string_json);

#endif /* __MODEL_JSON_H__ */
