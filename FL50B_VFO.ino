
// ------------------------------------------------------------------------------------------------------------------
// Introduction
// ------------------------------------------------------------------------------------------------------------------

/*
This is an Arduino Nano program developed by Steve Rawlings, G4ALG.   When used
with a Si5351 DDS chip, the program produces a variable frequncy oscillator for
use with an old 1960s valved transmitter, Model FL-50B, made by Yaesu of Japan.

This program is a branch of a much larger project for a home made low power CW
(morse) transmitter featuring:
- a DDS signal frequency VFO using a 10-pin Si5351 DDS chip;
- a 16 x 2 (1602) liquid crystal display that is compatible with the Hitachi
HD44780 driver;
- a software keyer for sending morse code; and,
- an integrated transmit/receive controller.

This program gererates a variable frequency taking account of the 'superhet'
frequency mixing that takes places within the FL-50B to derive the desired
transmit frequency.   This program provides no transmitter or receiver control
functions.  It simply generates the desired VFO frequency and displays the
resulting transmit frequency on a 1602 LCD display.

The FR-50B operates on the 10 m (28 MHz); 15 m (21 MHz); 20 m (14 MHz); 40 m (7
MHz) and 80 m (3.5 MHz) amateur bands.   Each of these five bands has a preset
frequency within the program, and 'Band Up' and 'Band Down' momentary push
buttons are used to cycle through the preset frequencies.   This is to make
changing bands easier by obviating the need to make large frequncy excursions
via the rotary encoder control.  

The following table indicates the relationship between the FL-50B transmit
frequency and the corresponding default frequency that needs to be generated by the DDS
chip:

Band      Default Operating Frequency            VFO Frequency           
 (m)                (MHz)                           (MHz) 
===========================================================================
 10                28.060                   Transmit Frequency - 5.1724 
 15                21.060                   Transmit Frequency - 5.1724 
 20                14.060                   Transmit Frequency - 5.1724 
 40                 7.030                   Transmit Frequency + 5.1724 
 80                 3.560                   Transmit Frequency + 5.1724




This program is largely based on a Universal VFO Controller program that was
originally written by Paul Taylor, VK3HN (https://vk3hn.wordpress.com/).

In writing his program which targets Ashar Farhan VU2ESE's Arduino Nano/si5351
module ('Raduino'), Paul Taylor recognised the support he received from: 
QUOTE
  - Przemek Sadowski, SQ9NJE (basic controller script)
  - Jason Mildrum NT7S (si5351 library)
  - too many others to mention (ideas, code snippets).
UNQUOTE


Version history:
V1.0, 11/10/2022 - First version.  Known bugs due to lack of contact de-bounce routines.


FURTHER READING: Read up about the concept of 'Class' (used to encapsulate
functions and variables) and how state can change/be manipulated.  Useful for
unit testing.  Class is the code, Object is the instance.

*/

// ------------------------------------------------------------------------------------------------------------------
// Coding Standards
// ------------------------------------------------------------------------------------------------------------------
// My prefered guidance document can be found at:
// https://chaste.cs.ox.ac.uk/trac/raw-attachment/wiki/CodingStandardsStrategy/codingStandards.pdf
//
// For this microcontroller program, the following naming conventions have been
// used.  Where abbreviations are used (such as LPF for low pass filter) within
// names or functions, the abbreviation will be transformed to title case. For
// example, LPF becomes Lpf.
//
// Note that the Arduino compiler will not accept changes to the mandatory
// setup() and loop() function names
//
//
// Global constant: UPPER_CASE with underscore separators
//       add _ip<pin number> suffix if associated with digital input pin
//       add _ipA<pin number> suffic if associated with analogue input pin
//       add _op<pin number> if associated with digital output pin
//       add _opA<pin number> if associated with analogue output pin
//
// Global variable name: TitleCamelCase comprising a descriptive name or
// compound name
//
// Local variable name: camelCase comprising a descriptive name or compound name
//
// Struct: TitleCamelCase comprising a descriptive name or compound name
//
// Functions: TitleCamelCase comprising a descriptive query or action statement,
// possibly prefixed with Is/Has/Get/Set
//
// Curly brackets:  Unless the opening bracket is used to enclose data, it
// should be placed by itself on a new line
//
//
// For loops: i retained
// See if VS will do global changes, including comments
//
//
// Clarity of code:
// As novice programmer, I find shorthand statements such as 'i++' difficult to
// assimilate, preferring formats such as i = i + 1.  Therefore, this program
// will avoid the use of shorthand C++ statements.
//
// ------------------------------------------------------------------------------------------------------------------
// Contents of this file
// ------------------------------------------------------------------------------------------------------------------
// For now, while I am learning about programming; C++; Arduino-related
// hardware; and software development tools, all text information specific to
// this project will be placed in this single file.  I recognise that this is
// bad practice. However, until I know what I need to know, I will find it
// easier to pack everything into this single file until I establish a system
// for splitting the various levels of knowledge, and sections of code, into
// separate files.
//
// The contents of this file are grouped as follows:
// Introduction
// Global declarations and initialisations
// Setup()
// Loop()
// Functions, in alphabetical order
//
//
//
//
//

