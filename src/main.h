#include <Arduino.h>

// include the library for RadioLib
#include <RadioLib.h>

#ifdef USE_LOW_POWER
#include "STM32LowPower.h"
#endif

/*
    STM32L431C8T6 - LoRa

    MISO    - PA6
    MOSI    - PA7
    SCK     - PA5
    NSS     - PA4
    RESET   - PA8
    DIO0    - PB1
    DIO1    - PB10
    DIO2    - PB11

*/

/* SX1278/RFM95 LoRa has the following connections: */
#define NSS PA4
#define RST PA8
#define DIO0 PB1
#define DIO1 PB10
#define DIO2 PB11

/* pins */
#define LED_PIN PC13 // Onboard LED (active LOW)
#define LORA_POWER_PIN PB15

SX1276 radio = new Module(NSS, DIO0, RST, DIO1);

#ifdef DEBUG_MAIN
// Redirect debug output to Serial2 (Tx on PA2)
HardwareSerial Serial2(USART2); // or HardwareSerial Serial2(PA3, PA2)
#define DEBUG_BEGIN(...) Serial2.begin(__VA_ARGS__)
#define DEBUG_PRINT(...) Serial2.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial2.println(__VA_ARGS__)
#else
#define DEBUG_BEGIN(...)
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

// Sleep this many microseconds. Notice that the sending and waiting for
// downlink will extend the time between send packets. You have to extract this
// time
#define SLEEP_INTERVAL 10000