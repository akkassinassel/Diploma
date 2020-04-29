#include <IRremote.h>         // подключение библиотеки для работы с ИК приёмником   
int RECV_PIN = 12;            // порт Arduino для получения сигналов от ИК приёмника         
IRrecv irrecv(RECV_PIN);      // объект для работы с ИК приёмником        
decode_results results;       // объект для хранения результатов приёма команд от пульта управления     
#define IR_Go      0x00ff629d // команда «ВПЕРЕД», кнопка «СТРЕЛКА ВВЕРХ»
#define IR_Back    0x00ffa857 // команда «НАЗАД», кнопка «СТРЕЛКА ВНИЗ»
#define IR_Left    0x00ff22dd // команда «НАЛЕВО», кнопка «СТРЕЛКА ВЛЕВО»
#define IR_Right   0x00ffc23d // команда «НАПРАВО», кнопка «СТРЕЛКА ВПРАВО»
#define IR_Stop    0x00ff02fd // команда «СТОП», кнопка «OK»
#define IR_ESC     0x00ff52ad // команда «СБРОС», кнопка «#»
#define IR_Mode    0x00ff42bd // команда «РЕЖИМ», кнопка «*»

#define IR_Slowdown    0x00ff6897 // команда «МЕДЛЕННЕЕ», кнопка «1»
#define IR_Speedup     0x00ff9867 // команда «БЫСТРЕЕ», кнопка «2»

/* Полный список кодов кнопок пульта смотрите в инструкции, входящей в состав данного набора */ 

int echoPin=A0;           // порт для контакта ECHO модуля HC-SR04
int trigPin=A1;           // порт для контакта TRIG модуля HC-SR04   

#define Lpwm_pin  5       // порт для регулирования скорости левого колеса    (ENA) 
#define Rpwm_pin  10      // порт для регулирования скорости правого колеса   (ENB)

/* Назначение портов управления H-мостом драйвера L298N */

int pinLB=2;           // для заднего хода левого колеса      (IN1)
int pinLF=4;           // для вращения левого колеса вперед   (IN2)
int pinRB=7;           // для заднего хода правого колеса     (IN3)
int pinRF=8;           // для вращения правого колеса вперед  (IN4)

unsigned char LRpwm_val = 100;  // скорость вращения колес

unsigned char mode = 0;			// переменная для определения текущего режима: 0 — режим дистанционного управления, 1 — автономный режим избегания препятствий   

int Car_state=0;        // переменная для хранения состояния машинки   

/* ******* Управление серводвигателем ******* */

int servopin=1;                 // порт для подключения сигнального провода серводвигателя
int myangle;                    // переменная для хранения угла поворота вала серводвигателя
int pulsewidth;                 // переменная для хранения ширины импульса
unsigned char midPosition=60;   // инициализация угла поворота 60°, нейтральное положение датчика HC-SR04, при котором он направлен вперед

void servopulse(int servopin,int myangle) // функция управления сервоприводом
{
  pulsewidth=(myangle*11)+500;        // перевод угла в ширину импульса в диапазоне от 500 мкс до 2480 мкс
  
  digitalWrite(servopin,HIGH);        // на сигнальном проводе выставляется ВЫСОКИЙ уровень
  delayMicroseconds(pulsewidth);      // ВЫСОКИЙ уровень удерживается вычисленную длительность (ширина импульса)
  digitalWrite(servopin,LOW);         // на сигнальном проводе устанавливается НИЗКИЙ уровень
  delay(20-pulsewidth/1000);          // ожидание начала следующей итерации           
}

void Set_servopulse(int set_val)      // Функция для управления серводвигателем               
{
  for(int i=0;i<=10;i++)            
    servopulse(servopin,set_val);   
}

/* *********************************** */


void M_Control_IO_config(void)  // конфигурирование портов платы Arduino   
{
  pinMode(pinLB,OUTPUT);      // цифровой порт 2, ВЫХОД   
  pinMode(pinLF,OUTPUT);      // цифровой порт 4, ВЫХОД  
  pinMode(pinRB,OUTPUT);      // цифровой порт 7, ВЫХОД   
  pinMode(pinRF,OUTPUT);      // цифровой порт 8, ВЫХОД  
  pinMode(Lpwm_pin,OUTPUT);   // цифровой порт 5, ВЫХОД (ШИМ/PWM) 
  pinMode(Rpwm_pin,OUTPUT);   // цифровой порт 10, ВЫХОД (ШИМ/PWM)   
}

