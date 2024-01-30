#include <SoftwareSerial.h>
SoftwareSerial mySerial(7, 8);
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define restpin 2

LiquidCrystal_I2C lcd(0x27, 16, 2);  
const int buttonPin = A2;
const int batteryPin = A0; 

int cyclenum = 0;
int currentWordIndex = 0;
int totalWords = 8;  // Total number of words
unsigned long buttonPressStartTime = 0;
bool showTopLineOnBottom = false;

const String words[] = {
  "Yes/Ok__",
  "No______",
  "Food____",
  "Drink___",
  "Washroom",
  "Emergncy",
  "Uncomfor",
  "STOP    "
};

void setup() {
  pinMode(13, OUTPUT);
  pinMode(restpin, OUTPUT);
  digitalWrite(restpin, 1);

  mySerial.begin(9600);
  Serial.begin(9600);
  lcd.init();
  lcd.init();
  lcd.backlight();
  pinMode(buttonPin, INPUT_PULLUP);

  displayCurrentWord();  // Initial display of the first word
}

void loop() {
  lcd.backlight();

  // Read the battery level
  int batteryLevel = analogRead(0);
  Serial.println(batteryLevel);
  batteryLevel = map(batteryLevel, 420, 580, 0, 100);
  Serial.println(batteryLevel);
  lcd.setCursor(12,0);
  lcd.print(batteryLevel); 
  lcd.setCursor(15,0); 
  lcd.print("%"); 
  
  lcd.setCursor(15,0);
  lcd.print("   ");
  cyclenum++;
  if (batteryLevel <= 10 && cyclenum>200) {
    // Battery level is less than or equal to 10%, send low battery message
    SendMessage("Low battery level: " + String(batteryLevel) + "%");
    delay(2000); // Display the low battery message for 2 seconds
    cyclenum = 0;
  } else if (digitalRead(buttonPin) == LOW) {
    if (buttonPressStartTime == 0) {
      buttonPressStartTime = millis();
    }

    // Check if the button has been pressed for at least 2 seconds
    if (millis() - buttonPressStartTime >= 2000) {
      showTopLineOnBottom = true;
      displayCurrentWordOnBottom();
    }
if ( currentWordIndex !=7) {
    if (millis() - buttonPressStartTime >= 6000) {
      showTopLineOnBottom = true;
      lcd.setCursor(9, 1);
      lcd.print(words[currentWordIndex]);
      if (SendMessage(words[currentWordIndex])) {
        // Message sent successfully
        lcd.clear();
      } else {
        // Message send failed, display an error message
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Error: Message Send");
        delay(2000); // Display error for 2 seconds
        lcd.clear();
      }
    }
}
if ( currentWordIndex ==7) {
    if (millis() - buttonPressStartTime >= 6000) {
      showTopLineOnBottom = true;
      lcd.setCursor(9, 1);
      lcd.print(words[currentWordIndex]);
      if (SendMessage("Patient is sleeping")) {
        // Message sent successfully
        lcd.clear();
      } else {
        // Message send failed, display an error message
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Error: Message Send");
        delay(2000); // Display error for 2 seconds
        lcd.clear();
      }
    }
}

  } else {
    if (buttonPressStartTime != 0) {
      // Button released, reset the timer
      buttonPressStartTime = 0;

      if (showTopLineOnBottom) {
        showTopLineOnBottom = false;
        displayCurrentWord();  // Reset back to normal display
      } else {
        // Cycle to the next word on a normal button press
        currentWordIndex = (currentWordIndex + 1) % totalWords;
        displayCurrentWord();
      }
    }
  }
}

void displayCurrentWord() {
  lcd.setCursor(0, 0);
  lcd.print(words[currentWordIndex]);
}

void displayCurrentWordOnBottom() {
  lcd.setCursor(0, 1);
  lcd.print(words[currentWordIndex]);
}

bool SendMessage(String message) {
  mySerial.println("AT+CMGF=1");
  delay(1000);
  mySerial.println("AT+CMGS=\"+94710134001\"\r");
  delay(1000);
  mySerial.println(message);
  delay(100);
  mySerial.println((char)26);
  delay(1000);

  // Wait for the response from the GSM module
  char response[100];
  int i = 0;
  while (mySerial.available()) {
    response[i++] = mySerial.read();
    if (i >= sizeof(response) - 1) {
      break;
    }
  }
  response[i] = '\0';

  // Check if the response contains "OK" to determine if the message was sent successfully
  if (strstr(response, "OK") != NULL) {
    digitalWrite(13, 1);
    delay(100);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(13, 1);
    delay(100);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(13, 1);
    delay(100);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(13, 1);
    delay(100);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(13, 1);
    delay(100);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(13, 1);
    delay(100);
    digitalWrite(13, 0);
    delay(100);
    digitalWrite(restpin, 0);
    return true; // Message sent successfully
  } else {
    digitalWrite(restpin, 0);
    return false; // Message send failed
  }
}
