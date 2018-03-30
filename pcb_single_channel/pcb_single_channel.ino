#include <WiFi101.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiSSLClient.h>
#include <WiFiUdp.h>
 
/*
 * This example is modified from the original file 
 * https://github.com/arduino-libraries/WiFi101/blob/master/examples/SimpleWebServerWiFi/SimpleWebServerWiFi.ino
 */
#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "NETGEAR45";      //  your network SSID (name)
char pass[] = "modernboat463";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int fan_pin = 6;
int mux_a = 7;
int mux_b = 8;
int mux_c = 9;
int mux_inh = 10;
int heater_pin = 13;

int i = 0;
bool val = true;
/*Sensor pin define*/
String dataline="";
unsigned long data_index = 0;

int num_average = 20;

int s1;
int s2;
int s3;
int s4;


int status = WL_IDLE_STATUS;
WiFiServer server(80);
void setup() {
  Serial.begin(9600);      // initialize serial communication
  Serial.print("Start Serial ");
  pinMode(fan_pin, OUTPUT);      // set the LED pin mode
  pinMode(heater_pin, OUTPUT);
  pinMode(mux_inh, OUTPUT);
  pinMode(mux_a, OUTPUT);
  pinMode(mux_b, OUTPUT);
  pinMode(mux_c, OUTPUT);
  
  // Check for the presence of the shield
  Serial.print("WiFi101 shield: ");

  // setup for the DAC
  analogWrite(A0,64);

  // setup for the heater
  digitalWrite(heater_pin, LOW);

  // setup for the fan
  digitalWrite(fan_pin, LOW);
  
  // setup for the mux
  digitalWrite(mux_inh, LOW);
  digitalWrite(mux_a, LOW);
  digitalWrite(mux_b, LOW);
  digitalWrite(mux_c, LOW);
  
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("NOT PRESENT");
    return; // don't continue
  }
  Serial.println("DETECTED");
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);
    //digitalWrite(fan_pin, HIGH);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
  digitalWrite(fan_pin, HIGH);
  digitalWrite(heater_pin, HIGH);
}
void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
  
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    //digitalWrite(fan_pin,HIGH);  			// give the fan high signal once client is connected
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        
        if (c == '\n') {                    // if the byte is a newline character
            
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
 
            // the content of the HTTP response follows the header:
            //client.print("Click <a href=\"/H\">here</a> turn the Fan on pin 9 on<br>");
            //client.print("Click <a href=\"/L\">here</a> turn the Fan on pin 9 off<br>");
            //client.print("A test for print out data:");
            data_index = data_index+1;
            
            //read sensor1:
            s1 = 0;
            digitalWrite(mux_b, LOW);
            digitalWrite(mux_a, LOW);
            delay(3550);
            for (i=0;i<num_average;i++) {
              s1 = s1+analogRead(A1);
            }
            s1 = s1/num_average;
            delay(500);
            
            
            //read sensor2:
            s2 = 0;
            digitalWrite(mux_b, LOW);
            digitalWrite(mux_a, HIGH);
            delay(3500);
            for (i=0;i<num_average;i++) {
              s2 = s2+analogRead(A1);
            }
            s2 = s2/num_average;
            delay(500);
            

            //read sensor3:
            s3 = 0;
            digitalWrite(mux_b, HIGH);
            digitalWrite(mux_a, LOW);
            delay(3500);
            for (i=0;i<num_average;i++) {
              s3 = s3+analogRead(A1);
            }
            s3 = s3/num_average;
            delay(500);
            

            //read sensor4:
            s4 = 0;
            digitalWrite(mux_b, HIGH);
            digitalWrite(mux_a, HIGH);
            delay(3500);
            for (i=0;i<num_average;i++) {
              s4 = s4+analogRead(A1);
            }
            s4 = s4/num_average;
            delay(500);
            

            digitalWrite(mux_b, LOW);
            digitalWrite(mux_a, LOW);
            dataline =String(data_index) +" "+ String(s1)+" "+String(s2)+" "+String(s3)+" "+String(s4);
            Serial.print(dataline);

            client.print(dataline);
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
 
        // Check to see if the client request was "GET /H" or "GET /L":
      /*
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(fan_pin, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(fan_pin, LOW);                // GET /L turns the LED off
        }
        */
      }
    }    
    
    
    client.stop();
  
    Serial.println("client disonnected");
    //digitalWrite(fan_pin,LOW);
  }
}
 
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
 
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
 
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
