// includes
#include "arduino_secrets.h"
#include <NTPClient.h>
#include <RTCZero.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#define ifrSensor1 4
#define ifrSensor2 5

// LCD pins <--> Arduino pins
const int RS = 6, EN = 7, D4 = 8, D5 = 9, D6 = 10, D7 = 11;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

                   
int Current = 0;                //Setting current number of occupants
bool WiFiStatus = false;        //Setting default WiFi Status
bool NPTStatus = false;         //Setting default NPT Status
bool AppStatus = false;         //Setting default App Inventor Status
bool ServerStatus = false;      //Setting default Server Status
String Entry = "";              //Setting default Entry text

// DEBUG
bool DEBUG = true;
// networking vars
int status = WL_IDLE_STATUS;      //connection status
//enter network data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // network SSID (name)
char pass[] = SECRET_PASS;    // network password (use for WPA, or use as key for WEP)
// timekeeping vars
WiFiUDP ntpUDP;
WiFiServer server(80);
NTPClient timeClient(ntpUDP, "pool.ntp.org");
RTCZero rtc;
unsigned long epochTime;
// algorithm vars
bool ifrSensor1state = false;
bool ifrSensor2state = false;
bool previfrSensor1state = false;
bool previfrSensor2state = false;
bool weAreTiming1;
bool weAreTiming2;
bool completedTiming1;
bool completedTiming2;
unsigned long firstEventAt;
unsigned long secondEventAt;
unsigned long timeBetweenEvents;
String html = "";
int counter = 0;
int roomUpperLimit = 5; //Setting max number of occupants
int roomLowerLimit = 0; 
int timeBetweenSensorEvents = 500; // milliseconds
int timeAfterSuccessfulDetection = 3; // seconds
// function_lib.h must be declared after var defininition
#include "function_lib.h"

void setup() {

  // wait for serial session to begin
  Serial.begin(9600);
  while (!Serial) {;/*wait for serial port to connect*/}
  if(DEBUG){Serial.println("Serial comms opened");}

  // IO pin setup
  pinMode(ifrSensor1, INPUT);
  pinMode(ifrSensor2, INPUT);
  if(DEBUG){Serial.println("Sensor pinmode set");}
     
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

  // start server and show connection status
  server.begin(); 
  printConnectionStatus(WiFi.localIP(),WiFi.SSID(),WiFi.RSSI());
  
  // date & time keeping setup
  timeKeepingSetup();

  lcd.begin(16, 2); // set up number of columns and rows

  //Set placeholders for indicators
  lcd.setCursor(0, 0);          // move cursor to   (0, 0)
  lcd.print("W:");              // WiFi placeholder (0, 0)

  lcd.setCursor(4, 0);          // move cursor to   (4, 0)
  lcd.print("N:");              // NPT placeholder (4, 0)

  lcd.setCursor(8, 0);          // move cursor to   (8, 0)
  lcd.print("A:");              // App placeholder (8, 0)

  lcd.setCursor(12, 0);         // move cursor to   (12, 0)
  lcd.print("S:");              // Server placeholder (12, 0)

  lcd.setCursor(12, 1);         // move cursor to   (13, 1)
  lcd.print("/");               // "/" placeholder (13, 1)


  // setup interrupts
  attachInterrupt(digitalPinToInterrupt(ifrSensor1), ifr1interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ifrSensor2), ifr2interrupt, CHANGE);
  ifr1interrupt();
  ifr2interrupt();
  if(DEBUG){Serial.println("Interrupts set");}

  setLCD(1,1,0,1,counter,roomUpperLimit,"Test");
  if(DEBUG){Serial.println("7 Seg");}
  
}

