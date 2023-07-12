//B-SPCE SOFTWARE
/*  Written and madee by Basil Amin
Property of Basil Amin


*/


//Framework
#include <Arduino.h>

//Module comms
#include <SPI.h>
#include <Wire.h>


//Graphics drivers and oled drivers
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Bluetooth
#include <BluetoothSerial.h>


//hexfiles
#include <Hexfiles.h>



// Next added
//talkie, wifi, SD, MPU6050, NRF24L01


Adafruit_SSD1306 oled;

BluetoothSerial bt; 



void setup()
{
oled.begin();
bt.begin("B-SPECS");
oled.setCursor(0,0);
 oled.setTextColor(WHITE);
 oled.setTextSize(1);
oled.clearDisplay();
oled.display();
oled.drawBitmap(0,0, Loading_screen, 128, 64, WHITE);

}

void loop()
{
if(bt.connected() == true)
{
bt.println("Hi there! welcome to Basify!");


}
}






