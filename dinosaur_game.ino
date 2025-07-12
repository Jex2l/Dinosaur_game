#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Set the LCD I2C address (try 0x27 or 0x3F depending on your module)
LiquidCrystal_I2C lcd(0x27, 16, 2); 
const int analogPin = A0;

void setup() {
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Windmill Voltage");
  delay(3000);
}

void loop() {
  int analogValue = analogRead(analogPin);
  // Convert to voltage: (Analog value / 1023) * 5V * 11 (because of 10k & 1k divider)
  float voltage = (analogValue * 5.0 / 1023.0) * 11.0;
  lcd.setCursor(0, 1);
  lcd.print("Voltage : ");
  lcd.print(voltage, 2);  // Display voltage with 2 decimal places
  lcd.print("       ");    // Clear extra chars
  delay(500);
}
