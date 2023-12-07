#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <NTPClient.h> 
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiMulti wifiMulti;
const char* ssid = "Wifi name";
const char* password = "Basil's wifi password";
const int httpsPort = 443;                                                          
const String url = "http://api.coindesk.com/v1/bpi/currentprice/BTC.json";
const String historyURL = "http://api.coindesk.com/v1/bpi/historical/close.json";
const String cryptoCode = "BTC";
WiFiClient client;                                                            //Create a new WiFi client
HTTPClient http;
 
String formattedDate;                                                         //Create variables to store the date and time
String dayStamp;
String timeStamp;
#define SCROLL_BUTTON_PIN 12
#define SELECT_BUTTON_PIN 14
String menuItems[] = {
  "TIME ELAPSED",
  "GAMES",
  "WIFI SCANNER"
  "BTC"
};


const uint32_t connectTimeoutMs = 10000;
const unsigned char bitcoinLogo [] PROGMEM =                                 {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x03, 0xe7, 0xc0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x03, 0xe7, 0xc0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x02, 0x24, 0x40, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x02, 0x24, 0x40, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x02, 0x24, 0x40, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0x24, 0x40, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x02, 0x3c, 0x40, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x03, 0xfe, 0x3c, 0x7c, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x1f, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x03, 0x00, 0x00, 0x01, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x03, 0xf0, 0x3f, 0x80, 0x60, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0xf0, 0x3f, 0xe0, 0x20, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x18, 0x30, 0x70, 0x30, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x30, 0x10, 0x30, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x30, 0x10, 0x30, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x10, 0x30, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x30, 0x20, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0xe0, 0x60, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x3f, 0xc0, 0xc0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x00, 0x01, 0xc0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x00, 0x00, 0xf0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x00, 0x00, 0x38, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x3f, 0xe0, 0x18, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x18, 0x30, 0xf8, 0x0c, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x1c, 0x0c, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x0c, 0x0c, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x18, 0x30, 0x04, 0x0c, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x30, 0x0c, 0x0c, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x18, 0x30, 0x0c, 0x0c, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x18, 0x30, 0x38, 0x0c, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x18, 0x01, 0xf0, 0x3f, 0xf0, 0x08, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x01, 0xe0, 0x1f, 0x00, 0x18, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x1c, 0x01, 0x00, 0x00, 0x00, 0x30, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0e, 0x03, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x0f, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0x03, 0xfe, 0x3c, 0x7e, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x02, 0x3c, 0x40, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x02, 0x24, 0x40, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x02, 0x24, 0x40, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x02, 0x24, 0x40, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x02, 0x24, 0x40, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x03, 0xe7, 0xc0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x03, 0xe7, 0xc0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void wifiNETSCAN(){

  display.clearDisplay();
  display.println("Scan Starting.");
  display.display();
  delay(750);

  display.clearDisplay();
  display.println("Scan Starting..");
  display.display();
  delay(750);

  display.clearDisplay();
  display.println("Scan Starting...");
  display.display();
  delay(750);

  display.clearDisplay();
  int n = WiFi.scanNetworks();

    if (n == 0) {
    display.clearDisplay();
    display.println( "Networks Not Found!");
    display.display();
    delay(750);
  } else {
    int indis = 0;
    for (int i = 0; i < n; ++i) {
      display.println(WiFi.SSID(i));
      display.display();
      delay(10);
      indis = indis + 10;
    }
  }
  delay(5000);
}
   
int currentMenuItem = 0;
const int numMenuItems = 4; // Adjust based on the number of menu items
void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Welcome to Basify:");
  
  for (int i = 0; i < numMenuItems; i++) {
    if (i == currentMenuItem) {
      display.print(">");
    } else {
      display.print(" ");
    }
    display.println(menuItems[i]);
  }
  
  display.display();
}
  unsigned long currentTime = millis();
void scrollMenu(int direction) {
  currentMenuItem = (currentMenuItem + direction + numMenuItems) % numMenuItems;
  updateDisplay();
}

  bool scrollButtonState = digitalRead(SCROLL_BUTTON_PIN);
  bool selectButtonState = digitalRead(SELECT_BUTTON_PIN);


