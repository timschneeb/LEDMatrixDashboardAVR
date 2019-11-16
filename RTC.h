#ifndef RTC_H_ 
#define RTC_H_

void InitRTC();
void SyncRTCTime(String input);
void SyncRTCDate(String input);
void SyncRTCUsingBuildTime();
String GetRTC(char mode);
#endif // RTC_H_