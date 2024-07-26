#include <UltrasonicSensor.h>

#define ECHO_PIN 14
#define TRIG_PIN 13
#define TEMP 28

UltrasonicSensor ultrasonic(TRIG_PIN, ECHO_PIN);

void setup() {
    Serial.begin(115200);
    ultrasonic.setTemperature(TEMP);
}

void loop() {
      int distance = ultrasonic.distanceInCentimeters();
      Serial.printf("D: %d\n",distance);
      if(distance < 10) { 
        delay(1000);
        Serial.println("Movement detected");
      }
      delay(30);
}
