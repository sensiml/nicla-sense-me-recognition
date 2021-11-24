#ifndef __MODEL_JSON_H__
#define __MODEL_JSON_H__

const char recognition_model_string_json[] = {"{\"NumModels\":1,\"ModelIndexes\":{\"0\":\"tflite_rank_0\"},\"ModelDescriptions\":[{\"Name\":\"tflite_rank_0\",\"ClassMaps\":{\"1\":\"figeight\",\"2\":\"idle\",\"3\":\"unknown\",\"4\":\"updown\",\"5\":\"wave\",\"6\":\"wheelcw\",\"0\":\"Unknown\"},\"ModelType\":\"TensorFlowLiteforMicrocontrollers\",\"FeatureFunctions\":[\"Variance\",\"Variance\",\"Variance\",\"Variance\",\"Variance\",\"Variance\",\"NegativeZeroCrossings\",\"NegativeZeroCrossings\",\"NegativeZeroCrossings\",\"NegativeZeroCrossings\",\"NegativeZeroCrossings\",\"NegativeZeroCrossings\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"LinearRegressionStats\",\"PositiveZeroCrossings\",\"Mean\",\"Mean\",\"Mean\",\"Mean\",\"Mean\",\"Mean\",\"Median\",\"Median\",\"Median\",\"Median\",\"Median\",\"Median\",\"Maximum\",\"Maximum\",\"Maximum\",\"Maximum\",\"Maximum\",\"Minimum\",\"Minimum\",\"Minimum\",\"Minimum\",\"Minimum\",\"AbsoluteSum\",\"AbsoluteSum\"]}]}"};

int recognition_model_string_json_len = sizeof(recognition_model_string_json);

#endif /* __MODEL_JSON_H__ */
