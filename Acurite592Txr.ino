const int Pin = 4;
int pinState=LOW;
int state = 0;

unsigned long last=0;
unsigned long lastTransition;

const int msgLength=56;
char thisBit;
byte msg[7];
char msgBit[msgLength];
char lastBit[msgLength];
int bitCnt;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  pinMode(4, INPUT);
  
  lastTransition=micros();
  
}

void loop() {

  if (digitalRead(4) ==  pinState)
  {
    for (int b=0; b<3; b++)
    {
      if (digitalRead(4) !=  pinState) return;
    }
    
    // transition
    unsigned long dur = micros() - lastTransition;
    lastTransition += dur;

top:    
    switch (state)
    {
      case 0:  // idle
        // waiting for sync pulse
        bitCnt = 0;
        if (pinState == HIGH && dur > 550 && dur < 650)
        {
          state = 1;
        }
        else
        {
          state = 0;
        }
        break;
      case 1: // first (low) half of bit
        if (dur >  150 && dur < 450)
        {
          thisBit = (dur < 300)?'0':'1';
          state = 2;
        }
        else
        {
          state = 0;
        }
        break;
        
      case 2: // second (high) half of bit
        if (dur >  150 && dur < 450)
        {
          msg[bitCnt>>3] = msg[bitCnt>>3] & ((0x80 >>(bitCnt & 0x7))^0xFF) | (dur < 300) << (7-(bitCnt & 0x7));
          
          if (((dur < 300)?'0':'1') == thisBit)
            msgBit[bitCnt++] = '?';
          else
            msgBit[bitCnt++] = thisBit;
            
          state = 1;
          if (bitCnt == msgLength)
          {
            // show changed bits
            for (int i=0; i < bitCnt; i++)
            {
              if (msgBit[i] != lastBit[i])
              {
                lastBit[i] = msgBit[i];
                Serial.print('x');
              }
              else
                Serial.print(' ');
              if ((i&7) == 7) Serial.print(' ');
            }
            Serial.println();

            for (int i=0; i < bitCnt; i++)
            {
              Serial.print(msgBit[i]);
              if ((i&7) == 7) Serial.print(' ');
            }
            int humidity = msg[3] & 0x7F;
            int temp = (((msg[4] & 0x7F) << 7) | (msg[5] & 0x7F))-1000;
            Serial.print(" - ");
            Serial.print(temp);
            Serial.print("C, ");
            Serial.print(humidity);
            Serial.print("%");
            
            Serial.println();
            Serial.flush();
            state = 0;
          }
        }
        else
        {
          state = 0;
          goto top;  // might be another sync high
        }
        break;
    }
    pinState = (pinState==LOW)?HIGH:LOW;
  }
}
