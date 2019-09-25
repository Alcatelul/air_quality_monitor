

#include <SoftwareSerial.h>
#define RX 6
#define TX 5
String AP = "nokia";       // CHANGE ME
String PASS = "salomiaalin"; // CHANGE ME
String API = "DRYD8WT9SQHXMWWW";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "Adafruit_ST7789.h" // Hardware-specific library for ST7789 
#include <math.h> 
#include "MQ135.h"
#include "DHT.h"
#include "ThingSpeak.h"
#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetClient client;
// On Arduino:  0 - 1023 maps to 0 - 5 volts
#define VOLTAGE_MAX 5.0
#define VOLTAGE_MAXCOUNTS 1023.0

unsigned long myChannelNumber = 485883;
const char * myWriteAPIKey = "DRYD8WT9SQHXMWWW";

MQ135 gasSensor = MQ135(A0);
#define DHTPIN 3     // what digital pin we're connected to
#define beeper    4                       //beeper on Digital 4
#define sensor    A0                      //sensor on Analog 0
#define DHTTYPE DHT11   // DHT 11
#define sensor2    2
#define TFT_CS     9
#define TFT_RST   7  // you can also connect this to the Arduino reset
                       // in which case, set this #define pin to -1!
#define TFT_DC     8

#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
// Local Network Settings
int VCalibrata=0;
int gasLevel = 0;                         //int variable for gas level
String quality ="";


DHT dht(DHTPIN, DHTTYPE);

void setup() {
 
  
  Serial.begin(9600); //start serial comms
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  pinMode(beeper,OUTPUT);//set beeper for output
  pinMode(sensor2,INPUT);
  pinMode(sensor,INPUT);                  //set sensor for input
   tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  // testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
 // delay(1000);
  //lcd.begin(16, 2);                       //initial LCD setup
  //lcd.setCursor (0,0);                    //splash screen and warmup
  //lcd.print("                ");
  //lcd.setCursor (0,1);
  //lcd.print("                ");
  //lcd.setCursor (0,0);
  //lcd.print("   Air Sensor   ");
  //lcd.setCursor (0,1);
  //lcd.print("   Warming Up   ");
  //delay(2000);                            //set for at least 2 minutes

 // lcd.setCursor (0,0);                    //clear screen
 //lcd.print("                ");
  //lcd.setCursor (0,1);
  //lcd.print("                ");

}

void loop() {


  valSensor = getSensorData();
 String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 
int  gasLevel = analogRead(sensor);
   float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
   if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


   // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
 
 Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  if(gasLevel<175){
    quality = "GOOD!           ";
  }
  else if (gasLevel >175 && gasLevel<225){
    quality = "Did you fart?   ";
  }
  else if (gasLevel >225 && gasLevel<300){
    quality = "Something dead?";
  }
  else if (gasLevel >300){
    quality = "Pretty fk'n bad ";
       // digitalWrite(beeper, HIGH);       
       // delayMicroseconds(10);       
       // digitalWrite(beeper, LOW);       
       // delayMicroseconds(10);
  }
  float rzero = gasSensor.getRZero();
//lcd.setCursor (0,0);
///lcd.print("CO2 PPM is:");
//lcd.setCursor(0,1);
float ppm = gasSensor.getPPM();
float Cppm = gasSensor.getCorrectedPPM( t, h);
//lcd.print(Cppm);
tft.setCursor(0, 0);
tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
tft.println("CO2 PPM:");
tft.setCursor(0, 20);
tft.setTextColor(ST77XX_RED);
  tft.setTextSize(2);
tft.println(Cppm);
tft.setCursor(0, 30);
//x y,xf,yf
 tft.drawLine(5, 50, 110, 50, ST77XX_BLUE);//linie sus
  tft.drawLine(5, 70, 110, 70, ST77XX_BLUE);//linie jos
    tft.drawLine(5, 50, 5, 70, ST77XX_BLUE);//linie stg
  tft.drawLine(110, 50, 110, 70, ST77XX_BLUE);//linie dr
   tft.drawLine(Cppm/3, 50, Cppm/3, 70, ST77XX_BLUE);//linie dr
    for (int16_t y=0; y < Cppm/3; y+=1) {
    tft.drawLine(y, 50, y,70, ST77XX_BLUE);
  }
delay(1000);
  tft.setCursor(0, 0);
 tft.fillScreen(ST77XX_BLACK);

 // Ethernet.begin(mac);
 // ThingSpeak.begin(client);
// ThingSpeak.writeField(myChannelNumber, 1, Cppm, myWriteAPIKey);
}


void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

int getSensorData(){
  return random(1000); // Replace with 
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }

