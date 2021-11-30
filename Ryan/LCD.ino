#include <LiquidCrystal.h>
#include <WiFi.h>

// LCD pins <--> Arduino pins
const int RS = 6, EN = 7, D4 = 8, D5 = 9, D6 = 10, D7 = 11;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

int Max = 10;                   //Setting max number of occupants
int Current = 0;                //Setting current number of occupants
bool WiFiStatus = false;        //Setting default WiFi Status
bool NPTStatus = false;         //Setting default NPT Status
bool AppStatus = false;         //Setting default App Inventor Status
bool ServerStatus = false;      //Setting default Server Status
String Entry = "";              //Setting default Entry text

void setup()
{
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

void loop()
{
//Set status indicators

  setLCD(1,0,1,0,Current,Max,"Test");
}
