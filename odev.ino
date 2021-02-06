//Abdullah TUNÇER - 1721012045
//

#include <Arduino_FreeRTOS.h>
#include <Servo.h>
#include <queue.h>
#include <LiquidCrystal.h>

//lcd
LiquidCrystal lcd(12, 13, 2, 3, 4, 5); //rs,e,d4,d5,d6,d7

//sıcaklık sensörü
int okunan_deger=0;
float sicaklik_gerilim=0;

//kuyruklar
QueueHandle_t xQueue_Sicaklik;
QueueHandle_t xQueue_Gaz;

//Led
int LED1=40;
int LED2=41;
int LED3=42;
int LED4=43;
bool durum=false;

//servo
Servo servo1;
Servo servo2;


//parametre gönderme
static const char *servoSec1="Servo1";
static const char *servoSec2="Servo2";


void setup() {
  // put your setup code here, to run once:

  //LED
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);

  lcd.begin(16,2);

  //Kuyruklar oluşturuluyor
  xQueue_Sicaklik=xQueueCreate(5,sizeof(int32_t));
  xQueue_Gaz=xQueueCreate(5,sizeof(int32_t));

  servo1.attach(9);
  servo2.attach(10);

  
  //Görevler oluşturuluyor
  xTaskCreate(vGorevLED1,
              "GorevLED1",
              100,
              NULL,
              2,
              NULL);
  xTaskCreate(vGorevLED2,
              "GorevLED2",
              100,
              NULL,
              2,
              NULL);
  xTaskCreate(vGorevLED3,
              "GorevLED3",
              100,
              NULL,
              2,
              NULL);

  xTaskCreate(vGorevLED4,
              "GorevLED2",
              100,
              NULL,
              2,
              NULL);                            

  xTaskCreate(vGorevSERVO,
              "GorevServo1",
              128,
              (void*)servoSec1,
              2,
              NULL);

  xTaskCreate(vGorevSERVO,
              "GorevServo2",
              128,
              (void*)servoSec2,
              2,
              NULL);

  xTaskCreate(vGonderSicaklik,
              "vGonderSicaklik",
              128,
              NULL,
              1,
              NULL);
              
  xTaskCreate(vGonderGaz,
              "vGonderGaz",
              128,
              NULL,
              1,
              NULL);

  xTaskCreate(vLCDYazdir,
              "vLCDYazdir",
              200,
              NULL,
              2,
              NULL);  
     
}

void loop() {
  // put your main code here, to run repeatedly:

}

