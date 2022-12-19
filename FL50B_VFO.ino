/*
// ****************************************************************************************************
// Contents of this file
// ****************************************************************************************************

Introduction
  Version History
  About this program
  Acknowledgements
  Hardware Connections

Coding Standards
  Abbreviations
  Naming of global constant
  Naming of global variable
  Naming of Local variable
  Naming of Struct
  Naming of Function
  Curly brackets
  Clarity of code
  Reference

Global declarations and initialisations
  Libraries
  Liquid Crystal Display
  Arduino Nano digital pin assignments
  Arduino Nano analogue pin assignments
  Global variables


Setup()

Loop()

Functions
  ChangeFrequency()
  GetSwSet1ButtonNumber()
  GetSwSet1ButtonNumberAtInstant()
  ReadAnalogPin()
  RefreshLcd()
  UpdateEeprom()



// ****************************************************************************************************
// Introduction
// ****************************************************************************************************

Version history
---------------
V1.0.00, 11/10/2022   First version.  
V1.0.04, 21/11/2022   Included push button debounce code written by Ben Rawlings, and a fix for an
                      excessive number of writes to the LCD by Colin Rawlings.
V1.0.05, 09/12/2022   Minor changes.  Known issue: Requires separate EEPROM initialisation script.
V1.0.06, 17/12/2022   Significant tidying of the code to remove redundant code, and rename variables.

About this program
------------------
This program has been developed for the Arduino Nano by Steve Rawlings, G4ALG.   

The program controls a Si5351 Direct Digital Synthesis (DDS) integrated circuit and a type 1602 
Liquid Crystal Display (LCD) to provide variable frequency oscillator (VFO) facilities.  
This program is specifically intended for use with the FL-50B transmitter, a valved transmitter 
made by Yaesu of Japan in the late 1960s and early 1970s.

The FL-50B transmitter operates on the 10, 15, 20, 40 and 80 metre amateur bands.  The original design 
allowed for use of fixed frequency (quartz crystal) operation, with the option of using an external VFO 
to control the transmit frequency.   The transmitter uses a single conversion architecture with a 5 MHz
intermediate frequency.  As result, and external VFO needs to provide a frequency range that is 
specific to each amateur band.

This program is a spin off of a much larger project.  As a result, some of the coding in this program
is redundant, and are merely artifacts of a more comprehensive VFO and transmitter control project.

This program provides no transmitter or receiver control functions.  It simply generates the desired 
VFO frequency and displays the resulting transmit frequency on a 1602 LCD display.

The FL-50B operates on five bands, these being the 10 m (28 MHz); 15 m (21 MHz); 20 m (14 MHz); 
40 m (7MHz); and 80 m (3.5 MHz) amateur bands.   'Band Up' and 'Band Down' momentary push buttons are 
used to cycle through the five bands.   Each of these five bands has a preset frequency at start-up.  
A rotary encoder is used to vary the frequency on the selected band.  A push button on the rotary 
encoder is used to select the frequency step size.  
 
The following table indicates the relationship between the FL-50B transmit frequency and the 
corresponding frequency that needs to be generated by the DDS chip:

Band      Preset Transmit Frequency            VFO Frequency           
 (m)                (MHz)                           (MHz) 
===========================================================================
 10                28.060                   Transmit Frequency - 5.1724 
 15                21.060                   Transmit Frequency - 5.1724 
 20                14.060                   Transmit Frequency - 5.1724 
 40                 7.030                   Transmit Frequency + 5.1724 
 80                 3.560                   Transmit Frequency + 5.1724


Acknowledgements
----------------
This work is largely based on a Universal VFO Controller program that was originally written 
by Paul Taylor, VK3HN (https://vk3hn.wordpress.com/).  In writing his program which targets 
Ashar Farhan VU2ESE's Arduino Nano/si5351 module ('Raduino'), Paul Taylor recognised the support 
he received from: 
QUOTE
  - Przemek Sadowski, SQ9NJE (basic controller script)
  - Jason Mildrum NT7S (si5351 library)
  - too many others to mention (ideas, code snippets).
UNQUOTE

As far as the tailoring of Paul's code to meet the needs of this project is concerned, I am
very grateful to my three sons Trevor, Colin, and Ben ('The Brothers Three') for their support 
and encouragement, and for their solutions to problems encountered during development.


Hardware connections
--------------------
          
            Arduino Nano module to LCD module Connections
            ---------------------------------------------

               D8 output    ____________     LCD RS (Register Select)
                  Pin 11                     Pin 4

               D9 output    ____________     LCD E (Enable)
                  Pin 12                     Pin 6

              D10 output    ____________     LCD D4
                  Pin 13                     Pin 11

               D11 output   ____________     LCD D5
                  Pin 14                     Pin 12

               D12 output   ____________     LCD D6
                  Pin 15                     Pin 13

               D13 output   ____________     LCD D7
                  Pin 16                     Pin 14



                      Other LCD Connections
                      ---------------------

                 LCD VSS    _____________    Ground
                   Pin 1                     

            LCD VDD (VCC)   _____________    +5 V        
                    Pin 2

                   LCD VD   _____________     Wiper of 10 k potentiometer 
                    Pin 3                     for setting contrast

                  LCD R/W   _____________     Ground
                    Pin 5                     

             LCD LED A (+)   _____________    via 220 ohm resistor to +5 V 
                    Pin 15                    

             LCD LED K (-)   _____________    Ground
                    Pin 16                    


                
             Si5351 module to Arduino Nano module I2C Connections
             ----------------------------------------------------

                    SDA       ____________     Arduino Nano A4 output 


                    SCL       ____________     Arduino Nano A5 output 
                                      

Further details of the hardware and the circuit diagram can be found at:
http://www.alg.myzen.co.uk/radio_g/qrp/fl50b_dds_vfo.htm




// ****************************************************************************************************
// Coding Standards
// ****************************************************************************************************

Abbreviations
-------------
Where abbreviations are included within names of variables or functions, the abbreviation is
transformed to title case. For example, LPF becomes Lpf


Naming of global constant
-------------------------
UPPER_CASE with underscore separators
   If associated with digital input pin: add _ip<pin number> suffix
   If associated with analogue input pin: add _ipA<pin number> suffix 
   If associated with digital output pin: add _op<pin number> suffix
   If associated with analogue output pin: add _opA<pin number> suffix


Naming of global variable
-------------------------
TitleCamelCase comprising a descriptive name or compound name


Naming of Local variable
------------------------
camelCase comprising a descriptive name or compound name


Naming of Struct
----------------
TitleCamelCase comprising a descriptive name or compound name


Naming of Function
------------------
TitleCamelCase comprising a descriptive query or action statement, possibly prefixed with 
a word indicated the purpose, such as Is/Has/Get/Set


Curly brackets
--------------
Unless the opening bracket is used to enclose data, it should be placed by itself on a new line


Clarity of code
---------------
This program avoids the use of shorthand C++ statements.  For example, 'i = i + 1', rather than 'i++'


Reference
---------
My prefered coding standards document can be found at:
https://chaste.cs.ox.ac.uk/trac/raw-attachment/wiki/CodingStandardsStrategy/codingStandards.pdf


*/

