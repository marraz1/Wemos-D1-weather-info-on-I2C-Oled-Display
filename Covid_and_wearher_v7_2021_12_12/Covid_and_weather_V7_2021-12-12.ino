/*
   Project Name: COVID-19
   Components Used: ESP32, OLED Display
   Author: Martynas Razbadauskis
*/

//Use this if you are using ESP32
//#include <WiFi.h>
//#include <HTTPClient.h>
#include <avr/pgmspace.h>
//Use this if you are using ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "icon.h"
#include "ArduinoJson.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>

Adafruit_SSD1306 display(128, 64, &Wire); //Using the Adafruit Library
//work
char* ssid = "Some";  //Add your WIFi-SSID
/*
char* ssid = "GUEST";  //Add your WIFi-SSID
String password =  "pass";  //Add your WIFi-password
//home
const char* ssidh = "50Butas ";  //Add your WIFi-SSID
const char* passwordh =  "999999999i";  //Add your WIFi-password
*/
int framecount = 0;

// set location and API key
String Location = "Kaunas";//Replace with your nearby city
String API_Key  = "39b1360637c08dce96b33f6e5c7ca617";// Copy your Own API Key from WetherStaion website
int req = 1;
int chek_covid = 1;
int chek_weather = 1;
JsonObject root;
JsonObject rootLT;
JsonObject root2;

long total_casesLT; // 193562
int new_casesLT; // 561
long recovered_casesLT; // 179364
int deceasedDaily; // 15
int total_deaths_casesLT; // 3162
long active_casesLT;

//======================
int ACTIVE_CASES = 0, RECOVERED_CASES = 0, DEATHS = 0; //some data storage strings
String HISTORY = "", LIVEDATA2 = "", LIVEDATA = "", NewCases = "";
int newcases = 0, newdead = 0, newrecovered = 0;
int TOTAL_CASES = 0;
//======================
float temp;               // get temperature in °C
int   humidity;           // get humidity in %
float pressure;           // get pressure in bar
float wind_speed;         // get wind speed in m/s
int  wind_degree;         // get wind degree in °
//======================


//=========Covid-19
const char* host = "api.apify.com";
const int httpsPort = 443;

