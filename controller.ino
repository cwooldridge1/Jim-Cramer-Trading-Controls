
#include <SPI.h>
#include <MFRC522.h>

// set pin numbers that will act as listener for buy and sell
const int buyButtonPin = 2; // the number of the pushbutton pin
const int sellButtonPin = 4;
const int authLightPin = 6;
const int notAuthLightPin = 7;
bool auth = false;

// RFID vars
#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
// variables to monitor state
int buyButtonState = 0; // variable for reading the pushbutton status
int buyButtonPrevState = 0;
int sellButtonState = 0;
int sellButtonPrevState = 0;

void setup()
{
    Serial.begin(9600); // Initialize serial communications with the PC
    SPI.begin();        // Initiate  SPI bus
    mfrc522.PCD_Init(); // Initiate MFRC522
    // initialize the buy/sell button pins as an input
    pinMode(buyButtonPin, INPUT);
    pinMode(sellButtonPin, INPUT);
    pinMode(authLightPin, OUTPUT);
    pinMode(notAuthLightPin, OUTPUT);
    Serial.begin(9600);
}

String getUid()
{
    String content = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    return content.substring(1);
}

// VALID CODE = B3 DE E5 1D
void loop()
{
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
    {
        String uid = getUid();
        if (uid == "B3 DE E5 1D") // change here the UID of the card/cards that you want to give access
        {
            Serial.println("authenticated");
            digitalWrite(notAuthLightPin, LOW);
            auth = true;
            // make light flicker
            for (int i = 0; i < 10; i++)
            {
                digitalWrite(authLightPin, HIGH);
                delay(50);
                digitalWrite(authLightPin, LOW);
                delay(50);
            }
        }
        else
        {
            Serial.println("invalid");
            digitalWrite(authLightPin, LOW);
            auth = false;
            for (int i = 0; i < 10; i++)
            {
                digitalWrite(notAuthLightPin, HIGH);
                delay(50);
                digitalWrite(notAuthLightPin, LOW);
                delay(50);
            }
        }
    }
    if (auth)
    {
        digitalWrite(authLightPin, HIGH);
        digitalWrite(notAuthLightPin, LOW);
    }
    else
    {
        digitalWrite(notAuthLightPin, HIGH);
        digitalWrite(authLightPin, LOW);
    }
    // read the state of the pushbutton value:
    buyButtonState = digitalRead(buyButtonPin);
    sellButtonState = digitalRead(sellButtonPin);
    // To prevent multiple signals we know that the button was pressed is the last state was high and the current is low
    if (auth && buyButtonPrevState == HIGH && buyButtonState == LOW)
    {
        Serial.println("BUY");
    }
    if (auth && sellButtonPrevState == HIGH && sellButtonState == LOW)
    {
        Serial.println("SELL");
    }
    buyButtonPrevState = buyButtonState;
    sellButtonPrevState = sellButtonState;
}