// ------------------------------------------------------------------------------------------------------------------
// Global declarations and initialisations
// ------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------
// LIBRARIES
// ------------------------------------------------------------------------------------------------------------------
#include <EEPROM.h> // IDE Standard
// #include <PCF8574.h> // PCF8574 library by Rob Tillaart 0.3.2  https://github.com/RobTillaart/PCF8574
#include <Rotary.h> // Ben Buxton https://github.com/brianlow/Rotary
#include <Wire.h>   // IDE Standard
#include <si5351.h> // Etherkit Si5331 library from NT7S,  V2.1.4   https://github.com/etherkit/Si5351Arduino

// ------------------------------------------------------------------------------------------------------------------
// Liquid Crystal Display (LCD) declarations and initialisations
// ------------------------------------------------------------------------------------------------------------------
#include <LiquidCrystal.h> // IDE Standard

/*
This project uses a 16 x 2 (1602) liquid crystal display that is compatible with
the Hitachi HD44780 driver.

LCD to Arduino Nano Connections:
--------------------------------
LCD RS (Register Select)  Pin 4 on LCD module connects to Pin 11 on Arduino Nano
module (D8 output) LCD E (Enable)            Pin 6 on LCD module connects to Pin
12 on Arduino Nano module (D9 output) LCD D4                    Pin 11 on LCD
module connects to Pin 13 on Arduino Nano module (D10 output) LCD D5 Pin 12 on
LCD module connects to Pin 14 on Arduino Nano module (D11 output) LCD D6 Pin 13
on LCD module connects to Pin 15 on Arduino Nano module (D12 output) LCD D7 Pin
14 on LCD module connects to Pin 16 on Arduino Nano module (D13 output)

Other LCD Connections:
----------------------
LCD VSS                   Pin 1 on LCD module connects to ground
LCD VDD (VCC)             Pin 2 on LCD module connects to +5V
LCD VD                    Pin 3 on LCD module connects to wiper of 10 k
potentiometer for setting contrast LCD R/W                   Pin 5 on LCD module
connects to ground LCD LED (+)               Pin 15 on LCD module connects via
220 ohm resistor to +5V LCD LED (-)               Pin 16 on LCD module connects
to ground
*/

// Initialise the LiquidCrystal library by defining the interface between the
// LCD and the Arduino Nano.
const byte LCD_RS_op8 = 8;   // LCD Register Select (Nano D8 output)
const byte LCD_E_op9 = 9;    // LCD Enable (Nano D9 output)
const byte LCD_D4_op10 = 10; // LCD D4 (Nano D10 output)
const byte LCD_D5_op11 = 11; // LCD D5 (Nano D11 output)
const byte LCD_D6_op12 = 12; // LCD D6 (Nano D12 output)
const byte LCD_D7_op13 = 13; // LCD D7 (Nano D13 output)

uint32_t _last_BandIndex = -1;
uint32_t _last_freq = -1;
// Used in RefreshLcd function to decide whether to update the display.

LiquidCrystal
    lcd(LCD_RS_op8, LCD_E_op9, LCD_D4_op10, LCD_D5_op11, LCD_D6_op12, LCD_D7_op13);

// ------------------------------------------------------------------------------------------------------------------
// Arduino Nano digital pin assignments
// ------------------------------------------------------------------------------------------------------------------
//
//                 0       Serial communications bus
//                 1       Serial communications bus

const byte ENCODER_A_ip2 = 2; // input from encoder pin A, pin D2 pulsed low
const byte ENCODER_B_ip3 = 3; // input from encoder pin B, pin D3 pulsed low

