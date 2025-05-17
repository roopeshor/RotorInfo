#define R1A 2  // green
#define R2A 3  // green
#define R1B 4  // white
#define R2B 5  // white
long fineCount1 = 0;
char direction1 = 'i';  // CW
long fineCount2 = 0;
char direction2 = 'i';  // CW

void setup() {
  // Set encoder pins as inputs
  pinMode(R1A, INPUT);
  pinMode(R1B, INPUT);
  pinMode(R2A, INPUT);
  pinMode(R2B, INPUT);

  // Setup Serial Monitor
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(R1A), countFiner1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(R2A), countFiner2, CHANGE);
}

void countFiner1() {
  /**
Clockwise case:

 R1A __¯¯¯¯¯_____¯¯¯¯¯_____¯¯¯¯¯_____¯¯¯¯¯
     
 R1B ____¯¯¯¯¯_____¯¯¯¯¯_____¯¯¯¯¯_____¯¯¯¯¯
      ^ measure (CW) 
*/
  if (digitalRead(R1A) == 1) {
    if (digitalRead(R1B) == 0) {
      // CW
      fineCount1++;
      direction1 = 'c';
    } else {
      // CCW
      fineCount1--;
      direction1 = 'a';
    }
  }
}

void countFiner2() {
  if (digitalRead(R2A) == 1) {
    if (digitalRead(R2B) == 0) {
      // CW
      fineCount2++;
      direction2 = 'c';
    } else {
      // CCW
      fineCount2--;
      direction2 = 'a';
    }
  }
}

void loop() {
  Serial.println(String(micros()) + "|" + direction1 + " " + fineCount1 + "|" + direction2 + " " + fineCount2);
  delay(150);
}