#include <BLEDevice.h> 
#include <BLEServer.h> 
#include <BLEUtils.h> 
#include <BLE2902.h> 

BLECharacteristic *pCharacteristic; 
bool deviceConnected = false; 
uint8_t value = 0; 

// Сайт для генерирования UUID: 
// https://www.uuidgenerator.net/ 

#define SERVICE_UUID "57da150a-4aec-11e9-8646-d663bd873d93" 
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8" 




void setup() 
{ 
Serial.begin(115200); 

// создаем BLE-устройство: 
BLEDevice::init("beacon5"); 
BLEDevice::setPower(ESP_PWR_LVL_N0 ); 
// Создаем BLE-сервер: 
BLEServer *pServer = BLEDevice::createServer(); 

// Создаем BLE-сервис: 
BLEService *pService = pServer->createService(SERVICE_UUID); 

// запускаем сервис: 
pService->start(); 

// запускаем оповещения (advertising): 
pServer->getAdvertising()->start(); 

} 

void loop() { 


}