// ****************************************************************************************************
//  Global declarations and initialisations
// ****************************************************************************************************

// =======================================================================================
//  Libraries
// =======================================================================================

#include <EEPROM.h>        // IDE Standard
#include <LiquidCrystal.h> // IDE Standard
#include <Rotary.h>        // Ben Buxton https://github.com/brianlow/Rotary
#include <si5351.h>        // Etherkit Si5331 library from NT7S,  V2.1.4   https://github.com/etherkit/Si5351Arduino
#include <Wire.h>          // IDE Standard

// =======================================================================================
//  Liquid Crystal Display
// =======================================================================================

// Initialise the LiquidCrystal library by defining the interface between the
// LCD and the Arduino Nano.
const byte LCD_RS_op8 = 8;   // LCD Register Select (Nano D8 output)
const byte LCD_E_op9 = 9;    // LCD Enable (Nano D9 output)
const byte LCD_D4_op10 = 10; // LCD D4 (Nano D10 output)
const byte LCD_D5_op11 = 11; // LCD D5 (Nano D11 output)
const byte LCD_D6_op12 = 12; // LCD D6 (Nano D12 output)
const byte LCD_D7_op13 = 13; // LCD D7 (Nano D13 output)

// Initialise LCD
LiquidCrystal
    lcd(LCD_RS_op8, LCD_E_op9, LCD_D4_op10, LCD_D5_op11, LCD_D6_op12, LCD_D7_op13);