// This project uses a 16 x 2 (1602) LCD module
// Perhaps create an object for 'LCD'?

// ------------------------------------------------------------------------------------------------------------------
// Arduino Nano analogue pin assignments
// ------------------------------------------------------------------------------------------------------------------
//
const byte SW_SET1_ipA2 = A2;

// Input from Switch Set 1 selects one of 10 pull down resistors.
// The resulting voltage at A2 is read during loop() and triggers an
// operation.

//    100: BAND_UP      Shifts VFO frequency up one band
//    209: BAND_DOWN    Shifts VFO frequency down one band
//    305: ENCODER_PB   Pressing the encoder push button changes the current
//                      tuning step with each short press (tens, hundreds, or
//                      thousands of Hz)
//
//
//                A3    Not used
//                A4    Used for SDA
//                A5    Used for SCL

// ------------------------------------------------------------------------------------------------------------------
// User Preferences
// ------------------------------------------------------------------------------------------------------------------

const int NUMBER_OF_BANDS = 5;
// The FL50B has five selectable bands

// ------------------------------------------------------------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------------------------------------------------------------

// Frequency Shift
// ---------------------------
unsigned long Txlo = 5712400; // Frequency of local oscillator in FR-50B in Hz

// Push-button
// ---------------------------
byte ButtonNumber;
byte OldButtonNumber = 0;

// I2C device addresses
// ---------------------------
// si5351  x60

// See: https://forum.arduino.cc/t/defining-a-struct-array/43699/2

// Combined Typedef and Structure declaration for 'band parameter set' records.
// These parameters can change with each band
typedef struct
{
    boolean active;
    uint32_t band;
    uint32_t radix;
} BandSet_type;

BandSet_type BandSet[NUMBER_OF_BANDS]; // array of band parameter sets
byte BandIndex;                        // index into BandSet array (representing the current band)
byte BandIndexPrevious;

Si5351 si5351; // I2C address defaults to x60 in the NT7S lib
Rotary r = Rotary(ENCODER_B_ip3, ENCODER_A_ip2);

bool FunctionState = false;
// if true, the next button pressed is interpreted as a special function button

// ------------------------------------------------------------------------------------------------------------------
// variables for controlling EEPROM writes
unsigned long LastFrequencyChangeTimer;
bool EepromUpdatedSinceLastFrequencyChange;
bool FrequencyChanged = false;

/**************************************/
/* Interrupt service routine for encoder frequency change */
/**************************************/
ISR(PCINT2_vect)
{
    unsigned char result = r.process();
    if (result == DIR_CW)
        ChangeFrequency(1);
    else if (result == DIR_CCW)
        ChangeFrequency(-1);
}

// ------------------------------------------------------------------------------------------------------------------
//     Start of setup() function.   Runs once.  Used to initialize variables,
//     pin modes, start using libraries, etc..
// ------------------------------------------------------------------------------------------------------------------

