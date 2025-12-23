// --- file: main.cpp ---
#include <Arduino.h>
#include <RadioLib.h>

#define NSS PA4
#define DIO0 PB1
#define RST PA8
#define DIO1 PB10
#define LED_BUILTIN PC13
#define LORA_ON PB15
#define DONE_PIN PB13 // TPL5110 DONE pin

SX1276 radio = new Module(NSS, DIO0, RST, DIO1);

void blink(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }
}

// crucial: TPL5110 shut-off command
void signalDone() {
  digitalWrite(DONE_PIN, HIGH); // tell TPL5110 "I'm done"
  delay(100);                   // allow latch
  // board will power off immediately
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LORA_ON, OUTPUT);
  pinMode(DONE_PIN, OUTPUT);
  digitalWrite(DONE_PIN, LOW); // must remain low until ready

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(LORA_ON, LOW); // Power the SX1276
  delay(3000);                // let power stabilize

  Serial.print("[SX1278] Initializing ... ");
  int state =
      radio.begin(915.0, 125.0, 9, 7, RADIOLIB_SX127X_SYNC_WORD, 17, 8, 0);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("success!");
  } else {
    Serial.print("failed, code ");
    Serial.println(state);
    blink(10);
    // if init fails, still tell TPL5110 to shut off
    signalDone();
    while (true)
      ;
  }
}

void loop() {
  Serial.print("[SX1278] Transmitting packet ... ");
  int state = radio.transmit("Hello Efraim!");

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(" success!");
    blink(2);
    Serial.print("[SX1278] Datarate:\t");
    Serial.print(radio.getDataRate());
    Serial.println(" bps");
  } else {
    Serial.print("failed, code ");
    Serial.println(state);
  }

  // *** IMPORTANT ***
  // After finishing ALL work, shut down the board
  signalDone();

  // MCU never reaches this line (power is cut)
  // delay(5000);
}