// ----------------------------------------------------------------------------------------------
//  Arduino Nano digital pin assignments
// ----------------------------------------------------------------------------------------------

//  Inherent               0;  // Serial communications bus
//  Inherent               1;  // Serial communications bus

const byte ENCODER_A_ip2 = 2; // input from encoder pin A, pin D2 pulsed low
const byte ENCODER_B_ip3 = 3; // input from encoder pin B, pin D3 pulsed low

// ----------------------------------------------------------------------------------------------
//  Arduino Nano analogue pin assignments
// ----------------------------------------------------------------------------------------------

const byte SW_SET1_ipA2 = A2;

/*
SW_SET1 is the input from Switch Set 1 which uses up to ten momemtary push button switches to select 
an associated pull down resistor to form a potential divider with a 470 ohm rersistor connected to + 5V.
The resulting DC voltage at A2 is read during loop() and converted to a number (ADC value).  Only three 
push buttons are used for this project (Band Up, Band Down, and Reduce Step Size).
*/
//    ADC
//   Value     Action                           Description
//   -----     ------                           -----------
//    100      BAND_UP         Shifts VFO frequency up one band
//                               (3.5 > 7 > 14 > 21 > 28 MHz, then 3.5 etc.)
//    209     BAND_DOWN        Shifts VFO frequency down one band
//                               (28 > 21 > 14 > 7 > 3.5 MHz, then 28 etc.)
//    305      REDUCE          Pressing the rotary encoder push button reduces the tuning step
//            STEP SIZE        size with each short press
//                               (10^4 >  10^3  > 10^2 > tens of Hz, then 10^4 etc.)
//
//

// ----------------------------------------------------------------------------------------------
//  Global variables
// ----------------------------------------------------------------------------------------------

// Variable used in RefreshLcd() to decide whether to update the display.
uint32_t BandIndexPrevious = -1;

// Variable used in RefreshLcd() to decide whether to update the display.
uint32_t FrequencyPrevious = -1;

// Variable for numerical push button identifier
byte ButtonNumber;

// Variable for the FL-50B's total number of selectable bands (10, 15, 20, 40 and 80 metres).
const int NUMBER_OF_BANDS = 5;

// Combined Typedef and Structure declaration for band parameters.
// These three band parameters apply on a per band basis (Status; Frequency; Step Size)
typedef struct
{
    boolean active;
    uint32_t Frequency;
    uint32_t StepSize;
} BandParameters;


// Array of BandParameters, one set per band
BandParameters Band[NUMBER_OF_BANDS];


// Variable for index into Band array
byte BandIndexCurrent;


// Initialisation of Si5351 DDS IC.  I2C address defaults to x60 in the NT7S si5351 library
Si5351 si5351;


// Variable for reading the two inputs that determine rotary encoder movement and direction
Rotary r = Rotary(ENCODER_B_ip3, ENCODER_A_ip2);


// Declare variables for controlling EEPROM writes
unsigned long LastFrequencyChangeTimer;
bool EepromUpdatedSinceLastFrequencyChange;


// Interrupt Service Routine (ISR) for reading rotary encoder
ISR(PCINT2_vect)
{
    unsigned char result = r.process();
    if (result == DIR_CW)
        ChangeFrequency(1);
    else if (result == DIR_CCW)
        ChangeFrequency(-1);
}

// ****************************************************************************************************
// Start of setup() function.   Runs once.  Used to initialize variables, pin modes,
//                              start using libraries, etc..
// ****************************************************************************************************

