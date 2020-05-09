#include <SoftwareSerial.h>

SoftwareSerial BT(0, 1); 
String readvoice;


int echoPin=A0;           
int trigPin=A1;            

#define Lpwm_pin  5       
#define Rpwm_pin  10      


int pinLB=2;           
int pinLF=4;           
int pinRB=7;           
int pinRF=8;

unsigned char LRpwm_val = 100; 

int Car_state=0;        

void M_Control_IO_config(void)  
{
  pinMode(pinLB,OUTPUT);      
  pinMode(pinLF,OUTPUT);      
  pinMode(pinRB,OUTPUT);     
  pinMode(pinRF,OUTPUT);      
  pinMode(Lpwm_pin,OUTPUT);   
  pinMode(Rpwm_pin,OUTPUT);   
}

void Set_Speed(unsigned char Left,unsigned char Right)  
{
  analogWrite(Lpwm_pin,Left);         
  analogWrite(Rpwm_pin,Right);
}
                

void advance()               
    {
     digitalWrite(pinRB,LOW);   
     digitalWrite(pinRF,HIGH);  
     digitalWrite(pinLB,LOW);   
     digitalWrite(pinLF,HIGH);  
     Car_state = 1;   
    }
void turnR()                  
    {
     digitalWrite(pinRB,HIGH);  
     digitalWrite(pinRF,LOW);   
     digitalWrite(pinLB,LOW);   
     digitalWrite(pinLF,HIGH); 
     Car_state = 4;
    }
void turnL()                  
    {
     digitalWrite(pinRB,LOW);   
     digitalWrite(pinRF,HIGH);  
     digitalWrite(pinLB,HIGH);  
     digitalWrite(pinLF,LOW);   
     Car_state = 3;
    }    
void stopp()                  
    {
     digitalWrite(pinRB,HIGH);  
     digitalWrite(pinRF,HIGH);  
     digitalWrite(pinLB,HIGH);  
     digitalWrite(pinLF,HIGH);  
     Car_state = 5;
    }
void back()                   
    {
     digitalWrite(pinRB,HIGH);  
     digitalWrite(pinRF,LOW);   
     digitalWrite(pinLB,HIGH);
     digitalWrite(pinLF,LOW);  
     Car_state = 2;  
    }



int Ultrasonic_Ranging()  
{ 
     
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
     
    int distance = pulseIn(echoPin, HIGH);      
    
    distance= distance*0.034/2;                     
     
    return distance;
} 

void setup() 
{ 
   M_Control_IO_config();          
   Set_Speed(100, 68); 
   Serial.begin(9600);              
   stopp();                       

   pinMode(echoPin, INPUT);       
   pinMode(trigPin, OUTPUT);        
   BT.begin(9600);
}
int H;  
void loop() 
{ 
  while (BT.available()){  
    delay(10);
    char c = BT.read(); 
    readvoice += c; 
  } 
  if (readvoice.length() > 0) {
    Serial.println(readvoice);
    delay(300); 
    String readvoice1;
   
    if(readvoice == "*forward#"){
      while (readvoice == "*forward#"){
        H = Ultrasonic_Ranging();
        delay(300);     
        if(H < 50){
          stopp();
        } 
        else{
          advance();
        }
        while (BT.available()){  
          delay(10); 
          char c = BT.read(); 
          readvoice += c; 
        }
      }
    }else if(readvoice == "*back#"){
      back();
    }else if (readvoice == "*left#"){
      turnL();
      delay(500);
      stopp();
    }else if (readvoice == "*right#"){ 
      turnR();
      delay(500);
      stopp();
    }else if (readvoice == "*stop#"){
      stopp();
    }else if (readvoice == "*back for 1 second#"){
      back();
      delay(1000);
      stopp();
    }else if (readvoice == "*back for 2 seconds#"){
      back();
      delay(2000);
      stopp();
    }else if (readvoice == "*back for 3 seconds#"){
      back();
      delay(3000);
      stopp();
    }else if (readvoice == "*back for 4 seconds#"){
      back();
      delay(4000);
      stopp();
    }else if (readvoice == "*back for 5 seconds#"){
      back();
      delay(5000);
      stopp();
    }
   readvoice="";
  }
}
