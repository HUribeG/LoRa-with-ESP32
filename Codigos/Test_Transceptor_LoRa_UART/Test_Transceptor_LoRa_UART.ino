/*

  Created on 23/01/2023
  By Uribe Garcia (HG)

  Code for transmitting and receiving tests 
  with UART type LoRa E220 modules

  The desired mode of operation will be coded 
  by the variable variable "MODE" being 1 for 
  transmitter and 0 for receiver.

  Pins can be varied between development boards, 
  verify connections or make the necessary 
  modifications before uploading the code. You can 
  choose between development boards with 1 for XIAO 
  and 0 for WeMos D1.


*/

#define MODE 0
#define BOARD 1

#if (BOARD) ///XIAO

  //LoRa pins
  #define AUX 10
  #define M0  9
  #define M1  8

  //RGB LED
  #define R 5
  #define G 4
  #define B 3

#else ///Wemos D1 mini

  //LoRa pins
  #define AUX D3
  #define M0  D1
  #define M1  D2

  //RGB LED
  #define R D6
  #define G D5
  #define B D7
    
#endif


long int i=0;

void setup() {

  pinMode(AUX, INPUT);
  pinMode(M0,  OUTPUT);
  pinMode(M1,  OUTPUT);  

  pinMode(R,   OUTPUT);
  pinMode(G,   OUTPUT);
  pinMode(B,   OUTPUT);
  
  Serial.begin(9600);

  #if (MODE) //Transmiter (WOR)
    
    digitalWrite(M0, 0);  //The Wemos board uses negative pin logic
    digitalWrite(M1, 1);  //In non negative config -> M0 == 1 & M1 == 0
    //Serial.print("Trans");
    delay(1000);
  
  #else    //Receiver (WOR)
    
    digitalWrite(M0, 0);
    digitalWrite(M1, 1);
    Serial.print("Receiv");
    delay(100);
  
  #endif

  
}

void loop() {
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);  
  
  /* 
    TRANSMITER:
      Every time data is sent the color green
      flashes, every 10 data flashes blue
  */
  #if (MODE)
  
  i++;
  Serial.print(i);
  Serial.print(" ");
  if(0 == (i % 10) && 0 != i){
    digitalWrite(B, LOW);      
    delay(200);
    digitalWrite(B, HIGH);
    delay(1800);
      
  }
  else{
    digitalWrite(R, LOW); 
    delay(200);
    digitalWrite(R, HIGH);
    delay(1800);
       
  }
  
  
  /*
    RECEIVER:
      Every time a data arrives it blinks green, 
      if there is a lost packet it will blink red, 
      if 10s go by without receiving packets the 
      color blue will light up.      
  */  
  #else   

    int previous_data = -1; 
    unsigned long previous_time = 0; 


    if (Serial.available()) { 
      int data = Serial.parseInt();
      //Serial.println(data);
      digitalWrite(G, LOW);
      delay(300); 
      digitalWrite(G, HIGH); 
      
      if (previous_data != -1 && data != previous_data + 1) { 
        digitalWrite(R, LOW); 
        delay(300); 
        digitalWrite(R, HIGH); 
      }
      
      previous_data = data; 
      previous_time = millis(); 
    } 
    else if (millis() - previous_time >= 10000) { 
      digitalWrite(B, LOW); 
    } 
    else {
      digitalWrite(B, HIGH); 
    }
    
    //delay(100); 
          
  #endif


}