void setup()
{
    
    // Set the serial communications data rate in bits per second (baud)
    Serial.begin(9600);

    
    // Initialise Wire library to connect the I2C bus
    Wire.begin();

    
    // Print project name to concole
    Serial.println("FL-50B VFO ");

    
    // Initialise LCD with device type (16 columns and 2 rows)
    lcd.begin(16, 2);

    
    // Display project name for 2 seconds, then clear it
    lcd.print("FL-50B VFO");
    delay(2000);
    lcd.clear();

   
    // Display version number (first row) and version date (second row) for 2 seconds, then clear it
    lcd.print("Build V01.06");
    lcd.setCursor(0, 1);
    lcd.print("19/12/2022");
    delay(2000);
    lcd.clear();

   
    // Display author details for 2 seconds, then clear it
    lcd.print("Steve Rawlings ");
    lcd.setCursor(0, 1);
    lcd.print("G4ALG ");
    delay(2000);
    lcd.clear();

    
    // Prepare for subsequent use of the LCD and show underscore cursor.
    lcd.cursor();

    
    // Set pinMode for Analogue A2 input (Switch Set 1) to pull up.
    pinMode(SW_SET1_ipA2, INPUT_PULLUP);

   
    // Initialise Pin Change Interrupt Control Register for rotary encoder
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
    sei();

    
    // Load Band array from EEPROM
    BandIndexCurrent = EEPROM.read(0);
    Serial.print("setup() eeprom: BandIndex=");
    Serial.println(BandIndexCurrent);

    
    // Allow for an (unlikely) reduction in NUMBER_OF_BANDS since last EEPROM update
    if (BandIndexCurrent >= NUMBER_OF_BANDS)
        BandIndexCurrent = 1;

    int element_len = sizeof(BandParameters);
    for (int i = 0; i < NUMBER_OF_BANDS;)
    {
        EEPROM.get(1 + (i * element_len), Band[i]);
        i = i + 1;
    };

    // Initialise and start the si5351 clocks.  Assumes the use of the default 25 MHz quartz crystal
    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 153125);
    si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);

    // Variable for actual frequency to be generated for a selected transmit frequency
    volatile uint32_t f;

    // Algorithm for determining actual VFO frequency to be generated for the transmit
    // frequency currently displayed on the LCD
    {
        if ((Band[BandIndexCurrent].Frequency) >= 10000000)
            f = Band[BandIndexCurrent].Frequency - 5172400;

        else
            f = Band[BandIndexCurrent].Frequency + 5172400;
    }

   
    // Initialise Si5351 frequency and output channel ('clock')
    si5351.set_freq(f * SI5351_FREQ_MULT, SI5351_CLK0);

    
    // Turn on the required output channel (CLK0)
    si5351.output_enable(SI5351_CLK0, 1);

   
    // Initialise variables relating to frequency change events
    LastFrequencyChangeTimer = millis();
    EepromUpdatedSinceLastFrequencyChange = false;
}

// ****************************************************************************************************
//  Start of loop() function.    This function loops repeatedly.
// ****************************************************************************************************

void loop()
{
    UpdateEeprom();
    RefreshLcd();

    {
        
        // Declare variable for actual frequency to be generated for a selected transmit frequency
        volatile uint32_t f;

        
        // Algorithm for determining actual VFO frequency to be generated for the transmit
        // frequency currently displayed on the LCD
        {
            if ((Band[BandIndexCurrent].Frequency) >= 10000000)
                f = Band[BandIndexCurrent].Frequency - 5172400;

            else
                f = Band[BandIndexCurrent].Frequency + 5172400;
        }

        
        // Initialise Si5351 frequency and output channel ('clock')
        si5351.set_freq(f * SI5351_FREQ_MULT, SI5351_CLK0);

        
        // Initialise variables relating to frequency change events
        LastFrequencyChangeTimer = millis();
        EepromUpdatedSinceLastFrequencyChange = false;
    }

    
    // Initialise variables for evaluating characteristics of button press
    bool ButtonHeldFlag = false;
    unsigned long ButtonPressedDuration = millis();

   
    // Store most recent Switch Set 1 button number to have been pressed
    ButtonNumber = GetSwSet1ButtonNumber();

   
    // Wait until the button in Switch Set 1 has been released
    while (ButtonNumber > 0 && GetSwSet1ButtonNumber() > 0)
    {
        delay(5);
    }

    
    // A switch Set 1 button has been pressed and released, so initiate action

    
    // Button 1, Switch Set 1 initiates the Band Up action
    if (ButtonNumber == 1)
    {
        Serial.println("<B1>BAND UP");
        if (BandIndexCurrent == (NUMBER_OF_BANDS - 1))
            BandIndexCurrent = 0;
        else
            BandIndexCurrent = BandIndexCurrent + 1;
    };

    // Button 2, Switch Set 1 initiates the Band Down action
    if (ButtonNumber == 2)
    {
        Serial.println("<B2>BAND DOWN");
        // If currently on lowest frequency band, then wrap to highest frequency band
        if (BandIndexCurrent == 0)
            BandIndexCurrent = (NUMBER_OF_BANDS - 1);
        else
            BandIndexCurrent = BandIndexCurrent - 1;
    };

    
    // Button 3, Switch Set 1 (Rotary Encoder push button) initiates the Reduce Step Size action
    if (ButtonNumber == 3)
        switch (Band[BandIndexCurrent].StepSize)
        {
        case 10:
            // Currently on minimum step size, so wrap to maximum step size.
            Band[BandIndexCurrent].StepSize = 10000;
            // clear residual < 1kHz frequency component from the active VFO
            //  uint16_t f = Band[BandIndex].Frequency % 1000;
            //  Band[BandIndex].Frequency = Band[BandIndex].Frequency - f;
            break;

        case 100:
            Band[BandIndexCurrent].StepSize = 10;
            break;

        case 1000:
            Band[BandIndexCurrent].StepSize = 100;
            break;

        case 10000:
            Band[BandIndexCurrent].StepSize = 1000;
            break;
        }
}

