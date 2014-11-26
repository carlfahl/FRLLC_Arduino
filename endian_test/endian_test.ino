/*
  Endian test
  
  From stackoverflow
  
  */
  
void setup() {
  Serial.begin(9600);
}

void loop () {
  Serial.println(endian());
  Serial.println(sizeof(int));
  Serial.println(sizeof(char));
  Serial.println(sizeof(short));
  delay(500);
}

int endian() {
    int i = 1;
    char *p = (char *)&i;

    if (p[0] == 1)
        return 0;
    else
        return 1;
}
