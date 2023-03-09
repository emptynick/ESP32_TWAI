#ifndef ESP32_TWAI
#define ESP32_TWAI

#include "driver/gpio.h"
#include "driver/twai.h"
#include "hal/twai_hal.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define twai ESP32TWAI::instance()

typedef void (*twaiMessageCallback)(twai_message_t message);
typedef void (*twaiEventCallback)();

class ESP32TWAI
{
  public:
    twai_message_t rxMessage;
    twai_message_t txMessage;

    ESP32TWAI(int crxPin = 4, int ctxPin = 5, long baudrate = 125000, twai_mode_t mode = TWAI_MODE_NORMAL, bool autorecover = true, uint8_t rx_queue_length = 5, uint8_t tx_queue_length = 5);
    ~ESP32TWAI();

    static ESP32TWAI *instance();
    static void initialize(int crxPin = 4, int ctxPin = 5, long baudrate = 125000, twai_mode_t mode = TWAI_MODE_NORMAL, bool autorecover = true, uint8_t rx_queue_length = 5, uint8_t tx_queue_length = 5);

    void setMode(twai_mode_t mode);
    void setBaudrate(long baudrate);
    esp_err_t install();
    esp_err_t start(bool async = false);
    esp_err_t end();
    int availableMessages();
    esp_err_t receiveMessage(uint16_t wait = 5);
    esp_err_t sendMessage(uint16_t wait = 5);
    void readAlerts(uint16_t wait = 5);
    void poll();

    bool isRunning() {
        return _driver_installed && _driver_started;
    }

    void onBusRecovered(twaiEventCallback bus_recovered_callback) {
        _bus_recovered_cb = bus_recovered_callback;
    }

    void onRXQueueFull(twaiEventCallback rx_queue_full_callback) {
        _rx_queue_full_cb = rx_queue_full_callback;
    }

    void onBusOff(twaiEventCallback bus_off_callback) {
        _bus_off_cb = bus_off_callback;
    }

    void onMessage(twaiMessageCallback message_received_cb) {
        _message_received_cb = message_received_cb;
    }

private:
    twai_timing_config_t _t_config;
    twai_general_config_t _g_config;
    twai_filter_config_t _f_config;
    twai_status_info_t _status_info;
    bool _driver_installed = false;
    bool _driver_started = false;
    uint32_t _alerts;

    bool async = false;
    bool autorecover = true;

    TaskHandle_t _task_handle;

    // Callbacks
    twaiEventCallback _bus_recovered_cb;
    twaiEventCallback _rx_queue_full_cb;
    twaiEventCallback _bus_off_cb;

    twaiMessageCallback _message_received_cb;
};

#endif