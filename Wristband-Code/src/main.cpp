#include "Arduino.h"
#include <string.h>
#include <Wire.h>
#include "MAX30105.h"
#include "analogWrite.h"
#include "heartRate.h"
#include <BLEDevice.h>
//#include <BLEUtils.h>
//#include <BLEScan.h>
//#include <BLEAdvertisedDevice.h>
#include <BLECast.h>
#include "PubSubClient.h" 
#include "WiFi.h"


//STARTING VARIABLES ========================================================================================
    int id = 2;
    std::string espNaam = "esp4";

    /*
    esp1 : ID = 3
    esp2 : ID = 4
    esp3 : ID = 1
    esp4 : ID = 2

    CODE = 3412
    */



    
    int codetime =5000;                                     // #ms tussen de knipperende esps
    int codetimelong =60000;                                     // #ms tussen de codes
    const unsigned long switchPeriod = 600000;                //switch voor partnerruil
    int grenswaardeTeVer  = 60;   
    int grenswaardeTeDicht = 40; 
    int irThreshold = 60000;                                  // Treshold voor de hartslagsensor
    int partnerTime = 30;                                      //aantal seconden om je partner te vinden
    int hartslagFoutenMax = 5;

    const unsigned long period = 1000;                        // #ms tussen het scannen van de andere ESP'S




    const char *cEspNaam = espNaam.c_str();
    BLECast bleCast(espNaam);            //naam ESP
    int codePeriod = 10000000;
    bool checkvolgorde = false;
    std::string doorstuurCode ="Wristband-code 3412";


    
    int foutenMarge =10;
    int foutCounter = 0;
    int codetimelongcounter =0;
    int ronde = 0;
    
    bool oneTime = false;
    bool wristbandEnable;

  


//VARIABLES WIFI =================================================================
#define SSID          "NETGEAR68"
#define PWD           "excitedtuba713"

#define MQTT_SERVER   "192.168.1.2"
#define MQTT_PORT     1883

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


//VARIABLES HARTSLAGSENSOR=========================================================
MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

bool hartratesensorEnable;
int hartslagFoutenTeller;

//Constants RGB LED ===============================================================
const int PIN_RED   = 16;
const int PIN_BLUE = 4;
const int PIN_GREEN  = 2;

//Variables BLE=========================================================================
BLEScan *pBLEScan;

    int d1;  //afstand tot ESP1
    int d2;  //afstand tot ESP2
    int d3;  //afstand tot ESP3
    int d4;  //afstand tot ESP4

    int distance1;  //afstand tot id1
    int distance2;  //afstand tot id2
    int distance3;  //afstand tot id3
    int distance4;  //afstand tot id4



    const int scanTimeSeconds = 1;
    uint8_t cnt = 0;
    char data[5];
    uint8_t mode;

    unsigned long startMillis;  
    unsigned long currentMillis;

    unsigned long switchStartMillis;
    unsigned long switchCurrentMillis;

    unsigned long codeStartMillis;
    unsigned long codeCurrentMillis;


    

//====================================================================================
//functJOInts

void setBlack(){
  analogWrite(PIN_RED,  0 ,255);
  analogWrite(PIN_GREEN, 0 ,255);
  analogWrite(PIN_BLUE,  0 ,255);
}

void setPink(){
  analogWrite(PIN_RED,  240 ,255);
  analogWrite(PIN_GREEN, 230 ,255);
  analogWrite(PIN_BLUE,  140 ,255);
}

void setWhite(){
  analogWrite(PIN_RED,  255,255);
  analogWrite(PIN_GREEN,  255,255);
  analogWrite(PIN_BLUE,  255 ,255);
}

void setOrange(){
  analogWrite(PIN_RED,   255 ,255);
  analogWrite(PIN_GREEN, 165 ,255);
  analogWrite(PIN_BLUE,  0,255);
}

void setRed(){
    analogWrite(PIN_RED,   255 ,255);
    analogWrite(PIN_GREEN, 0,255);
    analogWrite(PIN_BLUE,  0,255);
}

void setGreen(){
    analogWrite(PIN_RED,   0 ,255);
    analogWrite(PIN_GREEN, 255,255);
    analogWrite(PIN_BLUE,  0,255);
}

void setBlue(){
  analogWrite(PIN_RED,   0,255);
  analogWrite(PIN_GREEN, 0,255);
  analogWrite(PIN_BLUE,  255,255);
}

void setCyan(){
  analogWrite(PIN_RED,  0,255);
  analogWrite(PIN_GREEN, 255,255);
  analogWrite(PIN_BLUE,  50,255);
}

