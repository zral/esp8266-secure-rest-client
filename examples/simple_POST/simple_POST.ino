#include <ESP8266WiFi.h>
#include <WiFiRestClient.h>

const char* ssid = "";    // your network SSID (name)
const char* pass = "";    // your network password

const char* host = "arduino-http-lib-test.herokuapp.com";
const char* route = "/data";

void setup() {

  Serial.begin( 115200 );

  connectToWifi();  

  WiFiRestClient restClient( host );

  Serial.print( "Posting to http://" );
  Serial.print( host );
  Serial.println( route );

  String response;
  int statusCode = restClient.post( route, "POSTDATA", &response );
  
  Serial.print( statusCode );
  Serial.print( " " );
  Serial.println( response );
}

void loop() {
}

void connectToWifi() {

  Serial.println( "Attempting to connect to WPA network" );
  Serial.print( "SSID: ");
  Serial.println( ssid );

  WiFi.begin( ssid, pass );
  while( WiFi.status() != WL_CONNECTED ) {
      delay( 500 );
      Serial.print( "." );
  }

  Serial.println();
  Serial.println( "Connected to wifi" );
}

