#include <SevSeg.h>
#include <Rotary.h>
#include <OneButton.h>
#include <EEPROM.h>

#define regD 16
#define regL 17
#define regC 19
#define clockIn 21
#define resetIn 20
SevSeg sevseg;
Rotary rotary = Rotary(3, 2);
OneButton coderButton(15, true);
OneButton saveButton(18, true);

int menuSwitch = 0;
int writeto;
int channelSelect;
bool inPrev;
uint8_t clockVar[9];
uint8_t writetoBIN = 0B010;
bool clockToggleTrigger;
int triggerLenght = 30;//ms
int triggerLenghtLast;
long Pmill;
int presetPosition;
int deepPosition;
int bootLoadPreset;
int bootLoadPresetLast;
int animationPosition;
bool resetPosition;
bool saveLoad;
bool resetting;
long PmillAnimationChange;
long PmillfallingLightAnimation;
long PmillanimationTimeout;
long Pmillanimation1;
long Pmillanimation2;
long Pmillanimation3;
long Pmillanimation4;
byte animation1FrameCounter;
byte animation2FrameCounterBlue;
byte animation2FrameCounterGreen;
byte animation2FrameCounterDisplay;
byte animation3FrameCounter;
byte animation4FrameCounter;
bool animationsONOFF = 0;
uint8_t blueLedsAnimation;
uint8_t greenLedsAnimation;
uint8_t displayAnimation[4];
bool animationEnable = 1;
byte animationTimeout = 1;
bool animationWakeWithSave = 1;
byte animationChangeTime = 1;
bool animationEnableLast;
byte animationTimeoutLast;
bool animationWakeWithSaveLast;
byte animationChangeTimeLast;
bool resetAnimationTimeout;
byte animationNumber;
byte animationNumberLast;

uint8_t factoryResetControllLedsFrames[44] = {
  B000,//0
  B100,//1
  B110,//2
  B111,//3
  B111,//4
  B111,//5
  B111,//6
  B111,//7
  B111,//8
  B111,//9
  B111,//10
  B111,//11
  B111,//12
  B111,//13
  B111,//14
  B111,//15
  B111,//16
  B111,//17
  B111,//18
  B111,//19
  B111,//20
  B111,//21
  B111,//22
  B111,//23
  B000,//24
  B111,//25
  B000,//26
  B111,//27
  B000,//28
  B111,//29
  B000,//30
  B111,//31
  B000,//32
  B111,//33
  B000,//34
  B111,//35
  B000,//36
  B111,//37
  B000,//38
  B111,//39
  B000,//40
  B111,//41
  B000,//42
};
uint8_t factoryResetDisplayFrames[44][4] = {
  {B00000000, B00000000, B00000000}, //0
  {B00000000, B00000000, B00000000}, //1
  {B00000000, B00000000, B00000000}, //2
  {B00000000, B00000000, B00000000}, //3
  {B00100000, B00000000, B00000000}, //4
  {B00100001, B00000000, B00000000}, //5
  {B00100001, B00000001, B00000000}, //6
  {B00100001, B00000001, B00000001}, //7
  {B00100001, B00000001, B00000011}, //8
  {B00100001, B00000001, B00000111}, //9
  {B00100001, B00000001, B00001111}, //10
  {B00100001, B10001001, B00001111}, //11
  {B10101001, B10001001, B00001111}, //12
  {B10111001, B10001001, B00001111}, //13
  {B11111001, B10001001, B00001111}, //14
  {B11111111, B10111001, B00001111}, //15
  {B11111111, B11111001, B00001111}, //16
  {B11111111, B11111111, B00111111}, //17
  {B11111111, B11111111, B01111111}, //18
  {B11111111, B11111111, B01111111}, //19
  {B11111111, B11111111, B01111111}, //20
  {B11111111, B11111111, B01111111}, //21
  {B11111111, B11111111, B01111111}, //22
  {B11111111, B11111111, B01111111}, //23
  {B00000000, B00000000, B00000000}, //24
  {B11111111, B11111111, B01111111}, //25
  {B00000000, B00000000, B00000000}, //26
  {B11111111, B11111111, B01111111}, //27
  {B00000000, B00000000, B00000000}, //28
  {B11111111, B11111111, B01111111}, //29
  {B00000000, B00000000, B00000000}, //30
  {B11111111, B11111111, B01111111}, //31
  {B00000000, B00000000, B00000000}, //32
  {B11111111, B11111111, B01111111}, //33
  {B00000000, B00000000, B00000000}, //34
  {B11111111, B11111111, B01111111}, //35
  {B00000000, B00000000, B00000000}, //36
  {B11111111, B11111111, B01111111}, //37
  {B00000000, B00000000, B00000000}, //38
  {B11111111, B11111111, B01111111}, //39
  {B00000000, B00000000, B00000000}, //40
  {B11111111, B11111111, B01111111}, //41
  {B00000000, B00000000, B00000000}, //42
};
uint8_t factoryResetSelectLedsFrames[44] = {
  B00000000,//0
  B00000000,//1
  B00000000,//2
  B00000000,//3
  B00000000,//4
  B00000000,//5
  B00000000,//6
  B00000000,//7
  B00000000,//8
  B00000000,//9
  B00000000,//10
  B00000000,//11
  B00000000,//12
  B00000000,//13
  B00000000,//14
  B00000000,//15
  B00000000,//16
  B00000000,//17
  B00000000,//18
  B00001000,//19
  B10001100,//20
  B11001110,//21
  B01101011,//22
  B01110011,//23
  B00000000,//24
  B11111111,//25
  B00000000,//26
  B00000000,//27
  B00000000,//28
  B11111111,//29
  B00000000,//30
  B00000000,//31
  B00000000,//32
  B11111111,//33
  B00000000,//34
  B00000000,//35
  B00000000,//36
  B11111111,//37
  B00000000,//38
  B00000000,//39
  B00000000,//40
  B11111111,//41
  B00000000,//42
};
uint8_t factoryResetJackLedsFrames[44] = {
  B00000000,//0
  B00000000,//1
  B00000000,//2
  B00000000,//3
  B00000000,//4
  B00000000,//5
  B00000000,//6
  B00000000,//7
  B00000000,//8
  B00000000,//9
  B00000000,//10
  B00000000,//11
  B00000000,//12
  B00000000,//13
  B00000000,//14
  B00000000,//15
  B00000000,//16
  B00000000,//17
  B00000000,//18
  B00010000,//19
  B00110001,//20
  B01110011,//21
  B11010110,//22
  B11001110,//23
  B00000000,//24
  B00000000,//25
  B00000000,//26
  B11111111,//27
  B00000000,//28
  B00000000,//29
  B00000000,//30
  B11111111,//31
  B00000000,//32
  B00000000,//33
  B00000000,//34
  B11111111,//35
  B00000000,//36
  B00000000,//37
  B00000000,//38
  B11111111,//39
  B00000000,//40
  B00000000,//41
  B00000000,//42
};

