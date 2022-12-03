/*
Based on: 
https://randomnerdtutorials.com/esp8266-and-node-red-with-mqtt/
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DFRobot_BMP280.h"

// Display 
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Sensor
typedef DFRobot_BMP280_IIC    BMP;
BMP   bmp(&Wire, BMP::eSdoLow);
#define SEA_LEVEL_PRESSURE    1015.0f   // sea level pressure
// show last sensor operate status
void printLastOperateStatus(BMP::eStatus_t eStatus)
{
  switch (eStatus) {
    case BMP::eStatusOK:    Serial.println("everything ok"); break;
    case BMP::eStatusErr:   Serial.println("unknow error"); break;
    case BMP::eStatusErrDeviceNotDetected:    Serial.println("device not detected"); break;
    case BMP::eStatusErrParameter:    Serial.println("parameter error"); break;
    default: Serial.println("unknow status"); break;
  }
}

//net config
const char* ssid = "Your_WLAN_SSID";
const char* password = "Your_WLAN_PWD";
const char* mqtt_server = "YOUR_MQTT_SERVER_IP";
const char* device_name = "DEVICE_NAME" ; 
const char* temp_channel= "CHANNEL/temperature";
const char* press_channel= "CHANNEL/pressure";

WiFiClient espClient;
PubSubClient client(espClient);

long now = millis();
long lastMeasure = 0;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("WiFi OK"); 
  display.display();
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  display.clearDisplay(); 
  display.setTextSize(2);
  display.println("MQTT rx!");
  display.setTextSize(1);
  display.print("Top:");
  display.println(topic); 
  display.print("Msg:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    display.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  display.display(); 
  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  /*
  if(topic=="room/lamp"){
      Serial.print("Changing Room lamp to ");
      if(messageTemp == "on"){
        digitalWrite(lamp, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "off"){
        digitalWrite(lamp, LOW);
        Serial.print("Off");
      }
  }
  */
  //Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("MQTT conn...");
    Serial.println("Attempting MQTT connection...");
    display.display();
    // Attempt to connect
    if (client.connect(device_name)) {
      Serial.println("connected");
      display.setTextSize(2);
      display.println("Connect!");
      display.setTextSize(1);
      display.print("Name:");
      display.println(device_name);
      display.display(); 
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("room/lamp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      display.println("Error");
      display.setTextSize(1);
      display.println(client.state());
      display.display(); 
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  bmp.reset();
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  delay(500); // Pause for 2 seconds
  display.setTextColor(SSD1306_WHITE); 
  display.clearDisplay();
  display.setTextSize(1);
  display.print("Starting up...");
  display.display();
  while (bmp.begin() != BMP::eStatusOK) {
    Serial.println("bmp begin faild");
    display.clearDisplay();
    display.setTextSize(2);  
    display.setCursor(0, 0);
    display.print("BMP280 Err");
    display.display();
    printLastOperateStatus(bmp.lastOperateStatus);
    delay(2000);
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("BMP ok");
  display.display();
  //MQTT Stuff 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(2000);
}

// For this project, you don't need to change anything in the loop function. Basically it ensures that you ESP is connected to your broker
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");

  now = millis();
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > 30000) {
    lastMeasure = now;
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    uint32_t p = bmp.getPressure();
    // Read temperature as Celsius (the default)
    float t = bmp.getTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    // Computes temperature values in Celsius
    //float hic = t;
    static char temperatureTemp[10];
    dtostrf(t, 6, 2, temperatureTemp); 
    
    static char pressureTemp[10];
    dtostrf(p, 6, 2, pressureTemp);

    // Publishes Temperature and Humidity values
    client.publish(temp_channel, temperatureTemp);
    client.publish(press_channel, pressureTemp);
    //prints on console 
    Serial.print("Pressure [mBar]: ");
    Serial.println(p);
    Serial.print("Temperature:");
    Serial.print((t));
    Serial.println("°C");
    
    //print on display
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.print("  ");
    display.print((t));
    display.println(" C");
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setCursor(34, 17);
    display.print(p);
    display.println(" mBar");
    display.setCursor(25, 27);
    float   alti = bmp.calAltitude(SEA_LEVEL_PRESSURE, p);
    display.print(int(alti));
    display.println(" m Altitude");
    display.setCursor(31, 42);
    display.setTextSize(1); 
    display.println(device_name);
    display.setCursor(30, 52);
    display.println(WiFi.localIP());
    display.drawLine( 0,  37,  128,  37, SSD1306_WHITE);
    display.drawLine( 0,  39,  128,  39, SSD1306_WHITE);
    display.display();
    // Check if any reads failed and exit early (to try again).
    if (isnan(p) || isnan(t) ) {
      Serial.println("Failed to read from BMP sensor!");
      display.clearDisplay();
      display.setTextSize(2);  
      display.setCursor(0, 0);
      display.print("BMP280 Err");
      display.display();
      return;
    }
   } // end now loop
} 
