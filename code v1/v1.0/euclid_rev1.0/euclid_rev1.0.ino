#include <SevSeg.h>
#include <Rotary.h>
#include <OneButton.h>

#define regD 16
#define regL 17
#define regC 19
SevSeg sevseg;
Rotary rotary = Rotary(3, 2);
OneButton coderButton(15, true);
#define clockIn 21
#define resetIn 20

int writeto;
int channelselect;
bool inPrev;
uint8_t clockVar[9];
uint8_t writetoBIN = 0B100;
bool clockToggleTrigger;
int triggerLenght = 30;//ms
long Pmill;

uint8_t lenght[9] = {16, 16, 16, 16, 16, 16, 16, 16};
int hits[9] = {4, 4, 4, 4, 4, 4, 4, 4};
int offset[9] = {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t euclidState[9];

void regwrite(uint16_t inputvalue1, uint8_t inputvalue2, uint8_t inputvalue3, uint8_t inputvalue4) {
  digitalWrite(regL, LOW);
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
  for (int i = 7; i >= 0; i--) {
    digitalWrite(regC, LOW);
    digitalWrite(regD, bitRead(inputvalue4, i));
    digitalWrite(regC, HIGH);
  }
  digitalWrite(regL, HIGH);
}

void sequenceLooparound() {
  for (int i; i < 8; i++) {
    if (clockVar[i] > lenght[i] - 1) clockVar[i] = 0;
  }
}

void totalReset() {
  for (int i; i < 8; i++) {
    clockVar[i] = 0;
  }
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

void rotate() {
  unsigned char result = rotary.process();
  if (digitalRead(15) == 1) {
    switch (writeto) {
      case (0):
        if (result == DIR_CW) {
          hits[channelselect]++;
        }
        if (result == DIR_CCW) {
          hits[channelselect]--;
        }
        if (hits[channelselect] > lenght[channelselect]) hits[channelselect] = lenght[channelselect];
        if (hits[channelselect] < 0)  hits[channelselect] = 0;
        break;

      case (1):
        if (result == DIR_CW) {
          lenght[channelselect]++;
        }
        if (result == DIR_CCW) {
          lenght[channelselect]--;
        }
        if (lenght[channelselect] > 16) lenght[channelselect] = 16;
        if (lenght[channelselect] < 0) lenght[channelselect] = 0;
        break;

      case (2):
        if (result == DIR_CW && lenght != 0) {
          offset[channelselect]++;
        }
        if (result == DIR_CCW && lenght != 0) {
          offset[channelselect]--;
        }
        if (offset[channelselect] > lenght[channelselect] - 1) offset[channelselect] = 0;
        if (offset[channelselect] < 0) offset[channelselect] = lenght[channelselect] - 1;
        break;
    }
    euclidState[channelselect] = euclid(lenght[channelselect], hits[channelselect], offset[channelselect]);
  }
  if (digitalRead(15) == 0) {
    if (result == DIR_CW) {
      channelselect++;
    }
    if (result == DIR_CCW) {
      channelselect--;
    }
    if (channelselect > 7) channelselect = 7;
    if (channelselect < 0) channelselect = 0;
  }
}

int channelselectBIN(int channel) {
  uint8_t output;
  bitWrite(output, channel, 1);
  if (channel < 7) bitWrite(output, channel + 1, 0);
  if (channel > 0) bitWrite(output, channel - 1, 0);
  return output;
}

void refreshEuclid() {
  for (int i; i < 8; i++) {
  euclidState[i] = euclid(lenght[i], hits[i], offset[i]);
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
  
  if (Cmill - Pmill >= triggerLenght && writingState == 1){
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

void singleCoderClick() {
  writeto++;
  if (writeto > 2) writeto = 0;
  switch (writeto) {
    case (0):
      bitWrite(writetoBIN, 0, 0);
      bitWrite(writetoBIN, 1, 0);
      bitWrite(writetoBIN, 2, 1);
      break;
    case (1):
      bitWrite(writetoBIN, 0, 0);
      bitWrite(writetoBIN, 1, 1);
      bitWrite(writetoBIN, 2, 0);
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
  attachInterrupt(0, rotate, CHANGE);
  attachInterrupt(1, rotate, CHANGE);

  refreshEuclid();

  //Serial.begin(2000000);
}

void loop() {
  sevseg.setNumber(offset[channelselect] * 400 + lenght[channelselect] * 20 + hits[channelselect], 3, 1);
  regwrite(euclidState[channelselect], channelselectBIN(channelselect), gate2trigger(outputs4shiftBIN()), writetoBIN);
  clockRead();
  sequenceLooparound();
  coderButton.tick();
  sevseg.refreshDisplay();
  if (analogRead(resetIn) > 800) totalReset();
  /*Serial.print(clockVar[0]);
  Serial.print("\t");
  Serial.print(clockVar[1]);
  Serial.print("\t");
  Serial.print(clockVar[2]);
  Serial.print("\t");
  Serial.print(clockVar[3]);
  Serial.print("\t");
  Serial.print(clockVar[4]);
  Serial.print("\t");
  Serial.print(clockVar[5]);
  Serial.print("\t");
  Serial.print(clockVar[6]);
  Serial.print("\t");
  Serial.print(clockVar[7]);
  Serial.print("\t");*/
  //Serial.println(clockToggleTrigger);
}

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
