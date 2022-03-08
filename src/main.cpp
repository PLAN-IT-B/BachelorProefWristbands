#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLECast.h>
#include <string.h>


    BLEScan *pBLEScan;

//ADAPTABLE
    BLECast bleCast("esp4");
    const unsigned long period = 750;  //the value is a number of milliseconds

    int distance1;
    int distance2;
    int distance3;
    int distance4;

    const int scanTimeSeconds = 1;
    uint8_t cnt = 0;
    char data[5];
    uint8_t mode;

    unsigned long startMillis;  //some global variables available anywhere in the program
    unsigned long currentMillis;
    
    const byte ledPin = 13;    //using the built in LED


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
    }

    void sendsetup(){
        Serial.begin(115200);
        Serial.println("Starting BLE Beacon");

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
        //mode = 1;
        startMillis = millis();
        distance1 = 0;
        distance2 = 0;
        distance3 = 0;
        distance4 = 0;
    }



    void loop()
    {
        currentMillis = millis();

        if ((currentMillis-startMillis)<=period )
            {scanloop();
            }
        else
            {sendloop();
            startMillis = currentMillis;
            }
        Serial.println(distance1);
        Serial.println(distance2);
        Serial.println(distance3);
        Serial.println(distance4);
    }