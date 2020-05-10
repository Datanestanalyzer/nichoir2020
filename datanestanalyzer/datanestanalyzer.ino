//insert time to take a photo and collect data
const char *temps = "insert_time_like_in_the_example"; // example 16:00:00

// WiFi details
const char* ssid = "REPLACE_WITH_YOUR_SSID"; 
const char* password = "REPLACE_WITH_YOUR_PASSWORD";


// -----load cell (weight)------
#include "HX711.h"
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 4;
HX711 scale;
String weight;


//flash memory
#include <EEPROM.h>
#define EEPROM_SIZE 1


//// Disable brownour problems
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

//------http-----
#include "esp_http_client.h"
const char *post_url = "http://nichoir.ig.umons.ac.be/php-file-upload.php"; // Location where images are POSTED
//------camera photo -----------
#include "esp_camera.h"
#include "Arduino.h"
// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

int pictureNumber = 0;





//------Clock-time-and-date-----
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include<iostream>
#define _OPEN_SYS_ITOA_EXT
#include <stdio.h>
#include <stdlib.h>

String month;
String day;
String year;
String laDate;
String clock_time;

//-----publish----mqtt
#include <PubSubClient.h>
const char* mqtt_server = "broker.hivemq.com";
WiFiClient espClient;
PubSubClient client(espClient);
String DateTemperature;
String DateHumidity;
String DateMasse;
String datepublish;


//------Led of ESP32 cam----------
#include "driver/rtc_io.h"






// NTP allows us to synchronize the clock via a computer network using a reference
WiFiUDP ntpUDP;
int GMT = 2;
NTPClient timeClient(ntpUDP, "us.pool.ntp.org", GMT * 3600);



// RTC Libraries
#include <Wire.h>
#include <RtcDS3231.h>
#include <EepromAT24C32.h>
RtcDS3231<TwoWire> Rtc(Wire);
EepromAt24c32<TwoWire> RtcEeprom(Wire);
//RTC alarm
#define RtcSquareWavePin 13
#define RtcSquareWaveInterrupt 13
#define GPIO_INPUT_IO_0 13
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
  Serial.println("yes");

}

//-----SD card----------------
#include "FS.h"
#include "SD_MMC.h"
#include "SPI.h"
String message;




//--------DHT--------------
#include "DHT.h"
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define DHTPIN 2
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);




//--------WiFi------------
void WiFi_Setup() {

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}





//-----RTC Setup-----------
void RTC_Update() {
  // Do udp NTP lookup, epoch time is unix time - subtract the 30 extra yrs (946684800UL) library expects 2000
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime() - 946684800UL;
  Rtc.SetDateTime(epochTime);
}
bool RTC_Valid() {
  bool boolCheck = true;
  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC lost confidence in the DateTime!  Updating DateTime");
    boolCheck = false;
    RtcEeprom.Begin();
    RTC_Update();
  }
  if (!Rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now.  Updating Date Time");
    Rtc.SetIsRunning(true);
    boolCheck = false;
    RTC_Update();
  }
}

