/*
Arduino Nano script for FL-50B transmitter.
EEPROM initialiser script
*/

#include <Wire.h>
#include <EEPROM.h>

const int NUMBER_OF_BANDS = 5; // number of selectable bands 

// struct for 'band parameter set' records -- the parameters that will change with each band
typedef struct {
  boolean  active;
  uint32_t band;
  uint32_t radix;
} BandSet_type;

BandSet_type BandSet[NUMBER_OF_BANDS];         // array of band parameter sets
byte BandIndex;                          // index into BandSet array (representing the current band)

int t=0, second_counter=0;


void setup(){
  Serial.begin(9600);  
  Serial.println("setup()");

  // print EEPROM size
  int ee_len = EEPROM.length();
  Serial.print("EEPROM length =");
  Serial.println(ee_len);
  
  Serial.print("BandSet[] length =");
  Serial.println(sizeof(BandSet_type) * NUMBER_OF_BANDS);

  // clear the EEPROM contents
  for(int j=0; j<ee_len; j++)
  {
    EEPROM.write(j, 0);
  }

 // initialise BandSet array
  for(int n=0; n<NUMBER_OF_BANDS; n++) BandSet[n].active = 0;   // make sure all are inactive to start with 
  BandSet[0]  = (BandSet_type){1,   3560000ULL,  100};
  BandSet[1]  = (BandSet_type){1,   7030000ULL,  100};
  BandSet[2]  = (BandSet_type){1,  14060000ULL,  100};
  BandSet[3]  = (BandSet_type){1,  21060000ULL,  100};
  BandSet[4]  = (BandSet_type){1,  28060000ULL,  100};
//  BandSet[5]  = (BandSet_type){1,   7030000ULL,  100};
//  BandSet[6]  = (BandSet_type){1,  10116000ULL,  100};
//  BandSet[7]  = (BandSet_type){1,  14060000ULL,  100};
//  BandSet[8]  = (BandSet_type){1,  18096000ULL,  100};
//  BandSet[9]  = (BandSet_type){1,  21060000ULL,  100};
//  BandSet[10] = (BandSet_type){1,  24906000ULL,  100};
//  BandSet[11] = (BandSet_type){1,  28060000ULL,  100};
//  BandSet[12] = (BandSet_type){1,  50060000ULL,  100};
//  BandSet[13] = (BandSet_type){1,  70200000ULL,  100};
//  BandSet[14] = (BandSet_type){1, 144060000ULL,  100};
  

  // write the current band index (into BandSet[]) into EEPROM byte 0
  byte b = 0;
  EEPROM.write(0, b);

  // put the entire BandSet array into EEPROM
  int element_len = sizeof(BandSet_type);
  for(int i=0; i<NUMBER_OF_BANDS; i++)
  {
    EEPROM.put(1 + (i * element_len), BandSet[i]);
  };

// dump out the BandSet array for diagnostics
  for(int n=0; n < NUMBER_OF_BANDS ; n++)
  {
    Serial.print((int)BandSet[n].active);
    Serial.print(" ");
    Serial.print(BandSet[n].band);
    Serial.print(" ");
    Serial.print((long)BandSet[n].radix);
    Serial.println();
  }

    t = millis();
}


void loop()
{
  /*
  if((millis() - t) > 1000){
    t=millis();
    second_counter++;
    Serial.println(second_counter);
  }
      */
}