uint8_t animation1GreenLedsFrames[14] = {
  B001,//0
  B010,//1
  B100,//2
  B000,//3
  B000,//4
  B000,//5
  B000,//6
  B000,//7
  B000,//8
  B000,//9
  B000,//10
  B000,//11
  B000,//12
};
uint8_t animation1DisplayFrames[14][4] = {
  {B00000000, B00000000, B00000000},//0
  {B00000000, B00000000, B00000000},//1
  {B00000000, B00000000, B00000000},//2
  {B00000000, B00000000, B00000010},//3
  {B00000000, B00000000, B00000100},//4
  {B00000000, B00000000, B00000000},//5
  {B00000000, B00000000, B00000000},//6
  {B00000000, B00000000, B00000000},//7
  {B00000000, B00000000, B00000000},//8
  {B00000000, B00000000, B00000000},//9
  {B00000000, B00000000, B00000000},//10
  {B00010000, B00000000, B00000000},//11
  {B00100000, B00000000, B00000000},//12
};
uint8_t animation1BlueLedsFrames[14] = {
  B00000000,//0
  B00000000,//1
  B00000000,//2
  B00000000,//3
  B00000000,//4
  B00001000,//5
  B10000000,//6
  B01000000,//7
  B00100000,//8
  B00010000,//9
  B00000001,//10
  B00000000,//11
  B00000000,//12
};

uint8_t animation2GreenLedsFrames[7] = {
  B001,//0
  B010,//1
  B100,//2
  B100,//3
  B010,//4
  B001,//5
};
uint8_t animation2DisplayFrames[11][4] = {
  {B00000001, B00000000, B00000000},//0
  {B00000000, B00000001, B00000000},//1
  {B00000000, B00000000, B00000001},//2
  {B00000000, B00000000, B00000010},//3
  {B00000000, B00000000, B00000100},//4
  {B00000000, B00000000, B00001000},//5
  {B00000000, B00001000, B00000000},//6
  {B00001000, B00000000, B00000000},//7
  {B00010000, B00000000, B00000000},//8
  {B00100000, B00000000, B00000000},//9
};
uint8_t animation2BlueLedsFrames[9] = {
  B00001000,//0
  B10000000,//1
  B01000000,//2
  B00100000,//3
  B00010000,//4
  B00000001,//5
  B00000010,//6
  B00000100,//7
};

uint8_t animation3GreenLedsFrames[11] = {
  B001,//0
  B001,//1
  B010,//2
  B010,//3
  B100,//4
  B100,//5
  B100,//6
  B010,//7
  B010,//8
  B001,//9
};
uint8_t animation3DisplayFrames[11][4] = {
  {B00110000, B00000000, B00000000},//0
  {B00000110, B00000000, B00000000},//1
  {B00000000, B00110000, B00000000},//2
  {B00000000, B00000110, B00000000},//3
  {B00000000, B00000000, B00110000},//4
  {B00000000, B00000000, B00000110},//5
  {B00000000, B00000000, B00110000},//6
  {B00000000, B00000110, B00000000},//7
  {B00000000, B00110000, B00000000},//8
  {B00000110, B00000000, B00000000},//9
};
uint8_t animation3BlueLedsFrames[11] = {
  B00010001,//0
  B00010001,//1
  B00100010,//2
  B01000100,//3
  B10001000,//4
  B10001000,//5
  B10001000,//6
  B01000100,//7
  B00100010,//8
  B00010001,//9
};