void setup()
{
    Serial.begin(
        9600);    // Sets the serial comms data rate in bits per second (baud)
    Wire.begin(); // Initiates the Wire library and connects the I2C bus

    Serial.println("FL-50B VFO "); // Print project name to console

    lcd.begin(16,
              2);            // Initialise LCD and define device type (16 columns and 2 rows)
    lcd.print("FL-50B VFO"); // Display the project name
    delay(2000);
    lcd.clear(); // Clear the display

    lcd.print("Build V01.00")

        ;                    // Display the Build information
    lcd.setCursor(0, 1);     // Set cursor to first column, second row
    lcd.print("11/10/2022"); // Display version date.
    delay(2000);
    lcd.clear(); // Clear the display

    lcd.print("Steve Rawlings "); // Display Author
    lcd.setCursor(0, 1);          // Set cursor to first column, second row
    lcd.print("G4ALG ");
    delay(2000);
    lcd.clear(); // Clear the display

    lcd.cursor(); // Show the underscore cursor.  Preparation for subsequent
    // use of the display.

    // Set digital and analogue pins

    pinMode(SW_SET1_ipA2, INPUT_PULLUP); // switch bank is Pull-up

    PCICR |= (1 << PCIE2); // Enable Pin Change Interrupt Control Register for the
                           // encoder
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
    sei();

    // load up BandSet array from EEPROM

    BandIndex = EEPROM.read(0);
    Serial.print("setup() eeprom: BandIndex=");
    Serial.println(BandIndex);
    if (BandIndex >= NUMBER_OF_BANDS)
        BandIndex = 1; // in case NUMBER_OF_BANDS has been reduced since the last
                       // run (EEPROM update)
    BandIndexPrevious = BandIndex;

    int element_len = sizeof(BandSet_type);
    for (int i = 0; i < NUMBER_OF_BANDS;)
    {
        EEPROM.get(1 + (i * element_len), BandSet[i]);
        i = i + 1;
    };

    // initialise and start the si5351 clocks

    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0,
                153125); // If using 27Mhz xtal, put 27000000 instead of 0 (0 is
                         // the default xtal freq of 25Mhz)

    si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);

    // VFO
    Serial.print("VFO=");
    Serial.println(BandSet[BandIndex].band);
    volatile uint32_t f;

    {
        if ((BandSet[BandIndex].band) >= 10000000)
            f = BandSet[BandIndex].band - 5172400;

        else
            f = BandSet[BandIndex].band + 5172400;
    }

    si5351.set_freq(f * SI5351_FREQ_MULT, SI5351_CLK0);

    //   si5351.set_freq((BandSet[BandIndex].band - 5172400) * SI5351_FREQ_MULT,
    //   SI5351_CLK0); // set CLK0 to VFO freq for current band
    si5351.output_enable(SI5351_CLK0, 1); // turn VFO on

    FrequencyChanged = true;
    LastFrequencyChangeTimer = millis();
    EepromUpdatedSinceLastFrequencyChange = false;
}

