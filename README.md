# ThermoDude -  Wemos D1 Mini MQTT Temp Sensor with BMP280 and OLED Display 
This is just put together from some tutorials [Randomnerdtutorials](https://randomnerdtutorials.com/esp8266-and-node-red-with-mqtt/),
works well enough to have some cheap sensors flying around. 
I use these modules together with Mosquitto and NodeRED in a Raspberry Pi 3 for temperature monitoring at home. 
<p align="center">
    <img src="https://github.com/pgreendale/D1_BMP280_MQTT/blob/main/ThermoDude.png"
         alt="Application example" width="256" height="342" style="text-align:center">
    <figcaption>Application example</figcaption>
</p>


## Circuit 
<p align="center">
    <img src="https://github.com/pgreendale/D1_BMP280_MQTT/blob/main/MQTT_D1_BMP280.png"
         alt="Circuit"  width="270" height="181">
    <figcaption>Circuit overall</figcaption>
</p>

## Required Hardware 
* Wemos / Lolin D1 Mini or equivalent [D1 example](https://www.az-delivery.de/products/d1-mini?variant=28983931346&utm_source=google&utm_medium=cpc&utm_campaign=azd_de_google_performance-max_labelled-products&utm_content=&utm_term=&gclid=CjwKCAiAhKycBhAQEiwAgf19ejUDDPJHwq9djD_032-xvD4LKY-XZDFycc--IvuW0YG_h0-6AT73NhoCl_oQAvD_BwE)
* BMP280 Temp + Pressure Sensor [BMP280 Breakout example](https://www.berrybase.de/bmp280-breakout-board-2in1-sensor-fuer-temperatur-und-luftdruck?sPartner=g_shopping&gclid=CjwKCAiAhKycBhAQEiwAgf19enQ84-skYiH_ih1BCDzaZo_6FYZHl8o5LCdopuqMcimj4i_gABpTMRoCD1IQAvD_BwE)
* 128*64 OLED Display, better use Adafruit to support the Company [Oled Display example](https://www.berrybase.de/1.3-128x64-oled-display-sh1106-iic/i2c-interface-einfarbig-blau?c=2246)
* Case, 3D printed [Thingiverse, NodeMCU V3 Case ](https://www.thingiverse.com/thing:3605411)

## Required Software 
* ESP [ESP Arduino Libraries](http://arduino.esp8266.com/stable/package_esp8266com_index.json)
* Pubsubclient library [pucsubclient](https://github.com/knolleary/pubsubclient)
* BMP 280 Library [DFRobot_BMP280](https://github.com/DFRobot/DFRobot_BMP280)
* OLED Display Library (Adafruit Oled Library)[https://github.com/adafruit/Adafruit_SSD1306]

## How to proceed? 
* Get components, put them together as shown in circuit 
* Install arduino framework and needed libraries 
* Edit code, you'll need to put in your wlan ssid and password and your mqtt IP which needs to be in reach within network , change MQTT device ID and Topic  
* compile, upload

## Troubleshooting 
* some oled display have a different base address
* I experienced some issues during uploading new firmware while the program is running, caused by messages on the serial port
  * try it again in between 2 MQTT messages, shortly after temperature update 
