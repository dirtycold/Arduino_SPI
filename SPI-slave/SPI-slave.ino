#include <SPI.h>

/*
Derived from rickweil/Arduino_SPI
Refer: https://github.com/rickweil/Arduino_SPI
*/

const static int length         = 200;
char             buffer[length] = {0};
int              pos            = 0;
boolean          msg_ready      = false;

void setup (void)
{
    Serial1.begin (115200);

    SPCR |= bit (SPE);

    pinMode(MISO, OUTPUT);

    pos = 0;
    msg_ready = false;

    SPI.attachInterrupt();
}

ISR (SPI_STC_vect)
{
    if (pos < length - 1)
    {
        buffer[pos++] = SPDR;
        if(SPDR == '\n')
        {
            msg_ready = true;
            buffer[pos] = 0;
        }
    }
    else
    {
        pos = 0;
    }
}

void loop (void)
{
    if (msg_ready )
    {
        Serial1.print("rx<< ");
        Serial1.print(String(buffer));
        pos = 0;
        msg_ready = false;
    }
}