void Set_Speed(unsigned char Left,unsigned char Right)  // Установка скорости вращения колес      
{
  analogWrite(Lpwm_pin,Left);         
  analogWrite(Rpwm_pin,Right);
}

void slowdown() {             // уменьшение скорости   
  if(LRpwm_val>80) {LRpwm_val-=20;}
  
  Set_Speed(LRpwm_val, LRpwm_val);
}  

void speedup() {              // увеличение скорости
  if(LRpwm_val<230) {LRpwm_val+=20;}
  
  Set_Speed(LRpwm_val, LRpwm_val);
}                   

void advance()                // движение вперед
    {
     digitalWrite(pinRB,LOW);   // правое колесо вращается вперед
     digitalWrite(pinRF,HIGH);  // правое колесо вращается вперед
     digitalWrite(pinLB,LOW);   // левое колесо вращается назад
     digitalWrite(pinLF,HIGH);  // левое колесо вращается назад
     Car_state = 1;   
    }
void turnR()                  // поворот по часовой стрелке
    {
     digitalWrite(pinRB,HIGH);  // правое колесо вращается назад
     digitalWrite(pinRF,LOW);   // правое колесо вращается назад
     digitalWrite(pinLB,LOW);   // левое колесо вращается вперед
     digitalWrite(pinLF,HIGH);  // левое колесо вращается вперед
     Car_state = 4;
    }
void turnL()                  // поворот против часовой стрелки
    {
     digitalWrite(pinRB,LOW);   // правое колесо вращается вперед
     digitalWrite(pinRF,HIGH);  // правое колесо вращается вперед
     digitalWrite(pinLB,HIGH);  // левое колесо вращается назад
     digitalWrite(pinLF,LOW);   // левое колесо вращается назад
     Car_state = 3;
    }    
void stopp()                  // остановка
    {
     digitalWrite(pinRB,HIGH);  // правое колесо неподвижно
     digitalWrite(pinRF,HIGH);  // правое колесо неподвижно
     digitalWrite(pinLB,HIGH);  // левое колесо неподвижно
     digitalWrite(pinLF,HIGH);  // левое колесо неподвижно
     Car_state = 5;
    }
void back()                   // движение назад
    {
     digitalWrite(pinRB,HIGH);  // правое колесо вращается назад
     digitalWrite(pinRF,LOW);   // правое колесо вращается назад
     digitalWrite(pinLB,HIGH);  // левое колесо вращается назад
     digitalWrite(pinLF,LOW);   // левое колесо вращается назад
     Car_state = 2;  
    }

/* ******* Дистанционное управление ******** */
         
void IR_Control(void)           // обработка сигналов от пульта управления      
{
   unsigned long Key;           // переменная для хранения полученной команды
  
  if(irrecv.decode(&results)&&mode==1) {      // если машинка в режиме самоуправления (режим «1») и ИК приёмник принял какой-то сигнал…     
    if (results.value==IR_Mode || results.value==IR_Stop || results.value==IR_ESC) mode = 0;     // …и это сигнал с кодом кнопки «*», «#» или «OK» то изменить режим самоуправления на режим управления пультом (режим «0»)
  } else  if (irrecv.decode(&results)&&mode==0) {                                   // иначе выполнить код ниже:
      
    while(Key!=IR_ESC)            // цикл выполняется, пока не будет получена команда «СБРОС» (кнопка «#» на пульте)   
    {
     if(irrecv.decode(&results))  // проверка: получено ли новое значение   
     {
      Key = results.value;        // полученный от пульта код записывается в переменную Key                        
      
      if (mode == 0) {             // если машинка все еще в режиме управления пультом (режим «0»), то обрабатывается полученный сигнал      
        switch(Key)
         {
           case IR_Go: advance();         // если нажата клавиша «вверх», то ехать вперед    
           break;
           case IR_Back: back();          // если нажата клавиша «вниз», то ехать назад
           break;
           case IR_Left: turnL();         // если нажата клавиша «налево», то поворачивать налево    
           break;
           case IR_Right: turnR();        // если нажата клавиша «направо», то поворачивать направо
           break;
           case IR_Slowdown: slowdown();  // если нажать кнопку «1», то скорость снизится       
           break;
           case IR_Speedup: speedup();    // если нажать кнопку «2», то скорость повысится       
           break;
           case IR_Stop: stopp();         // если нажать кнопку «OK», то остановиться       
           break;
           case IR_Mode: mode = 1;        // если нажать кнопку «*», то машинка перейдет в режим самоуправления (режим «1»)       
           break;
           default:
           break;      
         }
      } 
     else break;                 // выход из цикла, если на предыдущей итерации был переход в режим «1»

     irrecv.resume();            // получение следующего значения
    }
  }
  }
  stopp();
}

