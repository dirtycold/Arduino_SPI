#include <SPI.h>

char tx [100];
char rx [100];
volatile byte pos;
volatile boolean process_it;

void setup (void)
{
  Serial.begin (115200);   // debugging
  
  // turn on SPI in slave mode
  SPCR |= bit (SPE);

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  
  // get ready for an interrupt 
  pos = 0;   // txfer empty
  process_it = false;

  // now turn on interrupts
  SPI.attachInterrupt();

}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect) {

  byte rd = SPDR;     // grab byte from SPI Data Register
  char wr;
  wr = toupper(rd);// +(rd != 0);  // create modified write data from read (e.g. increment)
 
  if (pos < sizeof(rx)) {
    tx[pos] = wr;     // tx_buffer with modified data
    rx[pos++] = rd;   // add to rx_buffer
  }
  SPDR = wr;
  if (rd == 0)        // NUL means time to process received data
    process_it = true;   
}

void loop (void) 
{
// wait for flag set in interrupt routine
  if (process_it) {
    tx [pos] = 0;  // make sure we're nul terminated
    rx [pos] = 0;  
    Serial.print(  String("rx << ") + String(rx));
    Serial.println(String("    tx >> ") + String(tx));
    pos = 0;
    process_it = false;
  }
}  
