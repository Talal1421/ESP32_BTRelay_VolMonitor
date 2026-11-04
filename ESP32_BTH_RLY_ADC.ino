// This code will program the esp32 to be a slave device theat will be controlled by a master device through Bluetooth,
// if an ON/OFF command is received then the esp32 will trigger the relay according to the command, The code will also program the esp32
// to read a battery voltage through ADC pin and print it to the master device
// Relay PIN is P16, ADC PIN is P34(ADC1_CHANNEL6). note that ADC2 pins is not compatible with Bluetooth and wifi 
// in order to read accurate values through the ADC, ensure that the output voltage from the voltage divider to the ADC PIN does not exceed 3V
// so for a 12.6maxvolt battery, the voltage divider should ouput 3V for a 12.6V battery voltage input, also ensure that (R2+R1)/R2 = 4.2
// ADC Attenuation is at default(11db), Resoloution at default(12bit, 0-4095) , do not change it.

#include "BluetoothSerial.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

String device_name = "ESP32-BT-Slave";  // Bluetooth name of the esp32

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

#define RLY_PIN    16
#define ADC_PIN    34
#define FILTER_LEN  30 // number of samples will be avged to know the actual voltage





BluetoothSerial SerialBT;
char incomingchar;  // will store each character received individually
String MSG; // a buffer to store all characters received in a message

uint32_t AN_Pot1_Buffer[FILTER_LEN] = {0};
int ADC_Raw_Value = 0; // for 12bitadc value range 0-4095
float Cal_Voltage_mV = 0.0; // voltage in mv after calibration function
int AN_Pot1_Raw = 0;
float Cal_Voltage_mV_avg = 0; // avg calibrated voltage for 30 readings (FILTER_LEN)
int AN_Pot1_i = 0;
float Filterd_Cal_Voltage_V_avg = 0; // avg voltage after factoring (last step of enhancing voltage readings)
static unsigned long lastSendTime = 0; 
float ResistorsRatioTObatteryV = 4.2; // (R2+R1)/R2 or BatteryVoltage/NewADCPinvoltage so 12.6/3 = 4.2






void setup() {
  Serial.begin(9600);
  SerialBT.begin(device_name);  //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  pinMode(RLY_PIN, OUTPUT);
  pinMode(ADC_PIN, INPUT);


 

  
}

void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  // incomingchar wil process each byte received alone , incomingchar will contain only one character at a time
  while (SerialBT.available()) {
    // Serial.write(SerialBT.read());
    incomingchar = SerialBT.read();
    MSG += incomingchar;
    }
    Serial.print(MSG); // print buffer value for debugging
    // for (int i = 0; i < 100; i++) for testing, on delay(500) off delay(500)
   if ( MSG == "ON"){
    delay(20);
    digitalWrite(RLY_PIN, HIGH);
    MSG = ""; // clear message 
    SerialBT.println("Command to turn on received");
    }
    else if(MSG == "OFF"){
    delay(20);
    digitalWrite(RLY_PIN, LOW);
    MSG = "";
    SerialBT.println("Command to turn off received");
   }
    else {
      MSG = "";
    }




