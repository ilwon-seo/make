#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <DS1302.h>  //https://github.com/msparks/arduino-ds1302
#include <MillisTimer.h>

LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
int recvPin = 2;                     // Tell IRremote which Arduino pin is connected to the IR Receiver (TSOP4838)
// Set the appropriate digital I/O pin connections. These are the pin
// assignments for the Arduino as well for as the DS1302 chip. See the DS1302
// datasheet:
//
//   http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
const int kCePin   = 9;    // Chip Enable
const int kIoPin   = 10;   // Input/Output
const int kSclkPin = 11;   // Serial Clock
IRrecv irrecv(recvPin);
IRsend irsend;                          // * An IR LED must be connected to Arduino PWM pin 3.
DS1302 rtc(kCePin, kIoPin, kSclkPin);    // Create a DS1302 object.

// Create a timer that fires every 1000 milliseconds.
MillisTimer timer1 = MillisTimer(1000);
bool isSended = false;
// This is the function that is called when the timer expires.
void myTimerFunction(MillisTimer &mt)
{
  Serial.print(Constant("Repeat: "));
  Serial.println(mt.getRemainingRepeats());
}

//+=======================================================================
// Configure the Arduino
//
void  setup ( )
{
  Serial.begin(9600);   // Status message will be sent to PC at 9600 baud
  irrecv.enableIRIn();   // Start the receiver
  lcd.init();            //initialize the lcd
  lcd.backlight();       //open the backlight
  // Initialize a new chip by turning off write protection and clearing the
  // clock halt flag. These methods needn't always be called. See the DS1302
  // datasheet for details.
  // rtc.writeProtect(false);
  // rtc.halt(false);
  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.
  // Time t(2018, 8, 15, 13, 13, 00, Time::kWednesday);
  // rtc.time(t);         // Set the time and date on the chip.

  // timer
  timer1.setInterval(1000);
  timer1.expiredHandler(myTimerFunction);
  timer1.setRepeats(60);
  //timer1.setRepeats(180);
}
 
//+=============================================================================
// Display IR code
//
void  ircode (decode_results *results)
{
  // Panasonic has an Address
  if (results->decode_type == PANASONIC) {
    Serial.print(results->address, HEX);
    Serial.print(":");
  }
 
  // Print Code
  Serial.print(results->value, HEX);
}
 
//+=============================================================================
// Display encoding type
//
void  encoding (decode_results *results)
{
  switch (results->decode_type) {
    default:
    case UNKNOWN:      Serial.print("UNKNOWN");       break ;
    case NEC:          Serial.print("NEC");           break ;
    case SONY:         Serial.print("SONY");          break ;
    case RC5:          Serial.print("RC5");           break ;
    case RC6:          Serial.print("RC6");           break ;
    case DISH:         Serial.print("DISH");          break ;
    case SHARP:        Serial.print("SHARP");         break ;
    case JVC:          Serial.print("JVC");           break ;
    case SANYO:        Serial.print("SANYO");         break ;
    case MITSUBISHI:   Serial.print("MITSUBISHI");    break ;
    case SAMSUNG:      Serial.print("SAMSUNG");       break ;
    case LG:           Serial.print("LG");            break ;
    case WHYNTER:      Serial.print("WHYNTER");       break ;
    case AIWA_RC_T501: Serial.print("AIWA_RC_T501");  break ;
    case PANASONIC:    Serial.print("PANASONIC");     break ;
    case DENON:        Serial.print("Denon");         break ;
  }
}

 //+=============================================================================
