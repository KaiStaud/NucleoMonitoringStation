#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <U8g2_for_Adafruit_GFX.h>

#include "RTClib.h"
#include "Adafruit_Si7021.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   11//9
#define OLED_CLK   13//10
#define OLED_DC    9//11
#define OLED_CS    10//12
#define OLED_RESET 8//13
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
//  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
// Switch to  Hardware SPI
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);

Adafruit_Si7021 sensor = Adafruit_Si7021();
U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;

RTC_DS3231 rtc;
TwoWire I2C3Port(PB4,PA8);

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting Display..."));
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  u8g2_for_adafruit_gfx.begin(display);                 // connect u8g2 procedures to Adafruit GFX
  Serial.println(F("Display started, attaching u8g2..."));


  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  display.clearDisplay();                               // clear the graphcis buffer  
  u8g2_for_adafruit_gfx.setFontDirection(0);            // left to right (this is default)
  u8g2_for_adafruit_gfx.setForegroundColor(WHITE);      // apply Adafruit GFX color
  
  u8g2_for_adafruit_gfx.setFont(u8g2_font_siji_t_6x10);  // icon font
  u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)

  u8g2_for_adafruit_gfx.setFont(u8g2_font_7x13_te);  // extended font
  u8g2_for_adafruit_gfx.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2_for_adafruit_gfx.setCursor(0,0);                // start writing at this position
  display.display();  

  u8g2_for_adafruit_gfx.println("Display setup done!");
  u8g2_for_adafruit_gfx.println("Searching for RTC...");

I2C3Port.begin();
  if (! rtc.begin(&I2C3Port)) {
    u8g2_for_adafruit_gfx.println("Couldn't find RTC");
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  u8g2_for_adafruit_gfx.println("RTC setup done!");
  u8g2_for_adafruit_gfx.println("Searching for Sensor...");

  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true)
      ;
  }

  u8g2_for_adafruit_gfx.print("SensorID:");
  switch(sensor.getModel()) {
    case SI_Engineering_Samples:
      u8g2_for_adafruit_gfx.println("SI engineering samples"); break;
    case SI_7013:
      u8g2_for_adafruit_gfx.println("Si7013"); break;
    case SI_7020:
      u8g2_for_adafruit_gfx.println("Si7020"); break;
    case SI_7021:
      u8g2_for_adafruit_gfx.println("Si7021"); break;
    case SI_UNKNOWN:
    default:
      u8g2_for_adafruit_gfx.println("Unknown");
  }

   
  Serial.print("Rev(");
  Serial.print(sensor.getRevision());
  Serial.println(")");
  Serial.println("Serial #"); Serial.print(sensor.sernum_a, HEX); Serial.println(sensor.sernum_b, HEX);
  u8g2_for_adafruit_gfx.println("--Setup done!--");
  display.display();  
  delay(2000);

}

void loop() {
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

  u8g2_for_adafruit_gfx.setFont(u8g2_font_inb16_mf);

  display.clearDisplay();
  u8g2_for_adafruit_gfx.setCursor(0,38);             // Start at top-left corner
  u8g2_for_adafruit_gfx.print(sensor.readTemperature(), 2);u8g2_for_adafruit_gfx.println("°C");
  u8g2_for_adafruit_gfx.setCursor(0,60);             // Start at top-left corner
  u8g2_for_adafruit_gfx.print(sensor.readHumidity(), 2);u8g2_for_adafruit_gfx.println("%");

     DateTime now = rtc.now();
  u8g2_for_adafruit_gfx.setCursor(15,14);             // Start at top-left corner

 u8g2_for_adafruit_gfx.setFont(u8g2_font_luBIS12_tf);
    u8g2_for_adafruit_gfx.print(now.hour(), DEC);
    u8g2_for_adafruit_gfx.print(':');
    u8g2_for_adafruit_gfx.print(now.minute(), DEC);
    u8g2_for_adafruit_gfx.print(':');
    u8g2_for_adafruit_gfx.print(now.second(), DEC);
    u8g2_for_adafruit_gfx.println();
  display.drawRect(4,0, 120, 18, WHITE);
  display.display();

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    delay(3000);
}

