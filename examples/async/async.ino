/*
    Simple async demonstration sending a message with ID 0x123 every 5 seconds and receiving messages printing their ID.
*/

#include <Arduino.h>

#include <TWAI.h>

#define CAN_RX_PIN      GPIO_NUM_3
#define CAN_TX_PIN      GPIO_NUM_4

void setup() {
    // Setup serial monitor
    Serial.begin(115200);

    // Start TWAI
    // 125kBits baudrate
    // Sended messages don't need to be acknowledged
    ESP32TWAI::initialize(CAN_RX_PIN, CAN_TX_PIN, 125000, TWAI_MODE_NO_ACK);

    Serial.println("[TWAI] Installing driver");
    esp_err_t install_err = twai->install();
    if (install_err == ESP_OK) {
        Serial.println("[TWAI] OK. Starting driver");
        esp_err_t start_err = twai->start(true);
        if (start_err == ESP_OK) {
            Serial.println("[TWAI] Running");
        } else {
            Serial.printf("[TWAI] Starting driver failed with error 0x%04X. Restarting in 5 seconds\n", start_err);
            delay(5000);
            esp_restart();
        }
    } else {
        Serial.printf("[TWAI] Installing driver failed with error 0x%04X. Restarting in 5 seconds\n", install_err);
        delay(5000);
        esp_restart();
    }

    twai->onBusOff([]() {
        Serial.println("[TWAI] Bus is off");
    });

    twai->onBusRecovered([](){
        Serial.println("[TWAI] Bus sucessfully recovered");
    });

    twai->onRXQueueFull([](){
        Serial.println("[TWAI] RX queue is full. Flushing it");
    });

    twai->onMessage([](twai_message_t message) {
        Serial.printf("[TWAI] Message received with ID 0x%04X\n", message.identifier);
    });
}

uint32_t last_sent = 0;

void loop {
    if (millis() - last_sent > 5000) {
        last_sent = millis();

        twai->txMessage.identifier = 0x123;
        twai->txMessage.data_length_code = 4;
        for (int i = 0; i < 4; i++) {  
            twai->txMessage.data[i] = i;
        }

        Serial.printf("[TWAI] [%d] Sent message with result 0x%04X\n", millis(), twai->sendMessage());
    }
}