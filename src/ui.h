/*
  PWR/SWR-Meter by Florian Thienel (DL3NEY)

  This software is published under the MIT License: https://www.tldrlegal.com/l/mit
  (c) Florian Thienel

  User Interface
*/

void initUi();
void showBargraph(int column, int row, int width, float max, float reading);
void printFloat(int column, int row, int width, float f);
void printInt(int column, int row, int width, int integer);
void printText(int column, int row, const char *text);