void APPLICATION_BTC() {

  display.clearDisplay();                                                     //Clear the display
  display.setTextColor(SSD1306_WHITE);                                        //Set the text colour to white
  display.drawBitmap(0, 0, bitcoinLogo, 128, 64, WHITE);                             //Display bitmap from array
  display.display();
  delay(2000);
 
  display.clearDisplay();                                                     //Clear the display
  display.setTextSize(1);                                                     //Set display parameters
  display.setTextColor(WHITE);
  display.println("Connecting to WiFi...");
  display.display();
 
  WiFi.begin(ssid, password);
 
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)                                        //Connect to the WiFi network
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
 
  display.println("Connected to: ");                                           //Display message once connected
  display.print(ssid);
  display.display();
  delay(1500);
  display.clearDisplay();
  display.display();


  
  Serial.print("Connecting to ");                                                       //Display url on Serial monitor for debugging
  Serial.println(url);
 
  http.begin(url);
  int httpCode = http.GET();                                                            //Get crypto price from API
  StaticJsonDocument<2000> doc;
  DeserializationError error = deserializeJson(doc, http.getString());
 
  if (error)                                                                            //Display error message if unsuccessful
  {
    Serial.print(F("deserializeJson Failed"));
    Serial.println(error.f_str());
    delay(2500);
    return;
  }
 
  Serial.print("HTTP Status Code: ");
  Serial.println(httpCode);
 
  String BTCUSDPrice = doc["bpi"]["USD"]["rate_float"].as<String>();                    //Store crypto price and update date in local variables
  String lastUpdated = doc["time"]["updated"].as<String>();
  http.end();
 
  Serial.print("Getting history...");
  StaticJsonDocument<2000> historyDoc;
  http.begin(historyURL);                                                               //Get historical crypto price from API
  int historyHttpCode = http.GET();
  DeserializationError historyError = deserializeJson(historyDoc, http.getString());
 
  if (historyError) {                                                                   //Display error message if unsuccessful
    Serial.print(F("deserializeJson(History) failed"));
    Serial.println(historyError.f_str());
    delay(2500);
    return;
  }
 
  Serial.print("History HTTP Status Code: ");
  Serial.println(historyHttpCode);
  JsonObject bpi = historyDoc["bpi"].as<JsonObject>();
  double yesterdayPrice;
  for (JsonPair kv : bpi) {
    yesterdayPrice = kv.value().as<double>();                                           //Store yesterday's crypto price
  }
 
  Serial.print("BTCUSD Price: ");                                                       //Display current price on serial monitor
  Serial.println(BTCUSDPrice.toDouble());
 
  Serial.print("Yesterday's Price: ");                                                  //Display yesterday's price on serial monitor
  Serial.println(yesterdayPrice);
  
  bool isUp = BTCUSDPrice.toDouble() > yesterdayPrice;                                  //Check whether price has increased or decreased
  double percentChange;
  String dayChangeString = "24hr Change:"; 
  if (isUp)                                                                             //If price has increased from yesterday
  {
    percentChange = ((BTCUSDPrice.toDouble() - yesterdayPrice) / yesterdayPrice) * 100;

  } 
  else                                                                                  //If price has decreased from yesterday
  {
    percentChange = ((yesterdayPrice - BTCUSDPrice.toDouble()) / yesterdayPrice) * 100;
    dayChangeString = dayChangeString + "-";
  
  }
 
  Serial.print("Percent Change: ");                                                     //Display the percentage change on the serial monitor
  Serial.println(percentChange);
 
  display.clearDisplay();                                                               //Clear the OLED display
  display.setTextSize(1);
  printCenter("BTC/USD", 0, 0);                                                         //Display the comparison header
 
  display.setTextSize(2);
  printCenter("$" + BTCUSDPrice, 0, 20);                                                //Display the current price
                                           
  display.setTextSize(1);                                                               //Display the change percentage
  dayChangeString = dayChangeString + percentChange + "%";
  printCenter(dayChangeString, 0, 55);
  display.display();                                                                    //Execute the new display
 
  http.end();                                                                           //End the WiFi connection
  esp_sleep_enable_timer_wakeup(900000000);
}
void printCenter(const String buf, int x, int y)                          //Function to centre the current price in the display width
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w, &h);                     //Calculate string width
  display.setCursor((x - w / 2) + (128 / 2), y);                          //Set cursor to print string in centre
  display.print(buf);                                                     //Display string
}
void APPLICATION_WIFISCAN() {
  wifiNETSCAN();
}

void APPLICATION_TIMEELSAPSED() {
 display.clearDisplay();
 display.println(currentTime);
 display.display();
}

void APPLICATION_GAMES(){


}
void (*actionFunctions[])() = {
APPLICATION_TIMEELSAPSED,
APPLICATION_GAMES,
APPLICATION_WIFISCAN,
APPLICATION_BTC
};

bool lastScrollButtonState = HIGH;
bool lastSelectButtonState = HIGH;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup() {
      WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
  Serial.begin(9600);
  pinMode(SCROLL_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SELECT_BUTTON_PIN, INPUT_PULLUP);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Welcome to Basify:");
  display.display();
  delay(1000);
}
void performAction(int menuItemIndex) {
   display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(menuItems[menuItemIndex]);
  display.display();
  
 
  actionFunctions[menuItemIndex]();
  delay(30000);
 if(selectButtonState == HIGH)
{
   // Display the action for 2 seconds
  updateDisplay(); // Return to the menu
}
}

void loop() {


  
  // Debounce the buttons
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    if (scrollButtonState != lastScrollButtonState) {
      if (scrollButtonState == LOW) {
        scrollMenu(1); // Scroll down
      }
      lastDebounceTime = currentTime;
    }
    if (selectButtonState != lastSelectButtonState) {
      if (selectButtonState == LOW) {
        performAction(currentMenuItem); // Select the current menu item
      }
      lastDebounceTime = currentTime;
    }
  }
  
  lastScrollButtonState = scrollButtonState;
  lastSelectButtonState = selectButtonState;
}

