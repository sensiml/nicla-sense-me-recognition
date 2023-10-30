/*
 * This sketch shows how nicla can be used in standalone mode.
 * Without the need for an host, nicla can run sketches that
 * are able to configure the bhi sensors and are able to read all
 * the bhi sensors data.
 */

#include "Arduino.h"
#include "sensor_stream.h"

#include "Arduino_BHY2.h"
#include "Nicla_System.h"  //must be placed after "ArduinoJSon.h"
#include <kb.h>
#include "kb_debug.h"

#if USE_BLE

#include "ArduinoBLE.h"

const char* nameOfPeripheral            = "Nicla Sense ME DCL Endpoint";
const char* uuidOfService               = "16480000-0525-4ad5-b4fb-6dd83f49546b";
const char* uuidOfConfigChar            = "16480001-0525-4ad5-b4fb-6dd83f49546b";
const char* uuidOfDataChar              = "16480002-0525-4ad5-b4fb-6dd83f49546b";
const char* nameOfPeripheral            = "Nicla Sense ME Recognition";
const char* RecognitionServiceUuid      = "42421100-5A22-46DD-90F7-7AF26F723159";
const char* RecognitionClassOnlyUuid    = "42421101-5A22-46DD-90F7-7AF26F723159";
const char* RecognitionClassFeatureUuid = "42421102-5A22-46DD-90F7-7AF26F723159";
const int   WRITE_BUFFER_SIZE           = 128;
bool        WRITE_BUFFER_FIXED_LENGTH   = false;

BLEService        recognitionService = BLEService(RecognitionServiceUuid);
BLECharacteristic classOnlyChar
    = BLECharacteristic(RecognitionClassOnlyUuid, BLERead | BLENotify, 4, true);
BLECharacteristic classFeaturesChar = BLECharacteristic(
    RecognitionClassFeatureUuid, BLERead | BLENotify, WRITE_BUFFER_SIZE, WRITE_BUFFER_FIXED_LENGTH);
BLEDevice central;

const bool WRITE_BUFFER_FIXED_LENGTH = false;

// BLE Service
static BLEService sensorColService(uuidOfService);

// RX / TX Characteristics
BLECharacteristic configChar(uuidOfConfigChar,
                             BLERead,
                             WRITE_BUFFER_SIZE,
                             WRITE_BUFFER_FIXED_LENGTH);
BLEDescriptor     configNameDescriptor("2901", "Sensor Configuration");
BLECharacteristic sensorDataChar(uuidOfDataChar,
                                 BLERead | BLENotify,
                                 20,
                                 WRITE_BUFFER_FIXED_LENGTH);
BLEDescriptor     sensorDataDescriptor("2901", "Sensor Data TX");
#endif  // USE_BLE


void sml_recognition_run(signed short *data, int num_sensors);

static int8_t json_cfg_buffer[WRITE_BUFFER_SIZE];

static unsigned long previousMs;
static uint16_t      interval        = 1000 / ODR_IMU;
static bool          config_received = false;
static bool          ble_connected;
static uint8_t       column_index = 0;

static unsigned long ledSetTimeMs;
uint8_t              ledColor = red;
bool                 led_is_on;

DynamicJsonDocument config_message(256);
#if USE_SECOND_SERIAL_PORT_FOR_OUTPUT
auto& dataOutSerial = Serial1;
#else
auto& dataOutSerial = Serial;
#endif  // USE_SECOND_SERIAL_PORT_FOR_OUTPUT

static void sendJsonConfig()
{
#if USE_BLE
    serializeJson(config_message, json_cfg_buffer, WRITE_BUFFER_SIZE);
    configChar.writeValue(json_cfg_buffer, WRITE_BUFFER_SIZE);
#else
    serializeJson(config_message, (void*) json_cfg_buffer, WRITE_BUFFER_SIZE);
    dataOutSerial.println((char*) json_cfg_buffer);
    dataOutSerial.flush();
#endif  // USE_BLE
}

#if SML_PROFILER
#define Serial1_OUT_CHARS_MAX 2048
float        recent_fv_times[MAX_VECTOR_SIZE];
unsigned int recent_fv_cycles[MAX_VECTOR_SIZE];
#else
#define Serial1_OUT_CHARS_MAX 512
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

static char     Serial1_out_buf[Serial1_OUT_CHARS_MAX];
static uint8_t  recent_fv[MAX_VECTOR_SIZE];
static uint16_t recent_fv_len;

static char* p_Serial1_out = Serial1_out_buf;

void sml_output_results(uint16_t model, uint16_t classification)
{
#if SML_PROFILER
    kb_print_model_cycles(model, Serial1_out_buf, recent_fv_cycles);
#else
    kb_print_model_result(model, classification, Serial1_out_buf, 1, recent_fv);
#endif

    Serial.print(Serial1_out_buf);
    Serial.println("");
    Serial.flush();
#if USE_BLE
    if (central.connected())
        Send_Notification(model, classification, features, fv_length);

#endif  // USE_BLE
}

void setLedColor(uint32_t color, unsigned long time)
{
    nicla::leds.setColor((RGBColors) color);
    ledSetTimeMs = time;

    led_is_on = (color != off);
}

void ledOff(unsigned long time) { setLedColor(off, time); }

void connectedLight()
{
#if USE_BLE
    ledColor = green;
#else
    ledColor = yellow;
#endif

    unsigned long time = millis();
    setLedColor(ledColor, time);
    delay(1000);
    ledOff(time + 1000);
}

void disconnectedLight()
{
    unsigned long time = millis();
    setLedColor(blue, time);
    ledColor = blue;
}