// ------------------------------------------------------------------------------------------------------------------
// End of setup() function
// ------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------
//     Start of loop() function.    This function loops repeatedly.
// ------------------------------------------------------------------------------------------------------------------
void loop()
{
    UpdateEeprom();
    RefreshLcd();

    // Update the display if the frequency has been changed (and we are not
    // transmitting)
    // if (!TxState && FrequencyChanged)                //  'and' changed to &&
    // what is the and operator doing????
    {
        volatile uint32_t f;

        {
            if ((BandSet[BandIndex].band) >= 10000000)
                f = BandSet[BandIndex].band - 5172400;

            else
                f = BandSet[BandIndex].band + 5172400;
        }

        si5351.set_freq(f * SI5351_FREQ_MULT, SI5351_CLK0);

        FrequencyChanged = false;
        LastFrequencyChangeTimer = millis();
        EepromUpdatedSinceLastFrequencyChange = false;
    } // endif FrequencyChanged

    //------------------------------------------------------------------------
    // if any of the buttons have been pressed...
    OldButtonNumber = 0; // clear the last command
    bool ButtonHeld = false;
    unsigned long ButtonPressedDuration = millis();

    ButtonNumber = GetSwSet1ButtonNumber();

    byte FirstButtonNumber = 0;
    while (ButtonNumber > 0)
    {
        // one of the multiplexed switches is being held down
        delay(5); // was 20
        if (FirstButtonNumber == 0)
            FirstButtonNumber = ButtonNumber;
        OldButtonNumber = ButtonNumber;
        ButtonNumber = GetSwSet1ButtonNumber();
    }

    ButtonNumber = FirstButtonNumber; // experiment to accept the first reading

    // if one of the buttons was pressed (and is now released) act on it...

    if (ButtonNumber == 1)
    {
        if (!FunctionState)
        {
            Serial.println("<B1>BAND DOWN");
            if (BandIndex == 0)
            {
                BandIndex = (NUMBER_OF_BANDS - 1);
            }
            else
            {
                BandIndex = BandIndex - 1;
            }
        }
        else
        {
            Serial.println("<F><B1>N/A");
            FunctionState = false;
        };
        FrequencyChanged = true;
    };

    if (ButtonNumber == 2)
    {
        // Button 2:
    };

    if (ButtonNumber == 3)
    {
    };

    if (ButtonNumber == 4)
    {
        if (!FunctionState)
        {
            if (ButtonHeld)
            {
                Serial.println("<B4>held-Tune");
                ButtonHeld = false;
            }
            else
            {
                Serial.println("<B4>BAND UP");
                //               Serial.print("B4 BandIndex="); Serial.print(BandIndex);
                //               Serial.print(" VFO=");
                //               Serial.println(BandSet[BandIndex].band);
                int BandIndexPrevious = BandIndex;
                if (BandIndex == (NUMBER_OF_BANDS - 1))
                    BandIndex = 0;
                else
                    BandIndex = BandIndex + 1;

                //               Serial.print("Aft BandIndex=");
                //               Serial.print(BandIndex); Serial.print(" VFO=");
                //               Serial.println(BandSet[BandIndex].band);
            }
        }
        else
        {

            lcd.clear();

            FunctionState = false;
        }
        FrequencyChanged = true;
    }

    if (ButtonNumber == 5)
    {
        Serial.println("<B5>Fn tgl");
        FunctionState = !FunctionState;
        if (FunctionState)
            Serial.println("Function...");
        FrequencyChanged = true;
    }

    if (ButtonNumber == 6)
    // Button 6: change frequency step up

    {
        if (!FunctionState)
            Serial.println("<B6>FREQUENCY STEP SIZE DOWN");
        else
            Serial.println("<F><B6>f step l");

        if (ButtonHeld)
        {
            Serial.println("<B6>held -- toggle IF filters");
        }
        else
        {

#if defined(SP_V) or defined(SP_6) or defined(SP_8) or defined(SP_9) or \
    defined(SP_11)
            switch (BandSet[BandIndex].radix)
            {
            case 10: {
                BandSet[BandIndex].radix = 100;
            }
            break;

    #ifdef SP_11
            case 100: {
                BandSet[BandIndex].radix = 10;
            }
            break;

    #else

            case 100: {
                BandSet[BandIndex].radix = 1000;
                // clear residual < 1kHz frequency component from the active VFO
                //  uint16_t f = BandSet[BandIndex].band % 1000;
                //  BandSet[BandIndex].band =  BandSet[BandIndex].band - f;
            }
            break;
    #endif

            case 1000: {
                BandSet[BandIndex].radix = 100;
            }
            break;

            case 10000: {
                BandSet[BandIndex].radix = 1000;
            }
            break;
            }
#else
            // default radix increment/decrement behaviour...
            switch (BandSet[BandIndex].radix)
            {
            case 10: {
                if (!FunctionState)
                {
                    // change radix up
                    BandSet[BandIndex].radix = 10000;
                    // clear residual < 1kHz frequency component from the active VFO
                    //  uint16_t f = BandSet[BandIndex].band % 1000;
                    //  BandSet[BandIndex].band = BandSet[BandIndex].band - f;
                }
                else
                {
                    FunctionState = false;
                    // change radix down
                    BandSet[BandIndex].radix = 100;
                    // clear residual < 100Hz frequency component from the active VFO
                    //  uint16_t f = BandSet[BandIndex].band % 100;
                    //  BandSet[BandIndex].band =  BandSet[BandIndex].band  - f;
                }
            }
            break;

            case 100: {
                if (!FunctionState)
                {
                    BandSet[BandIndex].radix = 10;
                }
                else
                {
                    FunctionState = false;
                    BandSet[BandIndex].radix = 1000;
                    // clear residual < 1kHz frequency component from the active VFO
                    //  uint16_t f = BandSet[BandIndex].band % 1000;
                    //  BandSet[BandIndex].band = BandSet[BandIndex].band - f;
                }
            }
            break;

            case 1000: {
                if (!FunctionState)
                {
                    BandSet[BandIndex].radix = 100;
                }
                else
                {
                    FunctionState = false;
                    BandSet[BandIndex].radix = 10000;
                }
                break;
            }

            case 10000: {
                if (!FunctionState)
                {
                    BandSet[BandIndex].radix = 1000;
                }
                else
                {
                    FunctionState = false;
                    BandSet[BandIndex].radix = 10;
                }
                break;
            }
            }
#endif
        } // else
        FrequencyChanged = true;
    }
}

// ------------------------------------------------------------------------------------------------------------------
// Functions
// ------------------------------------------------------------------------------------------------------------------

//**************************************/
//* Change frequency
//**************************************/

void ChangeFrequency(int dir)
{

    if (dir == 1) // Increment
    {
        BandSet[BandIndex].band =
            BandSet[BandIndex].band + BandSet[BandIndex].radix;
    }
    else
    {
        if (dir == -1) // Decrement

            BandSet[BandIndex].band =
                BandSet[BandIndex].band - BandSet[BandIndex].radix;
    };

    FrequencyChanged = 1;
};

