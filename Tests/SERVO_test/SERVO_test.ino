#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

#define LCD_SDA 21
#define LCD_SCL 22
LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myservo;

int posVal = 0;               //posizione servo
int servoPin = 32;            //PIN servo

void setup() {
  Serial.begin(115200);
  Serial.println("Inizializzo seriale.");
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2500);


  Wire.begin(LCD_SDA, LCD_SCL);         //Attach IIC Pin
  if(!i2CAddrTest(0x27)){
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Inserisci pila");
  lcd.setCursor(0,1);
  lcd.print("per iniziare!");
}

void loop() {
  myservo.write(90);
  lcd.clear();
  lcd.print("chiuso");

  delay(2000);

  myservo.write(0);
  lcd.clear();
  lcd.print("aperto");

  delay(2000);

}

bool i2CAddrTest(uint8_t addr){
  Wire.begin();
  Wire.beginTransmission(addr);
  if(Wire.endTransmission() == 0){
    return true;
  }
  return false;
}