// ****************************************************************************************************
//  Functions
// ****************************************************************************************************

// ===========================================================================================
// Vary transmit frequency based on new position of rotary encoder and current step size
void ChangeFrequency(int dir)
{

    if (dir == 1) // Increment
    {
        Band[BandIndexCurrent].Frequency =
            Band[BandIndexCurrent].Frequency + Band[BandIndexCurrent].StepSize;
    }
    else
    {
        if (dir == -1) // Decrement

            Band[BandIndexCurrent].Frequency =
                Band[BandIndexCurrent].Frequency - Band[BandIndexCurrent].StepSize;
    };
};

// ===========================================================================================
// Take readings of Switch Set 1 and map to a button number.
// Get 20 consecutive identical readings before returning the result.
byte GetSwSet1ButtonNumber()
{
    byte numberOfConsecutiveButtonResultsRequired = 20;
    byte numberOfConsecutiveButtonResults = 0;
    byte previousButtonNumber = -1;

    while (numberOfConsecutiveButtonResults < numberOfConsecutiveButtonResultsRequired)
    {
        int currentButtonNumber = GetSwSet1ButtonNumberAtInstant();

        if (currentButtonNumber == previousButtonNumber)
        {
            numberOfConsecutiveButtonResults = numberOfConsecutiveButtonResults + 1;
        }
        else
        {
            previousButtonNumber = currentButtonNumber;
        }
    }

    if (previousButtonNumber > 0)
    {
        Serial.print(" [Front button - ");
        Serial.print(previousButtonNumber);
        Serial.println("]");
    }

    return previousButtonNumber;
}

// ===========================================================================================
// Take a reading of Switch Set 1 and map it to a button number
byte GetSwSet1ButtonNumberAtInstant()
{
    byte b = 0;
    int z;
    z = ReadAnalogPin((byte)SW_SET1_ipA2);

    if (z > 59 && z < 141)
        b = 1; // 100  band up
    else if (z > 168 && z <= 249)
        b = 2; // 209  band down
    else if (z > 254 && z <= 345)
        b = 3; // 305  Step Size

    if (b > 0)
    {
        Serial.print(z);
        Serial.print(" : ");
    }
    return b;
}

// ===========================================================================================
// Read the analogue input pin 'p' and return its ADC value
int ReadAnalogPin(byte p)
{
    int i,
        val = 0,
        nbr_reads = 1; //  this used to average 2 readings.  Resulted in failure
                       //  to identify the correct button due to averaging of
                       //  voltage levels under contact bounce scenarios, hence
                       //  decoding to the wrong button number.  This function is under
                       //  review.
    for (i = 0; i < nbr_reads;)
    {
        val = val + analogRead(p);
        delay(1);
        i = i + 1;
    }
    return val / nbr_reads;
};