void loop() {

  // disable interrupts to stop states changing during comparing values
  detachInterrupt(digitalPinToInterrupt(ifrSensor1));
  detachInterrupt(digitalPinToInterrupt(ifrSensor2));
  
  // if sensor 1 state change
  if(ifrSensor1state != previfrSensor1state)
  {
    
   // if sensor one is active and we are not timing any other event
   if ((ifrSensor1state == LOW) && !weAreTiming1 && !weAreTiming2)
      {
        // start timer 2
        weAreTiming2 = true;
        if(DEBUG){Serial.print(" start ");}
        firstEventAt = millis();
        if(DEBUG){Serial.println(firstEventAt);}
      }
    
    // if sensor one is active & the other timer is on & this timer is not on
    if((ifrSensor1state == LOW) && weAreTiming1 && !weAreTiming2)
      {
        // stop timer 1
        weAreTiming1 = false;
        if(DEBUG){Serial.print(" end   ");}
        secondEventAt = millis();
        if(DEBUG){Serial.println(secondEventAt);}
        timeBetweenEvents = secondEventAt - firstEventAt;
        if(DEBUG){Serial.print("  Time between events ");}
        if(DEBUG){Serial.println(timeBetweenEvents);}
        // timer 1 has completed a measurement flag
        completedTiming1 = true;  
      } 
  // debounce  
  delay(50);    
  }
  
  // if sensor 2 state change
  if(ifrSensor2state != previfrSensor2state)
  {
    // if sensor 2 is active and we are not timing any other event
    if ((ifrSensor2state == LOW) && !weAreTiming1 && !weAreTiming2)
      {
        // start timer 1
        weAreTiming1 = true;
        if(DEBUG){Serial.print(" start ");}
        firstEventAt = millis();
        if(DEBUG){Serial.println(firstEventAt);}
      }
    // if sensor two is active & the other timer is on & this timer is not on
    if((ifrSensor2state == LOW) && weAreTiming2 && !weAreTiming1)
      {
        // stop timer 2
        weAreTiming2 = false;
        if(DEBUG){Serial.print(" end   ");}
        secondEventAt = millis();
        if(DEBUG){Serial.println(secondEventAt);}
        timeBetweenEvents = secondEventAt - firstEventAt;
        if(DEBUG){Serial.print("  Time between events ");}
        if(DEBUG){Serial.println(timeBetweenEvents);}
        // timer 2 has completed a measurement flag
        completedTiming2 = true;  
      }
  // debounce     
  delay(50);
  }

  // if the measured time is within range
  if((timeBetweenEvents <= timeBetweenSensorEvents) && completedTiming1)
  {
    // allow timer 1 to be started again
    completedTiming1 = false;
    Serial.println("IN");
    if(DEBUG){printRTC();}
    countUp();
    writeToString(true);
    for(int x = 0; x < timeAfterSuccessfulDetection; x++)
      {
        Serial.print("waiting...");
        delay(1000);
      }
    Serial.println("");
  }

  // if the measured time is outside range
  if((timeBetweenEvents > timeBetweenSensorEvents) && completedTiming1)
  {
    // allow timer 1 to be started again
    completedTiming1 = false;
    Serial.println("time too long");
    for(int x = 0; x < timeAfterSuccessfulDetection; x++)
    {
      Serial.print("waiting...");
      delay(1000);
    }
    Serial.println("");
  }
  
    // if the measured time is within rang
    if((timeBetweenEvents <= timeBetweenSensorEvents) && completedTiming2)
    {
      // allow timer 2 to be started again
      completedTiming2 = false;
      Serial.println("OUT");
      if(DEBUG){printRTC();}
      countDown();
      writeToString(false);
      for(int x = 0; x < timeAfterSuccessfulDetection; x++)
        {
          Serial.print("waiting...");
          delay(1000);
        }
      Serial.println("");
  }

  // if the measured time is outside range
  if((timeBetweenEvents > timeBetweenSensorEvents) && completedTiming2)
  {
    completedTiming2 = false;
    Serial.println("time too long");
    for(int x = 0; x < timeAfterSuccessfulDetection; x++)
      {
        Serial.print("waiting...");
        delay(1000);
      }
    Serial.println("");
  }

  // if the measured time is outside range
  if(((millis() - firstEventAt) >= 1000) && (weAreTiming1 || weAreTiming2))
  {
    weAreTiming1 = false;
    weAreTiming2 = false;
    Serial.println("detection timeout");
    for(int x = 0; x < timeAfterSuccessfulDetection; x++)
      {
        Serial.print("waiting...");
        delay(1000);
      }
    Serial.println("");
  }

  if(counter >= roomUpperLimit)
  {
    Serial.println("ROOM FULL");
    delay(10);
  }

// remember last state
previfrSensor1state = ifrSensor1state;
previfrSensor2state = ifrSensor2state;

// update displays
hostWebsite();
setLCD(1,1,0,1,counter,roomUpperLimit,"Test");

// reinstate the interrupts to look for sensor change
attachInterrupt(digitalPinToInterrupt(ifrSensor1), ifr1interrupt, CHANGE);
attachInterrupt(digitalPinToInterrupt(ifrSensor2), ifr2interrupt, CHANGE); 
}

