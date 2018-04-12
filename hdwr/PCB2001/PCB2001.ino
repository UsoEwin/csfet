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
#define fan_pin 13
#define heater_pin 6
#define tag 2001
#define delaytime 5000 //milliseconds

#define num_average 50
#define alpha 0.15
#define LLlength 8
#define ratio 100.00

#define RECOVERY 0
#define BASELINE 1
#define INCREASE 2

#define baseline_counter_threshold 2000 // 2000*1.4 seconds
#define heater_counter_threshold 15 //

int base_threshold_1 = 80; //experimental value (variable)
int inc_threshold_1 = 70; //experimental value (variable)
int rec_threshold_1 = 500; //experimental value (variable)
bool if_channel_1_inc = false;

int base_threshold_2 = 80; //experimental value (variable)
int inc_threshold_2 = 70; //experimental value (variable)
int rec_threshold_2 = 500; //experimental value (variable)
bool if_channel_2_inc = false;

int base_threshold_3 = 80; //experimental value (variable)
int inc_threshold_3 = 70; //experimental value (variable)
int rec_threshold_3 = 500; //experimental value (variable)
bool if_channel_3_inc = false;

int base_threshold_4 = 80; //experimental value (variable)
int inc_threshold_4 = 70; //experimental value (variable)
int rec_threshold_4 = 500; //experimental value (variable)
bool if_channel_4_inc = false;

char ssid[] = "NETGEAR45";      //  your network SSID (name)
char pass[] = "modernboat463";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int i = 0;
/*Sensor pin define*/
String dataline="";
unsigned long data_index = 0;

double d1 = NAN;
double pre_d1;
double d2 = NAN;
double pre_d2;
double d3 = NAN;
double pre_d3;
double d4 = NAN;
double pre_d4;

int s1;
int s2;
int s3;
int s4;

char FSM_status = RECOVERY;
char lock_flag = 1;
int heater_counter = 0;
int baseline_counter = 0;

double delta1[LLlength];
double LL1[LLlength];
double* p_delta_start1 = delta1;
double* p_delta_end1 = delta1;
double* p_LL_start1 = LL1;
double* p_LL_end1 = LL1;
double delta_sum1 = 0.0;
double pre_delta1 = 0.0;
double LL_sum1 = 0.0;
double pre_LL1 = 0.0;
double LL_sum1_diff = 0.0;

double delta2[LLlength];
double LL2[LLlength];
double* p_delta_start2 = delta2;
double* p_delta_end2 = delta2;
double* p_LL_start2 = LL2;
double* p_LL_end2 = LL2;
double delta_sum2 = 0.0;
double pre_delta2 = 0.0;
double LL_sum2 = 0.0;
double pre_LL2 = 0.0;
double LL_sum2_diff = 0.0;

double delta3[LLlength];
double LL3[LLlength];
double* p_delta_start3 = delta3;
double* p_delta_end3 = delta3;
double* p_LL_start3 = LL3;
double* p_LL_end3 = LL3;
double delta_sum3 = 0.0;
double pre_delta3 = 0.0;
double LL_sum3 = 0.0;
double pre_LL3 = 0.0;
double LL_sum3_diff = 0.0;

double delta4[LLlength];
double LL4[LLlength];
double* p_delta_start4 = delta4;
double* p_delta_end4 = delta4;
double* p_LL_start4 = LL4;
double* p_LL_end4 = LL4;
double delta_sum4 = 0.0;
double pre_delta4 = 0.0;
double LL_sum4 = 0.0;
double pre_LL4 = 0.0;
double LL_sum4_diff = 0.0;

unsigned long pre_time;
unsigned long cur_time = 0;

unsigned long offline_counter = 0;

