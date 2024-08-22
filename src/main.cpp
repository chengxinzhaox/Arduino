#include <Arduino.h>
/*
19 MC猜数字 - 1 电路搭建
太极创客
www.taichi-maker.com

此程序用于太极创客<<零基础入门学用Arduino教程>>中
MC猜数字电路测试使用。

2017-04-21
*/
void setup() {
  pinMode(2, INPUT_PULLUP);
  for (int i = 2; i <= 9; i++) {
    pinMode(i, OUTPUT);
  }
}

void displayClear(){
  // 清理显示内容
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
}

void displayNumber(int number){
  switch(number){
    case 1:
      digitalWrite(4, HIGH);
    digitalWrite(7, HIGH);
    break;
    case 2:
      digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    break;
    case 3:
      digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    break;
    case 4:
      digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    break;
    case 5:
      digitalWrite(3, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    break;
    case 6:
      digitalWrite(3, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    break;
    case 7:
      digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(7, HIGH);
    break;
    case 8:
      digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    break;
    case 9:
      digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    break;
    case 0:
      digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    break;
    default:
      digitalWrite(5, HIGH);
  }
}

// the loop function runs over and over again forever
void loop() {
  int myNumber = 0;
  while(myNumber <= 9){
    displayNumber(myNumber);
    delay(500);
    displayClear();
    myNumber++;
    while(!digitalRead(2)){
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
    }
    delay(500);
  }
}