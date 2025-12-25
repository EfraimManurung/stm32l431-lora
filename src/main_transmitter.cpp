// --- file: main.cpp ---
#include <Arduino.h>
#include <RadioLib.h>
#include <SPI.h>

/* ===================== Pins ===================== */
#define NSS PA4
#define DIO0 PB1
#define RST PA8
#define DIO1 PB10
#define LED_BUILTIN PC13
#define LORA_ON PB15

/* SPI pins (STM32 core default) */
#define SPI_SCK PA5
#define SPI_MISO PA6
#define SPI_MOSI PA7

/* ===================== Limits ===================== */
#define MAX_RADIO_RETRIES 5

/* ===================== Radio ===================== */
SX1276 *radio;

/* ===================== Utils ===================== */
void blink(uint8_t n) {
  for (uint8_t i = 0; i < n; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }
}

/* ===================== SPI Release ===================== */
void releaseSPI() {
  SPI.end();
  pinMode(SPI_SCK, INPUT);
  pinMode(SPI_MISO, INPUT);
  pinMode(SPI_MOSI, INPUT);
}

/* ===================== Radio Power Reset ===================== */
void resetRadioPower() {
  releaseSPI();

  digitalWrite(LORA_ON, HIGH); // power off
  delay(200);

  digitalWrite(RST, LOW);
  delay(10);
  digitalWrite(RST, HIGH);

  digitalWrite(LORA_ON, LOW); // power on
  delay(400);
}

/* ===================== Radio Init ===================== */
bool initRadio() {
  delete radio;
  radio = new SX1276(new Module(NSS, DIO0, RST, DIO1));

  int state =
      radio->begin(915.0, 125.0, 9, 7, RADIOLIB_SX127X_SYNC_WORD, 17, 8, 0);

  return state == RADIOLIB_ERR_NONE;
}

/* ===================== Setup ===================== */
void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LORA_ON, OUTPUT);
  pinMode(RST, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LORA_ON, LOW);
  delay(3000);

  radio = nullptr;

  Serial.print("[SX1276] Initializing ... ");
  if (!initRadio()) {
    Serial.println("FAILED");
    blink(10);
    while (true)
      ;
  }
  Serial.println("success!");
}

/* ===================== Loop ===================== */
void loop() {
  for (uint8_t attempt = 0; attempt < MAX_RADIO_RETRIES; attempt++) {
    Serial.print("[SX1276] Transmitting packet ... ");

    int state = radio->transmit("Hello Efraim!");

    if (state == RADIOLIB_ERR_NONE) {
      Serial.println("success!");
      Serial.print("[SX1276] Datarate:\t");
      Serial.print(radio->getDataRate());
      Serial.println(" bps");
      blink(2);
      delay(10000);
      return;
    }

    Serial.print("failed, code ");
    Serial.println(state);

    if (state == RADIOLIB_ERR_SPI_WRITE_FAILED) {
      Serial.println("SPI fault → full bus + radio reset");
      resetRadioPower();

      Serial.print("[SX1276] Reinitializing ... ");
      if (initRadio()) {
        Serial.println("success");
        continue;
      }
      Serial.println("FAILED");
    }

    break;
  }

  Serial.println("Unrecoverable radio error");
  blink(5);
  delay(10000);
}