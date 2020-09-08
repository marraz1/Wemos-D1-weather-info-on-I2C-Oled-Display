/*
   Project Name: COVID-19 DASHBOARD
   Components Used: ESP32, OLED Display

   Author: Vinod Mandavkar
   Instagram: https://www.instagram.com/maker.vinod/
   Instructables: https://www.instructables.com/member/makervinod/
   Youtube: https://www.youtube.com/c/VinodMandavkar
*/

//Use this if you are using ESP32
//#include <WiFi.h>
//#include <HTTPClient.h>

//Use this if you are using ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "ArduinoJson.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire); //Using the Adafruit Library
const char* ssid = "GUESTS";  //Add your WIFi-SSID
const char* password =  "ArielAgust";  //Add your WIFi-password

// set location and API key
String Location = "Vilnius, LT";//Replace with your nearby city
String API_Key  = "39b1360637c08dce96b33f6e5c7ca617";// Copy your Own API Key from WetherStaion website
int req = 1;
int chek_covid = 1;
int chek_weather = 1;
JsonObject root;
JsonObject root2;
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


void setup() {

  Serial.begin(115200);  //Starting Serial Communication
  WiFi.disconnect(true); //Disconecting the WiFi if we are previouly connected.

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64  |  You can change this to 0x3C if your display doesn't work
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  WiFi.begin(ssid, password); // Connecting to the WiFi

  while (WiFi.status() != WL_CONNECTED) { //Wait till we are connected
    animate_oled_wifi();  // Animate it on the Screen
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

      
      HTTPClient http;
      http.begin("http://covid.vinteq.in/api/get_data/?country=Lithuania"); //Specify the URL - You can change th country name as per your need to get the country specific data
      http.addHeader("auth-key", "29e2c06e5226cf2dc840e370ca5a128df2a7327c0345b48775341182be657ebb");   //Insert your Auth-key here in quotes (as String)
      int httpCode = http.GET();                       //Make the request
  
      if (httpCode > 0) 
      { //Check for the returning code
  
        String payload = http.getString();
        //Serial.println(payload);   //Print the Payload received
  
        DynamicJsonDocument doc(2 * payload.length());  //Flexible size
  
        // Parse JSON object
        DeserializationError error = deserializeJson(doc, payload);
        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
          return;
        }
        root = doc.as<JsonObject>(); //Mapping the data to Json Object
      }
      http.end(); //Free the resources
    }
  
    //  Handle the Data
    for (JsonPair kv : root) {
      if (String(kv.key().c_str()) == "history")
      {
        HISTORY = kv.value().as<String>();
      }   
    }
  
    //Removing Brackets
    HISTORY = HISTORY.substring(0, HISTORY.length() - 1);
    HISTORY = HISTORY.substring(1);
    //Serial.println(HISTORY);
  
    //We need the latest data - the first one
    String DATA = HISTORY.substring(0, HISTORY.indexOf('}') + 1);
    //Serial.println(DATA);
  
  //====================================
     //  Handle the Live Data
    for (JsonPair klv : root) {
      if (String(klv.key().c_str()) == "live_data")
      {
        LIVEDATA = klv.value().as<String>();
      }
    }
    
    //Removing Brackets
    LIVEDATA = LIVEDATA.substring(8, LIVEDATA.length() - 1);
    LIVEDATA = LIVEDATA.substring(1);
    //Serial.println(LIVEDATA);
  
    //We need the latest data - the first one
    String DATA2 = LIVEDATA.substring(0, LIVEDATA.indexOf('}') + 1);
    //Serial.println(DATA2);
  
    //======================================
  
    DynamicJsonDocument sub_doc(DATA.length() * 2);
    DeserializationError error = deserializeJson(sub_doc, DATA); //Converting the Extracted Nested Data from String to Json
    root = sub_doc.as<JsonObject>();
    for (JsonPair kv : root)
    {
      //Finding specific keys and extracting their values in varibales
      if (String(kv.key().c_str()) == "active_cases")
      {
        ACTIVE_CASES = kv.value().as<int>() ;
      }
      if (String(kv.key().c_str()) == "recovered_cases")
      {
        RECOVERED_CASES = kv.value().as<int>() ;
      }
      if (String(kv.key().c_str()) == "total_cases")
      {
        TOTAL_CASES = kv.value().as<int>() ;
      }
      if (String(kv.key().c_str()) == "deaths")
      {
        DEATHS = kv.value().as<int>();
      }    
    }
  
    DynamicJsonDocument sub_doc2(DATA2.length() * 2);
    DeserializationError error2 = deserializeJson(sub_doc2, DATA2); //Converting the Extracted Nested Data from String to Json
    root = sub_doc2.as<JsonObject>();
    for (JsonPair klv : root)
    {
      if (String(klv.key().c_str()) == "new")
      {
        NewCases = klv.value().as<String>();
      }
    }
    
    Serial.println("covid");
    Serial.println(chek_covid);
    chek_covid = 6;
  }
  else
  {
    chek_covid = chek_covid -1;
  }

  /*
     This is the display rountine
     What it does:
     Shows the above extracted data on the OLED Display
     Loops again as we need ot to display the same data for 1 minute before new data is fetched (You can change it according to your need)
     dealy of 5 seconds x 4 display values = 20 seconds per loop  - Repeating for 3 Times = 60 Seconds ~ 1 Minute
  */
  //for (byte i = 0; i < 3; i++)
  //{
    //Intro Page - Change according to your needs
    if (newcases > TOTAL_CASES || newcases < TOTAL_CASES)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("NEW CASES !!!");
        display.println(NewCases);
        display.display();
        newcases = TOTAL_CASES;
        Serial.printf("New cases was found");
        delay(5000);
    }
    else if (newdead > DEATHS || newdead < DEATHS)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("NEW DEAD !!!");
        display.println(DEATHS - newdead);
        display.display();
        newdead = DEATHS;
        Serial.printf("New cases was found");
        delay(5000);      
    }
    else if (newrecovered > RECOVERED_CASES || newrecovered < RECOVERED_CASES)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("NEW RECOVERED !!!");
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
    display.print("ACTIVE: ");
    display.setCursor(68, display.getCursorY());
    display.println(ACTIVE_CASES);
    display.display();
     //    Serial.println(NEW_CASES);
     display.setCursor(0, display.getCursorY());
    display.print("NEW CASES: ");
    display.setCursor(68, display.getCursorY());
    display.println(NewCases);
    display.display();
    //    Serial.println(RECOVERED_CASES);
    display.setCursor(0, display.getCursorY());
    display.print("RECOVERED: ");
    display.setCursor(68, display.getCursorY());
    display.println(RECOVERED_CASES);
    display.display();
    //    Serial.println(DEATHS);
    display.setCursor(0, display.getCursorY());
    display.print("DEATHS: ");
    display.setCursor(68, display.getCursorY());
    display.println(DEATHS);
    display.display();
    //    Serial.println(TOTAL_CASES);
    display.setCursor(0, display.getCursorY());
    display.print("TOTAL: ");
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
      display.print("  Miestas - Vilnius");// you can anything here to display on OLED.
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
  //}
}

//WiFi Connection Animation
void animate_oled_wifi()
{
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("Connecting");
  display.println("to WiFi...");
  display.setCursor(10, 0);
  display.println("   {-}   ");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("Connecting");
  display.println("to WiFi...");
  display.setCursor(10, 0);
  display.println("  { - }  ");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("Connecting");
  display.println("to WiFi...");
  display.setCursor(10, 0);
  display.println(" {  -  } ");
  display.display();
  delay(500);
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("Connecting");
  display.println("to WiFi...");
  display.setCursor(10, 0);
  display.println("{   -   }");
  display.display();
  delay(500);
}
