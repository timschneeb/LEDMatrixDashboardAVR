#include <RGBmatrixPanel.h>
#include "RTC.h"
#include "BMP280.h"
#include "Serial.h"
#include "MatrixPanel.h"

#define LED_BLACK 0

#define LED_RED_VERYLOW (3 << 11)
#define LED_RED_LOW (7 << 11)
#define LED_RED_MEDIUM (15 << 11)
#define LED_RED_HIGH (31 << 11)

#define LED_GREEN_VERYLOW (1 << 5)
#define LED_GREEN_LOW (15 << 5)
#define LED_GREEN_MEDIUM (31 << 5)
#define LED_GREEN_HIGH (63 << 5)

#define LED_BLUE_VERYLOW 3
#define LED_BLUE_LOW 7
#define LED_BLUE_MEDIUM 15
#define LED_BLUE_HIGH 31

#define LED_ORANGE_VERYLOW (LED_RED_VERYLOW + LED_GREEN_VERYLOW)
#define LED_ORANGE_LOW (LED_RED_LOW + LED_GREEN_LOW)
#define LED_ORANGE_MEDIUM (LED_RED_MEDIUM + LED_GREEN_MEDIUM)
#define LED_ORANGE_HIGH (LED_RED_HIGH + LED_GREEN_HIGH)

#define LED_PURPLE_VERYLOW (LED_RED_VERYLOW + LED_BLUE_VERYLOW)
#define LED_PURPLE_LOW (LED_RED_LOW + LED_BLUE_LOW)
#define LED_PURPLE_MEDIUM (LED_RED_MEDIUM + LED_BLUE_MEDIUM)
#define LED_PURPLE_HIGH (LED_RED_HIGH + LED_BLUE_HIGH)

#define LED_CYAN_VERYLOW (LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_CYAN_LOW (LED_GREEN_LOW + LED_BLUE_LOW)
#define LED_CYAN_MEDIUM (LED_GREEN_MEDIUM + LED_BLUE_MEDIUM)
#define LED_CYAN_HIGH (LED_GREEN_HIGH + LED_BLUE_HIGH)

#define LED_WHITE_VERYLOW (LED_RED_VERYLOW + LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_WHITE_LOW (LED_RED_LOW + LED_GREEN_LOW + LED_BLUE_LOW)
#define LED_WHITE_MEDIUM (LED_RED_MEDIUM + LED_GREEN_MEDIUM + LED_BLUE_MEDIUM)
#define LED_WHITE_HIGH (LED_RED_HIGH + LED_GREEN_HIGH + LED_BLUE_HIGH)

#define CLK 11
#define OE 9
#define LAT 10
#define A A0
#define B A1
#define C A2
#define D A3

#include "Bitmaps/wlanfull.c"
#include "Bitmaps/wlansemi.c"
#include "Bitmaps/wlanno.c"
#include "Bitmaps/wlanerror.c"
#include "Fonts/MinimalPixel.h"

static const uint8_t PROGMEM WLAN[][8] = {
    {B01111100,
     B10000010,
     B00111000,
     B01000100,
     B00010000},
    {B00000000,
     B00000000,
     B00111000,
     B01000100,
     B00010000},
    {B00000000,
     B00000000,
     B00000000,
     B00000000,
     B00010000}};

static const uint8_t PROGMEM LOADER[][8] = {
    {B00110000,
     B00001000,
     B10001000,
     B10001000,
     B01110000,
     B00000000,
     B00000000,
     B00000000},
    {B01010000,
     B10001000,
     B10001000,
     B10001000,
     B01110000,
     B00000000,
     B00000000,
     B00000000},
    {B01100000,
     B10000000,
     B10001000,
     B10001000,
     B01110000,
     B00000000,
     B00000000,
     B00000000},
    {B01110000,
     B10001000,
     B10000000,
     B10001000,
     B01110000,
     B00000000,
     B00000000,
     B00000000},
    {B01110000,
     B10001000,
     B10001000,
     B10000000,
     B01100000,
     B00000000,
     B00000000,
     B00000000},
    {B01110000,
     B10001000,
     B10001000,
     B10001000,
     B01010000,
     B00000000,
     B00000000,
     B00000000},
    {B01110000,
     B10001000,
     B10001000,
     B00001000,
     B00110000,
     B00000000,
     B00000000,
     B00000000},
    {B01110000,
     B10001000,
     B00001000,
     B10001000,
     B01110000,
     B00000000,
     B00000000,
     B00000000}};
