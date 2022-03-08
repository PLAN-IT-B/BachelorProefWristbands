#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLECast.h>
#include <string.h>


    BLEScan *pBLEScan;
    BLECast bleCast("esp1");

    const int scanTimeSeconds = 1;
    uint8_t cnt = 0;
    char data[5];
    uint8_t mode = 0;


    class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
    {
        void onResultOne(BLEAdvertisedDevice advertisedDevice){
            mode = 0;
                {
        if (advertisedDevice.getName().compare("esp2") == 0 && mode%4 ==1)
            {
                tekstInResult(advertisedDevice);
            }
        else if(advertisedDevice.getName().compare("esp3") == 0 && mode%4 ==2)
        {
                tekstInResult(advertisedDevice);
            }
        else if(advertisedDevice.getName().compare("esp4") == 0 && mode%4 ==3)
        {
            tekstInResult(advertisedDevice);
        }
        }
        }

        void onResulTwo(BLEAdvertisedDevice advertisedDevice){
            mode = 1;
                {
        if (advertisedDevice.getName().compare("esp1") == 0 && mode%4 ==1)
            {
                tekstInResult(advertisedDevice);
            }
        else if(advertisedDevice.getName().compare("esp3") == 0 && mode%4 ==2)
        {
                tekstInResult(advertisedDevice);
            }
        else if(advertisedDevice.getName().compare("esp4") == 0 && mode%4 ==3)
        {
            tekstInResult(advertisedDevice);
        }
    }}

        void onResultThree(BLEAdvertisedDevice advertisedDevice){
            mode = 2;
                {
        if (advertisedDevice.getName().compare("esp1") == 0 && mode%4 ==2)
            {
                tekstInResult(advertisedDevice);
            }
        else if(advertisedDevice.getName().compare("esp2") == 0 && mode%4 ==3)
        {
                tekstInResult(advertisedDevice);
            }
        else if(advertisedDevice.getName().compare("esp4") == 0 && mode%4 == 1)
        {
            tekstInResult(advertisedDevice);
        }
    }}

        void onResultFour(BLEAdvertisedDevice advertisedDevice){
            mode = 3;
                {
        if (advertisedDevice.getName().compare("esp1") == 0 && mode%4 ==3)
            {
                tekstInResult(advertisedDevice);
            }
        else if(advertisedDevice.getName().compare("esp2") == 0 && mode%4 ==1)
        {
                tekstInResult(advertisedDevice);
            }
        else if(advertisedDevice.getName().compare("esp3") == 0 && mode%4 ==2)
        {
            tekstInResult(advertisedDevice);
        }
    }}


        void tekstInResult(BLEAdvertisedDevice advertisedDevice){
                            Serial.print(advertisedDevice.getName().c_str());
                Serial.printf(": %d \n", advertisedDevice.getRSSI());
                Serial.printf(": %s \n", advertisedDevice.getManufacturerData().c_str());
                mode ++;
        }

        void onResult(BLEAdvertisedDevice advertisedDevice)
        {onResultOne(advertisedDevice);
        //onResulTwo(advertisedDevice);
        //onResultThree(advertisedDevice);
        //onResultFour(advertisedDevice);
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
        delay(1000);
        mode ++;
    }

    void setup()
    {
        scansetup();
        sendsetup();
    }



    void loop()
    {
        if (mode%4 ==0 )
            sendloop();
        else
            scanloop();
    }
