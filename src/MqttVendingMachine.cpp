#include "MqttVendingMachine.h"

MqttVendingMachine::MqttVendingMachine(Adafruit_LiquidCrystal *lcd, uint8_t ledPin, ELClientMqtt *mqtt, uint8_t numCompartments, Compartment **Compartments) : VendingMachine(lcd, ledPin, numCompartments, Compartments) {
  _mqtt = mqtt;
}

void MqttVendingMachine::mqttConnected() {
  for (uint8_t i = 1; i <=_numCompartments; i++) {
    char topicTrigger[27];
    sprintf(topicTrigger, "vendingmachine/%d/dispense", i);
    _mqtt->subscribe(topicTrigger);
  }
}

void MqttVendingMachine::mqttHandle(String topic, String data) {

}