uint32_t get_free_memory_size()
{
    const uint32_t START = 16 * 1024;
    uint8_t*       mem;
    uint32_t       size  = START;
    uint32_t       floor = 0;

    while (size > 0)
    {
        mem = (uint8_t*) malloc(size);
        if (mem != NULL)
        {
            free(mem);
            floor = size;
            size  = (size << 1);
            break;
        }
        else
        {
            size = size >> 1;
        }
    }

    while (size > floor)
    {
        mem = (uint8_t*) malloc(size);
        if (mem != NULL)
        {
            free(mem);
            break;
        }
        else
        {
            size--;
        }
    }

    Serial.print("free mem size: ");
    Serial.println(String(size));
    return size;
}

#if USE_BLE
/*
 * LEDS
 */

void onBLEConnected(BLEDevice central)
{
    Serial.print("Connected event, central: ");
    Serial.println(central.address());
    connectedLight();
    ble_connected = true;
}

void onBLEDisconnected(BLEDevice central)
{
    Serial.print("Disconnected event, central: ");
    Serial.println(central.address());
    disconnectedLight();
    BLE.setConnectable(true);
    ble_connected = false;
}


void setup_ble()
{
    if (!BLE.begin())
    {
        Serial.println("starting BLE failed!");
        while (1)
            ;
    }
    // Serial.println("BLE Started!");
    BLE.setLocalName(nameOfPeripheral);
    BLE.noDebug();


    recognitionService.addCharacteristic(classOnlyChar);
    recognitionService.addCharacteristic(classFeaturesChar);
    delay(1000);
    BLE.addService(recognitionService);
    BLE.setAdvertisedService(recognitionService);
    // Bluetooth LE connection handlers.
    BLE.setEventHandler(BLEConnected, onBLEConnected);
    BLE.setEventHandler(BLEDisconnected, onBLEDisconnected);

    Serial.println("BLE Init done!");
    PrintInfo();
}

void Send_Notification(uint16_t model_no,
                       uint16_t classification,
                       uint8_t* features,
                       uint16_t num_features)
{
    kp_output_t    base_output;
    kp_output_fv_t output_with_features;
    base_output.model          = model_no;
    base_output.classification = classification;

    output_with_features.model_out = base_output;
    if (features != NULL && classFeaturesChar.subscribed() && num_features > 0)
    {
        output_with_features.fv_len = num_features;
        memcpy(output_with_features.features, features, num_features);
        classFeaturesChar.writeValue((void*) &output_with_features, sizeof(kp_output_fv_t));
        Serial.println("Sending With Classification with Features");
    }
    if (classOnlyChar.subscribed())
    {
        Serial.println("Sending Classification");
        classOnlyChar.writeValue((void*) &base_output, sizeof(kp_output_t));
    }
}
#endif  //#if USE_BLE


void setup()
{
#if USE_BLE
    Serial.begin(SERIAL_BAUD_RATE_DEFAULT);
#else
    Serial.begin(SERIAL_BAUD_RATE_DEFAULT);
#endif
    while (!Serial)
        ;
    Serial.print("Starting up");    
    kb_model_init();
    NiclaSettings niclaSettings(NICLA_I2C, 0, NICLA_VIA_ESLOV, 0);
    BHY2.begin(niclaSettings);

#if USE_BLE
    setup_ble();
#endif  // USE_BLE

#if ENABLE_ACCEL || ENABLE_GYRO || ENABLE_MAG || ENABLE_GAS || ENABLE_TEMP
    column_index = setup_sensors(config_message, column_index);
#endif

    get_free_memory_size();
}


static uint8_t packetNum      = 0;
static uint8_t sensorRawIndex = 0;

void updateLED(unsigned long currentMs)
{
    if (ble_connected)
    {
        return;  // to minimize delay
    }

    if (led_is_on)
    {
        if ((int32_t) currentMs - (int32_t) ledSetTimeMs >= CFG_LED_ON_DURATION)
        {
            ledOff(currentMs);
        }
    }
    else
    {
        if ((int32_t) currentMs - (int32_t) ledSetTimeMs >= CFG_LED_OFF_DURATION)
        {
            setLedColor(ledColor, currentMs);
        }
    }
}

void loop()
{
    unsigned long currentMs;
    bool          connected_to_host = false;
    int ret;
    BHY2.update();

    currentMs = millis();
#if USE_BLE
    BLEDevice central = BLE.central();
    if (central)
    {
        if (central.connected())
        {
            // connectedLight();
        }
    }
    else
    {
        // disconnectedLight();
    }

    if (ble_connected)
    {
        connected_to_host = true;
    }

    #else
    connected_to_host = true;
    #endif
    
    if (connected_to_host)
    {
        // CHECKME
        if ((int32_t) currentMs - (int32_t) previousMs >= interval)
        {
            // save the last time you blinked the LED
            previousMs = currentMs;
#if ENABLE_ACCEL || ENABLE_GYRO || ENABLE_MAG
            sensorRawIndex = update_sensor_data_col(sensorRawIndex);
            packetNum++;

            if (packetNum == MAX_SAMPLES_PER_PACKET)
            {
                int16_t* pData = get_sensor_data_buffer();
                for(int i= 0; i< MAX_SAMPLES_PER_PACKET; i++)
                {
                    sml_recognition_run(pData + (i*MAX_NUMBER_OF_COLUMNS),1);
                }
                memset(pData, 0, MAX_NUMBER_OF_COLUMNS * MAX_SAMPLES_PER_PACKET * sizeof(int16_t));
                packetNum = 0;
                sensorRawIndex = 0;
            }

#endif  //#if ENABLE_ACCEL || ENABLE_GYRO || ENABLE_MAG
        }
    }

    updateLED(currentMs);
}