ADC_Raw_Value = analogRead(ADC_PIN);
Cal_Voltage_mV = readADC_Cal(ADC_Raw_Value);
Cal_Voltage_mV_avg = readADC_Avg(Cal_Voltage_mV);
unsigned long currentTime = millis();
  // printing voltage values every 3000 ms -> 3s
  if (currentTime - lastSendTime >= 1000) {

  
  float Cal_Voltage_V_avg =  Cal_Voltage_mV_avg/1000.0;
  // factoring function to redce error rate
  if(Cal_Voltage_V_avg < 2.75 && Cal_Voltage_V_avg >= 2.53) {
    Filterd_Cal_Voltage_V_avg = Cal_Voltage_V_avg*0.99;
    Serial.print("Battery Voltage: ");
    Serial.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
    Serial.println("V");
    SerialBT.print("Battery Voltage: ");
    SerialBT.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
    SerialBT.println("V");
  }
    else if(Cal_Voltage_V_avg < 2.53 && Cal_Voltage_V_avg >= 1.0) {
      Filterd_Cal_Voltage_V_avg = Cal_Voltage_V_avg*0.98;
      Serial.print("Battery Voltage: ");
      Serial.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
      Serial.println("V");
      SerialBT.print("Battery Voltage: ");
      SerialBT.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
      SerialBT.println("V");
    }
      else if(Cal_Voltage_V_avg < 1 && Cal_Voltage_V_avg >=0.5 ) {
        Filterd_Cal_Voltage_V_avg = Cal_Voltage_V_avg*0.97;
        Serial.print("Battery Voltage: ");
        Serial.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
        Serial.println("V");
        SerialBT.print("Battery Voltage: ");
        SerialBT.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
        SerialBT.println("V");
      }
        else if(Cal_Voltage_V_avg < 0.5 && Cal_Voltage_V_avg >=0.4){
          Filterd_Cal_Voltage_V_avg = Cal_Voltage_V_avg*0.95;
          Serial.print("Battery Voltage: ");
          Serial.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
          Serial.println("V");
          SerialBT.print("Battery Voltage: ");
          SerialBT.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
          SerialBT.println("V");
        }
          else if(Cal_Voltage_V_avg < 0.4 && Cal_Voltage_V_avg >=0.3){
           Filterd_Cal_Voltage_V_avg = Cal_Voltage_V_avg*0.93;
           Serial.print("Battery Voltage: ");
           Serial.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
           Serial.println("V");
           SerialBT.print("Battery Voltage: ");
           SerialBT.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
           SerialBT.println("V");
          }
            else if(Cal_Voltage_V_avg < 0.3 && Cal_Voltage_V_avg >=0.2){
              Filterd_Cal_Voltage_V_avg = Cal_Voltage_V_avg*0.9;
              Serial.print("Battery Voltage: ");
              Serial.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
              Serial.println("V");
              SerialBT.print("Battery Voltage: ");
              SerialBT.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
              SerialBT.println("V");
            }
              else if(Cal_Voltage_V_avg < 0.2 && Cal_Voltage_V_avg >=0.14){
               Filterd_Cal_Voltage_V_avg = Cal_Voltage_V_avg*0.86;
               Serial.print("Battery Voltage: ");
               Serial.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
               Serial.println("V");
               SerialBT.print("Battery Voltage: ");
               SerialBT.print( Filterd_Cal_Voltage_V_avg*ResistorsRatioTObatteryV, 3);
               SerialBT.println("V");
              }
                else{ // here put other if statment for the other high error rate voltages                 
                  Serial.print("Battery Voltage: ");
                  Serial.print((Cal_Voltage_mV_avg/1000.0)*ResistorsRatioTObatteryV , 3);
                  Serial.println("V");
                  SerialBT.print("Battery Voltage: ");
                  SerialBT.print((Cal_Voltage_mV_avg/1000.0)*ResistorsRatioTObatteryV , 3);
                  SerialBT.println("V");
                }
  lastSendTime = currentTime;
  }





   

  delay(50);
} // end of while loop






// calibration function
uint32_t readADC_Cal(int ADC_Raw)
{
  esp_adc_cal_characteristics_t adc_chars;
  
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 3300, &adc_chars);
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}
// averaging function
uint32_t readADC_Avg(float Voltt)
{
  int i = 0;
  float Sum = 0;
  
  AN_Pot1_Buffer[AN_Pot1_i++] = Voltt;
  if(AN_Pot1_i == FILTER_LEN)
  {
    AN_Pot1_i = 0;
  }
  for(i=0; i<FILTER_LEN; i++)
  {
    Sum += AN_Pot1_Buffer[i];
  }
  return (Sum/FILTER_LEN);
}



