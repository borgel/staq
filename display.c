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

// populate the answer panel from this question
static void PopulateAnswers(WINDOW* window, SEQuestion* question) {
   /*
    * how?
    * manually scrolling stream of panels?
    * menu of some sort?
    */

   // clear out any existing contents
   wclear(window);

   if(question->answerCount == 0) {
      wprintw(window, "No answers to this question\n");
      return;
   }

   wprintw(window, "starting answers pw\n");
   for(int i = 0; i < question->answerCount; i++) {
      SEAnswer* a = &question->answers[i];

      // TODO style these
      // TODO checkmark for accepted
      wprintw(window, "Score %d - #%d\n", a->score, a->answerId);
      //wprintw(window, "%s\n", a->bodyMarkdown);
      wprintw(window, "%s\n", a->body);

      // horizontal divider between questions
      // TODO set color
      whline(window, 0, COLS);
   }
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

      // store a pointer to the question this relates to
      set_item_userptr(questionItems[i], q);
   }

   // TODO colors The functions set_menu_fore() and set_menu_back() can be
   // used to change the attribute of the selected item and unselected item.
   // The names are misleading. They don't change menu's foreground or
   // background which would have been useless.

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

   // answers pad
   // lines, cols
   WINDOW* padAnswers = newpad(LINES, 100);
   //WINDOW* padAnswers = subpad(stdscr, LINES, 100, 0, COLS / 2);
   //box(padAnswers, 0, 0);

   //TODO print questions in q panel

   // start with the questions selected
   MENU* focusedMenu = menuQuestions;

   // input handling loop
   int c;
   int flag = 0;
   ITEM* itemSelected;
   do {
      c = wgetch(stdscr);
      switch(c)
      {
         // movement
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

         case 'g':
            menu_driver(focusedMenu, REQ_FIRST_ITEM);
            break;
         case 'G':
            menu_driver(focusedMenu, REQ_LAST_ITEM);
            break;

         case KEY_ENTER:
         case 10:
            // populate answer panel based on the selected question

            itemSelected = current_item(focusedMenu);
            //p = item_userptr(cur);
            //p((char *)item_name(cur));
            PopulateAnswers(padAnswers, (SEQuestion*)item_userptr(itemSelected));
            pos_menu_cursor(focusedMenu);

            //touchwin(stdscr);
            // int prefresh(WINDOW *pad, int pminrow, int pmincol,
            //              int sminrow, int smincol, int smaxrow, int smaxcol);
            //prefresh(padAnswers, 0, 0, 0, 0, LINES, 100);
            prefresh(padAnswers, 0, 0,
                  0, 100, LINES, COLS);

            break;

         // ways to quit
         case 'q':
            flag = 1;
            break;

         default:
            break;
      }
      //refresh();
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
