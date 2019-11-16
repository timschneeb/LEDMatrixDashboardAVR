#ifndef MATRIXPANEL_H_ 
#define MATRIXPANEL_H_
void PrintTime(int x = 9, int y = 7, int wd = 6);
void PrintTimeNoBuffer(int x = 9, int y = 7, int wd = 6);
void PrintTemp(int x = 18, int y = 18, int wd = 6);
void PrintTempNoBuffer(int x = 18, int y = 18, int wd = 6);
void AddScrollQueue(String str);
String ReadQueue();
void ScrollText(int y = 14);
#endif