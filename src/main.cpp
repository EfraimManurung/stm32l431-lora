/**
 * Author: Efraim Manurung
 * Email: efraim.manurung@gmail.com
 *
 */

/* main header file for definitions and etc */
#include "main.h"

// save transmission state between loops
int transmission_state = RADIOLIB_ERR_NONE;

// flag to indicate that a packet was sent
volatile bool transmitted_flag = true;

void set_flag(void) {
  // we sent a packet, set the flag
  transmitted_flag = true;
}

void setup() {
  /* Setup serial debug */
#ifdef DEBUG_MAIN
  DEBUG_BEGIN(9600);
#endif

  pinMode(NSS, OUTPUT);
  digitalWrite(NSS, HIGH);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(LORA_POWER_PIN, OUTPUT);
  digitalWrite(LORA_POWER_PIN, HIGH);

  /* Time for serial settings */
  delay(1000);

  // initialize SX1278 with default settings
#ifdef DEBUG_MAIN
  DEBUG_PRINTLN("[RFM95/SX1276] Initializing ... ");
#endif
  // digitalWrite(NSS, LOW); // Enable RFM95
  int state =
      radio.begin(915.0, 125.0, 9, 7, RADIOLIB_SX127X_SYNC_WORD, 17, 8, 0);
  if (state == RADIOLIB_ERR_NONE) {
#ifdef DEBUG_MAIN
    DEBUG_PRINTLN("[RFM95/SX1276] Initialized");
#endif
  } else {

#ifdef DEBUG_MAIN
    DEBUG_PRINT("[RFM95/SX1276] failed, code ");
    DEBUG_PRINTLN(state);
#endif
    while (true) {
      delay(10);
    }
  }

  // set the function that will be called
  // when packet transmission is finished
  radio.setPacketSentAction(set_flag);

// Configure low power
#ifdef USE_LOW_POWER
  LowPower.begin();
#endif
}

void loop() {

  // check if the previous transmission finished
  if (transmitted_flag) {

    // reset flag
    transmitted_flag = false;

    if (transmission_state == RADIOLIB_ERR_NONE) {
// packet was successfully sent
#ifdef DEBUG_MAIN
      DEBUG_PRINTLN("PACKET SUCCESSFULLY TRANSMITTED!");
#endif

      // NOTE: when using interrupt-driven transmit method,
      //       it is not possible to automatically measure
      //       transmission data rate using getDataRate()

    } else {
#ifdef DEBUG_MAIN
      DEBUG_PRINT(F("failed, code "));
      DEBUG_PRINTLN(transmission_state);
#endif
    }

    // clean up after transmission is finished
    // this will ensure transmitter is disabled,
    // RF switch is powered down etc.
    radio.finishTransmit();

    // wait before transmitting again
#ifdef USE_LOW_POWER
    LowPower.deepSleep(SLEEP_INTERVAL);
#else
    delay(SLEEP_INTERVAL);
#endif

    // send another one
#ifdef DEBUG_MAIN
    DEBUG_PRINTLN(F("[SX1278] Sending another packet ... "));
#endif

    // from float to uint16_t
    uint16_t tempInt = 100 * 1;
    uint16_t humInt = 100 * 1;
    // pressure is already given in 100 x mBar = hPa
    uint16_t pressInt = 1 / 10;

    String clientId = "NS001";
    // String str = String(t) + "," + String(h) + "," + String(p) + "," +
    // String(a);
    String str = String("[{\"h\":") + humInt + ",\"t\":" + tempInt +
                 ",\"p\":" + pressInt + "},";
    str += String("{\"node\":\"") + clientId + "}]";

#ifdef DEBUG_MAIN
    DEBUG_PRINT("JSON PAYLOAD: ");
    DEBUG_PRINTLN(str);
#endif

    transmission_state = radio.startTransmit(str.c_str());
    radio.sleep();
  }
}