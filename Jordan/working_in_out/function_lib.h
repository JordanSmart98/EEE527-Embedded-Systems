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
  printRTC(); // print rtc
}
