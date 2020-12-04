//LaunchControl_02.00
#include <WiFi.h>
#include "HX711.h"

const char* ssid     = "YammiesESP";
const char* password = "Buster01";
String header;

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 15;

bool staticTest = true;
bool Safety_1 = true;
bool Safety_2 = true;
bool Launch = false;
int IGN = 25;
unsigned long timeCounter = 0;
unsigned long timeInstance = 0;

long int force = 0;
long int delta = 0;
long int cal_Factor;
long int calF;
long int calT;
long int i;
int downstep = 200;
int failCount = 0;

HX711 scale;
WiFiServer server(80);

void setup() {
  pinMode(IGN,OUTPUT);
  digitalWrite(IGN,LOW);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 128);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   
  if (client) {                           
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        header += c;
        if (c == '\n') {    
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            //Gets Button States
            if (header.indexOf("GET /staticTest/ON") >= 0) {
              staticTest = true;}
            else if (header.indexOf("GET /staticTest/OFF") >= 0) {
              staticTest = false;}
            else if (header.indexOf("GET /Safety_1/ON") >= 0) {
              Safety_1 = true;}
            else if (header.indexOf("GET /Safety_1/OFF") >= 0) {
              Safety_1 = false;}
            else if (header.indexOf("GET /Safety_2/ON") >= 0) {
              Safety_2 = true;}
            else if (header.indexOf("GET /Safety_2/OFF") >= 0) {
              Safety_2 = false;}
            else if (header.indexOf("GET /Launch/LAUNCH") >= 0 && Safety_2==false && Safety_1==false) {
              Launch = true;}
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html{ font-family: Helvetica; margin: 0px auto; box-sizing: border-box; text-align: center;}");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".left, .right {width: 33%;}");
            client.println(".middle {width: 33%;}");
            client.println(".column {float: left; text-align: center;}");
            client.println(".button_test {background-color: #4CAF50; font-weight: bold; border: none; color: white; padding: 12px 20px;}");
            client.println(".button_safety {background-color: #4CAF50; font-size: 40; font-weight: bold; border: none; color: white; padding: 12px 20px;}");
            client.println(".button_launch {background-color: #666666; border: none; color: white; font: 60px; padding: 18px 40px;}</style></head>"); 
            
            //Title
            client.println("<body>");
            client.println("<h1 style=\"color: #13697E;\">LAUNCH CONTROL SYSTEM</h1>");

            
            client.println("<div>");
            client.println("<div class=\"column left\">");

            // Static Test
            client.println("<h2>Static Test</h2>");
            if (staticTest==true) {
              client.println("<p><a href=\"/staticTest/OFF\"><button class=\"button_test\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/staticTest/ON\"><button class=\"button button_test\" style =\"background-color: #666666;\">OFF</button></a></p>");
            }
            client.println("</div>");

            

            client.println("<div class =\"column middle\">");
            // Safety_1
            client.println("<h2>Safety 1</h2>");
            if (Safety_1==true) {
              client.println("<p><a href=\"/Safety_1/OFF\"><button class=\"button_safety\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Safety_1/ON\"><button class=\"button button_safety\" style =\"background-color: #FF0000;\">OFF</button></a></p>");
            }

            // Safety_2
            client.println("<h2>Safety 2</h2>");
            if (Safety_2==true) {
              client.println("<p><a href=\"/Safety_2/OFF\"><button class=\"button_safety\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Safety_2/ON\"><button class=\"button button_safety\" style =\"background-color: #FF0000;\">OFF</button></a></p>");
            }
            client.println("</div>");


            
            //Launch
            client.println("<div class =\"column right\">");
            timeInstance = millis();
            if(Launch == true && Safety_2==false && Safety_1==false && staticTest == false){
              client.println("<p><a href=\"/Launch/LAUNCH\"><button class=\"button_launch\" style =\"background-color: #FF0000;\">IGNITION</button></a></p>");
              digitalWrite(IGN,HIGH);
              while(true){
                timeCounter = millis();
                if(timeCounter >= timeInstance + 3000){
                  digitalWrite(IGN,LOW);
                  Launch = false;
                  Safety_1 = true, Safety_2 = true;
                  client.print("<HEAD>");
                  client.print("<meta http-equiv=\"refresh\" content=\"0;url=/\">");
                  client.print("</head>");
                  break;
                } 
                else{}
              }
            }
            else if(Launch == true && Safety_2==false && Safety_1==false && staticTest == true){
              client.println("<p><a href=\"/Launch/LAUNCH\"><button class=\"button_launch\" style =\"background-color: #FF0000;\">IGNITION</button></a></p>");
              digitalWrite(IGN,HIGH);
              client.println("Static Launch");
              while(true){
                timeCounter = millis();
                force = ((scale.read()/downstep) - cal_Factor);
                client.println(force);
                if(timeCounter >= timeInstance + 10000){
                  digitalWrite(IGN,LOW);
                  Launch = false;
                  Safety_1 = true, Safety_2 = true;
                  //client.print("<HEAD>");
                  //client.print("<meta http-equiv=\"refresh\" content=\"0;url=/\">");
                  //client.print("</head>");
                  break;
                } 
                else{}
              }
            }


            if (Safety_2==false && Safety_1==false && staticTest == true) {
              client.println("<p><a href=\"/Launch/LAUNCH\"><button class=\"button_launch\" style =\"background-color: #FF0000;\">LAUNCH</button></a></p>");
              client.println("Calibrating:");
              cal_Factor = scaleCal();
              client.println(cal_Factor);
              client.println("Reading:");
              force = scale.read()/downstep - cal_Factor;
              client.println(force);
            } 
            else if(Safety_2==false && Safety_1==false && staticTest == false) {
              client.println("<p><a href=\"/Launch/LAUNCH\"><button class=\"button_launch\" style =\"background-color: #FF0000;\">LAUNCH</button></a></p>");
            }
            else {
              client.println("<p><button class=\"button button_launch\">LAUNCH</button></p>");
            }
            client.println("</div>");


            // End stuff
            client.println("</body></html>");    
            client.println();
            break;
          } 
          else {
            currentLine = "";
          }
      }
        else if (c != '\r'){
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
  }
}
int scaleCal(){
  calF = 0;
  i = 0;
  while(i < 20 or failCount < 50){
    if (scale.is_ready()) {
      calT = scale.read()/downstep;
      calF = calT + calF;
      i ++;
      failCount = 0;
      }
    else{
      failCount++;
    }
  }
  calF = calF / i;
  return calF;
}
