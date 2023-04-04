//******************************************************************************
// Board: ESP32 Dev
// Interface into the Solcast Solar forcasting API
// https://toolkit.solcast.com.au
// You will need to create a FREE account.  This will allow a limited number
// of forcasts per day.
// Once you've registered and entered the details of your solar array, location
// etc, it will give you a URL which you can use to get the data.
// You'll also get an API key which is effectively your login.
// Enter these details below along with your WiFi credentials - and this will
// show the solar forcast in half hour intervals for tomorrow as well as a 
// cumulative total.
// The intention of this is to enable automation of your solar Inverter
// Battery charging based on solar gain
//
// Simon Rafferty 2023
//******************************************************************************

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <JsonStreamingParser.h>
#include <WiFiUdp.h>
//#include <Time.h>
//#include <TimeLib.h>

const char* ssid = "[Your SSID]";            // Replace with your network name
const char* password = "[Your Password]";    // Replace with your network password
const char* apiKey = "[Your API Key]";       // Replace with your Solcast API key
String url = "[Your Solcast URL]";           //Replace with the URL given on Solcast.  URL is specific to your location etc

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

/*
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}
*/

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

/*
  // Connect to NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  while (!time(nullptr)) {
    Serial.println("Waiting for NTP time sync...");
    delay(1000);
  }

  printLocalTime();
*/


  // Make the HTTP request
  HTTPClient http;
  http.begin(url);

  // Add User-Agent header
  http.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Firefox/87.0");
  http.addHeader("Accept-Encoding", "gzip, deflate, br");

  http.addHeader("Authorization", "Bearer " + String(apiKey));  

  
  int httpCode = http.GET();
  Serial.print("HTTP response code: ");
  Serial.println(httpCode);
  
  if (httpCode > 0) {

    DynamicJsonDocument doc(16384);
    String payload = http.getString();
    //Serial.println("HTTP payload:");
    //Serial.println(payload);
    
    DeserializationError error = deserializeJson(doc, payload);    
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    int count=0;
    bool bTomorrow = false;
    double dTomorrowTotal = 0.0;
    int nCount = 0;
    for (JsonObject forecast : doc["forecasts"].as<JsonArray>()) {
    
      //float pv_estimate = forecast["pv_estimate"]; // 3.8556, 3.6041, 3.281, 2.9187, 2.4915, 1.9523, ...
      String period_end = forecast["period_end"]; // "2023-04-03T14:00:00.0000000Z", ...

      //Other unused fields that are available
      //float forecast_pv_estimate10 = forecast["pv_estimate10"]; // 3.8556, 3.6041, 3.281, 2.9187, 2.4564, ...
      //float forecast_pv_estimate90 = forecast["pv_estimate90"]; // 3.8556, 3.6041, 3.281, 2.9187, 2.4915, ...
      //const char* period_end = forcast["period_end"];
      //const char* date_str = strstr(period_end, "T"); // get the date part


      int splitT = period_end.indexOf("T");
      int posHour = period_end.indexOf(":");
      String timeStamp = period_end.substring(splitT, period_end.length()-1);  //Time without preceeding T
      
      String sHour = timeStamp.substring(1, 3);
      String sMinute = timeStamp.substring(4, 6);
      String pv_estimate = forecast["pv_estimate"];
      
      int nHour = sHour.toInt();
      int nMinute = sMinute.toInt();
      float dEstimate = pv_estimate.toDouble();
      
      //Serial.print("JSON Hour: "); Serial.print(nHour);
      //Serial.print("   Minute: "); Serial.print(nMinute);
      int nTMins = nHour*60+nMinute;  
      if(nTMins < 30) bTomorrow = true; //Crossed midnight, get thext 24h of data
      
      if(bTomorrow && (nCount < 48)) {    
        //There are 48 readings in 24h - so once the first midnight is detected, get the next 48 readings 
        nCount++; 
        dTomorrowTotal = dTomorrowTotal + dEstimate;
        Serial.print("Time: "); Serial.print(nHour); Serial.print(":");Serial.print(nMinute);
        Serial.print("  Half Hour Estimate: "); Serial.print(dEstimate);
        Serial.print("    Cumulative Total "); Serial.print(dTomorrowTotal); Serial.print("kWh"); Serial.println();
      }
   
    }
  } else {
    Serial.println("HTTP request failed");
  }

  http.end();
}

void loop() {
  // Do nothing
}
