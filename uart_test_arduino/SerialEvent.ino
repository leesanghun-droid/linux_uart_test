
void setup() {

  Serial.begin(9600);

}

void loop() 
{
  
    Serial.write(0x4C); //L
    Serial.write(0x69); //i
    Serial.write(0x66); //f
    
    Serial.write(0x00); 
    Serial.write(0x09); 
    
    Serial.write(0x30); 
    Serial.write(0x31); 
    Serial.write(0x32); 
    Serial.write(0x33); 
    Serial.write(0x34); 
    
    Serial.write(0x08); 
    Serial.write(0x08); 
    Serial.write(0x08); 
    Serial.write(0x08); 
    
    delay(100);
}

