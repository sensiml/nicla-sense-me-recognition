# nicla-sense-me-recognition
Example application code for running a [SensiML Knowledge Pack](https://sensiml.com/products/knowledge-pack/) recognition app for the Bosch Nicla Sense ME board.  

## Choosing the correct SensiML hardware platform

In order to run a Knowledge Pack, you must first have a model generated via [SensiML Analytics Studio](https://sensiml.com/products/analytics-studio/).

When downloading a model, you will need to select `Bosch Nicla Sense ME` from the target device.


## Running your downloaded model

Unzip the download to a location of your choosing. In the folder, you will find the following directories:

* knowledgepack/
  * knowledgepack_project/
  * libsensiml/

Copy the files from `libsensiml` to lib/sensiml. It is **ok to overwrite these, but do not commit them in a pull request**. The files provided in this repository are for examples only.

Copy the files from `knowledgepack_project` to src/
This should be `sml_recognition_run.cpp`, which will contain the necessary model running code to enable recognition on the device. 

Build/upload the firmware.

### Adjusting IMU Frequency

If you wish to run the IMU at a different frequency (default for data capture and this example is 200 Hz), modify the `ODR_IMU` in `include/sensor_stream.h`
As the [Data Capture Lab](https://sensiml.com/products/data-capture-lab/) of the SensiML Toolkit does not actively configure the device, a capture configuration isn't generated when creating the model code.

## Viewing Model Output

For Serial output, simply connect to your Bosch Nicla Sense ME via any serial monitoring application. You should see an output similar to this:

``` json
{"ModelNumber":0,"Classification":3,"SegmentStart":268400,"SegmentLength":200,"FeatureVectorLength":6, "FeatureVector":[87,62,50,0,124,179]}
```