/* ******** Автономный режим ********* */

void Self_Control(void)     // функция для организации самоуправления
{
   int H;                         // переменная для хранения расстояния до препятствия перед машинкой        

   Set_servopulse(midPosition);   // возвращение вала серводвигателя в нейтральную позицию  

   H = Ultrasonic_Ranging();
   delay(300);   
   
   if(H < 15)                     // если препятствие слишком близко, то остановиться и сдать назад           
   {
       stopp();              
       delay(100);
       back();               
       delay(50);
    }
           
  if(H < 35)                      // если впереди препятствие, то…                    
      {
        stopp();                  // остановиться   
        delay(100);            
        Set_servopulse(5);        // повернуть датчик налево
        int L = Ultrasonic_Ranging(); // измерить расстояние до препятствий
        delay(300);      
         Set_servopulse(177);     // повернуть датчик направо   
        int R = Ultrasonic_Ranging(); // измерить расстояние до препятствий   
        delay(300);      

        if(L > R)                 // если слева больше свободного пространства, то…   
        {
           back();                // сдать назад   
           delay(100);      
           turnL();               // повернуть налево
           delay(400);                  
           stopp();               // остановиться
           delay(50);            
           Set_servopulse(midPosition); // установить датчик в нейтральное положение
           H = Ultrasonic_Ranging();    // измерить расстояние   
           delay(500); 
        }
          
        if(L <= R)                  // если справа больше свободного пространства, то…    
        {
           back();                  // сдать назад
           delay(100);  
           turnR();                 // повернуть направо
           delay(400);   
           stopp();                 // остановиться
           delay(50);            
           Set_servopulse(midPosition); // установить датчик в нейтральное положение
           H = Ultrasonic_Ranging();    // измерить расстояние 
           delay(300);        
        }   
        
        if (L < 25 && R < 25)       // если и справа, и слева препятствия слишком близко, то сдать назад     
        {
           stopp();            
           delay(50);
           back(); 
           delay(50);                   
        }          
      }
      else                          // если впереди нет препятствий, то продолжать движение прямо  
      {
      advance();                
      }                 
}

int Ultrasonic_Ranging()  // функция для определения расстояния с помощью ультразвукового датчика расстояния HC-SR04
{ 
  // генерация сигнала для определения расстояния:          
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
     
    int distance = pulseIn(echoPin, HIGH);      // определение частоты входного сигнала
    distance= distance/58;                      // перевод частоты в расстояние
     
    return distance;
} 

/* ******************************* */


void setup() 
{ 
   M_Control_IO_config();           // конфигурирование портов Arduino      
   Set_Speed(LRpwm_val, LRpwm_val); // установка изначальной скорости вращения колес
   //irrecv.enableIRIn();             // включение приёма команд от пульта   
   Serial.begin(9600);              // инициализация последовательного порта со скоростью 9600 бод
   stopp();                         // остановка машинки 

   pinMode(servopin,OUTPUT);        // порт управления серводвигателем
   Set_servopulse(midPosition);     // установка датчика расстояния в нейтральное положение
   pinMode(echoPin, INPUT);         // порт считывания показаний с датчика расстояния   
   pinMode(trigPin, OUTPUT);        // порт отправки сигналов на датчик расстояния            
   
}

void loop() 
{  
   //if (irrecv.decode(&results)) {
     // IR_Control();
      //irrecv.resume();        // приём следующего значения    
  //} 
  
  //if (mode==1)                // если выбран автономный режим, то выполнить функцию самоуправления   
    Self_Control(); 
}