void setMagenta(){
  analogWrite(PIN_RED,   255,255);
  analogWrite(PIN_GREEN, 0,255);
  analogWrite(PIN_BLUE,  255,255);
}

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks

    {
        void tekstInResult(BLEAdvertisedDevice advertisedDevice){
                

                if(advertisedDevice.getName().compare("esp1") == 0)
                    {d1 = advertisedDevice.getRSSI();}
                if(advertisedDevice.getName().compare("esp2") == 0)
                    {d2 = advertisedDevice.getRSSI();}
                if(advertisedDevice.getName().compare("esp3") == 0)
                    {d3 = advertisedDevice.getRSSI();}
                if(advertisedDevice.getName().compare("esp4") == 0)
                    {d4 = advertisedDevice.getRSSI();}

                //Serial.print(advertisedDevice.getName().c_str());
                //Serial.printf(": %d \n", advertisedDevice.getRSSI());
                //Serial.printf(": %s \n", advertisedDevice.getManufacturerData().c_str());

        }

        void onResult(BLEAdvertisedDevice advertisedDevice)
        {             
        if (advertisedDevice.getName().compare("esp1") == 0 || 
        advertisedDevice.getName().compare("esp2") == 0 ||
        advertisedDevice.getName().compare("esp3") == 0 ||
        advertisedDevice.getName().compare("esp4") == 0)
            {
                tekstInResult(advertisedDevice);
            }
        }
        
    };

void scansetup(){

        Serial.begin(115200);
        Serial.println("Scanning...");

        BLEDevice::init("Radiation SCAN");
        pBLEScan = BLEDevice::getScan(); // create new scan
        pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
        pBLEScan->setActiveScan(false); // active scan (true) uses more power, but get results faster
        pBLEScan->setInterval(100);
        pBLEScan->setWindow(99); // less or equal setInterval value

            
        

        distance1 = 0;
        distance2 = 0;
        distance3 = 0;
        distance4 = 0;
    }

void sendsetup(){
        bleCast.begin();
    }

void scanloop(){
        BLEScanResults foundDevices = pBLEScan->start(scanTimeSeconds, false);
        pBLEScan->clearResults();
    }

void sendloop(){
                // note -- if you have too much data, it will not be added to the adv payload

        if (cnt == 20){
            // reset
            cnt = 0;
        }

        if (cnt == 0){
            // regenerate "random" data
            int red = random(20, 50);
            int orange = random(50, 70);
            sprintf(data, "%02d&%02d", red, orange);
        }
        cnt += 1;
        
        std::string s = bleCast.setManufacturerData(data, sizeof(data));
        Serial.println(s.c_str());
        mode ++;
        delay(500);
        
    }

void initializeSensor(){
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

  hartratesensorEnable= true;
}

void setupLed(){
  pinMode(PIN_RED,   OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE,  OUTPUT);
}

void sendScanLoop(){
    currentMillis = millis();

  if ((currentMillis-startMillis)<=period )
      {sendloop();}
  else
      {scanloop();
      startMillis = currentMillis;}

  
}

void noWristband(){
  Serial.print(" No finger?");
  setRed();
  hartslagFoutenTeller++;

  if(hartslagFoutenTeller>hartslagFoutenMax){
    client.publish("TrappenMaar/buffer","grote fout");
    client.publish("TrappenMaar/buffer","grote fout");
    Serial.print("Grote Fout gestuurd");
    hartslagFoutenTeller = 0;
  }


  //client.publish("controlpanel/reset","Reset escaperoom");
  //delay(3000);
  //client.publish("wristbands/3cijfers","Wristband-code 8630");
  //delay(3000);

}

void wristbandReady(){
  delay(1000);
  for(int i=0; i<4;i++){
  setBlack();
  delay(40);
  setCyan();
  delay(40);
  }

  client.publish("controlpanel/status","Wristbands Ready");
  oneTime=false;
  //delay(3000);
}

void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi..");

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void resetESP(){
  ESP.restart();
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (messageTemp== "Reset escaperoom" ){
    resetESP();
  }

  if (messageTemp== "Reset Wristbands" ){
    resetESP();
  }

  if (messageTemp== "hartslagsensor uit" ){
    hartratesensorEnable = false;
  }

  if (messageTemp== "Stop Wristbands"){
    wristbandEnable = false;
  }

  if (messageTemp== "hartslagsensor aan" ){
    hartratesensorEnable = true;
  }

  if (messageTemp== "Herstart Wristbands"){
    wristbandEnable = true;
  }
  

  // Feel free to add more if statements to control more GPIOs with MQTT
    // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
    // Changes the output state according to the message
    // if (String(topic) == "esp32/output")
    // {
    //   Serial.print("Changing output to ");
    //   if (messageTemp == "on")
    //   {
    //     Serial.println("on");
    //     digitalWrite(LED_PIN, HIGH);
    //   }
    //   else if (messageTemp == "off")
    //   {
    //     Serial.println("off");
    //     digitalWrite(LED_PIN, LOW);
    //   }
    // }
}