// Display encoding type (String)
//
String  encodingStr (decode_results *results)
{
  switch (results->decode_type) {
    default:
    case UNKNOWN:      return "UNKNOWN";       break ;
    case NEC:          return "NEC";           break ;
    case SONY:         return "SONY";          break ;
    case RC5:          return "RC5";           break ;
    case RC6:          return "RC6";           break ;
    case DISH:         return "DISH";          break ;
    case SHARP:        return "SHARP";         break ;
    case JVC:          return "JVC";           break ;
    case SANYO:        return "SANYO";         break ;
    case MITSUBISHI:   return "MITSUBISHI";    break ;
    case SAMSUNG:      return "SAMSUNG";       break ;
    case LG:           return "LG";            break ;
    case WHYNTER:      return "WHYNTER";       break ;
    case AIWA_RC_T501: return "AIWA_RC_T501";  break ;
    case PANASONIC:    return "PANASONIC";     break ;
    case DENON:        return "Denon";         break ;
  }
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpInfo (decode_results *results)
{
  // Check if the buffer overflowed
  if (results->overflow) {
    Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWLEN");
    return;
  }
 
  // Show Encoding standard
  Serial.print("Encoding  : ");
  encoding(results);
  Serial.println("");
 
  // Show Code & length
  Serial.print("Code      : ");
  ircode(results);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
}
 
//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpRaw (decode_results *results)
{
  // Print Raw data
  Serial.print("Timing[");
  Serial.print(results->rawlen-1, DEC);
  Serial.println("]: ");
 
  for (int i = 1;  i < results->rawlen;  i++) {
    unsigned long  x = results->rawbuf[i] * USECPERTICK;
    if (!(i & 1)) {  // even
      Serial.print("-");
      if (x < 1000)  Serial.print(" ") ;
      if (x < 100)   Serial.print(" ") ;
      Serial.print(x, DEC);
    } else {  // odd
      Serial.print("     ");
      Serial.print("+");
      if (x < 1000)  Serial.print(" ") ;
      if (x < 100)   Serial.print(" ") ;
      Serial.print(x, DEC);
      if (i < results->rawlen-1) Serial.print(", "); //',' not needed for last one
    }
    if (!(i % 8))  Serial.println("");
  }
  Serial.println("");                    // Newline
}
 
//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpCode (decode_results *results)
{
  // Start declaration
  Serial.print("unsigned int  ");          // variable type
  Serial.print("rawData[");                // array name
  Serial.print(results->rawlen - 1, DEC);  // array size
  Serial.print("] = {");                   // Start declaration
 
  // Dump data
  for (int i = 1;  i < results->rawlen;  i++) {
    Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
    if ( i < results->rawlen-1 ) Serial.print(","); // ',' not needed on last one
    if (!(i & 1))  Serial.print(" ");
  }
 
  // End declaration
  Serial.print("};");  // 
 
  // Comment
  Serial.print("  // ");
  encoding(results);
  Serial.print(" ");
  ircode(results);
 
  // Newline
  Serial.println("");
 
  // Now dump "known" codes
  if (results->decode_type != UNKNOWN) {
 
    // Some protocols have an address
    if (results->decode_type == PANASONIC) {
      Serial.print("unsigned int  addr = 0x");
      Serial.print(results->address, HEX);
      Serial.println(";");
    }
 
    // All protocols have data
    Serial.print("unsigned int  data = 0x");
    Serial.print(results->value, HEX);
    Serial.println(";");
  }
}
void display(decode_results *results){
  String strVal = String(results->value,HEX);
  String strTiming = String(results->rawlen-1, DEC);
  String strBits = String(results->bits,DEC);
  //Serial.println(String(results->value,HEX));
  lcd.setCursor(0,0);                             // set the cursor to column 15, line 0
  lcd.print(strVal+"["+encodingStr(results)+"]"); // Print a message to the LCD.
  lcd.setCursor(0,1);                             // set the cursor to column 15, line 0  
  lcd.print(strTiming+" / "+strBits+" bits");     // Print a message to the LCD.
  delay(5000);                                    //wait for 5 seconds  
  lcd.clear();                                    //Clears the LCD screen and positions the cursor in the upper-left  corner.
}

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "SUN";
    case Time::kMonday: return "MON";
    case Time::kTuesday: return "TUE";
    case Time::kWednesday: return "WED";
    case Time::kThursday: return "THU";
    case Time::kFriday: return "FRI";
    case Time::kSaturday: return "SAT";
  }
  return "(unknown day)";
}

void displayRTC(){
  Time t = rtc.time();
  // Name the day of the week.
  const String day = dayAsString(t.day);
  // Format date and insert into the temporary buffer.
  char ymd[11];
  snprintf(ymd, sizeof(ymd), "%04d-%02d-%02d", t.yr, t.mon, t.date);
  // Format the time and insert into the temporary buffer.
  char hms[9];
  snprintf(hms, sizeof(hms), "%02d:%02d:%02d", t.hr, t.min, t.sec);
  char rpt[6];
  snprintf(rpt, sizeof(rpt), "(%03d)", timer1.getRemainingRepeats());
  lcd.setCursor(1,0);
  lcd.print(day.c_str());
  lcd.setCursor(5,0);
  lcd.print(ymd);
  lcd.setCursor(1,1);
  lcd.print(hms);
  lcd.setCursor(10,1);
  lcd.print(rpt);
}

//+=============================================================================
// The repeating section of the code
//
void  loop ( ){
  timer1.run();
  decode_results  results;       // Somewhere to store the results
  if (irrecv.decode(&results)) {   // Grab an IR code
    lcd.clear();                   // Clear the LCD screen and positions the cursor in the upper-left  corner.
    dumpInfo(&results);            // Output the results
    dumpRaw(&results);             // Output the results in RAW format
    dumpCode(&results);            // Output the results as source code
    Serial.println("");           // Blank line between entries
    display(&results);
    String strVal = String(results.value,HEX);
    if(strVal=="20df708f"){        // IR Code (to start countdown)
      if (!timer1.isRunning()) {
        timer1.start();
        Serial.println(Constant("Timer started!"));
      } else {
        timer1.reset();
        Serial.println(Constant("Timer resetted!"));
      }
    }
    delay(3000);
    irrecv.resume();              // Prepare for the next value
  }

// Test Function (serial COM monitor input) ////////////////////////////////////
  if(Serial.available()) {
    char input_char = (char)Serial.read();
    if(input_char == '0'){
      for (int i = 0; i < 3; i++){
        irsend.sendNEC(0x20DF708F, 32);   //Test IR Code for char '0'
        Serial.println("0x20DF708F, 32");
        delay(40);
      } 
    } else if(input_char == '1') {
      for (int i = 0; i < 3; i++) {
        irsend.sendNEC(0x20DF14EB, 32);  //Test IR Code for char '1'
        Serial.println("0x20DF14EB, 32");
        delay(40);
      }
    } else {
      Serial.println("You sent invalid command! Just only '0' or '1'");
    }
  }

// sendIR ////////////////////////////////////////////////////////////////////
  displayRTC();
  if (!timer1.isRunning()) {
    Serial.println(Constant("Timer finished!"));
  } else {
     if(1==timer1.getRemainingRepeats()){
      for (int i = 0; i < 3; i++){
        irsend.sendNEC(0x20DF10EF, 32);   //IR Code
        Serial.println("0x20DF10EF, 32");
        delay(40);
      }       
      Serial.println(Constant("Timer running!"));
    }
  }
}
