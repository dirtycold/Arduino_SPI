#include <SPI.h>
/*

This program implements a SPI SLAVE device that if configure for 
 - is_ascii mode, echos NUL terminated ASCII strings data back to the
    MASTER (and console), or
 - in binary mode, echoes the same data back to MASTER (and console).

The rising edge of the SPI SS line is used to signal an end-of-
    transmission delimiter.  This is absolutely necessary in binary 
    mode (you have to have SS wired to the D8 interrupt pin).  
    NUL terminators are sufficient for ASCII mode, but there could be
    data errors.

*/

char tx [100];
char rx [100];
volatile byte pos;
volatile boolean msg_ready;
bool is_ascii = true;

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
 
  // end of tx ISR sets message.
  // NOTE: You MUST hard wire SS (D53) to D21  !!!!    <<<<<<<<<<<< !!!
  // https://www.arduino.cc/en/Reference/AttachInterrupt
  attachInterrupt(digitalPinToInterrupt(21), eotISR, RISING);

  Serial.println("Type 'a' for ASCII mode, 'b' for binary.");
}

void eotISR() {
    msg_ready = true;
}


// SPI interrupt routine
ISR (SPI_STC_vect) {

    byte rd = SPDR;     // grab byte from SPI Data Register
    char wr = rd;
    
    if (is_ascii) {
        if (wr >= 'a' && wr <= 'z') // toupper
            wr -= ' ';
        if (pos < sizeof(rx)) {
            tx[pos] = wr;       // tx_buffer with MODIFIED data
            rx[pos++] = rd;     // add to rx_buffer
        }

        // allows ASCII mode to work without wires. 
        if (rd == '\0')  // ASCII NUL received
            msg_ready = true;
    } 
    else {
        if (pos < sizeof(rx)) {
            tx[pos] = rd;     // echo back RECEIVED data
            rx[pos++] = rd;   // add to rx_buffer
        }
    }

    SPDR = wr;
}

unsigned long timer=0;
void loop (void) 
{
    int ch = Serial.read();
    if (ch == 'a') 
        is_ascii = true;
    else if (ch == 'b') 
        is_ascii = false;

  // wait for a messsage (set in eotISR interrupt routine)
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
