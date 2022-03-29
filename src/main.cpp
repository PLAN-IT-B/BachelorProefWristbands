#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLECast.h>
#include <string.h>


   

//ADAPTABLE ========================================================================================

    BLECast bleCast("esp3");            //naam ESP
    const unsigned long period = 1100;  //the value is a number of milliseconds

//ADADPTABLE =======================================================================================

    BLEScan *pBLEScan;

    int distance1;  //afstand tot ESP1
    int distance2;  //afstand tot ESP2
    int distance3;  //afstand tot ESP3
    int distance4;  //afstand tot ESP4

    const int scanTimeSeconds = 1;
    uint8_t cnt = 0;
    char data[5];
    uint8_t mode;

    unsigned long startMillis;  
    unsigned long currentMillis;
    

    class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
    {
        void tekstInResult(BLEAdvertisedDevice advertisedDevice){
                

                if(advertisedDevice.getName().compare("esp1") == 0)
                    {distance1 = advertisedDevice.getRSSI();}
                if(advertisedDevice.getName().compare("esp2") == 0)
                    {distance2 = advertisedDevice.getRSSI();}
                if(advertisedDevice.getName().compare("esp3") == 0)
                    {distance3 = advertisedDevice.getRSSI();}
                if(advertisedDevice.getName().compare("esp4") == 0)
                    {distance4 = advertisedDevice.getRSSI();}

                Serial.print(advertisedDevice.getName().c_str());
                Serial.printf(": %d \n", advertisedDevice.getRSSI());
                Serial.printf(": %s \n", advertisedDevice.getManufacturerData().c_str());

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

    void setup()
    {
        scansetup();
        sendsetup();
    
        startMillis = millis();

        distance1 = 0;
        distance2 = 0;
        distance3 = 0;
        distance4 = 0;

        pinMode(4, OUTPUT);
    }


    void loop()
    {
        currentMillis = millis();

        if ((currentMillis-startMillis)<=period )
            {scanloop();}
        else
            {sendloop();
            startMillis = currentMillis;}


        
        if(distance3> 50 || distance1 > 50 || distance2 >50 ||distance4 >50 ||
        distance3 < (-50) || distance1 < (-50) || distance2 < (-50) ||distance4 < (-50))
        {
            digitalWrite(4, 0); 
        }
        else
        {
            digitalWrite(4,1);
            Serial.println(distance3);
        }
            
   
    }