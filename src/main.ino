#include <Arduino.h>
#include <ELClient.h>
#include <ELClientCmd.h>
#include <ELClientMqtt.h>
#include "KeypadHandler/KeypadHandler.h"
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include "HackableVendingMachine.h"
#include "KeypadHandler/delegates/HackableVendingMachineKeypadDelegate.h"
#include "Compartment.h"
#include "LedControl.h"

#include <menu.h>//menu macros and objects
#include <menuIO/serialIn.h>
#include <menuIO/serialOut.h>
using namespace Menu;

// KEYPAD
const byte KeypadRows= 4;
const byte KeypadCols= 3;
char keymap[KeypadRows][KeypadCols]=
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rPins[KeypadRows]= {28,30,32,34};
byte cPins[KeypadCols]= {22,24,26};

KeypadHandler kpd = KeypadHandler(makeKeymap(keymap), rPins, cPins, KeypadRows, KeypadCols);
//initialize an instance of class NewKeypad
//Keypad customKeypad( makeKeymap(keymap), rPins, cPins, KeypadRows, KeypadCols);

// LCD
byte euroChar[8] = {
	0b00000,
	0b00110,
	0b01001,
	0b11100,
	0b01000,
	0b11100,
	0b01001,
	0b00110
};
Adafruit_LiquidCrystal lcd(0);
MqttVendingMachine *vendingMachine;

// Menu
result exitAdministrator() {
  vendingMachine->state = operational;
  vendingMachine->resetDisplay();
  return proceed;
}

char* constMEM ipDigit MEMMODE="0123456789. ";
char* constMEM ipNr[] MEMMODE={ipDigit};
char host[]="192.168.178.104";//<-- menu will edit this text

unsigned int price1 = 100;
unsigned int price2 = 100;
unsigned int price3 = 100;
unsigned int price4 = 100;
MENU(compartmentMenu, "Pricing", doNothing, noEvent, wrapStyle
  ,FIELD(price1, "Compartment 1", " cts", 0, 10000, 10, 10, doNothing, noEvent, noStyle)
  ,FIELD(price2, "Compartment 2", " cts", 0, 10000, 10, 10, doNothing, noEvent, noStyle)
  ,FIELD(price3, "Compartment 3", " cts", 0, 10000, 10, 10, doNothing, noEvent, noStyle)
  ,FIELD(price4, "Compartment 4", " cts", 0, 10000, 10, 10, doNothing, noEvent, noStyle)
  ,EXIT("<Back")
);

bool mqttEnabled = false;
TOGGLE(mqttEnabled, mqttEnableMenu, "Enabled: ", doNothing, noEvent, wrapStyle
  ,VALUE("NO", false, doNothing, noEvent)
  ,VALUE("YES", true, doNothing, noEvent)
);

char* constMEM codeDigit MEMMODE="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char* constMEM codeNr[] MEMMODE={codeDigit};
char code1[]="00000000";
char code2[]="00000000";
char code3[]="00000000";
char code4[]="00000000";

MENU(mqttCompartmentMenu, "Compartment codes", doNothing, noEvent, wrapStyle
  ,EDIT("Compartment 1", code1, codeNr, doNothing, noEvent, noStyle)
  ,EDIT("Compartment 2", code2, codeNr, doNothing, noEvent, noStyle)
  ,EDIT("Compartment 3", code3, codeNr, doNothing, noEvent, noStyle)
  ,EDIT("Compartment 4", code4, codeNr, doNothing, noEvent, noStyle)
  ,EXIT("<Back")
);

MENU(mqttMenu, "MQTT", doNothing, noEvent, wrapStyle
  ,SUBMENU(mqttEnableMenu)
  ,EDIT("Host", host, ipNr, doNothing, noEvent, noStyle)
  ,SUBMENU(mqttCompartmentMenu)
  ,EXIT("<Back")
);

bool wifiEnabled = false;
TOGGLE(wifiEnabled, wifiEnableMenu, "Enabled: ", doNothing, noEvent, wrapStyle
  ,VALUE("NO", false, doNothing, noEvent)
  ,VALUE("YES", true, doNothing, noEvent)
);

