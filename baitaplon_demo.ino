#include <ESP8266WiFi.h>
int cb1 = 14;
int cb2 = 12;
int cb3 = 13;
int cb4 = 5;
int cb5 = 4;
int dc1a = 15;
int dc1b = 16;
int dc2a = 0;
int dc2b = 2;

int preDoc=4; ; // luu trang thai den truoc  
int flag =0 ; // luu trang thai doc
int preFlag = flag ;  // trang thai doc luc truoc 
int flagLine =0 ;
int error = 0;
int preError = 0;
int P, D, PIDvalue;
int Kp = 200 , Kd = 300;   // 230 -300
byte sgn[5]; // luu cac den 1 2 3 4 5 ->
int arrError[5];  
int PWM[5]= {900,850,700,650,600};
int heso[5]= {100, 150,200,230,260}; 
int result[30];
const char* ssid = "cslab-505";
const char* password = "ccslab-505";

WiFiServer server(80); // khoi tao socket
WiFiClient client;
IPAddress staticIP(192, 168, 15, 30);
IPAddress gateway(192, 168, 15, 1);
IPAddress subnet(255, 255, 255, 0);

void setupWifi()
{
   Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s \n", ssid);
  WiFi.begin(ssid, password);
  WiFi.config(staticIP, gateway, subnet);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
   Serial.println();
  
  if (WiFi.status() == WL_CONNECTED)
  {
 
  Serial.printf("Connected, mac address: %s\n", WiFi.macAddress().c_str());  
  Serial.println();
  Serial.print("DNS #1, #2 IP: ");
  WiFi.dnsIP().printTo(Serial);
  Serial.print(", ");
  WiFi.dnsIP(1).printTo(Serial);
  Serial.println();
  Serial.printf("pass: %s ", WiFi.psk().c_str() );
  Serial.println();
  server.begin();
  }
}

void Stop() {
  digitalWrite(dc1a, LOW);
  digitalWrite(dc1b, LOW);
  digitalWrite(dc2a, LOW);
  digitalWrite(dc2b, LOW);
}

byte  readSgn() {
   byte a =0; 
  sgn[0] = digitalRead(cb1);
  sgn[1] = digitalRead(cb2);
  sgn[2] = digitalRead(cb3) ;
  sgn[3] = digitalRead(cb4);
  sgn[4] = digitalRead(cb5);
 
  if(sgn[0]==sgn[4]&&sgn[0]==1){
    flag = 0; // nen mau den vach trang 
  }else{
     if(sgn[0]==sgn[4]&&sgn[0]==0)
        flag =1;  // nen mau trang vach den
     else{
         flag = preFlag; 
     } 
  }
 if(flag==0){
   sgn[0]^=1;
    sgn[1]^=1;
    sgn[2]^=1;
    sgn[3]^=1; 
    sgn[4]^=1;
  }
    sgn[1]<<=1;
    sgn[2]<<=2;
    sgn[3]<<=3; 
    sgn[4]<<=4; 
    a = sgn[0]|sgn[1]|sgn[2]|sgn[3]|sgn[4];
   return a; 
}


void calculatePID()
{
  byte t =readSgn();
  if(t==0 ){  // trang thai ra khoi duong  //xet pre -4->4
    if(preDoc==0||flagLine==15){
    digitalWrite(dc1a, LOW);
    digitalWrite(dc2a, LOW);
    while(t==0){
      // dieu khien di lui
        analogWrite(dc1b, 450);
        analogWrite(dc2b, 450);
        t = readSgn();
     }
     flagLine=0; 
     }else{
      t =preDoc;
      flagLine+=1; 
      }
    }
        error=result[t];
        if(error==10) {
        error =  preError;
        }  
      
   arrError[4]=  arrError[3];
   arrError[3]=  arrError[2]; 
   arrError[2]=  arrError[1];
   arrError[1]=  preError;
   arrError[0]= error;   
  preDoc = t ; 
  P = error;
  D = error - preError;
  
  PIDvalue = (heso[abs(P)]*P)  + (Kd * D);
   
 result[0] = preError = error;
}

void control() {
 calculatePID();
 int td1,td2 ; 
 
 int tem =0 ; 
  for(int i =0; i<=4 ;i++){
    if(arrError[i]>tem)tem = arrError[i]; 
  }
  td1 = (PWM[abs(tem)] + PIDvalue)>1023?1023:(PWM[abs(tem)] + PIDvalue);
  td2 = (PWM[abs(tem)] - PIDvalue)>1023?1023: (PWM[abs(tem)] - PIDvalue); 

  if(td1<0){
     digitalWrite(dc1a, LOW);
     analogWrite(dc1b, abs(td2));//td2
     analogWrite(dc2a, td2);
    }else{
      if(td2<0){
        
          digitalWrite(dc2a, LOW);
         analogWrite(dc1a, td1);
         analogWrite(dc2b,abs(td2) );//td1
        }else{
            digitalWrite(dc1b, LOW);
            digitalWrite(dc2b, LOW);
           analogWrite(dc1a, td1);
           analogWrite(dc2a, td2);
          }
      }
}
int compare(char*a,char*b,int len){
   for(int i =0; i<len; i++){
      if(a[i]!=b[i]) return 0;
    }
    return 1;
  }
void connectWifi() {
  client = server.available();
  if (client)
  {
    while (client.connected())
    {
      Stop();
      if (client.available())
      {
        char line[100];
        memset(line, 0, 100);
        client.readBytes(line, 100);

        if (compare(line,"lui",strlen("lui")))
        { client.println("di lui !");
          digitalWrite(dc1b, HIGH);
          digitalWrite(dc2b, HIGH);
         
          delay(500);
          digitalWrite(dc1b, LOW);
          digitalWrite(dc2b, LOW);
        }
        else if (compare(line,"tien",strlen("tien"))) { 
          client.println("di tien !");
       
           digitalWrite(dc1a, HIGH);
          digitalWrite(dc2a, HIGH);
         
          delay(500);
          digitalWrite(dc1a, LOW);
          digitalWrite(dc2a, LOW);
        }
        else {
          if (compare(line,"start",strlen("start"))) {
            client.println("xe chay!");
            while (!client.available()) {
              control();
            }
          }
          else {
             if (compare(line,"stop",strlen("stop")))
            {
              client.println("Stop!");
               Stop();
            } 
          }

        }
      }

    }
    client.stop();
  }
}

void setup() {
   arrError[0]= error; 
   arrError[1]=  arrError[0]; 
   arrError[2]=  arrError[1]; 
   arrError[3]=  arrError[2]; 
   arrError[4]=  arrError[3]; 
  for(int i = 0; i < 30;i++) result[i]=10; 
   result[7]=-4;
   result[0]=preError; 
   result[28]= 4;
   result[1]= -4;
   result[2]= -2;
   result[3]=-3;
   result[4]= 0;
   result[6]=-1; 
   result[12]= 1;
   result[8]= 2;
   result[24]=3;
   result[16]= 4;
    
  pinMode(cb1, INPUT);
  pinMode(cb2, INPUT);
  pinMode(cb3, INPUT);
  pinMode(cb4, INPUT);
  pinMode(cb5, INPUT);
  pinMode(dc1a, OUTPUT);
  pinMode(dc1b, OUTPUT);
  pinMode(dc2a, OUTPUT);
  pinMode(dc2b, OUTPUT);
  digitalWrite(dc1b, LOW);
  digitalWrite(dc2b, LOW);
  Stop();
  //setupWifi();
  Serial.begin(9600); 
}

void loop() {
 
//  connectWifi();
control();
Serial.println(error);
 
  
    }
