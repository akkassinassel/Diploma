#include <IRremote.h>        
int RECV_PIN = 12;            
IRrecv irrecv(RECV_PIN);           
decode_results results;         
#define IR_Go      0x00ff629d 
#define IR_Back    0x00ffa857 
#define IR_Left    0x00ff22dd 
#define IR_Right   0x00ffc23d 
#define IR_Stop    0x00ff02fd 
#define IR_ESC     0x00ff52ad 
#define IR_Mode    0x00ff42bd 

#define IR_Slowdown    0x00ff6897 
#define IR_Speedup     0x00ff9867 

int echoPin=A0;           
int trigPin=A1;           

#define Lpwm_pin  5       
#define Rpwm_pin  10     

int pinLB=2;           
int pinLF=4;           
int pinRB=7;           
int pinRF=8;           

unsigned char LRpwm_val = 100; 

unsigned char mode = 0;			

int Car_state=0;       



int servopin=1;                 
int myangle;                    
int pulsewidth;                
unsigned char midPosition=60;   

void servopulse(int servopin,int myangle) 
{
  pulsewidth=(myangle*11)+500;        
  
  digitalWrite(servopin,HIGH);        
  delayMicroseconds(pulsewidth);      
  digitalWrite(servopin,LOW);         
  delay(20-pulsewidth/1000);          
}

void Set_servopulse(int set_val)            
{
  for(int i=0;i<=10;i++)            
    servopulse(servopin,set_val);   
}




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

void slowdown() {           
  if(LRpwm_val>80) {LRpwm_val-=20;}
  
  Set_Speed(LRpwm_val, LRpwm_val);
}  

void speedup() {            
  if(LRpwm_val<230) {LRpwm_val+=20;}
  
  Set_Speed(LRpwm_val, LRpwm_val);
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


void IR_Control(void)           
{
   unsigned long Key;         
  
  if(irrecv.decode(&results)&&mode==1) {      
    if (results.value==IR_Mode || results.value==IR_Stop || results.value==IR_ESC) mode = 0;     
  } else  if (irrecv.decode(&results)&&mode==0) {                                   
    while(Key!=IR_ESC)            
    {
     if(irrecv.decode(&results))  
     {
      Key = results.value;     
      if (mode == 0) {             
        switch(Key)
         {
           case IR_Go: advance();        
           break;
           case IR_Back: back();          
           break;
           case IR_Left: turnL();           
           break;
           case IR_Right: turnR();        
           break;
           case IR_Slowdown: slowdown();       
           break;
           case IR_Speedup: speedup();          
           break;
           case IR_Stop: stopp();             
           break;
           case IR_Mode: mode = 1;              
           break;
           default:
           break;      
         }
      } 
     else break;                 

     irrecv.resume();            
    }
  }
  }
  stopp();
}



void Self_Control(void)     
{
   int H;                                

   Set_servopulse(midPosition);    

   H = Ultrasonic_Ranging();
   delay(300);   
   
   if(H < 15)                               
   {
       stopp();              
       delay(100);
       back();               
       delay(50);
    }
           
  if(H < 35)                                         
      {
        stopp();                  
        delay(100);            
        Set_servopulse(5);        
        int L = Ultrasonic_Ranging(); 
        delay(300);      
         Set_servopulse(177);        
        int R = Ultrasonic_Ranging();   
        delay(300);      

        if(L > R)          
        {
           back();               
           delay(100);      
           turnL();               
           delay(400);                  
           stopp();           
           delay(50);            
           Set_servopulse(midPosition); 
           H = Ultrasonic_Ranging();   
           delay(500); 
        }
          
        if(L <= R)                
        {
           back();             
           delay(100);  
           turnR();               
           delay(400);   
           stopp();                
           delay(50);            
           Set_servopulse(midPosition); 
           H = Ultrasonic_Ranging();   
           delay(300);        
        }   
        
        if (L < 25 && R < 25)       
        {
           stopp();            
           delay(50);
           back(); 
           delay(50);                   
        }          
      }
      else                         
      {
      advance();                
      }                 
}

int Ultrasonic_Ranging()  
{ 
      
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
     
    int distance = pulseIn(echoPin, HIGH);     
    distance= distance/58;                      
     
    return distance;
} 



void setup() 
{ 
   M_Control_IO_config();             
   Set_Speed(LRpwm_val, LRpwm_val); 
   //irrecv.enableIRIn();               
   Serial.begin(9600);             
   stopp();                         

   pinMode(servopin,OUTPUT);        
   Set_servopulse(midPosition);     
   pinMode(echoPin, INPUT);         
   pinMode(trigPin, OUTPUT);                
   
}

void loop() 
{  
   //if (irrecv.decode(&results)) {
     // IR_Control();
      //irrecv.resume();      
  //} 
  
  //if (mode==1)                
    Self_Control(); 
}
