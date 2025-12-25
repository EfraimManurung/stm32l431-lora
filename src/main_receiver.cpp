// --- file: main_receiver.cpp ---
#include <Arduino.h>
#include <RadioLib.h>

// === Pin definitions (adjust ONLY if wiring differs) ===
#define NSS PA4
#define DIO0 PB1
#define RST PA8
#define DIO1 PB10
#define LED_BUILTIN PC13
#define LORA_ON PB15

// SX1276 instance
SX1276 radio = new Module(NSS, DIO0, RST, DIO1);

// ------------------------------------------------------

void blink(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LORA_ON, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LORA_ON, LOW); // power SX1276
  delay(3000);                // power stabilization

  Serial.print("[SX1276] Initializing ... ");

  int state = radio.begin(915.0,                     // frequency (MHz)
                          125.0,                     // bandwidth (kHz)
                          9,                         // spreading factor
                          7,                         // coding rate 4/7
                          RADIOLIB_SX127X_SYNC_WORD, // sync word
                          17,                        // preamble length
                          8,                         // CRC
                          0                          // gain (auto)
  );

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("success!");
    blink(3);
  } else {
    Serial.print("failed, code ");
    Serial.println(state);
    blink(10);
    while (true)
      ;
  }
}

void loop() {
  Serial.println("[SX1276] Waiting for incoming transmission ...");

  String str;
  int state = radio.receive(str); // blocking receive

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("✔ Packet received!");
    blink(2);

    Serial.print("[SX1276] Data: ");
    Serial.println(str);

    Serial.print("[SX1276] RSSI: ");
    Serial.print(radio.getRSSI());
    Serial.println(" dBm");

    Serial.print("[SX1276] SNR: ");
    Serial.print(radio.getSNR());
    Serial.println(" dB");

    Serial.print("[SX1276] Frequency error: ");
    Serial.print(radio.getFrequencyError());
    Serial.println(" Hz");

  } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println("⏱ RX timeout");
  } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
    Serial.println("⚠ CRC mismatch");
  } else {
    Serial.print("❌ RX failed, code ");
    Serial.println(state);
  }

  delay(500);
}