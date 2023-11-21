#define SERIAL_USB

int normal1 = 38;  //38
int normal2 = 36;  //36
int normal3 = 34;  //34
int normal4 = 32;  //32
int normal5 = 30;  //30
int normal6 = 28;  //28
int normal7 = 26;  //26

int peking1 = 41; //53
int peking2 = 43; //51
int peking3 = 45; //49
int peking4 = 47; //47
int peking5 = 49; //45
int peking6 = 51; //43
int peking7 = 53; //41

int kenong1 = 39; //39
int kenong2 = 37; //37
int kenong3 = 35; //35
int kenong5 = 33; //33 
int kenong6 = 31; //31
int kenong7 = 29; //29 
int gong = 27; //27

enum Playing {
  STOP,
  PLAY
};

int state = LOW;
int val = 0;
int index;
int ketKenong = 0;
int ketGong = 0;
int ketPengulangan = 0;
int interval = 500;
int tempInterval = 500;
int strInterval = 500;
int dataLength;
String data = "";
String input;
String command;
String value;
Playing playing = STOP;

int note;
int normal;
int peking;
int kenong;
int touchTime = 100;

int totalNote = 1;
int i;

void setup() {
  #ifdef NATIVE_USB
    SerialUSB.begin(1);  //Baudrate is irevelant for Native USB
  #endif

  #ifdef SERIAL_USB
    Serial.begin(9600);
    while (!Serial)
      ;  // wait for Leonardo enumeration, others continue immediately
  #endif

  pinMode(normal1, OUTPUT);
  pinMode(normal2, OUTPUT);
  pinMode(normal3, OUTPUT);
  pinMode(normal4, OUTPUT);
  pinMode(normal5, OUTPUT);
  pinMode(normal6, OUTPUT);
  pinMode(normal7, OUTPUT);

  pinMode(peking1, OUTPUT);
  pinMode(peking2, OUTPUT);
  pinMode(peking3, OUTPUT);
  pinMode(peking4, OUTPUT);
  pinMode(peking5, OUTPUT);
  pinMode(peking6, OUTPUT);
  pinMode(peking7, OUTPUT);

  pinMode(kenong1, OUTPUT);
  pinMode(kenong2, OUTPUT);
  pinMode(kenong3, OUTPUT);
  pinMode(kenong5, OUTPUT);
  pinMode(kenong6, OUTPUT);
  pinMode(kenong7, OUTPUT);
  pinMode(gong, OUTPUT);

  digitalWrite(normal1, HIGH);
  digitalWrite(normal2, HIGH);
  digitalWrite(normal3, HIGH);
  digitalWrite(normal4, HIGH);
  digitalWrite(normal5, HIGH);
  digitalWrite(normal6, HIGH);
  digitalWrite(normal7, HIGH);

  digitalWrite(peking1, HIGH);
  digitalWrite(peking2, HIGH);
  digitalWrite(peking3, HIGH);
  digitalWrite(peking4, HIGH);
  digitalWrite(peking5, HIGH);
  digitalWrite(peking6, HIGH);
  digitalWrite(peking7, HIGH);

  digitalWrite(kenong1, HIGH);
  digitalWrite(kenong2, HIGH);
  digitalWrite(kenong3, HIGH);
  digitalWrite(kenong5, HIGH);
  digitalWrite(kenong6, HIGH);
  digitalWrite(kenong7, HIGH);
  digitalWrite(gong, HIGH);
}

// Run forever
void loop() {
  if (playing == PLAY) {
    value = "play:" + String(interval) + ":" + String(ketPengulangan);
    sendData(value);
    note = String(data[index]).toInt();
    if (note > 0) {
      if (note == 1) {
        normal = normal1;
        peking = peking1;
        kenong = kenong1;
      } else if (note == 2) {
        normal = normal2;
        peking = peking2;
        kenong = kenong2;
      } else if (note == 3) {
        normal = normal3;
        peking = peking3;
        kenong = kenong3;
      } else if (note == 4) {
        normal = normal4;
        peking = peking4;
        kenong = kenong5;
      } else if (note == 5) {
        normal = normal5;
        peking = peking5;
        kenong = kenong5;
      } else if (note == 6) {
        normal = normal6;
        peking = peking6;
        kenong = kenong6;
      } else if (note == 7) {
        normal = normal7;
        peking = peking7;
        kenong = kenong7;
      }

      digitalWrite(normal, LOW);
      digitalWrite(peking, LOW);
      ++ketKenong;
      if (ketKenong >= 2) {
        digitalWrite(kenong, LOW);
        ketKenong = 0;
      }

      ++ketGong;
      if (ketGong >= 8) {
        digitalWrite(gong, LOW);
        ketGong = 0;
      }

      delay(touchTime);
      digitalWrite(normal, HIGH);
      digitalWrite(peking, HIGH);
      digitalWrite(kenong, HIGH);
      digitalWrite(gong, HIGH);
      
      delay(interval / 2 - touchTime);
      digitalWrite(peking, LOW);
      
      delay(touchTime);
      digitalWrite(peking, HIGH);
      
      delay(interval / 2 - touchTime);
    } else {
      for (i = 0; i < 5; i++) {
        delay(interval / 5);
      }
    }

    index++;
    if (index >= totalNote - 3) {
      ketPengulangan++;
      if(ketPengulangan > 2) {
        interval = tempInterval;
        index = 0;
        ketPengulangan = 0;
        sendData("finish");
        playing = STOP;        
      } else {        
        index = 1;
        ketGong = 0;
      } 
    }
    
    if(ketPengulangan >= 2) {
      if(index >= totalNote - 11) {
        if(interval != tempInterval * 1.5) { 
          interval = tempInterval * 1.5;
        }

        if(index == totalNote - 7) {                    
          interval = tempInterval * 2;
        } else if (index == totalNote - 6) {
          interval = tempInterval * 2.25;
        } else if (index == totalNote - 5) {
          interval = tempInterval * 2.5;
        } else if (index == totalNote - 4) {
          interval = tempInterval * 3;
        }
      }
    }
  } else {
    sendData("standby");
    delay(100);
  }
}

void sendData(String data) {
  #ifdef NATIVE_USB
    SerialUSB.println(data);  // need a end-line because wrmlh.csharp use readLine method to receive data
  #endif

  #ifdef SERIAL_USB
    Serial.println(data);  // need a end-line because wrmlh.csharp use readLine method to receive data
  #endif
}

void serialEvent() {
  if (Serial.available()) {
    #ifdef NATIVE_USB
        input = SerialUSB.readString();
    #endif
    #ifdef SERIAL_USB
        input = Serial.readString();
    #endif

    sendData("input: " + input);
    command = input.substring(0, 4);
    if (command == "data") {
      value = getValue(input, 58, 1);
      interval = value.toInt();
      tempInterval = value.toInt();
      value = getValue(input, 58, 2);
      sendData("DATA: " + value);
      if (value != data) {
        data = value;
        index = 0;
        ketKenong = 0;
        ketGong = 0;
        ketPengulangan = 0;
        totalNote = data.length();
        playing = STOP;
      }
    }
    if (command == "play") {
      playing = PLAY;
      sendData("playing: " + data);
    }
    if (command == "stop") {
      playing = STOP;
      index = 0;
      sendData("stopping");
    }
    if (command == "spee") {
      value = getValue(input, 58, 1);
      interval = value.toInt();
      tempInterval = value.toInt();
      sendData("speed: " + value);
    }
  }
}

String getValue(String str, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = str.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (str.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? str.substring(strIndex[0], strIndex[1]) : "";
}