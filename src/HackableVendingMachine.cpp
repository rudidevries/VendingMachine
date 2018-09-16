#include "HackableVendingMachine.h"

HackableVendingMachine::HackableVendingMachine(Adafruit_LiquidCrystal *lcd, uint8_t ledPin, ELClientMqtt *mqtt, uint8_t numCompartments, Compartment **Compartments) : MqttVendingMachine(lcd, ledPin, mqtt, numCompartments, Compartments) {

}