//--------publish--mqtt-------
void reconnect(String DateTemerature, String DateHumidity, String DateMasse ) {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    //String clientId = "ESP8266Client-";
    //clientId += String(random(0xffff), HEX);
    String clientId = "clientId_Nichoir2020";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("esp32/Temperaturedatanestanalyzer2020", DateTemerature.c_str());
      client.publish("esp32/Huimiditedatanestanalyzer2020", DateHumidity.c_str());
      client.publish("esp32/Massedatanestanalyzer2020", DateMasse.c_str());
      // ... and resubscribe
      //client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//--------http--------
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
  switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      Serial.println("HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      Serial.println("HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      Serial.println("HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      Serial.println();
      Serial.printf("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA:
      Serial.println();
      Serial.printf("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      if (!esp_http_client_is_chunked_response(evt->client)) {
        // Write out data
        // printf("%.*s", evt->data_len, (char*)evt->data);
      }
      break;
    case HTTP_EVENT_ON_FINISH:
      Serial.println("");
      Serial.println("HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      Serial.println("HTTP_EVENT_DISCONNECTED");
      break;
  }
  return ESP_OK;
}


//*****************Setup*****************************
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);
  rtc_gpio_hold_dis(GPIO_NUM_4);
  // -----load cell---------
  Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  if (scale.is_ready()) {
    scale.set_scale(8708);
    delay(2000);
    weight = String(scale.get_units(10));
    Serial.println(weight);
    message = String("weight:") + weight + String("g");
  }
  
  




  //------camera photo -------
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }



  //Initialize Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }



  //connection to WiFi
  WiFi_Setup();

  // Start NTP Time Client
  timeClient.begin();
  delay(2000);
  timeClient.update();

  //udp the RTC
  Wire.begin(15, 14); //(SDA, SCL)
  Rtc.Begin();
  RTC_Update();

  if (!RTC_Valid()) {
    RTC_Update();
  }
  // set the interupt pin to input mode
  pinMode(RtcSquareWavePin, INPUT);
  
  RtcDateTime currTime = Rtc.GetDateTime();
  //get the right date via getdt
  getdt(currTime, month, day, year, clock_time);
  laDate =  String(RightMonth(month) + ' ' + day + ' ' + year);
  datepublish = String(month + '-' + day + '-' + year);

  //--------DHT-----
  //Initialize the DHT sensor with the .begin() method.
  dht.begin();
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  message = message + String("\tHumidity: " + String(h) + " %\t Temperature: " + String (t) + " °C\t ");
  message = message + String("taken at ") + String(laDate) + "at " + String(clock_time) ;
  DateTemperature = datepublish + String(t);
  DateHumidity = datepublish + String(h);
  DateMasse = datepublish + weight;
  Serial.println(message);



  //------camera photo -------
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  // Take Picture with Camera
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return ;
  }





  //-----SD card--------
  SD_MMC.begin("/sdcard", true);
  if (!SD_MMC.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  //    Serial.print("SD Card Type: ");
  //    if(cardType == CARD_MMC){
  //      Serial.println("MMC");
  //    } else if(cardType == CARD_SD){
  //      Serial.println("SDSC");
  //    } else if(cardType == CARD_SDHC){
  //      Serial.println("SDHC");
  //    } else {
  //      Serial.println("UNKNOWN");
  //    }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  //Serial.printf("SD Card Size: %lluMB\n", cardSize);

  //write in a file the data (weight,humidity,temperature,time,date)
  fs::FS &fs = SD_MMC;
  File file_1 = fs.open("/data.txt", FILE_WRITE);

  if (!file_1) {
    Serial.println("Failed to open file for writing");
    return;
  }

  if (file_1.print(message.c_str())) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file_1.close();



  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
  pictureNumber = EEPROM.read(0) + 1;

  // Path where new picture will be saved in SD Card
  String path = "/picture" + String(pictureNumber) + ".jpg";

  //--------Camera Photo--------
  //save the photo in Sd card
  File file_2 = fs.open(path.c_str(), FILE_WRITE);
  if (!file_2) {
    Serial.println("Failed to open file in writing mode");
  }
  else {
    file_2.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
  file_2.close();





  //-----publish---mqtt
  client.setServer(mqtt_server, 1883);
  delay(5000);
  if (!client.connected()) {

    reconnect(DateTemperature, DateHumidity, DateMasse);
  }
  //client.loop();


  esp_http_client_handle_t http_client;

  esp_http_client_config_t config_client = {0};
  config_client.url = post_url;
  config_client.event_handler = _http_event_handler;
  config_client.method = HTTP_METHOD_POST;

  http_client = esp_http_client_init(&config_client);

  esp_http_client_set_post_field(http_client, (const char *)fb->buf, fb->len);

  esp_http_client_set_header(http_client, "Content-Type", "image/jpg");

  esp_err_t err1 = esp_http_client_perform(http_client);
  if (err1 == ESP_OK) {
    Serial.print("esp_http_client_get_status_code: ");
    Serial.println(esp_http_client_get_status_code(http_client));
  }

  esp_http_client_cleanup(http_client);

  esp_camera_fb_return(fb);

  // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);
  delay(1000);
  //RTC alarm
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeAlarmOne);
  RtcDateTime alarmTime = RtcDateTime(laDate.c_str(), temps);
  //  RtcDateTime alarmTime = RtcDateTime(laDate.c_str(), temps.c_str()); // into the future
  DS3231AlarmOne alarm1(
    alarmTime.Day(),
    alarmTime.Hour(),
    alarmTime.Minute(),
    alarmTime.Second(),
    DS3231AlarmOneControl_HoursMinutesSecondsMatch);
  Rtc.SetAlarmOne(alarm1);
  // throw away any old alarm state before we ran
  Rtc.LatchAlarmsTriggeredFlags();
  // setup external interupt
  //attachInterrupt(RtcSquareWaveInterrupt, InteruptServiceRoutine, CHANGE);
  err = gpio_isr_handler_add(GPIO_NUM_13, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
  if (err != ESP_OK) {
    Serial.printf("handler add failed with error 0x%x \r\n", err);
  }
  err = gpio_set_intr_type(GPIO_NUM_13, GPIO_INTR_ANYEDGE);
  if (err != ESP_OK) {
    Serial.printf("set intr type failed with error 0x%x \r\n", err);
  }
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0);
  //go to sleep
  Serial.println("i'm going to sleep ");
  esp_deep_sleep_start();
}





void loop() {

}





#define countof(a) (sizeof(a) / sizeof(a[0]))
//get dt allow us to get the date of the day from RtcDateTime where the date is saved in another Type
void getdt(const RtcDateTime& dt, String &month, String &day, String &year, String &clock_time)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());


  month = String(datestring[0]) + String(datestring[1]);
  day = String(datestring[3]) + String(datestring[4]);
  year = String(datestring[6]) + String(datestring[7]) + String(datestring[8]) + String(datestring[9]);
  clock_time = String(datestring[11]) + String(datestring[12]) + ":" + String(datestring[14]) + String(datestring[15]) + ":" + String(datestring[17]) + String(datestring[18]);
}

//RightMonth converts the month as a number to a particular String
String RightMonth(String month) {
  if (month == "01") {
    return "Jan";
  }
  if (month == "02") {
    return "Feb";
  }
  if (month == "03") {
    return "Mar";
  }
  if (month == "04") {
    return "Apr";
  }
  if (month == "05") {
    return "May";
  }
  if (month == "06") {
    return "Jun";
  }
  if (month == "07") {
    return "Jul";
  }
  if (month == "08") {
    return "Aug";
  }
  if (month == "09") {
    return "Sep";
  }
  if (month == "10") {
    return "Oct";
  }
  if (month == "11") {
    return "Nov";
  }
  if (month == "12") {
    return "Dec";
  }

}




//print date and time
void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );

  Serial.print(datestring);
}
