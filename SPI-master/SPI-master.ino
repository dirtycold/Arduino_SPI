#include <SPI.h>

const int SPI_SS = A15;            // set pin A15 as the slave select

void setup() {
  Serial.begin(115200);
  pinMode(SPI_SS, OUTPUT);        // set the slaveSelectPin
  SPI.begin();                   // initialize SPI:
  SPI.setClockDivider(SPI_CLOCK_DIV16);
}

void loop() {
  char tx[20];
  char rx[20];
  int ii;
  
  // create a message
  sprintf(tx, "%ld", millis());
  Serial.print(String("tx >> ") + String(tx));

  int len = strlen(tx);

  // send / rx message
  digitalWrite(SPI_SS, LOW);        // take the SS pin low to select the slave
  
  //SPI.transfer(tx, len+1);
  for(int ii=0; ii<len+1; ii++ ) {		// +1 is to NUL terminate
    char cc = SPI.transfer(tx[ii]);
    if(ii>0)
      rx[ii-1] = cc;
  }
  
  digitalWrite(SPI_SS, HIGH);       // take the SS pin high to de-select the slave
  rx[len]=0;						// ensure NUL terminated response
  
  Serial.println(String("    rx << ") + String(rx));
  delay(500);      
}