boolean checkTeVer(){
  if(id==1 ||id ==2){
    if(distance1 < (-grenswaardeTeVer) || (distance2 < (-grenswaardeTeVer) )){

    return true;
    }
  return false;
  }
  else if(id==3 || id ==4){
    if(distance3 < (-grenswaardeTeVer) || (distance4 < (-grenswaardeTeVer) )){

    return true;
    }
  return false;
  }
  return false;

}

boolean checkTeDicht(){
if (id ==1 || id == 2){
  if(distance3 > (-grenswaardeTeDicht) || distance4 > (-grenswaardeTeDicht) ){

    return true;
  }
  return false;
}

else if (id ==3 || id == 4){
  if(distance1 > (-grenswaardeTeDicht) || distance2 > (-grenswaardeTeDicht) ){

    return true;
  }
  return false;
}
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // CREATE UNIQUE client ID!
    // in Mosquitto broker enable anom. access
    if (client.connect(cEspNaam))
    {
      wristbandReady();
      Serial.println("connected");
      // Subscribe
      // Vul hieronder in naar welke directories je gaat luisteren.
      //Voor de communicatie tussen de puzzels, check "Datacommunicatie.docx". (terug tevinden in dezelfde repository) 
      client.subscribe("controlpanel/reset");
      client.subscribe("Wristbands");
      client.subscribe("controlpanel/status");
      client.subscribe("trappenmaar/buffer");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void fixDistance(){
  if (espNaam.compare("esp1") == 0){
    if(id==1){
      distance1 = d1;
      distance2 = d2;
      distance3 = d3;
      distance4 = d4;
      doorstuurCode = "1234";
    }
    else if(id==2){
      distance2 = d1;
      distance3 = d2;
      distance4 = d3;
      distance1 = d4;
      doorstuurCode = "4123";
    }
    else if(id==3){
      distance3 = d1;
      distance4 = d2;
      distance1 = d3;
      distance2 = d4;
      doorstuurCode = "3412";
    }
    else if(id==4){
      distance4 = d1;
      distance1 = d2;
      distance2 = d3;
      distance3 = d4;
      doorstuurCode = "2341";
    }

  }
  if (espNaam.compare("esp2") == 0){
    if(id==1){
      distance4 = d1;
      distance1 = d2;
      distance2 = d3;
      distance3 = d4;
      doorstuurCode = "2341";
    }
    else if(id==2){
      distance1 = d1;
      distance2 = d2;
      distance3 = d3;
      distance4 = d4;
      doorstuurCode = "1234";
    }
    else if(id==3){
      distance2 = d1;
      distance3 = d2;
      distance4 = d3;
      distance1 = d4;
      doorstuurCode = "4123";
    }
    else if(id==4){
      distance3 = d1;
      distance4 = d2;
      distance1 = d3;
      distance2 = d4;
      doorstuurCode = "3412";
    }

  }
  if (espNaam.compare("esp3") == 0){
    if(id==1){
      distance3 = d1;
      distance4 = d2;
      distance1 = d3;
      distance2 = d4;
      doorstuurCode = "3412";
    }
    else if(id==2){
      distance4 = d1;
      distance1 = d2;
      distance2 = d3;
      distance3 = d4;
      doorstuurCode = "2341";
    }
    else if(id==3){
      distance1 = d1;
      distance2 = d2;
      distance3 = d3;
      distance4 = d4;
      doorstuurCode = "1234";
    }
    else if(id==4){
      distance2 = d1;
      distance3 = d2;
      distance4 = d3;
      distance1 = d4;
      doorstuurCode = "4123";
    }

  }
  if (espNaam.compare("esp4") == 0){
    if(id==1){
      distance2 = d1;
      distance3 = d2;
      distance4 = d3;
      distance1 = d4;
      doorstuurCode = "4123";
    }
    else if(id==2){
      distance3 = d1;
      distance4 = d2;
      distance1 = d3;
      distance2 = d4;
      doorstuurCode = "3412";
    }
    else if(id==3){
      distance4 = d1;
      distance1 = d2;
      distance2 = d3;
      distance3 = d4;
      doorstuurCode = "2341";
    }
    else if(id==4){
      distance1 = d1;
      distance2 = d2;
      distance3 = d3;
      distance4 = d4;
      doorstuurCode = "1234";
    }

  }
}

void schuifdoor(){
  switchCurrentMillis = millis();
  if ((switchCurrentMillis - switchStartMillis)>= switchPeriod )
      {ronde++;
        client.publish("eindpuzzel/timer","partnerruil");
        for(int i=0; i<200;i++){
          setBlack();
          delay(40);
          setRed();
          delay(40);
        }


        if(id == 4){
          id = 1;
          
        }
        else{
          id++;
        }

      }
  Serial.print("id: ");
  Serial.println(id);
  
      
}

void teVer(){
  setBlue();
  Serial.print("Aantal Fouten: ");
  Serial.println(foutCounter);
  Serial.print("Ronde: ");
  Serial.println(ronde);
  Serial.println("TE VER");
  foutCounter++;

    if(ronde==0){
      if((codeCurrentMillis - codeStartMillis)/1000>= (2*partnerTime)){
        if ((foutCounter%foutenMarge) == 0){
          client.publish("TrappenMaar/buffer","kleine fout");
          Serial.println("fout gestuurd");
        }
      }
    }
    else{
      if((codeCurrentMillis - codeStartMillis)/1000>= partnerTime){
        if ((foutCounter%foutenMarge) == 0){
          client.publish("TrappenMaar/buffer","kleine fout");
          Serial.println("fout gestuurd");
          }
        
        }
    }
    
}

void teDicht(){
  setPink();
  Serial.print("Aantal Fouten: ");
  Serial.println(foutCounter);
  Serial.print("Ronde: ");
  Serial.println(ronde);
  Serial.println("TE DICHT");
  foutCounter++;

    if(ronde==0){
      if((codeCurrentMillis - codeStartMillis)/1000>= (2*partnerTime)){
        if ((foutCounter%foutenMarge) == 0){
          client.publish("TrappenMaar/buffer","kleine fout");
          Serial.println("fout gestuurd");
        }
      }
    }
    else{
      if((codeCurrentMillis - codeStartMillis)/1000>= partnerTime){
        if ((foutCounter%foutenMarge) == 0){
          client.publish("TrappenMaar/buffer","kleine fout");
          Serial.println("fout gestuurd");
          }
        
        }
    }
    
  }
  
void code(){
  delay(1000);
  codeCurrentMillis = millis();
  Serial.print((codeCurrentMillis - codeStartMillis)/1000);
  Serial.println("s");
  if((codeCurrentMillis-(codetimelongcounter+1)*codetimelong)>=codeStartMillis){ 
    if (((codeCurrentMillis - codeStartMillis)>= (codePeriod+(codetimelongcounter+1)*codetimelong)) && ((codeCurrentMillis - codeStartMillis)<= ((codePeriod+(codetimelongcounter+1)*codetimelong)+2000))){
      for(int i=0; i<10;i++){
        setBlack();
        delay(20);
        setMagenta();
        delay(20);
  
      }
      codetimelongcounter++;  
    }
}
    if ((codeCurrentMillis - switchStartMillis)>=switchPeriod){
      codeStartMillis = codeCurrentMillis;
      codetimelongcounter=0;
    }

}

void sentCode(){
   if ((switchCurrentMillis - switchStartMillis)>= switchPeriod )
      {
        switchStartMillis = switchCurrentMillis;
        doorstuurCode= "Wristband-code " + doorstuurCode;
        client.publish("wristbands/3cijfers",doorstuurCode.c_str());
        Serial.print("code sent: ");
        Serial.println(doorstuurCode.c_str());
        
      }
}


//=====================================================================================
//=====================================================================================
void setup()
{ 
  Serial.begin(115200);
  Serial.println("Initializing...");
  switchStartMillis = millis();
  codeStartMillis = millis();
  startMillis = millis();

  wristbandEnable = true;

  setupLed();
  setRed();
  initializeSensor(); //--------------------------------------------------- IR!!!
  setCyan();

  scansetup();
  sendsetup();

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  //wristbandReady();

  client.publish("wristbands/3cijfers",doorstuurCode.c_str());
  Serial.print("code sent: ");
  Serial.println(doorstuurCode.c_str());

}

//========================================================================================================
//========================================================================================================
void loop(){
  codePeriod = codetime + id*codetime;  

  schuifdoor(); 
  fixDistance();
  sentCode();

  sendScanLoop();
  code();

  long irValue;

  if (hartratesensorEnable){
    irValue = particleSensor.getIR(); //--------------------------------------------IR!!!!
  }
  else {
      irValue = irThreshold+1; //-------------------------------------------nIR!!
  }
//WIFI RECONNECT===================================================================
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 5000)
  {
    lastMsg = now;
  }

  if(wristbandEnable){
//=================================================================================
    if (irValue < irThreshold){
      noWristband();
    }
      
      
    if (irValue > irThreshold){
      hartslagFoutenTeller=0;
      
      if(checkTeVer()){
        teVer();

        if(checkTeDicht()){
          teDicht();
        }
      }
      else if(checkTeDicht()){
        teDicht();
      }
      else{
        setGreen();
      }
    }
    
      Serial.println();

    

  
  /*setBlue();
  delay(1000);
  setRed();
  delay(1000);
  setGreen();
  delay(1000);
  setBlack();
  delay(1000) ; */
}
else{
  setBlack();
}
}