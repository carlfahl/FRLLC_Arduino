/*
  Serial_test
 */
 
unsigned char inByte1;

unsigned char char testByte = 0x37;

// the setup routine runs once when you press reset:
void setup() {
  // Serial is the USB connection to host computer.
  Serial.begin(9600);
  // On the Leonardo a sperate serial is availabe on PINs 0 and 1 - Serial 1
  Serial1.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  while (Serial1.available() > 0)
  {
      inByte1 = Serial1.read();
      Serial.print("I received: ");
      //Serial1.println(inByte1);
      Serial.println(inByte1, OCT);
      Serial.print("Test char:");
      Serial.println(testByte, OCT);
  }
}