const int LOADER_LEN = sizeof(LOADER) / 8;

RGBmatrixPanel *matrix = new RGBmatrixPanel(A, B, C, D, CLK, LAT, OE, true, 64);

bool bmpsuccess = false;

String currentScrollText = "";
uint16_t currentScrollColor = LED_RED_VERYLOW;
String scrollqueue = "";
int scrollPosition = matrix->width();
int scrollMin = 0;
int loaderCount = 0;
int wlanCount = 2;
long loopCount = 0;
bool loaderEnabled = false;

serial *ser = new serial;
void setup()
{
  InitRTC();
  bmpsuccess = initBMP280();
  //SyncRTCUsingBuildTime();
  matrix->begin();
  Serial.begin(9600);
  matrix->setTextColor(LED_RED_MEDIUM);
  matrix->setTextWrap(false);
  AddScrollQueue(F("64x32 LED Matrix"));
  AddScrollQueue(GetRTC('D'));

  //matrix->drawFastHLine(8, 10, 48, LED_ORANGE_VERYLOW);
}

void loop()
{
  ser->recvWithStartEndMarkers();
  ser->showNewData();
  matrix->fillScreen(0);

  PrintTime(2, 2, 6);
  ScrollText();

  //Statusbar
  PrintTemp(1, 30, 4);

  //matrix->drawBitmap(14,26,WLAN[wlanCount],8,5,LED_ORANGE_VERYLOW);
  if ((loopCount % 6) == 0)
  {
    wlanCount--;
    if (wlanCount < 0)
      wlanCount = 2;
  }
  if (loaderEnabled)
    matrix->drawBitmap(matrix->width() - 6, 26, LOADER[loaderCount], 8, 5, LED_ORANGE_VERYLOW);
  if ((loopCount % 2) == 0)
  {
    loaderCount++;
    if (loaderCount > 7)
      loaderCount = 0;
  }

  matrix->swapBuffers(false);
  if (++loopCount > 1000000)
    loopCount = 0;
}
void ShowLoader(bool on)
{
  loaderEnabled = on;
}
void AddScrollQueue(String str)
{
  scrollqueue = scrollqueue + str + "^";
}
String ReadQueue()
{
  String remaining = "";
  String next = "";

  char *tmp;
  int i = 0;
  char c[scrollqueue.length() + 1];
  scrollqueue.toCharArray(c, scrollqueue.length() + 1);
  tmp = strtok(c, "^");
  while (tmp)
  {
    if (i == 0)
      next = String(tmp);
    else
      remaining = remaining + "^" + String(tmp);
    tmp = strtok(NULL, "^");
    i++;
  }

  scrollqueue = remaining;
  scrollMin = next.length() * -1 * 6;
  return next;
}
void ScrollText(int y = 14)
{
  if (currentScrollText == "")
  {
    scrollPosition = matrix->width();
    String read = ReadQueue();
    char *tmp;
    int i = 0;
    char c[read.length() + 1];
    read.toCharArray(c, read.length() + 1);
    tmp = strtok(c, "|");
    while (tmp)
    {
      if (i == 0)
      {
        currentScrollText = String(tmp);
        currentScrollColor = LED_ORANGE_VERYLOW;
      }
      else
      {
        char *tmp2;
        int j = 0;
        char c[strlen(tmp) + 1];
        strcpy(c, tmp);
        tmp2 = strtok(c, ",");

        int r = 0, g = 0, b = 0;
        while (tmp2)
        {
          Serial.println(tmp2);
          if (j == 0)
            r = atoi(tmp2);
          else if (j == 1)
            g = atoi(tmp2);
          else if (j == 2)
            b = atoi(tmp2);
          tmp2 = strtok(NULL, ",");
          j++;
        }
        if (!(r == 0 && g == 0 && b == 0))
        {
          currentScrollColor = matrix->Color333(r, g, b);
          Serial.println(currentScrollColor);
        }
      }
      tmp = strtok(NULL, "|");
      i++;
    }
  }
  else
  {
    //Serial.println(scrollPosition);
    if ((--scrollPosition) < scrollMin)
    {
      currentScrollText = "";
    }
    else
    {
      --scrollPosition;
      matrix->setFont();
      matrix->setTextColor(currentScrollColor);
      matrix->setCursor(scrollPosition, y);
      matrix->print(currentScrollText);
    }
  }
}

