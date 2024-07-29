/*
MIT License

Copyright (c) 2024 Andrea Carozzi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <ESP32Servo.h>
#include <UltrasonicSensor.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#define ANALOG_PIN 25
#define LCD_SDA 21
#define LCD_SCL 22

#define PIR_PIN 18  

#define MAX_9V 10.0
#define MIN_9V 8.0

#define MAX_3V 3.5
#define MIN_3V 1.9

#define MAX_1_5V 1.6
#define MIN_1_5V 1.1

#define R1 20000.0
#define R2 10000.0

#define SERVO_PIN 32
#define SERVO_OPEN 0
#define SERVO_CLOSE 90

#define ECHO_PIN 14
#define TRIG_PIN 13
#define TEMP 28

float voltage;
bool timerEnded = false;

Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
TimerHandle_t xServoTimer;
UltrasonicSensor ultrasonic(TRIG_PIN, ECHO_PIN);

enum VoltageType {
  CARICA_9V,
  SCARICA_9V,
  CARICA_3V,
  SCARICA_3V,
  CARICA_1_5V,
  SCARICA_1_5V,
  V_ERRATO
};

void setup() {
  Serial.begin(115200);
  Wire.begin(LCD_SDA, LCD_SCL);
  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }

  lcd.init();
  lcd.backlight();
  startScreen();

  myservo.setPeriodHertz(50);
  myservo.attach(SERVO_PIN, 500, 2500);
  myservo.write(SERVO_CLOSE);

  ultrasonic.setTemperature(TEMP);

  xServoTimer = xTimerCreate("ServoTimer", pdMS_TO_TICKS(10000), pdFALSE, (void*)0, timerCallback);
}

void loop() {
  float analogValue = analogRead(ANALOG_PIN);
  
  voltage = 0.00080566 * analogValue;
  voltage = ReadVoltage(ANALOG_PIN);
  voltage = voltage / (R2 / (R1 + R2));

  String voltstr = String(voltage, 3);
  Serial.println("Read: " + String(analogValue) + " Voltage: " + String(voltage, 3));

  VoltageType voltageType = getVoltageType(voltage);

  switch (voltageType) {
    case CARICA_9V:
      printLcd("9V carica");
      break;
    case SCARICA_9V:
      printLcd("9V scarica!");
      servoHandle();
      break;
    case CARICA_3V:
      printLcd("3V carica");
      startScreen();
      break;
    case SCARICA_3V:
      printLcd("3V scarica!");
      servoHandle();
      break;
    case CARICA_1_5V:
      printLcd("1.5V carica");
      startScreen();
      break;
    case SCARICA_1_5V:
      printLcd("Pila scarica!");
      servoHandle();
    case V_ERRATO:
    default:
      break;
  }

  delay(500);
}

/**
 *  servoHandle - funzione che aspetta il rilevamento del sensore
 *  INPUT: /
 *  OUTPUT: /
**/
void servoHandle() {
  Serial.println("Inizio Funzione");
  myservo.write(SERVO_OPEN);
  Serial.println("Servo ON");
  lcd.clear();
  lcd.setCursor(0, 0);
  
  if (xServoTimer != NULL) {
    xTimerStart(xServoTimer, 0);
  } 

  while (true) {
    if(timerEnded){
      timerEnded = false;
      break;  
    }
    int distance = ultrasonic.distanceInCentimeters();
    Serial.printf("D: %d\n",distance);
    if(distance < 10) { 
      delay(1000);
      Serial.println("Movement detected");
      xTimerStop(xServoTimer, 0);
      Serial.println("Timer bloccato");
      break;        
    }
    delay(10);
  }
  
  myservo.write(SERVO_CLOSE);
  Serial.print("Servo OFF");
  startScreen();

  Serial.println("Fine Funzione");
}

/**
 *  timerCallback - funzione che scatta al termine del timer
 *  INPUT: /
 *  OUTPUT: /
**/
void timerCallback(TimerHandle_t xTimer) {
  Serial.println("Timer expired, closing servo");
  myservo.write(SERVO_CLOSE);
  startScreen();
  timerEnded = true;
}

/**
 *  ReadVoltage - restituisce il voltaggio letto su un pin
 *  INPUT: pin - pin dal quale leggere
 *  OUTPUT: double - voltaggio rilevato
**/
double ReadVoltage(byte pin) {
  double reading = analogRead(pin);
  if (reading < 1 || reading > 4095) return 0;
  return -0.000000000000016 * pow(reading, 4) + 0.000000000118171 * pow(reading, 3) - 0.000000301211691 * pow(reading, 2) + 0.001109019271794 * reading + 0.034143524634089;
}

/**
 *  getVoltageType - restituisce il tipo di voltaggio rilevato in base al valore fornito
 *  INPUT: voltage - valore numerico del voltaggio rilevato
 *  OUTPUT: VoltageType - enum con il tipo di voltaggio
**/
VoltageType getVoltageType(float voltage) {
  if (voltage > MIN_9V && voltage < MAX_9V) {
    return CARICA_9V;
  } else if (voltage <= MIN_9V && voltage > MAX_3V) {
    return SCARICA_9V;
  } else if (voltage > MIN_3V && voltage < MAX_3V) {
    return CARICA_3V;
  } else if (voltage <= MIN_3V && voltage > MAX_1_5V) {
    return SCARICA_3V;
  } else if (voltage > MIN_1_5V && voltage < MAX_1_5V) {
    return CARICA_1_5V;
  } else if (voltage <= MAX_1_5V && voltage > 0.1) {
    return SCARICA_1_5V;
  } else {
    return V_ERRATO;
  }
}

/**
 *  printLcd - stampa un messaggio su schermo LCD in maniera pulita
 *  INPUT: message - messaggio da stampare
 *  OUTPUT: /
**/
void printLcd(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  delay(3000);
  startScreen();
}

/**
 *  startScreen - riporta lo schermo LCD alla schermata "base" da cui ripartire
 *  INPUT: /
 *  OUTPUT: /
**/
void startScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Collega pila");
  lcd.setCursor(0, 1);
  lcd.print("per iniziare!");
}

/** 
 *  i2CAddrTest - testa se il device richiesto è presente all'indirizzo fornito
 *  INPUT: addr - indirizzo da controllare
 *  OUTPUT: boolean - true se il device all'indirizzo addr è presente, false altrimenti
**/
bool i2CAddrTest(uint8_t addr) {
  Wire.begin();
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}
