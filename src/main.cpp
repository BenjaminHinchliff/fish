#include <iostream>

#include <curses.h>

int main()
{
  initscr();
  printw("Hello Curses!!!");
  refresh();
  getch();
  endwin();
  
  return 0;
}