// ===========================================================================================
//  Update the LCD if something has changed since last update
void RefreshLcd()
{
    // Variable used to format displayed frequency according to number of significant figures
    uint16_t f;

    // Variable used to hold current frequency for current band
    uint32_t FrequencyCurrent;
    FrequencyCurrent = Band[BandIndexCurrent].Frequency;

    // Check whether BandIndex has changed.  If so, set the cursor position and
    // update LCD with current Band.
    if (BandIndexCurrent != BandIndexPrevious)
    {
        lcd.setCursor(0, 0);

        if (BandIndexCurrent == 0)
        {
            lcd.print("80m");
        }
        else if (BandIndexCurrent == 1)
        {
            lcd.print("40m");
        }
        else if (BandIndexCurrent == 2)
        {
            lcd.print("20m");
        }
        else if (BandIndexCurrent == 3)
        {
            lcd.print("15m");
        }
        else if (BandIndexCurrent == 4)
        {
            lcd.print("10m");
        }
        lcd.print("  ");
        lcd.print(" ");
    }

    // Check whether BandIndex has changed.
    // If so, set the cursor and update LCD with FL-50B PA Loading and Grid crib
    // details for the band.

    if (BandIndexCurrent != BandIndexPrevious)
    {
        lcd.setCursor(0, 1);

        if (BandIndexCurrent == 0) // 80m
        {
            lcd.print("LDG:5.0 GRID:2.3");
        }
        else if (BandIndexCurrent == 1) // 40m
        {
            lcd.print("LDG:7.5 GRID:1.5");
        }
        else if (BandIndexCurrent == 2) // 20m
        {
            lcd.print("LDG:4.2 GRID:6.2");
        }
        else if (BandIndexCurrent == 3) // 15m
        {
            lcd.print("LDG:4.6 GRID:4.3");
        }
        else if (BandIndexCurrent == 4) //10m
        {
            lcd.print("LDG:5.4 GRID:2.5");
        }
    }

    if (BandIndexCurrent != BandIndexPrevious)
    {
        BandIndexPrevious = BandIndexCurrent;
    }

    // Check whether frequency has changed.
    // If so, set the cursor position and update LCD with new frequency.

    if (FrequencyCurrent != FrequencyPrevious)
    {

        lcd.setCursor(6, 0);

        f = FrequencyCurrent / 1000000;
        if (f < 10)
            lcd.print(' ');
        lcd.print(f);
        lcd.print('.');

        f = (FrequencyCurrent % 1000000) / 1000;
        if (f < 100)
            lcd.print('0');
        if (f < 10)
            lcd.print('0');
        lcd.print(f);
        lcd.print('.');

        f = FrequencyCurrent % 1000;
        if (f < 100)
            lcd.print('0');
        if (f < 10)
            lcd.print('0');
        lcd.print(f);
        lcd.print(" ");
    }

    if (FrequencyCurrent != FrequencyPrevious)
    {
        FrequencyPrevious = FrequencyCurrent;
    }

    byte CursorPosition = 14;

    switch (Band[BandIndexCurrent].StepSize)
    {
    case 10:
        lcd.setCursor(CursorPosition, 0);
        break;

    case 100:
        lcd.setCursor(CursorPosition - 1, 0);
        break;

    case 1000:
        lcd.setCursor(CursorPosition - 3, 0);
        break;

    case 10000:
        lcd.setCursor(CursorPosition - 4, 0);
        break;
    };
}

// ===========================================================================================
void UpdateEeprom()
{
    if (abs(millis() - LastFrequencyChangeTimer) > 10000)
    {
        if (EepromUpdatedSinceLastFrequencyChange == false)
        {
            // do the eeprom write
            // Serial.println("*** eeprom write");
            EEPROM.write(
                0, BandIndexCurrent); // write the band index (BandIndex) to the first byte

            int element_len = sizeof(BandParameters);
            for (int i = 0;
                 i < NUMBER_OF_BANDS;) // write each element of the Band array
            {
                EEPROM.put(1 + (i * element_len), Band[i]);
                i = i + 1;
            }
            EepromUpdatedSinceLastFrequencyChange = true;
        }
    }
};
