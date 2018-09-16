#ifndef MqttVendingMachine_H
  #define MqttVendingMachine_H

  #include "VendingMachine.h"
  #include <ELClientMqtt.h>

  class MqttVendingMachine : public VendingMachine {
    protected:
      ELClientMqtt *_mqtt;
    public:
      MqttVendingMachine(Adafruit_LiquidCrystal *lcd, uint8_t ledPin, ELClientMqtt *mqtt, uint8_t numCompartments, Compartment **Compartments);
      void mqttConnected();
      void mqttHandle(String topic, String data);
  };

#endif