/* 
 * Interrupt function for ifrSensor1
 *  Retrives status of ifrSensor1 and returns it to the ifrSensor2state global 
*/  
void ifr1interrupt()
{
    ifrSensor1state = digitalRead(ifrSensor1);
}

/* 
 * Interrupt function for ifrSensor2
 *  Retrives status of ifrSensor2 and returns it to the ifrSensor2state global 
*/  
void ifr2interrupt()
{
    ifrSensor2state = digitalRead(ifrSensor2);
}

void hostWebsite() 
{
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) 
  {
    int clientFirstValue = millis();
    Serial.println("new client");
    // an http request ends with a blank line
    bool currentLineIsBlank = true;
    while (client.connected()) 
    {
      //Serial.println("conncted but not available");
      if(millis() - clientFirstValue >= 2000){break;}
      if (client.available()) 
      {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) 
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println(html);
          client.println("</html>");
          break;
        }
        if (c == '\n') 
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void countUp()
{
  if((counter + 1) <= roomUpperLimit)
  {
    counter++;
    Serial.println(counter);
  }
  else
  {
    Serial.println("Upper Limit");
  }
 
}

void countDown()
{
  if((counter - 1) >= roomLowerLimit)
  {
    counter--;
    Serial.println(counter);
  }
  else
  {
    Serial.println("Lower Limit");
  }  
}

void writeToString(bool detectionDirection)
{
  if(detectionDirection){
    html += String("IN") + String("\t");
  }
  else{
    html += String("OUT") + String("\t");
  }

  html += String(counter) + String("\t");
  
  html += String(rtc.getDay()) + "/" + String(rtc.getMonth()) + "/" + String(rtc.getYear()) + "\t";
  // Print date...
  html += String(print2digits(rtc.getHours())) + ":" + String(print2digits(rtc.getMinutes())) + ":" + String(print2digits(rtc.getSeconds())) + "<br>";  
}

void setLCD(bool WiFiStatus, bool NPTStatus, bool AppStatus, bool ServerStatus, int Current, int Max, String Entry) //
{
  lcd.setCursor(2, 0);          // move cursor to   (2, 0)
  if(WiFiStatus)                //WiFi status       (2, 0)
{  
  lcd.write("Y");               
  }
  else
{
    lcd.write("N");
  }
  
  lcd.setCursor(6, 0);          // move cursor to   (6, 0)
  if(NPTStatus)                 //NPT status        (6, 0)
{  
  lcd.write("Y");               
  }
  else
{
    lcd.write("N");
  }
  
  lcd.setCursor(10, 0);          // move cursor to   (10, 0)
  if(AppStatus)                  //App Inventor status(10, 0)
{  
  lcd.write("Y");               
  }
  else
{
    lcd.write("N");
  }

  lcd.setCursor(14, 0);          // move cursor to    (14, 0)
  if(ServerStatus)               //Server status      (14, 0)
{  
  lcd.write("Y");               
  }
  else
{
    lcd.write("N");
  }

  lcd.setCursor(14,1);            //move cursor to  (14,1)
  lcd.print(Max);                 //max number of occupants

  lcd.setCursor(10,1);            //move cursor to  (11,1)
  lcd.print(Current);             //current number of occupants

  lcd.setCursor(0,1);             //move cursor to (2,1)
  lcd.print(Entry);               //Display text
}
