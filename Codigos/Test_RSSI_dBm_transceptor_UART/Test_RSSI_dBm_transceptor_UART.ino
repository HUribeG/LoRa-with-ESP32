/*

  Created on 01/03/2023
  By: Uribe Garcia (HG)

  Code for transmitting and receiving tests 
  with UART type LoRa E220 modules

  The desired mode of operation will be coded 
  by the variable variable "MODE" being 0 for 
  transmitter(Wemos D1) and 1 for receiver(Xiao).


  //Pin mapping

 * E22 ----- XIAO SAMD21
 * M0  ----- 9 
 * M1  ----- 8
 * TX  ----- 0   
 * RX  ----- 1   
 * AUX ----- 10   
 * VCC ----- VIN
 * GND ----- GND    

 -------------------------------

 * E22 ----- WeMos D1 mini
 * M0  ----- D1 
 * M1  ----- D2 
 * TX  ----- D4 
 * RX  ----- D3 
 * AUX ----- D0 
 * VCC ----- 5V
 * GND ----- GND  

 * Pai attention e22 support RSSI, 
 * if you want use that functionality you must 
 * enable RSSI on configuration
 * configuration.TRANSMISSION_MODE.enableRSSI = RSSI_ENABLED;
 
  Code based on the "Send_Transparent" example 
  from Renzo Mischianti's Ebyte LoRa E220 library.
 


*/
#include  "Arduino.h"
#include <SoftwareSerial.h>

#define BOARD 1


#if BOARD //XIAO
  
  //LoRa E220
  #define AUX 10
  #define M0  9
  #define M1  8

  //RGB 
  #define R 5
  #define G 4
  #define B 3
  

  SoftwareSerial receiver(1, 0); // (Xiao) RX, TX
  
  char RSSI_data[10];
  int RSSI;
  String command = "C0 C1 C2 C3";
  

#else //Wemos D1 mini

  #define ENABLE_RSSI true

  #include "LoRa_E22.h"

  //LoRa E220 || Wemos D1 mini
  #define AUX     D0
  #define M0      D1
  #define M1      D2
  #define RX      D3
  #define TX      D4    

  //RGB
  #define R D6
  #define G D5
  #define B D7 

  SoftwareSerial trans(TX, RX); //(Wemos) RX, TX
  
  LoRa_E22 e22ttl(&trans, AUX, M0, M1); // AUX M0 M1
  
#endif

void setup() {
  
#if BOARD

  SerialUSB.begin(115200);
  receiver.begin(9600);
  receiver.setTimeout(10000); //More distan -> more time

  pinMode(AUX, INPUT);
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);

#else

  Serial.begin(9600);
  delay(500);

  e22ttl.begin();

  Serial.println("Im going 2 send it yo");

  ResponseStatus rs = e22ttl.sendMessage("Hello, world?");
  Serial.println(rs.getResponseDescription());

  
#endif
}

void loop() {
  
#if BOARD

  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);

  receiver.println(command);
  receiver.flush();
  delay(100);

  receiver.readBytes(RSSI_data, 4);
  RSSI =  (RSSI_data);

  int dBm = -(255 - RSSI); //Eq by manufacter

  if(-89 <= dBm){
    digitalWrite(B, LOW);
  }
  else if (-97 <= dBm) {
    digitalWrite(G, LOW);
  }
  else if(-102 <= dBm){
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
  }
  else {
    digitalWrite(R, LOW);
  }
  
  SerialUSB.print("RSSI:");
  SerialUSB.print(",");
  SerialUSB.print(RSSI);
  SerialUSB.print(",");
  SerialUSB.print("dBm:");
  SerialUSB.print(",");
  Serial.println(dBm);
  delay(2000);
    
#else

  	// If something available
  if (e22ttl.available()>1) {
	  // read the String message
#ifdef ENABLE_RSSI
	ResponseContainer rc = e22ttl.receiveMessageRSSI();
#else
	ResponseContainer rc = e22ttl.receiveMessage();
#endif
	// Is something goes wrong print error
	if (rc.status.code!=1){
		Serial.println(rc.status.getResponseDescription());
	}else{
		// Print the data received
		Serial.println(rc.status.getResponseDescription());
		Serial.println(rc.data);
#ifdef ENABLE_RSSI
		Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);

    trans.println(rc.rssi, DEC);
    
#endif
	}
  }
  if (Serial.available()) {
	  String input = Serial.readString();
	  e22ttl.sendMessage(input);
  }

#endif
}
