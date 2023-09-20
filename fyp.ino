#include <ArduinoBLE.h>
#include <Arduino_LSM6DSOX.h>
#include <LiquidCrystal.h>

//BLE related

float data = 0;
BLEService customService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service
// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10000-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEFloatCharacteristic acc("19B10000-E8F2-537E-4F6C-D104768A1215", BLERead | BLENotify );


//LCD related
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//End of starting


void setup() {
  Serial.begin(9600);

  // set LED's pin to output mode
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);         // turn off the LED

  // initialization of BLE
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    
    while (1);
  }

    // set advertised local name and service UUID:
  BLE.setLocalName("Nano RP2040");
  BLE.setAdvertisedService(customService);


    // add the characteristic to the service and service itself
  customService.addCharacteristic(switchCharacteristic);
  customService.addCharacteristic(acc);
  BLE.addService(customService);

    // set the initial value for the characteristic:
  switchCharacteristic.writeValue(0);

    // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
   digitalWrite(LED_BUILTIN, HIGH);
    
  //set up accelermeter
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }
  
   Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in g's");
  Serial.println("X\tY\tZ");
  

  //set up LCD
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Initialized");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if BLE connected:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    
    // while connected:
    while (central.connected()) {
      // if received signal
      // react to the signal by the LED:
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()==1) {   // any value other than 0
          Serial.println("LED on");
          digitalWrite(LED_BUILTIN, HIGH);         // will turn the LED on
        } else {                              // a 0 value
          Serial.println(F("LED off"));
          digitalWrite(LED_BUILTIN, LOW);          // will turn the LED off
        }
      }

        readAcc();

    acc.writeValue(data);
    }

    lcd.setCursor(0, 0);
    lcd.print("Current acc.");
    lcd.setCursor(0, 1);
    lcd.print(Acc);

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW);         // when tdisconnects, turn off the LED
  }

}

void readAcc(){
  float x, y, z, acc;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    acc=sqrt(x*x+y*y+z*z);
    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(z);
    Serial.print('\t');
    Serial.print(acc);
    Serial.print('\t');
    data = acc;
    Serial.print(data);

  }
}