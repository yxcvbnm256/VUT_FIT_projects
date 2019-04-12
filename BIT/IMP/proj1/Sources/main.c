#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

// 7-segmentovy display
#define leftDisplay (*(volatile char*)(0x200))
#define rightDisplay (*(volatile char*)(0x201))

// DILSwitch
#define DILSwitch (*(volatile char*)(0x202))
#define ModeOn 0x80
#define ModeCount 0x40
#define ModeUp 0x20
#define ModeRight 0x10
// nastaveni modulu RTC
#define RTCSet 0x1D

byte final = 0;
int cycleCount = 0;
byte index;
byte CNTN[4];
const int Number[8] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07};

/* funkce, ktera vraci 0 nebo 1 na zaklade stavu DILSwitch, na ktery se ptame */
byte GetState (byte state) {
  return (((DILSwitch & state) == state) ? 1 : 0);
} 

/* funkce vykreslujici data na display */
void setDisplay() {    
  leftDisplay = Number[index];
  rightDisplay = Number[CNTN[index]];   
}

/* funkce nastavujici index zobrazovaneho nibblu */
void setIndex() {
  byte i = 0;
  index = 0;
  for (i = 0; i <= 3; i++) {
    if ((CNTN[i] != 0 && CNTN[i] != 7) && i > index)
      index = i;
  }    
}

/* funkce nastavujici index/hodnotu cntn */ 
void set() {
  byte temp = DILSwitch & 0xF;
  // pokud jsou spodni 4 bity DILSwitch nenulove, je mozne jimi nastavit index/hodnotu CNTN
  if (temp != 0) {
    // pokud se nastavuje hodnota CNTN, tj. pravy display
    if (GetState(ModeRight)) {
      // posledni tri bity dat (hodnota muze nabyvat maximalne 7)
      temp = DILSwitch & 0x7;
      // pokud se hodnota zmenila    
      if (CNTN[index] != temp) {
        CNTN[index] = temp; 
        setDisplay();
      }         
    }   
    // pokud se nastavuje index, tj. levy display    
    else {
      // posledni dva bity dat (index muze nabyvat maximalne 3)
      temp = DILSwitch & 0x3;
      // pokud se index zmenil
      if (index != temp) {
        index = temp; 
        setDisplay();
      }
    }        
  }   
}

/* funkce, ktera nastavi citac do defaultniho stavu */
void defaultState(){
  index = 0;  
  CNTN[0] = CNTN[1] = CNTN[2] = CNTN[3] = 0;
  DILSwitch  = 0x00;
  setDisplay();  
}

/* funkce, ktera nastavi citac do konecneho stavu */
void finalState() {
  if (cycleCount == 0) {
    defaultState();
    final = 0;
  }
  else {
    cycleCount--;
    if (rightDisplay == 0 && leftDisplay == 0)
      setDisplay();
    else
      rightDisplay = leftDisplay = 0;
  }  
}

/* Funkce zarizujici citani */
void count() {
  int i = 0;
  switch (GetState(ModeUp)) {
    // pokud je smer citani nahoru
    case 1: 
      // pokud je citac na maximalni hodnote, prejde do finalState
      if (CNTN[0] == 7 && CNTN[1] == 7 && CNTN[2] == 7 && CNTN[3] == 7) {
        cycleCount = 3;
        final = 1;
        finalState();
      }
      // jinak cita
      else {          
        for (i; i <= 3; i++) {
          if (i == 0) {
            CNTN[i]++;
            cycleCount++;
          }            
          else if (CNTN[i-1] == 8) {
            CNTN[i]++;
            CNTN[i-1] = 0;
          }
        }
        setIndex();
        setDisplay();  
      }      
      break; 
    // pokud je smer citani dolu      
    case 0:          
      if (CNTN[0] == 0 && CNTN[1] == 0 && CNTN[2] == 0 && CNTN[3] == 0) {
        if (cycleCount == 0) {
          CNTN[0] = CNTN[1] = CNTN[2] = CNTN[3] = 7;
        }
        else {
          cycleCount = 3;
          final = 1;
          finalState();  
        }           
      }
      // jinak cita
      else {
        for (i; i <= 3; i++) {             
          if (i == 0) {
            CNTN[i]--;
            cycleCount++;
          }            
          else if (CNTN[i-1] == 255) {
            CNTN[i-1] = 7;
            CNTN[i]--;
          }
        }
        setIndex();
        setDisplay();
      }      
      break;       
  }   
}

/* funkce zarizujici blikani v modu set */
void flash() {
  if (GetState(ModeRight)) {      
    if (rightDisplay == 0)
      setDisplay();
    else
      rightDisplay = 0;
    }
    else {
      if (leftDisplay == 0)
        setDisplay();
      else
        leftDisplay = 0;
    }  
}

/* obsluha preruseni z ÀTC */
interrupt VectorNumber_Vrtc void ISR_rtc(void)
{
  RTCSC = RTCSC | 0x80;
  if (final == 1) {
    finalState();          
  }
  else if (GetState(ModeOn)) {
    if (GetState(ModeCount))
      count();
    else {
      flash();
    }
  }    
}

void main(void) {
  byte isOn = 0;
  RTCMOD = 0x00; //nastupna hrana    
  EnableInterrupts; /* enable interrupts */
  /* include your code here */  
  defaultState();      
  for(;;) {
    // pokud citac byl aktivni a nasledne byl nastaven bit Start do 0, vrati se citac do pocatecniho stavu a vypnou se hodiny v RTC
    if (!GetState(ModeOn) && isOn == 1) {
      isOn = 0;
      defaultState();
      RTCSC = 0;
    }
    // pokud je citac zapnuty
    else if(GetState(ModeOn)) {
      // pokud byl v minulem cyklu citac vypnuty, zapne se hodinovy signal RTC
      if (isOn == 0) {
        RTCSC = RTCSet;
        isOn = 1;
      } 
      // pokud je bit Count v 0, znamena to rezim Set       
      if (GetState(ModeCount) == 0) {
        set(); 
      }  
    }    
    __RESET_WATCHDOG(); /* feeds the dog */
  } /* loop forever */  
  /* please make sure that you never leave main */
}