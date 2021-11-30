void printConnectionStatus(long int ipAddress, String ssid, long rssi){
  // print connection status
  Serial.println("Connected");
  Serial.print("IP Address: ");
  Serial.print(ipAddress % 256);
  Serial.print(".");
  Serial.print(ipAddress / 256 % 256);
  Serial.print(".");
  Serial.print(ipAddress / 256 / 256 % 256);
  Serial.print(".");
  Serial.println(ipAddress / 256 / 256 / 256);
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
}

String print2digits(int number) {
  String returnChar = "";
  if (number < 10) {
    returnChar += "0";
  }
  returnChar += number;
  return(returnChar);
}

void printRTC()
{
  // Print date...
  Serial.print(rtc.getDay());
  Serial.print("/");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  Serial.print(rtc.getYear());
  Serial.print("\t");
  // ...and time
  Serial.print(print2digits(rtc.getHours()));
  Serial.print(":");
  Serial.print(print2digits(rtc.getMinutes()));
  Serial.print(":");
  Serial.print(print2digits(rtc.getSeconds()));
  Serial.println(); 
}

void getTimeFromNTP()
{
  // initialize NTP
  timeClient.update();
  epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);  
}

void timeKeepingSetup(){
  timeClient.begin(); // start NTP
  timeClient.setTimeOffset(0); // GMT 0
  getTimeFromNTP(); // get NTP epoch
  rtc.begin(); // start rtc
  timeClient.update(); // update NTP epoch
  rtc.setEpoch(epochTime); // set rtc to NTP epoch
  NPTStatus = true;
  printRTC(); // print rtc
}

void lcdStatusSetup(){
  lcd.clear();
  lcd.setCursor(0, 0);          // move cursor to   (0, 0)
  lcd.print("W:  ");              // WiFi placeholder (0, 0)
  lcd.setCursor(4, 0);          // move cursor to   (4, 0)
  lcd.print("N:  ");              // NPT placeholder (4, 0)
  lcd.setCursor(8, 0);          // move cursor to   (8, 0)
  lcd.print("T:  ");              // Timer placeholder (8, 0)
  lcd.setCursor(12, 0);         // move cursor to   (12, 0)
  lcd.print("U:  ");              // Server placeholder (12, 0)
  lcd.setCursor(13, 1);         // move cursor to   (13, 1)
  lcd.print("/");               // "/" placeholder (13, 1)
}


void setLCD(bool WiFiStatus, bool NPTStatus, bool AppStatus, bool uploadStatus, int Current, int Max, String Entry) //
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
  if(uploadStatus)               //Server status      (14, 0)
    {  
      lcd.write("Y");               
    }
  else
    {
      lcd.write("N");
    }
  lcd.setCursor(14,1);            //move cursor to  (14,1)
  lcd.print(print2digits(Max));   //max number of occupants

  lcd.setCursor(11,1);              //move cursor to  (12,1)
  lcd.print(print2digits(Current)); //current number of occupants

  lcd.setCursor(0,1);             //move cursor to (2,1)
  lcd.print(Entry);               //Display text
}

void updateLCDDisplayText(){
  lcd.setCursor(0,1);             //move cursor to (2,1)
  lcd.print(lcdText);             //Display text
}

void updateUploadStatusText(){
  lcd.setCursor(14,0);             //move cursor to (2,1)
  if(uploadStatus)               //Server status      (14, 0)
    {  
      lcd.write("Y");               
    }
  else
    {
      lcd.write("N");
    }
}

void updateTimerStatusText(){
  lcd.setCursor(10,0);             //move cursor to (2,1)
  if(TimerStatus)               //Server status      (14, 0)
    {  
      lcd.write("Y");               
    }
  else
    {
      lcd.write("N");
    }
}

void waitLoop(){
  for(int x = 0; x < timeAfterSuccessfulDetection; x++)
  {
    Serial.print("waiting...");
    lcd.print(".");         
    delay(1000);
  }
  lcdText = "  READY   "; // length 10
  updateLCDDisplayText();
  Serial.println("");
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