void PrintTemp(int x = 18, int y = 18, int wd = 6)
{
  int cursorX = x;
  int cursorY = y;
  int wide = wd;

  matrix->setFont(&MinimalPixel);

  matrix->setTextSize(1);

  matrix->setCursor(cursorX, cursorY);
  String t = String(round((readTemperature() * 10) / 10) - 2) + "`";
  if (!bmpsuccess || t.indexOf("-145") == 0)
  {
    matrix->setCursor(cursorX, cursorY);
    matrix->print("Fail");
  }
  else
  {
    for (int w = 0; w < 5; w++)
    {
      matrix->setCursor(cursorX, cursorY);
      matrix->setTextColor(LED_ORANGE_VERYLOW);
      matrix->print(t.charAt(w));
      cursorX += wide;
    }
  }
}

void PrintTime(int x = 9, int y = 7, int wd = 6)
{
  int cursorX = x;
  int cursorY = y;
  int wide = wd;
  matrix->setTextSize(1);
  matrix->setFont();

  matrix->setCursor(cursorX, cursorY);
  matrix->setTextColor(LED_RED_VERYLOW);
  matrix->print('<');
  cursorX += wide;

  String h = GetRTC('h');
  String m = GetRTC('m');
  String s = GetRTC('s');
  String yr = GetRTC('y');
  if (h == "Disconnected")
  {
    matrix->print("RTC fail");
    cursorX += wide * 8;
    matrix->setCursor(cursorX - 1, cursorY);
    matrix->setTextColor(LED_RED_VERYLOW);
    matrix->print('>');
    cursorX += wide;
    ShowLoader(true);
  }
  else 
  {
    if (yr == "2000") ShowLoader(true); //Check rtc data
    else ShowLoader(false);
    for (int w = 0; w < 2; w++)
    {
      matrix->setCursor(cursorX, cursorY);
      matrix->setTextColor(matrix->ColorHSV(30, 1000, 100, false));
      matrix->print(h.charAt(w));
      cursorX += wide;
    }

    matrix->setCursor(cursorX, cursorY);
    matrix->setTextColor(LED_ORANGE_VERYLOW);
    matrix->print(':');
    cursorX += wide;

    for (int w = 0; w < 2; w++)
    {
      matrix->setCursor(cursorX, cursorY);
      matrix->setTextColor(matrix->ColorHSV(30, 1000, 100, false));
      matrix->print(m.charAt(w));
      cursorX += wide;
    }

    matrix->setCursor(cursorX, cursorY);
    matrix->setTextColor(LED_ORANGE_VERYLOW);
    matrix->print(':');
    cursorX += wide;

    String s = GetRTC('s');
    for (int w = 0; w < 2; w++)
    {
      matrix->setCursor(cursorX, cursorY);
      matrix->setTextColor(matrix->ColorHSV(30, 1000, 100, false));
      matrix->print(s.charAt(w));
      cursorX += wide;
    }

    matrix->setCursor(cursorX - 1, cursorY);
    matrix->setTextColor(LED_RED_VERYLOW);
    matrix->print('>');
    cursorX += wide;
  }
}