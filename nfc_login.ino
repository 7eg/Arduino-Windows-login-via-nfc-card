#include <SPI.h>
#include <MFRC522.h>
#include <Keyboard.h>

#define SS_PIN  10
#define RST_PIN  9

MFRC522 rfc(SS_PIN, RST_PIN);

// --- CONFIGURATION ---
struct Card {
  String uid;
  const char* password;
};

Card AUTHORIZED_CARDS[] = {
  { "B3C95194", "20729278" },
  // { "AABBCCDD", "another_password" },
};
#include <SPI.h>
#include <MFRC522.h>
#include <Keyboard.h>

#define SS_PIN  10
#define RST_PIN  9

MFRC522 rfc(SS_PIN, RST_PIN);

// --- CONFIGURATION ---
const String AUTHORIZED_UID = "B3C95194";
const char*  PASSWORD       = "20729278";
// ---------------------

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfc.PCD_Init();
  Keyboard.begin();

  delay(5000); // Safety window: unplug within 5s if something goes wrong
  Serial.println("System Ready. Scan card to login.");
}

void loop() {
  if (!rfc.PICC_IsNewCardPresent() || !rfc.PICC_ReadCardSerial()) return;

  // Build UID string
  String uid = "";
  for (byte i = 0; i < rfc.uid.size; i++) {
    if (rfc.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfc.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  if (uid == AUTHORIZED_UID) {
    // Send structured event for the dashboard FIRST, then act
    Serial.println("AUTH_EVENT:" + uid + ":GRANTED");

    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();
    delay(1000);

    Keyboard.print(PASSWORD);
    delay(100);

    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();
  } else {
    Serial.println("AUTH_EVENT:" + uid + ":DENIED");
  }

  delay(2000);
  rfc.PICC_HaltA();
}

const int CARD_COUNT = sizeof(AUTHORIZED_CARDS) / sizeof(AUTHORIZED_CARDS[0]);
// ---------------------

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfc.PCD_Init();
  Keyboard.begin();

  delay(5000);
  Serial.println("System Ready. Scan card to login.");
}

void loop() {
  if (!rfc.PICC_IsNewCardPresent() || !rfc.PICC_ReadCardSerial()) return;

  String uid = "";
  for (byte i = 0; i < rfc.uid.size; i++) {
    if (rfc.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfc.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  Card* matched = nullptr;
  for (int i = 0; i < CARD_COUNT; i++) {
    if (uid == AUTHORIZED_CARDS[i].uid) {
      matched = &AUTHORIZED_CARDS[i];
      break;
    }
  }

  if (matched != nullptr) {
    Serial.println("AUTH_EVENT:" + uid + ":GRANTED");

    // Shift wakes the screen and advances past the splash without submitting anything.
    // Safe in all 3 states: sleeping, splash showing, or password field already visible.
    Keyboard.press(KEY_LEFT_SHIFT);
    delay(100);
    Keyboard.releaseAll();
    delay(700);

    Keyboard.press(KEY_LEFT_SHIFT);
    delay(100);
    Keyboard.releaseAll();
    delay(1000);

    Keyboard.print(matched->password);
    delay(100);

    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();

  } else {
    Serial.println("AUTH_EVENT:" + uid + ":DENIED");
  }

  delay(2000);
  rfc.PICC_HaltA();
}
