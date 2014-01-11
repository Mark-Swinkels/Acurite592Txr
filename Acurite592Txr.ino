const int Samples = 50;
const unsigned int Resolution = 50;
const int Pin = 4;
int c=0;
int pinState=LOW;
int state = 0;

bool collecting = false;
unsigned int High[Samples];
unsigned int Low[Samples];
unsigned long last=0;
unsigned long lastTransition;
unsigned int msgBit[42];
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
      case 1: // seen a sync high expect low
        if (dur >  150 && dur < 450)
        {
          state = 2;
        }
        else
        {
          state = 0;
        }
        break;
      case 2: // seen a sync low expect a short or long
        if (dur >  150 && dur < 450)
        {
          msgBit[bitCnt++] =dur;
          if (bitCnt > 38)
          {
            for (int i=0; i < bitCnt; i++)
            {
              if (msgBit[i] < 300)
                Serial.print('.');
              else
                Serial.print('1');
            }
            Serial.print(" - ");
            for (int i=0; i < bitCnt; i++)
            {
                Serial.print(msgBit[i]);
                Serial.print(", ");
            }
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
