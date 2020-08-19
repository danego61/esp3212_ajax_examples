#include <WiFi.h>  
#include <Wire.h>
#include "index.h"
#include "Adafruit_HTU21DF.h"
/*
 Hello 
 I design developer tools for embedded electronic systems. You can buy my projects design files.
 https://www.altiumdesignerprojects.com
*/
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

const char* ssid     = "TP-LINK-MCU";     
const char* password = "15253545";  

WiFiServer server(80);

String convert_char = index_page; 

String HTTP_req; 

void setup(){
  Serial.begin(115200);
  
  if (!htu.begin()) { Serial.println("Couldn't find sensor!"); }
  
  delay(10);
  Serial.println();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
 
WiFiClient client = server.available();   // listen for incoming clients

if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                
                if (c == '\n' && currentLineIsBlank) {
                    // remainder of header follows below, depending on if
                    // web page or XML page is requested
                    // Ajax request - send XML file
                    if (HTTP_req.indexOf("request") > -1) {
                        // send rest of HTTP header
                        client.println("HTTP/1.1 200 OK");                        
                        client.println("Content-Type: text/xml");
                        client.println("Connection: keep-alive");
                        client.println();
                        // send XML file containing input states
                        XML_response(client);
                    }
                    else {  // web page request

                        client.println(convert_char); 

                    }
                    // display received HTTP request on serial port
                    Serial.println(HTTP_req);
                    HTTP_req = "";            // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)

}

// send the XML file with switch statuses and analog value
void XML_response(WiFiClient cl)
{

  char temp_text[8];
  char hum_text[8];
  
  float temp = htu.readTemperature();
  float hum = htu.readHumidity();
  
  dtostrf(temp,4,2,temp_text);  
  strcat(temp_text," C");
  
  dtostrf(hum,4,2,hum_text);  
  strcat(hum_text," %");
  
  cl.println("<?xml version = \"1.0\" ?>");
  cl.println("<inputs>");  
  cl.println("<temp>");
  cl.println(temp_text);
  cl.println("</temp>");
  cl.println("<hum>");
  cl.println(hum_text);
  cl.println("</hum>");  
  cl.println("</inputs>");

  
}
