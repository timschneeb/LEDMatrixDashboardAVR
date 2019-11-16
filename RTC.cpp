#include <DS1302.h>
#include <WString.h>
#include <Arduino.h>
#define rstPin 39
#define dataPin 37
#define clkPin 35

DS1302 rtc(rstPin, dataPin, clkPin);
void InitRTC(){
  rtc.writeProtect(false);
  rtc.halt(false);
  pinMode(31,OUTPUT);
  pinMode(33,OUTPUT);
  digitalWrite(31,HIGH); 
  digitalWrite(33,LOW); 
}

void SyncRTCTime(String input) { //Input 22:54:08
  rtc.writeProtect(false);
  rtc.halt(false);
  char i_buf[input.length()];
  input.toCharArray(i_buf, input.length());

  int h, m, s;
  char* sTime = strtok(i_buf, ":");
  int countT = 0;
  while (sTime != NULL) {
    int a = atoi(sTime);
    if (countT == 0) h = a;
    else if (countT == 1) m = a;
    else if (countT == 2) s = a;
    sTime = strtok(NULL, ":");
    countT++;
  }
  countT = 0;

  Time ot = rtc.time();
  Time t(ot.yr, ot.mon, ot.date, h, m, s, Time::kSunday);
  rtc.time(t);
}
void SyncRTCDate(String input) { //Input 2019-06-01
  rtc.writeProtect(false);
  rtc.halt(false);
  char i_buf[input.length()];
  input.toCharArray(i_buf, input.length());

  int y, mo, d;
  char* sDate = strtok(i_buf, "-");
  int countD = 0;
  while (sDate != NULL) {
    if (countD == 0) y = atoi(sDate);
    else if (countD == 1) mo = atoi(sDate);
    else if (countD == 2) d = atoi(sDate);
    sDate = strtok (NULL, "-");
    countD++;
  }
  countD = 0;
  Time ot = rtc.time();
  Time t(y, mo, d, ot.hr, ot.min, ot.sec, Time::kSunday);

  rtc.time(t);
}
void SyncRTCUsingBuildTime() {
  rtc.writeProtect(false);
  rtc.halt(false);

  int h, m, s, y, mo, d;
  char* sTime = strtok(__TIME__, ":");
  int countT = 0;
  while (sTime != NULL) {
    int a = atoi(sTime);
    if (countT == 0) h = a;
    else if (countT == 1) m = a;
    else if (countT == 2) s = a;
    sTime = strtok(NULL, ":");
    countT++;
  }
  countT = 0;

  char* sDate = strtok(__DATE__, "  ");
  int countD = 0;
  while (sDate != NULL) {
    if (countD == 0) {
      String s = String(sDate);
      if (s == "Jan") mo = 1;
      else if (s == "Feb") mo = 2;
      else if (s == "Mar") mo = 3;
      else if (s == "Apr") mo = 4;
      else if (s == "May") mo = 5;
      else if (s == "Jun") mo = 6;
      else if (s == "Jul") mo = 7;
      else if (s == "Aug") mo = 8;
      else if (s == "Sep") mo = 9;
      else if (s == "Oct") mo = 10;
      else if (s == "Nov") mo = 11;
      else if (s == "Dec") mo = 12;
    }
    else if (countD == 1) d = atoi(sDate);
    else if (countD == 2) y = atoi(sDate);
    sDate = strtok (NULL, " ");
    countD++;
  }
  countD = 0;

  Time t(y, mo, d, h, m, s , Time::kSunday);

  rtc.time(t);
}
//modes: h,m,s;d,M,y;T,D
String GetRTC(char mode){
  Time t = rtc.time();
  String hr = String(t.hr);
  String min = String(t.min);
  String sec = String(t.sec);
  String date = String(t.date);
  String mon = String(t.mon);
  String rtcTime = "";

  if (hr == "27" || min == "165" || sec == "85") {
    rtcTime = "Disconnected";
  } else {
    if(mode=='T'){
    if (hr.length() == 1)hr = "0" + hr;
    if (min.length() == 1)min = "0" + min;
    if (sec.length() == 1)sec = "0" + sec;
        rtcTime = hr + ":" + min + ":" + sec;
    }
    else if(mode=='D'){
        rtcTime = String(t.date) + "." + String(t.mon) + "." + String(t.yr);
    }
    else if(mode=='h'){
        if (hr.length() == 1)hr = "0" + hr;
        rtcTime = hr;
    }
    else if(mode=='m'){
        if (min.length() == 1)min = "0" + min;
        rtcTime = min;
    }
    else if(mode=='s'){
        if (sec.length() == 1)sec = "0" + sec;
        rtcTime = sec;
    }
    else if(mode=='d'){
        if (date.length() == 1)date = "0" + date;
        rtcTime = date;
    }
    else if(mode=='M'){
        if (mon.length() == 1)mon = "0" + mon;
        rtcTime = mon;
    }
    else if(mode=='y'){
        rtcTime = String(t.yr);
    }
  }
  return rtcTime;
}