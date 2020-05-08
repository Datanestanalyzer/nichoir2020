# Data Nest Analyzer nichoir2020
> Here goes your awesome project description!

## Table of contents
* [Description](#Description)
* [Screenshots](#screenshots)
* [Technologies](#technologies)
* [Setup](#setup)
* [Status](#status)
* [Inspiration](#inspiration)
* [Contact](#contact)

## Description
This repository hosts the code wich allow to take a photo, weight and measure temperature and relative humidity of the inside of the nest at a predefined time. All these data is saved into microSD card. Moreover, the data is sent to a webserver. We use two methods : http and MQTT. The photo is sent using http and the other data is sent using MQTT. Finally, when all of this is done correctly, we put the ESP32-CAM into deep sleep and wake it up at the predefined time.
## Screenshots
![Example screenshot](./img/screenshot.png)

## Technologies
* ARDUINO 1.8.12

## Setup
Describe how to install / setup your local environement / add link to demo version.
We use Arduino IDE to program the ESP32-CAM board. So, you need to have Arduino IDE installed as well as the ESP32 add-on. Follow one of the next tutorials to install the ESP32 add-on, if you haven’t already:
 *Windows instructions : https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
 *Mac and Linux instructions : https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-mac-and-linux-instructions/

Before uploading the code, you need to insert your network credentials in the following variables: 
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

To upload the code, follow the next steps:
1) Go to Tools > Board and select AI-Thinker ESP32-CAM.
2) Go to Tools > Port and select the COM port the ESP32 is connected to.
3) Then, click the upload button to upload the code.
4) When you start to see these dots on the debugging window as shown below, press the ESP32-CAM on-board RST button.

Important : GPIO 0 (pin) needs to be connected to GND(pin) using a wire so that you’re abale to upload code.

After a few seconds, the code should be successfully uploaded to your board.

Finally, you should disconnet the GPIO 0 to GND and restart the ESP32-CAM by pressing RST button.





## Status
Project is: _in progress_

## Inspiration
https://randomnerdtutorials.com/esp32-cam-take-photo-save-microsd-card/
https://randomnerdtutorials.com/complete-guide-for-dht11dht22-humidity-and-temperature-sensor-with-arduino/
https://github.com/bogde/HX711/tree/master/examples
https://www.hivemq.com/blog/mqtt-client-library-encyclopedia-arduino-pubsubclient/
https://github.com/rodan/ds3231


## Contact
Created by ELGHAZOUANI Nada and ABDOUN Abderrahman - feel free to contact us on datanestanalyzerDNA@gmail.com !