// DigiCert High Assurance EV Root CA
const char trustRoot[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
X509List cert(trustRoot);




void setup() {

  Serial.begin(115200);  //Starting Serial Communication
  WiFi.disconnect(true); //Disconecting the WiFi if we are previouly connected.

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64  |  You can change this to 0x3C if your display doesn't work
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

 // WiFi.begin(ssid, password); // Connecting to the WiFi

   //*****************************************

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // Uncomment and run it once, if you want to erase all the stored information
  //wifiManager.resetSettings();
  
  // set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  // or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  // if you get here you have connected to the WiFi
  Serial.println("Connected.");
  //**********************************************
  
  int wifitry = 10;
  
  while (WiFi.status() != WL_CONNECTED) 
  { //Wait till we are connected
    animate_oled_wifi(ssid);  // Animate it on the Screen
  }
  
  display.clearDisplay();
  display.display(); //update the display

  Serial.println("Connected to the WiFi network");
}

//This is what we do repeatedly !!
void loop()
{
    if (chek_covid == 1)
    {
    
    if ((WiFi.status() == WL_CONNECTED)) 
    { //Check the current connection status
    //==========New apify====================
      // Set time via NTP, as required for x.509 validation
      configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    
      Serial.print("Waiting for NTP time sync: ");
      time_t now = time(nullptr);
      while (now < 8 * 3600 * 2) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
      }
      Serial.println("");
      struct tm timeinfo;
      gmtime_r(&now, &timeinfo);
      Serial.print("Current time: ");
      Serial.print(asctime(&timeinfo));
    
      // Use WiFiClientSecure class to create TLS connection
      WiFiClientSecure client;
      Serial.print("Connecting to ");
      Serial.println(host);
    
      Serial.printf("Using certificate: %s\n", trustRoot);
      client.setTrustAnchors(&cert);
    
      if (!client.connect(host, httpsPort)) {
        Serial.println("Connection failed");
        return;
      }
    
      String url = "/v2/key-value-stores/xhGDb8VTqjtm1AQL6/records/LATEST?disableRedirect=true";
      Serial.print("Requesting URL: ");
      Serial.println(url);
    
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "User-Agent: BuildFailureDetectorESP8266\r\n" +
                   "Connection: close\r\n\r\n");
    
      Serial.println("Request sent");
      String line;
      String silale;
      String LTcovid;
      char b[] = "}";
      char a[] = "{";
      int all = 2;
      int readtime = 1;
      while (client.connected()) {
        line = client.readStringUntil('{'); //https://arduinojson.org/v6/how-to/reduce-memory-usage/
        if (line == "\r") {
          Serial.println("Headers received");
          Serial.println(line);
          break;
        }
        
        if(all == readtime)
        {
        //  int sizes = line.length();
        //  line.remove(sizes-6, 5);
        //  LTcovid = String(a + line + '0' + b);
          LTcovid = String(a + line);
          //LTcovid = String(LTcovid + line);
          Serial.println("LTcovid");
          Serial.println(LTcovid);
        }
        
        /*if(line.indexOf("Šilalės r. sav.") != -1)
        {
        silale = line;  
        silale = String(a + line);
        Serial.println("Šilale");
        Serial.println(silale);
        }  */     
        readtime++;
      }
      
     /* line = String(line + b);
      Serial.println("Antras");
      Serial.println(line);
      String payload = client.readStringUntil('[');
      if (payload.startsWith("\"region\": \"Šilalės r. sav.\"")) { //(payload.startsWith("\"region\": \"Šilalės r. sav.\""))
        Serial.println("esp8266/Arduino CI successfull!");
      } else {
        Serial.println("esp8266/Arduino CI has failed");
      }
      Serial.println("Reply was:");
      Serial.println("==========");
      Serial.println(payload);
      Serial.println("==========");
      Serial.println("Closing connection");
    //=======
    */
  //==============Šilalė=========================
   /*     DynamicJsonDocument doc(2 * silale.length());  //Flexible size
  
        // Parse JSON object
        DeserializationError error = deserializeJson(doc, silale);
        if (error)
        {
          Serial.print(F("deserializeJson() failed: corona"));
          Serial.println(error.c_str());
          return;
        }
        root = doc.as<JsonObject>(); //Mapping the data to Json Object
*/
//======================================================
//=============Lietuva================================
        DynamicJsonDocument docLT(2 * LTcovid.length());  //Flexible size
  
        // Parse JSON object
        DeserializationError errorLT = deserializeJson(docLT, LTcovid);
        if (errorLT)
        {
          Serial.print(F("deserializeJson() failed: corona"));
          Serial.println(errorLT.c_str());
          return;
        }
        rootLT = docLT.as<JsonObject>(); //Mapping the data to Json Object
        //===============Lietuva===============
        total_casesLT = docLT["deceased"]; // 193562
        new_casesLT = docLT["newCases"]; // 561
        recovered_casesLT = docLT["recovered"]; // 179364
        deceasedDaily = docLT["deceasedDaily"]; // 15
        total_deaths_casesLT = docLT["deceased"]; // 3162
        active_casesLT = total_casesLT - recovered_casesLT;
        Serial.println("Lietuva");
        Serial.println(total_casesLT);    
        Serial.println(new_casesLT);
        Serial.println(active_casesLT);
        Serial.println(recovered_casesLT); 
        Serial.println(deceasedDaily);   
        Serial.println(total_deaths_casesLT);

        
    }
    //===============Lietuva===============
   /* long total_casesLT = rootLT["infected"];
    String new_casesLT = rootLT["infectedDaily"];
    int active_casesLT = rootLT["active"];
    long recovered_casesLT = rootLT["recovered"];    
    int total_deaths_casesLT = rootLT["deceased"];
    Serial.println("Lietuva");
    Serial.println(total_casesLT);    
    Serial.println(new_casesLT);
    Serial.println(active_casesLT);
    Serial.println(recovered_casesLT);    
    Serial.println(total_deaths_casesLT);
*/
   /* NewCasesLT = new_cases;
    ACTIVE_CASES = active_cases;
    RECOVERED_CASES = recovered_cases;
    TOTAL_CASES = total_cases;
    DEATHS = total_deaths_cases;
*/
    
    //===============Silale===============
  /* String new_cases = root["newCases"];
    int active_cases = root["active"];
    int recovered_cases = root["recovered"];
    int total_cases = root["infected"];
    int total_deaths_cases = root["deceased"];
    Serial.println("Šilalė");
    Serial.println(new_cases);
    Serial.println(active_cases);
    Serial.println(recovered_cases);
    Serial.println(total_cases);
    Serial.println(total_deaths_cases);
*/
    NewCases = new_casesLT;
    ACTIVE_CASES = active_casesLT;
    RECOVERED_CASES = recovered_casesLT;
    TOTAL_CASES = total_casesLT;
    DEATHS = total_deaths_casesLT;
    
    Serial.println("covid");
    Serial.println(chek_covid);
    chek_covid = 6;
  }
  else
  {
    chek_covid = chek_covid -1;
  }

  //for (byte i = 0; i < 3; i++)
  //{
    //Intro Page - Change according to your needs
    if (newcases > TOTAL_CASES || newcases < TOTAL_CASES)
    {
        //==================animation====================
         hourse_animation();
        //======================================
        display.clearDisplay();
        display.drawBitmap(0, 0, ill, 128, 64, WHITE);
        display.display();
        delay(5000);
        
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(" NEW CASES");
        display.println();
        display.setCursor(40, display.getCursorY());
        display.println(NewCases);
        display.display();
        newcases = TOTAL_CASES;
        Serial.printf("New cases was found");
        delay(5000);
    }
    else if (newdead > DEATHS || newdead < DEATHS)
    {
        //==================animation====================
          hourse_animation();
        //======================================
        display.clearDisplay();
        display.drawBitmap(0, 0, deadd, 128, 64, WHITE);
        display.display();
        delay(5000);
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(20, 0);
        display.println(" DEADS ");
        display.println();
        display.setCursor(40, display.getCursorY());
        display.println(DEATHS - newdead);
        display.display();
        newdead = DEATHS;
        Serial.printf("New cases was found");
        delay(5000);      
    }
    else if (newrecovered > RECOVERED_CASES || newrecovered < RECOVERED_CASES)
    {
        //======================================
          hourse_animation();
        //======================================
        display.clearDisplay();
        display.drawBitmap(0, 0, recover, 128, 64, WHITE);
        display.display();
        delay(5000);
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(" RECOVERED");
        display.println();
        display.setCursor(40, display.getCursorY());
        display.println(RECOVERED_CASES - newrecovered);
        display.display();
        newrecovered = RECOVERED_CASES;
        Serial.printf("New cases was found");
        delay(5000);
        
    }
    else
    {
      Serial.printf("New cases not found");
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Statistic - COVID-19");
    display.println("Countri - Lithuania");
    display.println();
    display.display();
    //    Serial.println(ACTIVE_CASES);
    display.setCursor(0, display.getCursorY());
    display.print("Active: ");
    display.setCursor(68, display.getCursorY());
    display.println(ACTIVE_CASES);
    display.display();
     //    Serial.println(NEW_CASES);
     display.setCursor(0, display.getCursorY());
    display.print("New cases: ");
    display.setCursor(68, display.getCursorY());
    display.println(NewCases);
    display.display();
    //    Serial.println(RECOVERED_CASES);
    display.setCursor(0, display.getCursorY());
    display.print("Recovered: ");
    display.setCursor(68, display.getCursorY());
    display.println(RECOVERED_CASES);
    display.display();
    //    Serial.println(DEATHS);
    display.setCursor(0, display.getCursorY());
    display.print("Deaths: ");
    display.setCursor(68, display.getCursorY());
    display.println(DEATHS);
    display.display();
    //    Serial.println(TOTAL_CASES);
    display.setCursor(0, display.getCursorY());
    display.print("Total: ");
    display.setCursor(68, display.getCursorY());
    display.println(TOTAL_CASES);
    display.display();
    delay(10000);

    //======================================
   if (chek_weather == 1)
   { 
    HTTPClient http;  //Declare an object of class HTTPClient
 
    // specify request destination
    http.begin("http://api.openweathermap.org/data/2.5/weather?q=" + Location + "&APPID=" + API_Key);  // !!
 
    int httpCode = http.GET();  // send the request
 
      if (httpCode > 0)  // check the returning code
      {
        String payload = http.getString();   //Get the request response payload
   
        DynamicJsonDocument root(2 * payload.length());
   
        // Parse JSON object
        //JsonObject& root = jsonBuffer.parseObject(payload);
       /* deserializeJson(root, payload);
        
        if (!root.success()) {
          Serial.println(F("Parsing failed!"));
          return;
        }*/
  
        DeserializationError error = deserializeJson(root, payload);
        if (error) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(error.c_str());
            return;
        }
   
        temp = (float)(root["main"]["temp"]) - 273.15;        // get temperature in °C
        humidity = root["main"]["humidity"];                  // get humidity in %
        pressure = (float)(root["main"]["pressure"]) / 1000;  // get pressure in bar
        wind_speed = root["wind"]["speed"];                   // get wind speed in m/s
        wind_degree = root["wind"]["deg"];                     // get wind degree in °
  
              
      }
      Serial.println("covid");
      Serial.println(chek_weather);
      chek_weather = 6;  
      http.end();   //Close connection
    }
    else
    {
      chek_weather = chek_weather -1;
    }
      /*
      // print data
      Serial.printf("Temperature = %.2f°C\r\n", temp);
      Serial.printf("Humidity    = %d %%\r\n", humidity);
      Serial.printf("Pressure    = %.3f bar\r\n", pressure);
      Serial.printf("Wind speed  = %.1f m/s\r\n", wind_speed);
      Serial.printf("Wind degree = %d°\r\n\r\n", wind_degree);
      Serial.printf("Request = %d°\r\n\r\n", req = req + 1);
      */

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("  Oru prognoze");
      display.print("  Miestas - Kaunas");// you can anything here to display on OLED.
      display.setCursor(0, 24);
      display.printf("Temperature: %5.2f C\r\n", temp);
      display.printf("Humidity   : %d %%\r\n", humidity);
      display.printf("Pressure   : %.3fbar\r\n", pressure);
      display.printf("Wind speed : %.1f m/s\r\n", wind_speed);
      display.printf("Wind degree: %d", wind_degree);
      display.drawRect(109, 24, 3, 3, WHITE);     // put degree symbol ( ° )
      display.drawRect(97, 56, 3, 3, WHITE);
      display.display(); 
      delay(10000);
    //========================================== 
}