int status = WL_IDLE_STATUS;
WiFiServer server(80);
void setup() {
  Serial.begin(9600);      // initialize serial communication
  Serial.print("Start Serial ");
  pinMode(fan_pin, OUTPUT);      // set the LED pin mode
  pinMode(heater_pin, OUTPUT);
  digitalWrite(heater_pin, LOW);
  // Check for the presence of the shield
  Serial.print("WiFi101 shield: ");

  // setup for the DAC
  analogWrite(A0,110);
  
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("NOT PRESENT");
    return; // don't continue
  }
  Serial.println("DETECTED");
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    digitalWrite(fan_pin, LOW);
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);
    //digitalWrite(fan_pin, HIGH);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 3 seconds for connection:
    delay(delaytime);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}
void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
  
  if (client) {                             // if you get a client,
    offline_counter = 0;
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
            s1 = 0;
            s2 = 0;
            s3 = 0;
            s4 = 0;
            delay(20);
            for (i=0;i<num_average;i++) {
              s1 = s1+analogRead(A1);
              s2 = s2+analogRead(A2);
              s3 = s3+analogRead(A3);
              s4 = s4+analogRead(A4);
            }
            delay(20);
            s1 = s1/num_average;
            s2 = s2/num_average;
            s3 = s3/num_average;
            s4 = s4/num_average;
            pre_d1 = d1;
            pre_d2 = d2;
            pre_d3 = d3;
            pre_d4 = d4;
            
            pre_time = cur_time;
            d1 = (isnan(d1)) ? s1: s1*alpha+(1-alpha)*d1;
            d2 = (isnan(d2)) ? s2: s2*alpha+(1-alpha)*d2;
            d3 = (isnan(d3)) ? s3: s3*alpha+(1-alpha)*d3;
            d4 = (isnan(d4)) ? s4: s4*alpha+(1-alpha)*d4;
            cur_time = millis();

            Serial.println("Delta-Time = " + String(cur_time-pre_time));

            dataline = String(tag)+ " "+String(data_index) +" "+ String(s1)+" "+ String(d1) +" "+ String(s2)+" "+ String(d2) +" "+ String(s3)+" "+ String(d3)+" "+ String(s4)+" "+ String(d4)+" ";
            if (!isnan(pre_d1)) {
              *p_LL_end1 = sqrt(pow((cur_time - pre_time)/1000.00,2) + pow(ratio*(d1-pre_d1),2));
              *p_LL_end2 = sqrt(pow((cur_time - pre_time)/1000.00,2) + pow(ratio*(d2-pre_d2),2));
              *p_LL_end3 = sqrt(pow((cur_time - pre_time)/1000.00,2) + pow(ratio*(d3-pre_d3),2));
              *p_LL_end4 = sqrt(pow((cur_time - pre_time)/1000.00,2) + pow(ratio*(d4-pre_d4),2));
              
              //Serial.print(String(sqrt(pow((cur_time - pre_time),2) + pow(ratio*(d1-pre_d1),2)))+" Here is it\n");
              LL_sum1_diff = *p_LL_end1 - pre_LL1;
              LL_sum1 = LL_sum1 + LL_sum1_diff;
              LL_sum2_diff = *p_LL_end2 - pre_LL2;
              LL_sum2 = LL_sum2 + LL_sum2_diff;
              LL_sum3_diff = *p_LL_end3 - pre_LL3;
              LL_sum3 = LL_sum3 + LL_sum3_diff;
              LL_sum4_diff = *p_LL_end4 - pre_LL4;
              LL_sum4 = LL_sum4 + LL_sum4_diff;
              
              p_LL_end1 = LL1 + (((p_LL_end1-LL1)+1) % LLlength);
              p_LL_end2 = LL2 + (((p_LL_end2-LL2)+1) % LLlength);
              p_LL_end3 = LL3 + (((p_LL_end3-LL3)+1) % LLlength);
              p_LL_end4 = LL4 + (((p_LL_end4-LL4)+1) % LLlength);
              
              *p_delta_end1 = ratio*(d1-pre_d1);
              *p_delta_end2 = ratio*(d2-pre_d2);
              *p_delta_end3 = ratio*(d3-pre_d3);
              *p_delta_end4 = ratio*(d4-pre_d4);
              
              delta_sum1 = delta_sum1 + *p_delta_end1 - pre_delta1;
              delta_sum2 = delta_sum2 + *p_delta_end2 - pre_delta2;
              delta_sum3 = delta_sum3 + *p_delta_end3 - pre_delta3;
              delta_sum4 = delta_sum4 + *p_delta_end4 - pre_delta4;

              p_delta_end1 = delta1 + (((p_delta_end1-delta1)+1) % LLlength);
              p_delta_end2 = delta2 + (((p_delta_end2-delta2)+1) % LLlength);
              p_delta_end3 = delta3 + (((p_delta_end3-delta3)+1) % LLlength);
              p_delta_end4 = delta4 + (((p_delta_end4-delta4)+1) % LLlength);
              
              //Serial.print("LL_pointer_end = " + String(p_LL_end-LL) + "\n");
              //Serial.print("delta_pointer_end = " + String(p_delta_end-delta) + "\n");
              if (p_LL_end1 == p_LL_start1) {
                pre_LL1 = *p_LL_start1;
                pre_LL2 = *p_LL_start2;
                pre_LL3 = *p_LL_start3;
                pre_LL4 = *p_LL_start4;
                
                p_LL_start1 = LL1 + (((p_LL_start1-LL1)+1) % LLlength);
                p_LL_start2 = LL2 + (((p_LL_start2-LL2)+1) % LLlength);
                p_LL_start3 = LL3 + (((p_LL_start3-LL3)+1) % LLlength);
                p_LL_start4 = LL4 + (((p_LL_start4-LL4)+1) % LLlength);
                
                pre_delta1 = *p_delta_start1;
                pre_delta2 = *p_delta_start2;
                pre_delta3 = *p_delta_start3;
                pre_delta4 = *p_delta_start4;
                
                p_delta_start1 = delta1 + (((p_delta_start1-delta1)+1) % LLlength);
                p_delta_start2 = delta2 + (((p_delta_start2-delta2)+1) % LLlength);
                p_delta_start3 = delta3 + (((p_delta_start3-delta3)+1) % LLlength);
                p_delta_start4 = delta4 + (((p_delta_start4-delta4)+1) % LLlength);
                //Serial.print("LL_pointer_end = " + String(p_LL_start-LL)+ "\n");
                //Serial.print("delta_pointer_end = " + String(p_delta_start-delta)+ "\n");
                
                dataline += String(LL_sum1) + " " + String(delta_sum1) + " " + String(LL_sum2) + " " + String(delta_sum2)+ " " + String(LL_sum3) + " " + String(delta_sum3) + " " + String(LL_sum4) + " " + String(delta_sum4);
              } else {
                dataline += "0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0";
              }
            } else {
              dataline += "0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0";
            }
            
            Serial.print("FSM_status=");
            switch(FSM_status) {
              case RECOVERY:
                Serial.println("RECOVERY\n");
                if (heater_counter > heater_counter_threshold) {
                  heater_counter = 0;
                  digitalWrite(heater_pin, LOW);
                  Serial.println("Heater is OFF");
                  lock_flag = 1;
                } else if (heater_counter > 0) {
                  heater_counter++;
                }
                if (LL_sum1 <= base_threshold_1 &&
                    LL_sum2 <= base_threshold_2 &&
                    LL_sum3 <= base_threshold_3 && 
                    LL_sum3 <= base_threshold_3 &&
                    lock_flag) {
                  FSM_status = BASELINE;
                  baseline_counter = 0;
                }
                break;
              case BASELINE:
                Serial.println("BASELINE\n");
                baseline_counter++;
                if_channel_1_inc = (LL_sum1_diff > inc_threshold_1);
                if_channel_2_inc = (LL_sum2_diff > inc_threshold_2);
                if_channel_3_inc = (LL_sum3_diff > inc_threshold_3);
                if_channel_4_inc = (LL_sum4_diff > inc_threshold_4);
                if (if_channel_1_inc || if_channel_2_inc || if_channel_3_inc || if_channel_4_inc) {
                  FSM_status = INCREASE;
                } else if (baseline_counter > baseline_counter_threshold) {
                  FSM_status = RECOVERY;
                  heater_counter = 1;
                  lock_flag = 0;
                  digitalWrite(heater_pin, HIGH);
                  Serial.println("Heater is ON");
                }
                break;
              case INCREASE:
                Serial.println("INCREASE\n");
                if (((abs(LL_sum1-delta_sum1) > rec_threshold_1) && if_channel_1_inc) ||
                    ((abs(LL_sum2-delta_sum2) > rec_threshold_2) && if_channel_2_inc) ||
                    ((abs(LL_sum3-delta_sum3) > rec_threshold_3) && if_channel_3_inc) ||
                    ((abs(LL_sum4-delta_sum4) > rec_threshold_4) && if_channel_4_inc)) {
                  FSM_status = RECOVERY;
                  heater_counter = 1;
                  digitalWrite(heater_pin, HIGH);
                  Serial.println("Heater is ON");
                }
                break;
            }
            if (heater_counter) {
              dataline += " 1";
            } else {
              dataline += " 0";
            }
            if (FSM_status == RECOVERY) {
              dataline += " "+String(RECOVERY);
            } else if (FSM_status == INCREASE) {
              dataline += " "+String(INCREASE);
            } else if (FSM_status == BASELINE) {
              dataline += " "+String(BASELINE);
            }
            
            Serial.println(dataline);

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
    } //END OF WHILE loop   
    client.stop();
    Serial.println("client disonnected");
    //if client is disconnected, turn to initial state
  } else {
    offline_counter++;
    //if client is disconnected, turn to initial state, delay more time
    if (offline_counter == 400) {
      Serial.println("disconnected from client\n\n\n\n\n\n");
      FSM_status = RECOVERY;
      heater_counter = 0;
      baseline_counter = 0;
      digitalWrite(heater_pin, LOW);
      lock_flag = 1;
      data_index = 0;
      d1 = NAN;
      d2 = NAN;
      d3 = NAN;
      d4 = NAN;
      cur_time = 0;

      p_delta_start1 = delta1;
      p_delta_end1 = delta1;
      p_LL_start1 = LL1;
      p_LL_end1 = LL1;
      delta_sum1 = 0.0;
      pre_delta1 = 0.0;
      LL_sum1 = 0.0;
      pre_LL1 = 0.0;
      LL_sum1_diff = 0.0;

      p_delta_start2 = delta2;
      p_delta_end2 = delta2;
      p_LL_start2 = LL2;
      p_LL_end2 = LL2;
      delta_sum2 = 0.0;
      pre_delta2 = 0.0;
      LL_sum2 = 0.0;
      pre_LL2 = 0.0;
      LL_sum2_diff = 0.0;

      p_delta_start3 = delta3;
      p_delta_end3 = delta3;
      p_LL_start3 = LL3;
      p_LL_end3 = LL3;
      delta_sum3 = 0.0;
      pre_delta3 = 0.0;
      LL_sum3 = 0.0;
      pre_LL3 = 0.0;
      LL_sum3_diff = 0.0;

      p_delta_start4 = delta4;
      p_delta_end4 = delta4;
      p_LL_start4 = LL4;
      p_LL_end4 = LL4;
      delta_sum4 = 0.0;
      pre_delta4 = 0.0;
      LL_sum4 = 0.0;
      pre_LL4 = 0.0;
      LL_sum4_diff = 0.0;
    }
    if (offline_counter >= 400) {
      delay(delaytime);
    }
  }
  delay(10);
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