uint8_t animation4GreenLedsFrames[21] = {
  B100, //0
  B010, //1
  B001, //2
  B000, //3
  B000, //4
  B000, //5
  B000, //6
  B000, //7
  B000, //8
  B000, //9
  B000, //10
  B000, //11
  B000, //12
  B000, //13
  B000, //14
  B000, //15
  B000, //16
  B000, //17
  B000, //18
  B000, //19
};
uint8_t animation4DisplayFrames[21][4] = {
  {B00000000, B00000000, B00000000}, //0
  {B00000000, B00000000, B00000000}, //1
  {B00000000, B00000000, B00000000}, //2
  {B00100000, B00000000, B00000000}, //3
  {B00010000, B00000000, B00000000}, //4
  {B00000000, B00000000, B00000000}, //5
  {B00000000, B00000000, B00000000}, //6
  {B00000000, B00000000, B00000000}, //7
  {B00000000, B00000000, B00000000}, //8
  {B00000000, B00010000, B00000000}, //9
  {B00000000, B00100000, B00000000}, //10
  {B00000000, B00000001, B00000000}, //11
  {B00000000, B00000010, B00000000}, //12
  {B00000000, B00000100, B00000000}, //13
  {B00000000, B00000000, B00000000}, //14
  {B00000000, B00000000, B00000000}, //15
  {B00000000, B00000000, B00000000}, //16
  {B00000000, B00000000, B00000000}, //17
  {B00000000, B00000000, B00000100}, //18
  {B00000000, B00000000, B00000010}, //19
};
uint8_t animation4BlueLedsFrames[21] = {
  B00000000, //0
  B00000000, //1
  B00000000, //2
  B00000000, //3
  B00000000, //4
  B00000001, //5
  B00010000, //6
  B00100000, //7
  B00000010, //8
  B00000000, //9
  B00000000, //10
  B00000000, //11
  B00000000, //12
  B00000000, //13
  B00000100, //14
  B01000000, //15
  B10000000, //16
  B00001000, //17
  B00000000, //18
  B00000000, //19
};