/**
 * Take a reading of the front panel buttons and map it to a button number.
 * Requiring three consecutive identical readings before accepting the result.
*/
byte GetSwSet1ButtonNumber()
{
    byte numberOfConsecutiveButtonResultsRequired = 3;
    byte numberOfConsecutiveButtonResults = 0;
    byte previousButtonNumber = -1;

    while(numberOfConsecutiveButtonResults < numberOfConsecutiveButtonResultsRequired) {
        int currentButtonNumber = GetSwSet1ButtonNumberAtInstant();

        if (currentButtonNumber == previousButtonNumber) {
            numberOfConsecutiveButtonResults = numberOfConsecutiveButtonResults + 1;
        } else {
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
} // GetSwSet1ButtonNumber()

byte GetSwSet1ButtonNumberAtInstant()
// Take a reading of the front panel buttons and map it to a button number
// (0..4)
{
    byte b = 0;
    int z;
    z = ReadAnalogPin((byte)SW_SET1_ipA2);
    //  Serial.print("Frnt bttn="); Serial.println(z);

    if (z > 59 && z < 141)
        b = 4; // 100  band up
    else if (z > 168 && z <= 249)
        b = 1; // 209  band down
    else if (z > 254 && z <= 345)
        b = 6; // 305  radix
               // else if (z >= 0 && z <= 40)   b = ?;  //   0  net switch

    if (b > 0)
    {
        Serial.print(z);
        Serial.print(" : ");
    }
    return b;
} // GetSwSet1ButtonNumberAtInstant()

int ReadAnalogPin(byte p)
{
    // Take an averaged reading of analogue pin 'p'
    int i,
        val = 0,
        nbr_reads = 1; //  this used to average 2 readings!  Resulted in failure
                       //  to identify the correct button due to averaging of
                       //  voltage levels under contact bounce scenarios, hence
                       //  decoding to the wrong button number!
    for (i = 0; i < nbr_reads;)
    {
        val = val + analogRead(p);
        delay(1);
        i = i + 1;
    }
    return val / nbr_reads;
};

byte ReadSwitchSet2()
{
    // Reads the keyer memory buttons (switch set 2) and returns the button
    // nuSwitchSet2Buttoner as a byte; 0 if not pressed
    byte SwitchSet2Button = 0;
    int voltageLevel;
    //    voltageLevel = ReadAnalogPin(SW_SET2_ipA3);    // read the analog pin

    // Serial.print("Kyr pshbtn voltageLevel="); Serial.println(voltageLevel);

    // Voltage level (0-1023) to button look-up

    if (voltageLevel > 471 && voltageLevel < 551)
        SwitchSet2Button = 1; //  MEM1
    if (voltageLevel > 594 && voltageLevel < 674)
        SwitchSet2Button = 2; //  MEM2
    if (voltageLevel > 676 && voltageLevel < 756)
        SwitchSet2Button = 3; //  MEM3
    if (voltageLevel > 770 && voltageLevel < 850)
        SwitchSet2Button = 4; //  MEM4

    if (SwitchSet2Button > 0)
    {
        Serial.print("Keyer pushbutton=");
        Serial.print(SwitchSet2Button);
        Serial.print(", voltageLevel=");
        Serial.println(voltageLevel);
    }
    return SwitchSet2Button;
}

/*

void RxTxControl(char c)
{
    // if necessary, activates the receiver or the transmitter
    // 'c' may be either 'T' or 'R'
    // and, in the future, 'N' (Net) to indicate low level TX carrier 'on' while
    // receiver running.

    //  if(!TxState && c =='T') ChangeStateRxToTx();
    //  else
    //    if(TxState && c =='R') ChangeStateTxToRx();
    // else
    //  //  if(!TxState &&  c =='N')  receive_to_NET();

    // in  all other cases, do nothing!
}

*/

/*

Scratch Pad Area to learn about Classes.

This learning exercise is aimed at creating a Class and an Object to refresh the
LCD such that the current Function 'RefreshLcd' is replaced through an
object-oriented approach.



// clang-format: off
class Display {             // Create the Display class.  Here, display is a
noun. public:               // 'public' specifies that access to
attributes and
                            // methods are
                            // accessible from outside the class.


  Display()                // Create the Display public method.  Here, Display
is a verb.


  // assign the following values to the attribute 'lcd' within the LiquidCrystal
class
  // where LiquidCrystal is private to
  : lcd(LCD_RS_op8, LCD_E_op9, LCD_D4_op10, LCD_D5_op11, LCD_D6_op12,
LCD_D7_op13)
  , _text_changed{true}
  , _text("")
  { lcd.begin(16, 2);  }

  void Refresh()
  {

    if (!_text_changed)
      return;

    lcd.setCursor(0, 1);
    lcd.print(_text);

    _text_changed = false;
  }

  bool SetText(String text) {
    if (text.length() > 16) {
      return false;
    }

    _text = text;
    _text_changed = true;

    return true;
  }

private:
  String _text;
  bool _text_changed;
  LiquidCrystal lcd;
};

Display display;     //Create the display object of Display

void setup() {
  // set up the LCD's number of columns and rows:
  // lcd.begin(16, 2);
  // // Print a message to the LCD.
  // lcd.print("hello, world!");
}

void loop() {
  //
  auto new_text = String(millis() / 1000);

  auto ok = display.SetText(new_text);
  if (!ok) {
    // Serial.println(String("[Error] invalid text: ") + new_text);
  }

  display.Refresh();
}

*/

void RefreshLcd()
{
    // Update the LCD
    uint16_t f, g;
    uint32_t band_l;
    band_l = BandSet[BandIndex].band;

    // Check whether BandIndex has changed.  If so, set the cursor position and
    // update LCD with wavelength.

    if (BandIndex != _last_BandIndex)
    {
        lcd.setCursor(0, 0);

        if (BandIndex == 0)
        {
            lcd.print("80m");
        }
        else if (BandIndex == 1)
        {
            lcd.print("40m");
        }
        else if (BandIndex == 2)
        {
            lcd.print("20m");
        }
        else if (BandIndex == 3)
        {
            lcd.print("15m");
        }
        else if (BandIndex == 4)
        {
            lcd.print("10m");
        }
        lcd.print("  ");
        lcd.print(" ");
    }

    // Check whether BandIndex has changed.
    // If so, set the cursor and update LCD with FL-50B PA Loading and Grid crib
    // details for the band.

    if (BandIndex != _last_BandIndex)
    {
        lcd.setCursor(0, 1);

        if (BandIndex == 0) // 80m
        {
            lcd.print("LDG:5.0 GRID:2.3");
        }
        else if (BandIndex == 1) // 40m
        {
            lcd.print("LDG:7.5 GRID:1.5");
        }
        else if (BandIndex == 2) // 20m
        {
            lcd.print("LDG:4.2 GRID:6.2");
        }
        else if (BandIndex == 3) // 15m
        {
            lcd.print("LDG:4.6 GRID:4.3");
        }
        else if (BandIndex == 4) //10m
        {
            lcd.print("LDG:5.4 GRID:2.5");
        }
    }

    if (BandIndex != _last_BandIndex)
    {
        _last_BandIndex = BandIndex;
    }

    // Check whether frequency has changed.
    // If so, set the cursor position and update LCD with new frequency.

    if (band_l != _last_freq)
    {

        lcd.setCursor(6, 0);

        f = band_l / 1000000;
        if (f < 10)
            lcd.print(' ');
        lcd.print(f);
        lcd.print('.');

        f = (band_l % 1000000) / 1000;
        if (f < 100)
            lcd.print('0');
        if (f < 10)
            lcd.print('0');
        lcd.print(f);
        lcd.print('.');

        f = band_l % 1000;
        if (f < 100)
            lcd.print('0');
        if (f < 10)
            lcd.print('0');
        lcd.print(f);
        lcd.print(" ");
    }

    if (band_l != _last_freq)
    {
        _last_freq = band_l;
    }

    byte CursorPosition = 14;

    switch (BandSet[BandIndex].radix)
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

// ------------------------------------------------------------------------------------------------------------------
// EEPROM

void UpdateEeprom()
{
    if (abs(millis() - LastFrequencyChangeTimer) > 10000)
    {
        if (EepromUpdatedSinceLastFrequencyChange == false)
        {
            // do the eeprom write
            // Serial.println("*** eeprom write");
            EEPROM.write(
                0, BandIndex); // write the band index (BandIndex) to the first byte

            int element_len = sizeof(BandSet_type);
            for (int i = 0;
                 i < NUMBER_OF_BANDS;) // write each element of the BandSet array
            {
                EEPROM.put(1 + (i * element_len), BandSet[i]);
                i = i + 1;
            }
            EepromUpdatedSinceLastFrequencyChange = true;
        }
    }
};