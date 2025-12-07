#include <dht11.h>
#include <LiquidCrystal.h>
#define DHT11PIN 7

dht11 DHT11;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int dry = 0;
const int wet = 280;
char *soluri[] = { "sandy", "peaty", "chalky", "silty", "fertile", "clay" };
const int buttonPin = 8;

int buttonState = 0;
int i = -1;

//const int led=8;
const int sensor_pin = A1;
int sensor;
const int threshold1 = 400;
const int threshold2 = 800;

int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

int screenMode = 0;
unsigned long lastScreenChange = 0;
unsigned long lastButtonPressTime = 0;

void button_press() {
  i++;
  if (i > 5) i = 0;

  screenMode = 3;
  lastButtonPressTime = millis();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Soil type:");

  lcd.setCursor(0, 1);
  lcd.print(soluri[i]);

  Serial.print("Soil: ");
  Serial.println(soluri[i]);

  lastButtonPressTime = millis();
}

bool isDry(int h) {
  if (i == 0) return (h < 8);
  if (i == 1) return (h < 10);
  if (i == 2) return (h < 13);
  if (i == 3) return (h < 15);
  if (i == 4) return (h < 17);
  if (i == 5) return (h < 18);
  return false;
}

bool isMoist(int h) {
  if (i == 0) return (h > 30);
  if (i == 1) return (h > 28);
  if (i == 2) return (h > 26);
  if (i == 3) return (h > 25);
  if (i == 4) return (h > 23);
  if (i == 5) return (h > 20);
  return false;
}

void setup() {
  // put your setup code here, to run once:
  lcd.setCursor(0, 0);
  //lcd.print("corbi");
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.

  pinMode(buttonPin, INPUT);
  lcd.print("for selecting, ");
  lcd.setCursor(0, 1);
  lcd.print("hold button");


  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int reading = digitalRead(buttonPin);

  // ---- Button press (single trigger) ----
  if (reading == HIGH && lastButtonState == LOW && (millis() - lastDebounceTime) > debounceDelay) {
    button_press();
    lastDebounceTime = millis();
  }
  lastButtonState = reading;

  // ---- Change screen every 2 seconds ----
  /*if ((millis() - lastScreenChange > 2000) && (millis() - lastButtonPressTime > 3000)) {
    screenMode++;
    if (screenMode > 2) screenMode = 0;
    lastScreenChange = millis();
    lcd.clear();
  }*/

  if (screenMode != 3 && (millis() - lastScreenChange > 4000)) {
    screenMode++;
    if (screenMode > 2) screenMode = 0;
    lastScreenChange = millis();
    lcd.clear();
  }

  // After 3 seconds, exit soil type screen
  if (screenMode == 3 && (millis() - lastButtonPressTime > 3000)) {
    screenMode = 0;
    lcd.clear();
  }

  // ---- Screen 0: Soil humidity ----
  if (screenMode == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Soil Humidity:");

    int sensorVal = analogRead(A0);
    int percentageHumidity = map(sensorVal, wet, dry, 100, 0);

    lcd.setCursor(0, 1);
    lcd.print(percentageHumidity);
    lcd.print("%  ");

    /*Serial.println(analogRead(A0));
    delay(100);*/

    lcd.setCursor(5, 1);
    if (i == -1) lcd.print("Select soil ");
    else if (isDry(percentageHumidity)) lcd.print("DRY        ");
    else if (isMoist(percentageHumidity)) lcd.print("MOIST      ");
    else lcd.print("Optimal    ");
    delay(100);
  }

  // ---- Screen 1: Air temp/humidity ----
  else if (screenMode == 1) {
    int chk=DHT11.read(DHT11PIN);
    lcd.setCursor(0, 0);
    lcd.print("Air Humidity:    "); lcd.setCursor(13,0); lcd.print(DHT11.humidity);
    lcd.setCursor(0, 1);
    lcd.print("Temperature:    "); lcd.setCursor(14,0); lcd.print(DHT11.temperature);  // replace later with DHT

    Serial.print("temp:"); Serial.println(DHT11.temperature);
    Serial.print("hum:"); Serial.println(DHT11.humidity);
    delay(100);
  }

  // ---- Screen 2: Light level ----
  else if (screenMode == 2) {
    int light = analogRead(sensor_pin);

    lcd.setCursor(0, 0);
    lcd.print("Light Level:");

    lcd.setCursor(0, 1);
    if (light < threshold1) lcd.print("TOO LOW    ");
    else if (light < threshold2) lcd.print("GOOD LIGHT ");
    else lcd.print("TOO HIGH   ");
  }

  else if (screenMode == 3) {
  lcd.setCursor(0,0);
  lcd.print("Soil type:");
  lcd.setCursor(0,1);
  lcd.print(soluri[i]);
}
}