//WiFi Connection Animation
void animate_oled_wifi(char* ssid)
{         
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.print("Connecting to "); 
  display.println(ssid);
  display.println("to WiFi...");
  display.setCursor(10, 0);
  display.println("   {-}   ");
  display.display();
  Serial.println("Connecting to the WiFi network");
  delay(500);
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Connecting to "); 
  display.println(ssid);
  display.println("to WiFi...");
  display.setCursor(10, 0);
  display.println("  { - }  ");
  display.display();
  Serial.println("Connecting to the WiFi network");
  delay(500);
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Connecting to "); 
  display.println(ssid);
  display.println("to WiFi...");
  display.setCursor(10, 0);
  display.println(" {  -  } ");
  display.display();
  Serial.println("Connecting to the WiFi network");
  delay(500);
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Connecting to "); 
  display.println(ssid);
  display.println("to WiFi...");
  display.setCursor(10, 0);
  display.println("{   -   }");
  display.display();
  Serial.println("Connecting to the WiFi network");
  delay(500);
}

void hourse_animation()
{
      //==================animation====================
          for (int i = 0; i < 3; i++)
          {
            while ( framecount < 14 ) 
            {
            display.fillRect(0, 0, 128, 64, WHITE);
            framecount = framecount + 1;
  
            if(framecount == 0){
              display.drawBitmap(0, 0, horse00, 128, 64, BLACK);
              delay(100);
            }
              
            if(framecount == 1){
              display.drawBitmap(0, 0, horse01, 128, 64, BLACK);
              delay(100);
            }
              
            if(framecount == 2){
              display.drawBitmap(0, 0, horse02, 128, 64, BLACK);
              delay(100);
            }
              
            if(framecount == 3){
              display.drawBitmap(0, 0, horse03, 128, 64, BLACK);
              delay(100);
            }
            
            if(framecount == 4){
              display.drawBitmap(0, 0, horse04, 128, 64, BLACK);
              delay(100);
            }
          
            if(framecount == 5){
              display.drawBitmap(0, 0, horse05, 128, 64, BLACK);
              delay(100);
            }
          
            if(framecount == 6){
              display.drawBitmap(0, 0, horse06, 128, 64, BLACK);
              delay(100);
            }
          
            if(framecount == 7){
              display.drawBitmap(0, 0, horse07, 128, 64, BLACK);
              delay(100);
            }
          
            if(framecount == 8){
              display.drawBitmap(0, 0, horse08, 128, 64, BLACK);
              delay(100);
            }
          
            if(framecount == 9){
              display.drawBitmap(0, 0, horse09, 128, 64, BLACK);
              delay(100);
            }
          
            if(framecount == 10){
              display.drawBitmap(0, 0, horse10, 128, 64, BLACK);
              delay(100);
            }
          
            if(framecount == 11){
              display.drawBitmap(0, 0, horse11, 128, 64, BLACK);
              delay(100);
            }
          
            if(framecount == 12){
              display.drawBitmap(0, 0, horse12, 128, 64, BLACK);
              delay(100);
            }
          
            if(framecount == 13){
              display.drawBitmap(0, 0, horse13, 128, 64, BLACK);
              delay(100);
            }
         
            if(framecount == 14){
              display.drawBitmap(0, 0, horse14, 128, 64, BLACK);
              delay(100);
            }
          
            display.display();
  
            }
        }
          if(framecount == 14){
            framecount = 0;
          } 
        //======================================
}
