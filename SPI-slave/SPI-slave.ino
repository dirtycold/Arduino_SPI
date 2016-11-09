#include <SPI.h>

char tx [100];
char rx [100];
volatile byte pos;
volatile boolean msg_ready;
bool is_ascii = false;
int messages = 0;

void setup (void)
{
  Serial.begin (115200);   // debugging
  
  // turn on SPI in slave mode
  SPCR |= bit (SPE);

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  
  // get ready for an interrupt 
  pos = 0;   // txfer empty
  msg_ready = false;

  // now turn on interrupts
  SPI.attachInterrupt();
  attachInterrupt(digitalPinToInterrupt(21), eotISR, RISING);
}  // end of setup

void eotISR() {
    messages++;
    msg_ready = true;
}


// SPI interrupt routine
ISR (SPI_STC_vect) {

    byte rd = SPDR;     // grab byte from SPI Data Register
    // echo back, but if ASCII, toupper(c)
    char wr = is_ascii ? toupper(rd) : rd;

    if (pos < sizeof(rx)) {
        tx[pos] = wr;     // tx_buffer with modified data
        rx[pos++] = rd;   // add to rx_buffer
    }

    SPDR = wr;
    if ((rd == 0 && is_ascii))       //ASCII NUL terminator
        msg_ready = true;   
}

unsigned long timer=0;
void loop (void) 
{
  // wait for a messsage (ascii set in interrupt routine, hex set below)
  if (msg_ready ) {
    if (is_ascii) {
        tx [pos] = 0;  // make sure we're nul terminated
        rx [pos] = 0;  
        Serial.print(String("rx << ") + String(rx));
        Serial.println(String("    tx >> ") + String(tx));
    }
    else {
        Serial.print("rx<< ");
        for (int ii = 0; ii < pos; ii++) {
            if (rx[ii] < HEX) 
                Serial.print("0");
            Serial.print((unsigned char)rx[ii], HEX);
            Serial.print(" ");
        }
        Serial.println("");
    }
    pos = 0;
    msg_ready = false;
  }
}  