uint8_t lenght[9] = {16, 16, 16, 16, 16, 16, 16, 16};
int hits[9] = {4, 4, 4, 4, 4, 4, 4, 4};
int offset[9] = {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t euclidState[9];

void regwrite(uint8_t inputvalue1, uint8_t inputvalue2, uint8_t inputvalue3) {
  digitalWrite(regL, LOW);
  for (int i = 7; i >= 0; i--) {
    digitalWrite(regC, LOW);
    digitalWrite(regD, bitRead(inputvalue1, i));
    digitalWrite(regC, HIGH);
  }
  for (int i = 7; i >= 0; i--) {
    digitalWrite(regC, LOW);
    digitalWrite(regD, bitRead(inputvalue2, i));
    digitalWrite(regC, HIGH);
  }
  for (int i = 7; i >= 0; i--) {
    digitalWrite(regC, LOW);
    digitalWrite(regD, bitRead(inputvalue3, i));
    digitalWrite(regC, HIGH);
  }
  digitalWrite(regL, HIGH);
}

void clockRead() {
  int inState = analogRead(clockIn);
  if (inState > 800 && inPrev == 0) {
    clockVar[0]++;
    clockVar[1]++;
    clockVar[2]++;
    clockVar[3]++;
    clockVar[4]++;
    clockVar[5]++;
    clockVar[6]++;
    clockVar[7]++;
    inPrev = 1;
    clockToggleTrigger = 1;
  }
  if (inState < 200 && inPrev == 1) inPrev = 0;
}

int gate2trigger(uint8_t gate) {
  long Cmill = millis();
  bool writingState;
  uint8_t triggerOut;

  if (clockToggleTrigger == 1) {
    bitWrite(triggerOut, 0, bitRead(gate, 0));
    bitWrite(triggerOut, 1, bitRead(gate, 1));
    bitWrite(triggerOut, 2, bitRead(gate, 2));
    bitWrite(triggerOut, 3, bitRead(gate, 3));
    bitWrite(triggerOut, 4, bitRead(gate, 4));
    bitWrite(triggerOut, 5, bitRead(gate, 5));
    bitWrite(triggerOut, 6, bitRead(gate, 6));
    bitWrite(triggerOut, 7, bitRead(gate, 7));
    Pmill = Cmill;
    clockToggleTrigger = 0;
    writingState = 1;
  }

  if (Cmill - Pmill >= triggerLenght && writingState == 1) {
    bitWrite(triggerOut, 0, 0);
    bitWrite(triggerOut, 1, 0);
    bitWrite(triggerOut, 2, 0);
    bitWrite(triggerOut, 3, 0);
    bitWrite(triggerOut, 4, 0);
    bitWrite(triggerOut, 5, 0);
    bitWrite(triggerOut, 6, 0);
    bitWrite(triggerOut, 7, 0);
    writingState = 0;
  }
  return triggerOut;
}

void totalReset() {
  for (int i; i < 8; i++) {
    clockVar[i] = 0;
  }
}

void sequenceLooparound() {
  for (int i; i < 8; i++) {
    if (clockVar[i] > lenght[i] - 1) clockVar[i] = 0;
  }
}

void refreshEuclid() {
  for (int i; i < 8; i++) {
    euclidState[i] = euclid(lenght[i], hits[i], offset[i]);
  }
}

int channelSelectBIN(int channel) {
  uint8_t output;
  bitWrite(output, channel, 1);
  if (channel < 7) bitWrite(output, channel + 1, 0);
  if (channel > 0) bitWrite(output, channel - 1, 0);
  return output;
}

void clearPreset(int presetNumber) {
  for (int i; i < 8; i++) {
    EEPROM.write((i + 0) + (presetNumber * 24), 0);
    EEPROM.write((i + 8) + (presetNumber * 24), 0);
    EEPROM.write((i + 16) + (presetNumber * 24), 0);
  }
}

void loadPreset(int presetNumber) {
  for (int i; i < 8; i++) {
    lenght[i] = EEPROM.read((i + 0) + (presetNumber * 24));
    hits[i] = EEPROM.read((i + 8) + (presetNumber * 24));
    offset[i] = EEPROM.read((i + 16) + (presetNumber * 24));
    if (lenght[i] > 16) lenght[i] = 16;
    if (hits[i] > 16) hits[i] = 16;
    if (offset[i] > 16) offset[i] = 16;
  }
  refreshEuclid();
}

void savePreset(int presetNumber) {
  for (int i; i < 8; i++) {
    EEPROM.write((i + 0) + (presetNumber * 24), lenght[i]);
    EEPROM.write((i + 8) + (presetNumber * 24), hits[i]);
    EEPROM.write((i + 16) + (presetNumber * 24), offset[i]);
  }
}

void(* resetFunc) (void) = 0;

void factoryResetAction() {
  for (int presetNumber; presetNumber < 16; presetNumber++) {
    for (int i; i < 8; i++) {
      EEPROM.write((i + 0) + (presetNumber * 24), 16);//lenght
      EEPROM.write((i + 8) + (presetNumber * 24), 4);//hits
      EEPROM.write((i + 16) + (presetNumber * 24), 0);//offset
    }
  }
  EEPROM.write(0, 16);//lenght
  EEPROM.write(8, 4);//hits
  EEPROM.write(16, 0);//offset
  EEPROM.write(1, 16);//lenght
  EEPROM.write(9, 4);//hits
  EEPROM.write(17, 0);//offset
  EEPROM.write(2, 16);//lenght
  EEPROM.write(10, 4);//hits
  EEPROM.write(18, 0);//offset
  EEPROM.write(3, 16);//lenght
  EEPROM.write(11, 4);//hits
  EEPROM.write(19, 0);//offset
  EEPROM.write(4, 16);//lenght
  EEPROM.write(12, 4);//hits
  EEPROM.write(20, 0);//offset
  EEPROM.write(5, 16);//lenght
  EEPROM.write(13, 4);//hits
  EEPROM.write(21, 0);//offset
  EEPROM.write(6, 16);//lenght
  EEPROM.write(14, 4);//hits
  EEPROM.write(22, 0);//offset
  EEPROM.write(7, 16);//lenght
  EEPROM.write(15, 4);//hits
  EEPROM.write(23, 0);//offset
  EEPROM.write(1000, 30);
  EEPROM.write(1001, 0);
  EEPROM.write(1002, 1);
  EEPROM.write(1003, 5);
  EEPROM.write(1004, 1);
  EEPROM.write(1005, 5);
  resetFunc();
}

void factoryResetAnimation(int frame) {
  sevseg.setSegments(factoryResetDisplayFrames[frame]);
  regwrite(factoryResetSelectLedsFrames[frame], factoryResetJackLedsFrames[frame], factoryResetControllLedsFrames[frame]);
}

void factoryReset() {
  int i;
  long Pmill;
  while (digitalRead(15) == 1 && digitalRead(3) == 1 && digitalRead(2) == 1 && digitalRead(18) == 1) {
    long Cmill = millis();
    sevseg.refreshDisplay();
    if (Cmill - Pmill >= 200) {
      if (i >= 43) factoryResetAction();
      if (i < 43) factoryResetAnimation(i);
      i++;
      Pmill = Cmill;
      resetting = 1;
    }
  }
}

int outputs4shiftBIN() {
  uint8_t output;
  bitWrite(output, 0, bitRead(euclidState[0], clockVar[0]));
  bitWrite(output, 1, bitRead(euclidState[1], clockVar[1]));
  bitWrite(output, 2, bitRead(euclidState[2], clockVar[2]));
  bitWrite(output, 3, bitRead(euclidState[3], clockVar[3]));
  bitWrite(output, 4, bitRead(euclidState[4], clockVar[4]));
  bitWrite(output, 5, bitRead(euclidState[5], clockVar[5]));
  bitWrite(output, 6, bitRead(euclidState[6], clockVar[6]));
  bitWrite(output, 7, bitRead(euclidState[7], clockVar[7]));
  return output;
}

void rotate() {
  unsigned char result = rotary.process();
  switch (menuSwitch) {
    case (0):
      if (digitalRead(15) == 1) {
        switch (writeto) {
          case (0):
            if (result == DIR_CW) hits[channelSelect]++;
            if (result == DIR_CCW) hits[channelSelect]--;

            if (hits[channelSelect] > lenght[channelSelect]) hits[channelSelect] = lenght[channelSelect];
            if (hits[channelSelect] < 0)  hits[channelSelect] = 0;
            break;

          case (2):
            if (result == DIR_CW) lenght[channelSelect]++;
            if (result == DIR_CCW) lenght[channelSelect]--;

            if (lenght[channelSelect] > 16) lenght[channelSelect] = 16;
            if (lenght[channelSelect] < 0) lenght[channelSelect] = 0;
            break;

          case (1):
            if (result == DIR_CW && lenght != 0) offset[channelSelect]++;
            if (result == DIR_CCW && lenght != 0) offset[channelSelect]--;

            if (offset[channelSelect] > lenght[channelSelect] - 1) offset[channelSelect] = 0;
            if (offset[channelSelect] < 0) offset[channelSelect] = lenght[channelSelect] - 1;
            break;
        }
        euclidState[channelSelect] = euclid(lenght[channelSelect], hits[channelSelect], offset[channelSelect]);
      }
      if (digitalRead(15) == 0) {
        if (result == DIR_CW) channelSelect++;
        if (result == DIR_CCW) channelSelect--;

        if (channelSelect > 7) channelSelect = 7;
        if (channelSelect < 0) channelSelect = 0;
      }
      break;

    case (1):
      if (result == DIR_CW) presetPosition++;
      if (result == DIR_CCW) presetPosition--;

      if (presetPosition > 15) presetPosition = 15;
      if (presetPosition < 0) presetPosition = 0;
      break;

    case (2):
      if (result == DIR_CW) deepPosition++;
      if (result == DIR_CCW) deepPosition--;

      if (deepPosition > 3) deepPosition = 3;
      if (deepPosition < 0) deepPosition = 0;
      break;

    case (3):
      if (result == DIR_CW) triggerLenght += 5;
      if (result == DIR_CCW) triggerLenght -= 5;

      if (triggerLenght > 250) triggerLenght = 250;
      if (triggerLenght < 0) triggerLenght = 0;
      break;

    case (4):
      if (result == DIR_CW) bootLoadPreset++;
      if (result == DIR_CCW) bootLoadPreset--;

      if (bootLoadPreset > 15) bootLoadPreset = 15;
      if (bootLoadPreset < 0) bootLoadPreset = 0;
      break;

    case (5):
      if (result == DIR_CW) animationPosition++;
      if (result == DIR_CCW) animationPosition--;

      if (animationPosition > 3) animationPosition = 3;
      if (animationPosition < 0) animationPosition = 0;
      break;
    case (6):
      if (resetting == 1 && (result == DIR_CW || result == DIR_CCW)) resetting = 0;
      if (resetting == 0) if (result == DIR_CW || result == DIR_CCW) resetPosition = !resetPosition;
      break;

    case (7):
      if (result == DIR_CW || result == DIR_CCW) animationEnable = !animationEnable;
      break;

    case (8):
      if (result == DIR_CW) animationTimeout++;
      if (result == DIR_CCW) animationTimeout--;

      if (animationTimeout > 30) animationTimeout = 30;
      if (animationTimeout < 1) animationTimeout = 1;
      break;

    case (9):
      if (result == DIR_CW || result == DIR_CCW) animationWakeWithSave = !animationWakeWithSave;
      break;

    case (10):
      if (result == DIR_CW) animationChangeTime++;
      if (result == DIR_CCW) animationChangeTime--;

      if (animationChangeTime > 60) animationChangeTime = 60;
      if (animationChangeTime < 1) animationChangeTime = 1;
      break;

  }
  resetAnimationTimeout = 1;
}

void singleCoderClick() {
  switch (menuSwitch) {
    case (0):
      writeto++;
      if (writeto > 2) writeto = 0;
      switch (writeto) {
        case (0):
          bitWrite(writetoBIN, 0, 0);
          bitWrite(writetoBIN, 1, 1);
          bitWrite(writetoBIN, 2, 0);
          break;
        case (1):
          bitWrite(writetoBIN, 0, 0);
          bitWrite(writetoBIN, 1, 0);
          bitWrite(writetoBIN, 2, 1);
          break;
        case (2):
          bitWrite(writetoBIN, 0, 1);
          bitWrite(writetoBIN, 1, 0);
          bitWrite(writetoBIN, 2, 0);
          break;
        default:
          writeto = 0;
          break;
      }
      break;

    case (1):
      menuSwitch = 0;
      if (saveLoad == 0) loadPreset(presetPosition);
      else savePreset(presetPosition);
      break;

    case (2):
      switch (deepPosition) {
        case (0):
          if (digitalRead(15) == 1) menuSwitch = 3;
          triggerLenghtLast = triggerLenght;
          break;

        case (1):
          menuSwitch = 4;
          bootLoadPresetLast = bootLoadPreset;
          break;

        case (2):
          menuSwitch = 5;
          break;

        case (3):
          menuSwitch = 6;
          break;
      }
      break;

    case (3):
      menuSwitch = 2;
      EEPROM.write(1000, triggerLenght);
      break;

    case (4):
      menuSwitch = 2;
      EEPROM.write(1001, bootLoadPreset);
      break;

    case (5):
      switch (animationPosition) {
        case (0):
          menuSwitch = 7;
          animationEnableLast = animationEnable;
          break;
        case (1):
          menuSwitch = 8;
          animationTimeoutLast = animationTimeout;
          break;
        case (2):
          menuSwitch = 9;
          animationWakeWithSaveLast = animationWakeWithSave;
          break;
        case (3):
          menuSwitch = 10;
          animationChangeTimeLast = animationChangeTime;
          break;
      }
      break;

    case (6):
      if (resetting == 0) {
        switch (resetPosition) {
          case (0):
            menuSwitch = 2;
            break;
          case (1):
            factoryReset();
            break;
        }
        break;
      }
      else resetting = 0;

    case (7):
      menuSwitch = 5;
      EEPROM.write(1002, animationEnable);
      break;

    case (8):
      menuSwitch = 5;
      EEPROM.write(1003, animationTimeout);
      break;

    case (9):
      menuSwitch = 5;
      EEPROM.write(1004, animationWakeWithSave);
      break;

    case (10):
      menuSwitch = 5;
      EEPROM.write(1005, animationChangeTime);
      break;
  }
  resetAnimationTimeout = 1;
}

void singleSaveClick() {
  switch (menuSwitch) {
    case (0):
      menuSwitch = 1;
      saveLoad = 0;
      break;

    case (1):
      menuSwitch = 0;
      saveLoad = 0;
      break;

    case (2):
      if (digitalRead(15) == 1) menuSwitch = 0;
      break;

    case (3):
      menuSwitch = 2;
      triggerLenght = triggerLenghtLast;
      break;

    case (4):
      menuSwitch = 2;
      bootLoadPreset = bootLoadPresetLast;
      break;

    case (5):
      menuSwitch = 2;
      break;

    case (6):
      if (resetting == 0) menuSwitch = 2;
      else resetting = 0;
      break;

    case (7):
      menuSwitch = 5;
      animationEnable = animationEnableLast;
      break;

    case (8):
      menuSwitch = 5;
      animationTimeout = animationTimeoutLast;
      break;

    case (9):
      menuSwitch = 5;
      animationWakeWithSave = animationWakeWithSaveLast;
      break;

    case (10):
      menuSwitch = 5;
      animationChangeTime = animationChangeTimeLast;
      break;

    default:
      menuSwitch = 1;
      break;
  }
  if (animationWakeWithSave == 1 || animationsONOFF == 0) resetAnimationTimeout = 1;
  else {
    PmillAnimationChange = millis();
    animationNumber++;
  }
}

void longSavePress() {
  switch (menuSwitch) {
    case (0):
      saveLoad = 1;
      menuSwitch = 1;
      break;

    case (1):
      if (saveLoad == 1) {
        clearPreset(presetPosition);
        menuSwitch = 0;
      }
      break;
  }
}

void sevsegDisplay() {
  if (animationsONOFF == 1) {
    sevseg.setSegments(displayAnimation);
  }
  else {
    switch (menuSwitch) {
      case (0):
        sevseg.setNumber(offset[channelSelect] + lenght[channelSelect] * 400 + hits[channelSelect] * 20, 3, 1);
        break;

      case (1):
        sevseg.setNumber(presetPosition + 1);//save/load
        break;

      case (2)://enter deep menu
        switch (deepPosition) {
          case (0):
            sevseg.setChars("tri");//trigger lenght
            break;

          case (1):
            sevseg.setChars("loa");//boot load
            break;

          case (2):
            sevseg.setChars("ani");//animations
            break;

          case (3):
            sevseg.setChars("rst");//factory reset
            break;
        }
        break;

      case (3):
        sevseg.setNumber(triggerLenght);//adjusting trigger lenght
        break;

      case (4):
        sevseg.setNumber(bootLoadPreset + 1);//adjusting boot load preset
        break;

      case (5):
        switch (animationPosition) {//animation menu
          case (0):
            sevseg.setChars("ena");//animation enable
            break;

          case (1):
            sevseg.setChars("tou");//animation timeout (when will animations turn on)
            break;

          case (2):
            sevseg.setChars("ake");//wake up with save button or use it to change the animation
            break;

          case (3):
            sevseg.setChars("cht");//time when the animation changes
            break;
        }
        break;

      case (6):
        switch (resetPosition) {
          case (0):
            sevseg.setChars("can");
            break;

          case (1):
            sevseg.setChars("ok");
            break;
        }
        break;

      case (7)://adjusting animation enable
        if (animationEnable == 1) sevseg.setChars("ON");
        if (animationEnable == 0) sevseg.setChars("OFF");
        break;

      case (8):
        sevseg.setNumber(animationTimeout);//adjusting animation timeout
        break;

      case (9)://adjusting wake with save
        if (animationWakeWithSave == 1) sevseg.setChars("ENA");
        if (animationWakeWithSave == 0) sevseg.setChars("DIS");
        break;

      case (10):
        sevseg.setNumber(animationChangeTime);//adjusting animation change time
        break;
    }
  }
}

uint8_t variableSwitcher(uint8_t variable0, uint8_t variable1, bool Bswitch) {
  uint8_t out;
  if (Bswitch == 1) out = variable1;
  else out = variable0;
  return out;
}

void fallingLightAnimation(int timing) {
  long Cmill = millis();
  if (Cmill - PmillfallingLightAnimation >= timing) {
    bitWrite(blueLedsAnimation, 7, bitRead(blueLedsAnimation, 3));
    bitWrite(blueLedsAnimation, 6, bitRead(blueLedsAnimation, 2));
    bitWrite(blueLedsAnimation, 5, bitRead(blueLedsAnimation, 1));
    bitWrite(blueLedsAnimation, 4, bitRead(blueLedsAnimation, 0));

    bitWrite(blueLedsAnimation, 3, bitRead(displayAnimation[2], 3));
    bitWrite(blueLedsAnimation, 2, bitRead(displayAnimation[1], 3));
    bitWrite(blueLedsAnimation, 1, bitRead(displayAnimation[1], 3));
    bitWrite(blueLedsAnimation, 0, bitRead(displayAnimation[0], 3));

    bitWrite(displayAnimation[0], 3, bitRead(displayAnimation[0], 6));
    bitWrite(displayAnimation[1], 3, bitRead(displayAnimation[1], 6));
    bitWrite(displayAnimation[2], 3, bitRead(displayAnimation[2], 6));

    bitWrite(displayAnimation[0], 6, bitRead(displayAnimation[0], 0));
    bitWrite(displayAnimation[1], 6, bitRead(displayAnimation[1], 0));
    bitWrite(displayAnimation[2], 6, bitRead(displayAnimation[2], 0));

    bitWrite(displayAnimation[0], 0, bitRead(greenLedsAnimation, 0));
    bitWrite(displayAnimation[1], 0, bitRead(greenLedsAnimation, 1));
    bitWrite(displayAnimation[2], 0, bitRead(greenLedsAnimation, 2));

    bitWrite(greenLedsAnimation, 2, random(2));
    bitWrite(greenLedsAnimation, 1, random(2));
    bitWrite(greenLedsAnimation, 0, random(2));
    PmillfallingLightAnimation = Cmill;
  }
}

void animation1(int timing) {
  long Cmill = millis();
  if (Cmill - Pmillanimation1 >= timing) {
    blueLedsAnimation = animation1BlueLedsFrames[animation1FrameCounter];
    greenLedsAnimation = animation1GreenLedsFrames[animation1FrameCounter];
    displayAnimation[0] = animation1DisplayFrames[animation1FrameCounter][0];
    displayAnimation[1] = animation1DisplayFrames[animation1FrameCounter][1];
    displayAnimation[2] = animation1DisplayFrames[animation1FrameCounter][2];
    animation1FrameCounter++;
    if (animation1FrameCounter > 12) animation1FrameCounter = 0;
    Pmillanimation1 = Cmill;
  }
}

void animation2(int timing) {
  long Cmill = millis();
  if (Cmill - Pmillanimation2 >= timing) {
    blueLedsAnimation = animation2BlueLedsFrames[animation2FrameCounterBlue];
    greenLedsAnimation = animation2GreenLedsFrames[animation2FrameCounterGreen];
    displayAnimation[0] = animation2DisplayFrames[animation2FrameCounterDisplay][0];
    displayAnimation[1] = animation2DisplayFrames[animation2FrameCounterDisplay][1];
    displayAnimation[2] = animation2DisplayFrames[animation2FrameCounterDisplay][2];
    animation2FrameCounterBlue++;
    animation2FrameCounterGreen++;
    animation2FrameCounterDisplay++;
    if (animation2FrameCounterBlue > 7) animation2FrameCounterBlue = 0;
    if (animation2FrameCounterGreen > 5) animation2FrameCounterGreen = 0;
    if (animation2FrameCounterDisplay > 9) animation2FrameCounterDisplay = 0;
    Pmillanimation2 = Cmill;
  }
}

void animation3(int timing) {
  long Cmill = millis();
  if (Cmill - Pmillanimation3 >= timing) {
    blueLedsAnimation = animation3BlueLedsFrames[animation3FrameCounter];
    greenLedsAnimation = animation3GreenLedsFrames[animation3FrameCounter];
    displayAnimation[0] = animation3DisplayFrames[animation3FrameCounter][0];
    displayAnimation[1] = animation3DisplayFrames[animation3FrameCounter][1];
    displayAnimation[2] = animation3DisplayFrames[animation3FrameCounter][2];
    animation3FrameCounter++;
    if (animation3FrameCounter > 9) animation3FrameCounter = 0;
    Pmillanimation3 = Cmill;
  }
}

void animation4(int timing) {
  long Cmill = millis();
  if (Cmill - Pmillanimation4 >= timing) {
    blueLedsAnimation = animation4BlueLedsFrames[animation4FrameCounter];
    greenLedsAnimation = animation4GreenLedsFrames[animation4FrameCounter];
    displayAnimation[0] = animation4DisplayFrames[animation4FrameCounter][0];
    displayAnimation[1] = animation4DisplayFrames[animation4FrameCounter][1];
    displayAnimation[2] = animation4DisplayFrames[animation4FrameCounter][2];
    animation4FrameCounter++;
    if (animation4FrameCounter > 19) animation4FrameCounter = 0;
    Pmillanimation4 = Cmill;
  }
}

void animations() {
  long Cmill = millis();
  if (animationsONOFF == 0) PmillAnimationChange = Cmill;
  if (Cmill - PmillAnimationChange > animationChangeTime * 60000) {
    animationNumber++;
    PmillAnimationChange = Cmill;
  }
  if (animationNumber > 4) animationNumber = 0;
  if (animationNumber != animationNumberLast) {
    animationNumberLast = animationNumber;
    blueLedsAnimation = 0;
    greenLedsAnimation = 0;
    displayAnimation[0] = 0;
    displayAnimation[1] = 0;
    displayAnimation[2] = 0;
  }
  switch (animationNumber) {
    case (0):
      fallingLightAnimation(100);
      break;

    case (1):
      animation1(100);
      break;

    case (2):
      animation2(100);
      break;

    case (3):
      animation3(100);
      break;

    case (4):
      animation4(100);
      break;
  }
}

void animationTimeoutMeasuring(int timeout) {
  long Cmill = millis();
  if (Cmill - PmillanimationTimeout >= timeout * 60000 && Cmill - PmillanimationTimeout < timeout * 60000 + 100 /*just so this loop doesnt keep looping*/ ) {
    animationsONOFF = 1;
  }
  if (resetAnimationTimeout == 1) {
    PmillanimationTimeout = Cmill;
    resetAnimationTimeout = 0;
    animationsONOFF = 0;
  }
}

void setup() {
  pinMode(regD, OUTPUT);
  pinMode(regL, OUTPUT);
  pinMode(regC, OUTPUT);
  pinMode(clockIn, INPUT);
  pinMode(resetIn, INPUT);
  byte numDigits = 3;
  byte digitPins[] = {11, 12, 13};
  byte segmentPins[] = {4, 5, 6, 7, 8, 9, 10, 14};
  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);
  sevseg.setBrightness(100);

  coderButton.attachClick(singleCoderClick);
  saveButton.attachClick(singleSaveClick);
  saveButton.attachLongPressStart(longSavePress);
  attachInterrupt(0, rotate, CHANGE);
  attachInterrupt(1, rotate, CHANGE);

  triggerLenght = EEPROM.read(1000);
  bootLoadPreset = EEPROM.read(1001);
  animationEnable = EEPROM.read(1002);
  animationTimeout = EEPROM.read(1003);
  animationWakeWithSave = EEPROM.read(1004);
  animationChangeTime = EEPROM.read(1005);
  loadPreset(bootLoadPreset);

  refreshEuclid();
}

