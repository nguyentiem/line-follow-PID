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
int sgn[5]; // 1 2 3 4 5 ->
int error = 0 ;
int preError = 0;
int Kp = 100, Ki = 0 , Kd = 340;

int P, I, D, PIDvalue;



void readSgn() {
  sgn[0] = digitalRead(cb1) ^ 1;
  sgn[1] = digitalRead(cb2) ^ 1;
  sgn[2] = digitalRead(cb3) ^ 1;
  sgn[3] = digitalRead(cb4) ^ 1;
  sgn[4] = digitalRead(cb5) ^ 1;

}
void caculateError() {
  readSgn();
  if ((sgn[0] == 0 ) && (sgn[1] == 0 ) && (sgn[2] == 0 ) && (sgn[3] == 0 ) && (sgn[4] == 1 )) error = 4;

  else if ((sgn[0] == 0 ) && (sgn[1] == 0 ) && (sgn[2] == 0 ) && (sgn[3] == 1 ) && (sgn[4] == 1 )) error = 3;

  else if ((sgn[0] == 0 ) && (sgn[1] == 0 ) && (sgn[2] == 0 ) && (sgn[3] == 1 ) && (sgn[4] == 0 )) error = 2;

  else if ((sgn[0] == 0 ) && (sgn[1] == 0 ) && (sgn[2] == 1 ) && (sgn[3] == 1 ) && (sgn[4] == 0 )) error = 1;

  else if ((sgn[0] == 0 ) && (sgn[1] == 0 ) && (sgn[2] == 1 ) && (sgn[3] == 0 ) && (sgn[4] == 0 )) error = 0;

  else if ((sgn[0] == 0 ) && (sgn[1] == 1 ) && (sgn[2] == 1 ) && (sgn[3] == 0 ) && (sgn[4] == 0 )) error = - 1;

  else if ((sgn[0] == 0 ) && (sgn[1] == 1 ) && (sgn[2] == 0 ) && (sgn[3] == 0 ) && (sgn[4] == 0 )) error = -2;

  else if ((sgn[0] == 1 ) && (sgn[1] == 1 ) && (sgn[2] == 0 ) && (sgn[3] == 0 ) && (sgn[4] == 0 )) error = -3;

  else if ((sgn[0] == 1 ) && (sgn[1] == 0 ) && (sgn[2] == 0 ) && (sgn[3] == 0 ) && (sgn[4] == 0 )) error = -4;
  else{
     error = 0;
     P = error;
     I = 0;
     D = 0;
     PIDvalue = 0;
     preError = error;
    }
    
}

void Stop() {
  digitalWrite(dc1a, LOW);
  digitalWrite(dc1b, LOW);
  digitalWrite(dc2a, LOW);
  digitalWrite(dc2b, LOW);
}

void calculatePID()
{
  caculateError(); 
  P = error;
  I = I + error;
  D = error - preError;
  PIDvalue = (Kp * P) + (Ki * I) + (Kd * D);
  preError = error;
}

void control() {
  calculatePID();
  analogWrite(dc1a, 512*(1-abs(P*0.1)+0.1) + PIDvalue);
  analogWrite(dc2a, 512*(1-abs(P*0.1)+0.1  s) - PIDvalue);

}

void setup() {
  Serial.begin(9600);
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

}

void loop() {
 control();
}
