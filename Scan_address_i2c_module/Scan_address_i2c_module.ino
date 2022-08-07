#include <Wire.h>
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  while (!Serial);
  
}
void loop()
{
Serial.println("-----------------------");
  Serial.println("I2C Device Scanner");
  Serial.println("-----------------------");
  byte error;
  byte address;
  int Devices;
  Serial.println("Scanning...");
  delay(1000);
   Serial.println("--------------------------------");
  Devices = 0;
  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
      {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println("  !");
      Devices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (Devices == 0)
  {
    Serial.println("No I2C devices found\n");
  }
  else
  {
    Serial.println("-------------- Done!!! -------------");
  }
  delay(1000);
}