void loop() {
  if (animationEnable == 1) {
    animations();
    animationTimeoutMeasuring(animationTimeout);
  }
  if (animationEnable == 0) {
    animationsONOFF = 0;
    resetAnimationTimeout = 1;
  }
  sevsegDisplay();
  regwrite(variableSwitcher(channelSelectBIN(channelSelect), blueLedsAnimation, animationsONOFF), gate2trigger(outputs4shiftBIN()), variableSwitcher(writetoBIN, greenLedsAnimation, animationsONOFF));
  clockRead();
  sequenceLooparound();
  coderButton.tick();
  saveButton.tick();
  sevseg.refreshDisplay();
  if (analogRead(resetIn) > 800) totalReset();
  if (digitalRead(15) == 0 && digitalRead(18) == 0) {
    menuSwitch = 2;
    deepPosition = 0;
  }
}

//////////////////////
//  EUCLIDIAN MATH  //
//////////////////////
unsigned int euclid(int n, int k, int o) { // inputs: n=total, k=beats, o = offset
  int pauses = n - k;
  int pulses = k;
  int offset = o;
  int steps = n;
  int per_pulse = pauses / k;
  int remainder = pauses % pulses;
  unsigned int workbeat[n];
  unsigned int outbeat;
  uint16_t outbeat2;
  unsigned int working;
  int workbeat_count = n;
  int a;
  int b;
  int trim_count;

  for (a = 0; a < n; a++) { // Populate workbeat with unsorted pulses and pauses
    if (a < pulses) {
      workbeat[a] = 1;
    }
    else {
      workbeat[a] = 0;
    }
  }

  if (per_pulse > 0 && remainder < 2) { // Handle easy cases where there is no or only one remainer
    for (a = 0; a < pulses; a++) {
      for (b = workbeat_count - 1; b > workbeat_count - per_pulse - 1; b--) {
        workbeat[a] = ConcatBin(workbeat[a], workbeat[b]);
      }
      workbeat_count = workbeat_count - per_pulse;
    }

    outbeat = 0; // Concatenate workbeat into outbeat - according to workbeat_count
    for (a = 0; a < workbeat_count; a++) {
      outbeat = ConcatBin(outbeat, workbeat[a]);
    }

    if (offset > 0) {
      outbeat2 = rightRotate(offset, outbeat, steps); // Add offset to the step pattern
    }
    else {
      outbeat2 = outbeat;
    }

    return outbeat2;
  }

  else {
    if (pulses == 0) {
      pulses = 1;  //  Prevent crashes when k=0 and n goes from 0 to 1
    }
    int groupa = pulses;
    int groupb = pauses;
    int iteration = 0;
    if (groupb <= 1) {
    }

    while (groupb > 1) { //main recursive loop
      /*Serial.print("Pulses: ");   //  Help debug crash
        Serial.println(pulses);
        Serial.print("Pauses: ");
        Serial.println(pauses);
        Serial.print("Iterations: ");
        Serial.println(iteration);*/
      if (groupa > groupb) { // more Group A than Group B
        int a_remainder = groupa - groupb; // what will be left of groupa once groupB is interleaved
        trim_count = 0;
        for (a = 0; a < groupa - a_remainder; a++) { //count through the matching sets of A, ignoring remaindered
          workbeat[a] = ConcatBin(workbeat[a], workbeat[workbeat_count - 1 - a]);
          trim_count++;
        }
        workbeat_count = workbeat_count - trim_count;

        groupa = groupb;
        groupb = a_remainder;
      }

      else if (groupb > groupa) { // More Group B than Group A
        int b_remainder = groupb - groupa; // what will be left of group once group A is interleaved
        trim_count = 0;
        for (a = workbeat_count - 1; a >= groupa + b_remainder; a--) { //count from right back through the Bs
          workbeat[workbeat_count - a - 1] = ConcatBin(workbeat[workbeat_count - a - 1], workbeat[a]);

          trim_count++;
        }
        workbeat_count = workbeat_count - trim_count;
        groupb = b_remainder;
      }

      else if (groupa == groupb) { // groupa = groupb
        trim_count = 0;
        for (a = 0; a < groupa; a++) {
          workbeat[a] = ConcatBin(workbeat[a], workbeat[workbeat_count - 1 - a]);
          trim_count++;
        }
        workbeat_count = workbeat_count - trim_count;
        groupb = 0;
      }

      else {
        //Serial.println("ERROR");
      }
      iteration++;
    }

    outbeat = 0; // Concatenate workbeat into outbeat - according to workbeat_count
    for (a = 0; a < workbeat_count; a++) {
      outbeat = ConcatBin(outbeat, workbeat[a]);
    }

    if (offset > 0) {
      outbeat2 = rightRotate(offset, outbeat, steps); // Add offset to the step pattern
    }
    else {
      outbeat2 = outbeat;
    }

    return outbeat2;
  }
}
uint16_t rightRotate(int shift, uint16_t value, uint8_t pattern_length) {
  uint16_t mask = ((1 << pattern_length) - 1);
  value &= mask;
  return ((value >> shift) | (value << (pattern_length - shift))) & mask;
}
int findlength(unsigned int bnry) {
  boolean lengthfound = false;
  int length = 1; // no number can have a length of zero - single 0 has a length of one, but no 1s for the sytem to count
  for (int q = 32; q >= 0; q--) {
    int r = bitRead(bnry, q);
    if (r == 1 && lengthfound == false) {
      length = q + 1;
      lengthfound = true;
    }
  }
  return length;
}
unsigned int ConcatBin(unsigned int bina, unsigned int binb) {
  int binb_len = findlength(binb);
  unsigned int sum = (bina << binb_len);
  sum = sum | binb;
  return sum;
}