char* constMEM charDigit MEMMODE="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz ";
char* constMEM charValid[] MEMMODE={charDigit};
char ssid[]="                    ";
char pass[]="                    ";

MENU(wifiMenu, "WIFI", doNothing, noEvent, wrapStyle
  ,SUBMENU(wifiEnableMenu)
  ,EDIT("SSID", ssid, charValid, doNothing, noEvent, noStyle)
  ,EDIT("PASS", pass, charValid, doNothing, noEvent, noStyle)
  ,EXIT("<Back")
);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(compartmentMenu)
  ,SUBMENU(mqttMenu)
  ,SUBMENU(wifiMenu)
  ,OP("Exit",exitAdministrator,enterEvent)
);

#define MAX_DEPTH 3

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  //,LIQUIDCRYSTAL_OUT(lcd,{0,0,20,4})2
  ,NONE
);

//keypadIn kpad(kpd);
//NAVROOT(nav,mainMenu,MAX_DEPTH,kpad,out);
serialIn serial(Serial);
NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);


// COIN ACCEPTOR
const byte coinPin = 2;
volatile uint8_t pulses = 0;

// WIFI Connectivity
ELClient esp(&Serial3, &Serial3);
ELClientCmd cmd(&esp);
ELClientMqtt mqtt(&esp);

void wifiCb(void* response) {
  ELClientResponse *res = (ELClientResponse*)response;
  if (res->argc() == 1) {
    uint8_t status;
    res->popArg(&status, 1);

    if(status == STATION_GOT_IP) {

    } else {

    }
  }
}

// Callback when MQTT is connected
void mqttConnected(void* response) {
  Serial.println("MQTT connected");
  vendingMachine->mqttConnected();
}

// Callback when MQTT is disconnected
void mqttDisconnected(void* response) {
  Serial.println("MQTT disconnected");
}

// Callback when an MQTT message arrives for one of our subscriptions
void mqttData(void* response) {
  ELClientResponse *res = (ELClientResponse *)response;

  Serial.print("Received: topic=");
  String topic = res->popString();
  Serial.println(topic);

  Serial.print("data=");
  String data = res->popString();
  Serial.println(data);

  vendingMachine->mqttHandle(topic, data);
}

void setupEsp() {
  // TODO enable/disable WIFI
  if (false) {
    Serial3.begin(115200);
    esp.wifiCb.attach(wifiCb);

    bool ok;
    uint8_t tried = 0;
    do {
      ok = esp.Sync();      // sync up with esp-link, blocks for up to 2 seconds
      if (!ok) Serial.println("EL-Client sync failed!");
      tried++;
    } while(!ok && tried <= 5);

    mqtt.connectedCb.attach(mqttConnected);
    mqtt.disconnectedCb.attach(mqttDisconnected);
    mqtt.dataCb.attach(mqttData);
    mqtt.setup();
  }
}

// VendingMachine
void coinPulse() {
  pulses++;
}

void setup() {
  Serial.begin(115200);
  Serial.println("SETUP");
  setupEsp();

  lcd.createChar(0, euroChar); // TODO fix use of euro symbol
  lcd.begin(20, 4);
  Serial.println("LCD Initialized");

  Compartment *compartments[4] = {
    new Compartment(1, 100),
    new Compartment(2, 100),
    new Compartment(3, 100),
    new Compartment(4, 100)
  };

  Serial.println("LedControl Initialized");
  vendingMachine = new HackableVendingMachine(&lcd, 36, &mqtt, 4, compartments);
  Serial.println("VendingMachine Initialized");

  VendingMachineKeypadDelegate *d = new HackableVendingMachineKeypadDelegate(vendingMachine, &lcd);
  kpd.setDelegate(d);

  pinMode(coinPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(coinPin), coinPulse, RISING);
  Serial.println("Coin Acceptor Initialized");
}

void loop() {
  if (vendingMachine->state == operational) {
    kpd.loop();
  }
  else {
    nav.poll();
  }

  // Update coin value
  cli();
  uint8_t p = pulses;
  pulses = 0;
  sei();
  for (int i = 0; i < p; i++) {
    vendingMachine->tenCentPulse();
  }

  vendingMachine->loop();
}
