#define BLYNK_TEMPLATE_ID "TMPL6xATOtNOF" //your ID from blink
#define BLYNK_TEMPLATE_NAME "Monitoring" //your name aplication in your blink
#define BLYNK_AUTH_TOKEN "rtsjRqiCIwOxuyo-8uIJUL43HdTDaNpA" //your token in your blink

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define sensor 33 
#define relay 4
#define ledRed 26  // GPIO untuk LED Merah
#define ledGreen 27 // GPIO untuk LED Hijau

// Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
BlynkTimer timer;

// Enter your Auth token
char auth[] = "rtsjRqiCIwOxuyo-8uIJUL43HdTDaNpA"; //your token in your blink

// Enter your WIFI SSID and password
char ssid[] = "*********"; //your name wifi
char pass[] = "*********"; //your password wifi

// Declare the soil moisture value as a global variable
int moistureValue = 0;
int value = 0;

// Manual control flags
bool manualPumpControl = false;
bool manualRedLedControl = false;
bool manualGreenLedControl = false;
bool manualMode = false;  // Untuk mengaktifkan mode manual

void setup() {
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  lcd.init();
  lcd.backlight();
  pinMode(relay, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  digitalWrite(relay, HIGH); // Pompa mati awal
  digitalWrite(ledRed, LOW);
  digitalWrite(ledGreen, HIGH);

  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(200);  // Delay animasi LCD
  }
  lcd.clear();
}

// Manual control for the pump
BLYNK_WRITE(V1) {
  manualPumpControl = true;
  bool pumpState = param.asInt();
  if (pumpState == 1) {
    digitalWrite(relay, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Pompa Manual ON");
  } else {
    digitalWrite(relay, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Pompa Manual OFF");
  }
}

// Manual control for the red LED
BLYNK_WRITE(V2) {
  manualRedLedControl = true;
  bool redLedState = param.asInt();
  if (redLedState == 1) {
    digitalWrite(ledRed, HIGH);
  } else {
    digitalWrite(ledRed, LOW);
  }
}

// Manual control for the green LED
BLYNK_WRITE(V3) {
  manualGreenLedControl = true;
  bool greenLedState = param.asInt();
  if (greenLedState == 1) {
    digitalWrite(ledGreen, HIGH);
  } else {
    digitalWrite(ledGreen, LOW);
  }
}

// Switch to toggle between manual and automatic mode
BLYNK_WRITE(V4) {
  manualMode = param.asInt();
  if (manualMode == 1) {
    lcd.setCursor(0, 1);
    lcd.print("Mode: Manual   ");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Mode: Automatic");
  }
}

void loop() {
  Blynk.run(); // Run the Blynk library

  // Read and calculate moisture value
  value = analogRead(sensor);
  value = map(value, 0, 4095, 0, 100);  // Convert value to 0-100 range
  moistureValue = (value - 100) * -1;  // Calculate moisture value
  Blynk.virtualWrite(V0, moistureValue);  // Send moisture value to Blynk
  Serial.println(moistureValue);

  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(moistureValue);
  lcd.print("%");

  // Automatic control only if manual mode is not active
  if (!manualMode) {
    if (moistureValue < 40) {  // Threshold for soil moisture
      digitalWrite(relay, LOW);  // Turn on the pump
      lcd.setCursor(0, 1);
      lcd.print("Pompa Auto ON ");
    } else {
      digitalWrite(relay, HIGH);  // Turn off the pump
      lcd.setCursor(0, 1);
      lcd.print("Pompa Auto OFF");
    } 
  }

  // Control red LED based on moisture value
  if (!manualRedLedControl) {
    digitalWrite(ledRed, (moistureValue < 50) ? HIGH : LOW);
  }

  // Control green LED based on moisture value
  if (!manualGreenLedControl) {
    digitalWrite(ledGreen, (moistureValue >= 50) ? HIGH : LOW);
  }

  delay(200); // Menunggu sebelum loop berikutnya
}
