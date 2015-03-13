#include <stdlib.h>
#include <string.h>

#include <curses.h>
#include <menu.h>

#include <locale.h>

#include "display.h"

#define DCOLOR_NORMAL       1
#define DCOLOR_H1           2
#define DCOLOR_ALERT        3

static int g_initialized = 0;

// populate the answer panel
static void PopluateAnswers(SEAnswer* answers) {
   /*
    * how?
    * manually scrolling stream of panels?
    * menu of some sort?
    */
}

DispError DoDisplay(SEQuestion** questions, int numQuestions) {
   //printw("starting display\n");
   if(!g_initialized) {
      return DISP_NOT_INIT;
   }
   if(!questions || !*questions) {
      return DISP_BAD_PARAM;
   }

   // uhh, does this have to be null terminated? thats the example
   ITEM** questionItems = (ITEM**)calloc(numQuestions + 1, sizeof(ITEM*));

   SEQuestion* q;
   for(int i = 0; i < numQuestions; i++) {
      q = questions[i];

      //FIXME test code rm this
      //printw("Question %d\n", q->questionId);

      //fprintf(stderr, "reallocing for %d questions, size %lu\n", i, i * sizeof(ITEM*));
      //TODO stringify score?, MUST hold onto pointer so curses can use it later
      questionItems[i] = new_item(q->title, "");
   }

   MENU* menuQuestions = new_menu(questionItems);
   WINDOW *my_menu_win;

   // get height of window to get max lines to show
   int maxHeight = (LINES > numQuestions) ? numQuestions : LINES;

   //my_menu_win = newwin(10, 40, 4, 4);
   my_menu_win = stdscr;
   keypad(my_menu_win, TRUE);

   set_menu_win(menuQuestions, my_menu_win);
   //set_menu_sub(menuQuestions, derwin(my_menu_win, 6, 38, 3, 1));
   set_menu_format(menuQuestions, maxHeight, 1);

   set_menu_mark(menuQuestions, ">");
   // unicode check mark selector
   //set_menu_mark(menuQuestions, "\xe2\x9c\x93");

   //box(my_menu_win, 0, 0);
   post_menu(menuQuestions);
   wrefresh(my_menu_win);
   refresh();

   //TODO open 2 panels with menus (questions, and answer stream)

   //TODO print questions in q panel

   // start with the questions selected
   MENU* focusedMenu = menuQuestions;

   // input handling loop
   int c;
   int flag = 0;
   do {
      c = wgetch(stdscr);
      switch(c)
      {
         // TODO make scroll work in active panel
         case 'j':
         case KEY_DOWN:
            menu_driver(focusedMenu, REQ_DOWN_ITEM);
            break;
         case 'k':
         case KEY_UP:
            menu_driver(focusedMenu, REQ_UP_ITEM);
            break;
         case KEY_NPAGE:
            menu_driver(focusedMenu, REQ_SCR_DPAGE);
            break;
         case KEY_PPAGE:
            menu_driver(focusedMenu, REQ_SCR_UPAGE);
            break;

         // ways to quit
         case 'q':
            flag = 1;
            break;

         default:
            break;
      }
      refresh();
   } while(!flag);

   fprintf(stderr, "done with display\n");

   return DISP_OK;
}

DispError DisplayInit() {
   // don't initialize things more then once
   if(g_initialized) {
      return DISP_ERROR;
   }

   // get us to a place where we'll display Unicode
   setlocale(LC_CTYPE, "");

   // curses init
   initscr();
   start_color();
   cbreak();
   noecho();
   keypad(stdscr, TRUE);

   // build some color pairs for later
   init_pair(DCOLOR_NORMAL, COLOR_WHITE, COLOR_BLACK);
   init_pair(DCOLOR_H1, COLOR_CYAN, COLOR_BLACK);
   init_pair(DCOLOR_ALERT, COLOR_RED, COLOR_BLACK);

   g_initialized = 1;

   return DISP_OK;
}

DispError DisplayCleanup() {
   if(!g_initialized) {
      return DISP_OK;
   }

   g_initialized = 0;

   // TODO curses teardown? or does that happen after DoDisplay?
   endwin();

   return DISP_OK;
}
