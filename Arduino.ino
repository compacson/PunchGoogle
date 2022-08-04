// Example Arduino/ESP8266 code to upload data to Google Sheets when a button is pressed
// Follow setup instructions found here:
// https://github.com/StorageB/Google-Sheets-Logging
// reddit: u/StorageB107
// email: StorageUnitB@gmail.com


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <Wiegand.h>
WIEGAND wg;

// Enter network credentials:
const char* ssid     = "Chez Bob et Choux";
const char* password = "123Qwerty!";

// Enter Google Script Deployment ID:
const char *GScriptId = "AKfycbya7jbH_t8dQGUlZVRRynW1Mx7uR5dvVYliX4dkmCDL3OpGXjHv9zgq4IzPHPogu2GH";

// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1):
String payload_base =  "{\"command\": \"append_row\", \"sheet_name\": \"Data\", \"values\": ";
String payload = "";
//script.google.com/macros/s/AKfycbya7jbH_t8dQGUlZVRRynW1Mx7uR5dvVYliX4dkmCDL3OpGXjHv9zgq4IzPHPogu2GH/exec
// Google Sheets setup (do not edit)
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;

// Declare variables that will be published to Google Sheets
int value0 = 0;
int value1 = 0;
//int value2 = 0;
// Declare variables that will keep track of whether or not the data has been published
bool data_published = false;
int error_count = 0;
int Pieso = D6;

void setup() {

  Serial.begin(115200);        
  delay(10);
  Serial.println('\n');
  
  wg.begin(D1,D2);
  pinMode(Pieso,OUTPUT);
  digitalWrite(Pieso, HIGH);
  // Connect to WiFi
  WiFi.begin(ssid, password);             
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
     digitalWrite(Pieso, LOW);
     delay(50);
     digitalWrite(Pieso, HIGH);
     delay(25);
     digitalWrite(Pieso, LOW);
     delay(50);
     digitalWrite(Pieso, HIGH);
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){ 
    int retval = client->connect(host, httpsPort);
    if (retval == 1){
       flag = true;
       Serial.println("Connected");
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    return;
  }
  delete client;    // delete HTTPSRedirect object
  client = nullptr; // delete HTTPSRedirect object
}


void loop() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
     digitalWrite(Pieso, LOW);
     delay(50);
     digitalWrite(Pieso, HIGH);
     delay(25);
     digitalWrite(Pieso, LOW);
     delay(50);
     digitalWrite(Pieso, HIGH);
  }
   
    if(wg.available())
  {
    //Serial.print("Wiegand HEX = ");
    //Serial.print(wg.getCode(),HEX);
    //Serial.print(", DECIMAL = ");
    //Serial.print(wg.getCode());
    //Serial.print(", Type W");
    //Serial.println(wg.getWiegandType());    
    value0 = wg.getCode();

    // before attempting to publish to Google Sheets, set the data_published variable to false and error_count to 0
    data_published = false;
    error_count = 0;
    
    // the while loop will attempt to publish data up to 3 times
    while(data_published == false && error_count < 3){

      static bool flag = false;
      if (!flag){
        client = new HTTPSRedirect(httpsPort);
        client->setInsecure();
        flag = true;
        client->setPrintResponseBody(true);
        client->setContentTypeHeader("application/json");
      }
      if (client != nullptr){
        if (!client->connected()){
          client->connect(host, httpsPort);
        }
      }
      else{
        Serial.println("Error creating client object!");
      }

      // Create json object string to send to Google Sheets
      payload = payload_base + "\"" + value0 +  "," + value1 + "\"}"; //"," + value2 + "\"}";
      Serial.println(url);
      Serial.println(host);
      Serial.println(payload);
      // Publish data to Google Sheets
      //Serial.println("Publishing data...");
      //Serial.println(payload);
    
      if(client->POST(url, host, payload)){ 
        data_published = true;
       digitalWrite(Pieso, LOW);
       delay(100);
       digitalWrite(Pieso, HIGH);
       delay(100);
       digitalWrite(Pieso, LOW);
       delay(100);
       digitalWrite(Pieso, HIGH);
      }
      else{
       // do stuff here if publish was not successful
       Serial.println("Error while connecting");

       digitalWrite(Pieso, LOW);
       delay(1000);
       digitalWrite(Pieso, HIGH);
       error_count++;
      }
      yield();
    } 
  }
}