void vGorevLED1(void *pvParameters){
  Serial.begin(9600);
  (void) pvParameters;
  for(;;){
    if(durum==false){
      vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    digitalWrite(LED1,LOW);
    //Serial.println("Görev LED1");
    vTaskDelay(3000/portTICK_PERIOD_MS);
    digitalWrite(LED1,HIGH);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    durum=true;
  }
}


void vGorevLED2(void *pvParameters){
  Serial.begin(9600);
  (void) pvParameters;
  for(;;){
    if(durum==false){
      vTaskDelay(2000/portTICK_PERIOD_MS);
    }
    digitalWrite(LED2,LOW);
    //Serial.println("Görev LED2");
    vTaskDelay(3000/portTICK_PERIOD_MS);
    digitalWrite(LED2,HIGH);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    durum=true;
  }
}

void vGorevLED3(void *pvParameters){
  Serial.begin(9600);
  (void) pvParameters;
  for(;;){
    if(durum==false){
      vTaskDelay(3000/portTICK_PERIOD_MS);
    }
    digitalWrite(LED3,LOW);
    //Serial.println("Görev LED3");
    vTaskDelay(3000/portTICK_PERIOD_MS);
    digitalWrite(LED3,HIGH);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    durum=true;
  }
}

void vGorevLED4(void *pvParameters){
  Serial.begin(9600);
  (void) pvParameters;
  for(;;){
    if(durum==false){
      vTaskDelay(4000/portTICK_PERIOD_MS);
    }
    digitalWrite(LED4,LOW);
    //Serial.println("Görev LED4");
    vTaskDelay(3000/portTICK_PERIOD_MS);
    digitalWrite(LED4,HIGH);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    durum=true;
  }
}



void vGorevSERVO(void *pvParameters){
  char *pcTaskName;
  pcTaskName= (void *)pvParameters;
  Serial.begin(9600);
  for(;;){
    //Serial.println("Servolar");
    
    if (pcTaskName=="Servo1") servo1.write(100);
    if (pcTaskName=="Servo2") servo2.write(180);

    vTaskDelay(1000/portTICK_PERIOD_MS);
    
    if (pcTaskName=="Servo1") servo1.write(20);
    if (pcTaskName=="Servo2") servo2.write(0);
    
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void vGonderSicaklik(void *pvParameters){
  float sicaklik;
  BaseType_t xDurum;
  vTaskDelay(1000/portTICK_PERIOD_MS);
  for(;;){
    okunan_deger=analogRead(A10);
    sicaklik_gerilim=(okunan_deger / 1023.0)*5000;
    sicaklik=sicaklik_gerilim/10.0;
    xDurum=xQueueSendToBack(xQueue_Sicaklik, &sicaklik, 0);  // son parametre bekleme süresindir. 0 doğrudan göndermeyi sağlar
    /*
    if(xDurum ==pdPASS){
      Serial.print("Göndrildi Sıcaklık = ");
      Serial.println(sicaklik);
    }
    else{
      Serial.println("Kuyruğa yazdırılamadı");
    }*/
    vTaskDelay(1000/portTICK_PERIOD_MS);
    taskYIELD();
  }
}

void vGonderGaz(void *pvParameters){
  int32_t gazDegeri=0;
  BaseType_t xDurum;
  vTaskDelay(1000/portTICK_PERIOD_MS);
  for(;;){
    gazDegeri=analogRead(A2);
    xDurum=xQueueSendToBack(xQueue_Gaz, &gazDegeri, 0);  // son parametre bekleme süresindir. 0 doğrudan göndermeyi sağlar
    /*
    if(xDurum ==pdPASS){
      Serial.print("Gönderildi Gaz= ");
      Serial.println(gazDegeri);
    }
    else{
      Serial.println("Kuyruğa yazdırılamadı");
    }
    */
    vTaskDelay(1000/portTICK_PERIOD_MS);
    taskYIELD();
  }
  
}


void vLCDYazdir(void *pvParameters){
  float sicaklikAl=0;
  int32_t gazAl=0;
  Serial.begin(9600);
  Serial.println("CLEARDATA");
  Serial.println("LABEL, TARIH, ZAMAN, SICAKLIK SENSORU, GAZ SENSORU");
  
  for(;;){
    lcd.clear();
    
    Serial.print("DATA, DATE, TIME,");
    
    if(xQueueReceive(xQueue_Sicaklik,&sicaklikAl,portMAX_DELAY)==pdPASS){
      //Serial.print("Alındı Sıcaklık= ");
      //Serial.println(sicaklikAl);
      
      Serial.print(sicaklikAl);
      Serial.print(",");
      
      lcd.setCursor(0,0);
      lcd.print("Sicaklik:");
      lcd.print(sicaklikAl);
    }
    if(xQueueReceive(xQueue_Gaz,&gazAl,portMAX_DELAY)==pdPASS){
      //Serial.print("Alındı Gaz= ");
      //Serial.println(gazAl);
      
      Serial.println(gazAl);
      
      lcd.setCursor(0,1);
      if(gazAl>550){
        lcd.print("Gaz Var");
      }
      else{
        lcd.print("Gaz Yok");
      }
    }
    vTaskDelay(60000/portTICK_PERIOD_MS);
    taskYIELD();
  }
}
