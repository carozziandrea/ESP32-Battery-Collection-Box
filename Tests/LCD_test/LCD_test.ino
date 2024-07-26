#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define LCD_SDA 21
#define LCD_SCL 22
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  Serial.begin(9600);
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
  lcd.setCursor(0,0);
  lcd.print("TEST");
  delay(1000);
}

bool i2CAddrTest(uint8_t addr){
  Wire.begin();
  Wire.beginTransmission(addr);
  if(Wire.endTransmission() == 0){
    return true;
  }
  return false;
}
