#include <stdlib.h>
#include <string.h>

#include <curses.h>
#include <menu.h>
#include <panel.h>

#include <locale.h>

#include "display.h"

#define DCOLOR_NORMAL       1
#define DCOLOR_H1           2
#define DCOLOR_ALERT        3

#define WIDTH_THIRD        (COLS / 3)

static int g_initialized = 0;

// yep, this module is using globals for now. Sorry 'bout that
static WINDOW* windowQuestions;
static WINDOW* windowAnswers;

static PANEL* panelQuestions;
static PANEL* panelAnswers;

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

   // print the question
   wprintw(window, "%d - %s\n", question->score, question->title);
   wprintw(window, "%s\n", question->body);

   wprintw(window, "starting answers pw\n");
   for(int i = 0; i < question->answerCount; i++) {
      SEAnswer* a = &question->answers[i];

      // TODO style these
      // TODO checkmark for accepted
      wprintw(window, "Score %d - #%d\n", a->score, a->answerId);
      //wprintw(window, "%s\n", a->bodyMarkdown);
      wprintw(window, "%s\n", a->body);

      // horizontal divider between questions?
      // TODO set color
      //whline(window, 0, COLS);
   }
}

// bundle together setup stuff for questions
static void InitQuestionsWindow(MENU** menuQuestionsPtr, SEQuestion** questions, int numQuestions){
   // does this have to be null terminated? thats the example
   ITEM** questionItems = (ITEM**)calloc(numQuestions + 1, sizeof(ITEM*));

   SEQuestion* q;
   for(int i = 0; i < numQuestions; i++) {
      q = questions[i];

      //TODO stringify score?, MUST hold onto pointer so curses can use it later
      questionItems[i] = new_item(q->title, "");

      // store a pointer to the question this relates to
      set_item_userptr(questionItems[i], q);
   }

   // TODO colors The functions set_menu_fore() and set_menu_back() can be
   // used to change the attribute of the selected item and unselected item.
   // The names are misleading. They don't change menu's foreground or
   // background which would have been useless.

   *menuQuestionsPtr = new_menu(questionItems);
   MENU* menuQuestions = *menuQuestionsPtr;

   // get height of window to get max lines to show
   int maxX, maxY;
   getmaxyx(windowQuestions, maxY, maxX);
   int maxHeight = (maxY > numQuestions) ? numQuestions : maxY;

   set_menu_win(menuQuestions, windowQuestions);
   // set the menu in a subwindow, leaving room for a border
   set_menu_sub(menuQuestions, derwin(windowQuestions, maxY - 1, maxX - 1, 1, 1));
   set_menu_format(menuQuestions, maxHeight - 2, 1);

   set_menu_mark(menuQuestions, ">");
   // unicode check mark selector
   //set_menu_mark(menuQuestions, "\xe2\x9c\x93");

   post_menu(menuQuestions);
   //wrefresh(my_menu_win);
   //refresh();
}

DispError DoDisplay(SEQuestion** questions, int numQuestions) {
   //printw("starting display\n");
   if(!g_initialized) {
      return DISP_NOT_INIT;
   }
   if(!questions || !*questions) {
      return DISP_BAD_PARAM;
   }

   MENU* menuQuestions = NULL;
   InitQuestionsWindow(&menuQuestions, questions, numQuestions);

   //WINDOW* padAnswers = newpad(LINES, 100);
   // FIXME total size needs to enough for all answers? what if we hit the end?
   int maxX, maxY;
   getmaxyx(windowAnswers, maxY, maxX);

   int minX, minY;
   getbegyx(windowAnswers, minY, minX);

   // these coords are where we will draw the pad. but we don't need a window
   // WINDOW* windowAnswersInner = derwin(windowAnswers, maxY - 2, maxX - 2, 1, 1);

   //WINDOW* padAnswers = subpad(derwin(windowAnswers, maxY - 1, maxX - 1, 1, 1), LINES, 100, 0, 0);
   //WINDOW* padAnswers = subpad(windowAnswersInner, LINES, 100, 0, 0);

   // ugh, ok so here goes. there is a pad that holds all the
   // content, and a pad that acts as a movable window to
   // display a subset of it
   // FIXME maxX? also, #lines? there is a max content length
   WINDOW* padAnswersContent = newpad(999, maxX);
   // the viewport size is the max size of the inside of
   // windowAnswers
   WINDOW* padAnswersViewport = subpad(padAnswersContent,
         maxY - 2, maxX - 2, 0, 0);

   //wprintw(windowQuestions, "pad content vport @ %p\n", padAnswersContent);
   //wprintw(windowQuestions, "pad answers vport @ %p\n", padAnswersViewport);

   // final refresh before we run forever
   update_panels();
   doupdate();

   // input handling loop
   int c;
   int flag = 0;
   int answersTop = 0;
   int questionsFocused = 1;
   ITEM* itemSelected;
   do {
      c = wgetch(stdscr);

      // take different actions depending on which panel is focused
      if(questionsFocused) {
         // operate questions menu
         switch(c)
         {
            // movement
            case 'j':
            case KEY_DOWN:
               menu_driver(menuQuestions, REQ_DOWN_ITEM);
               break;
            case 'k':
            case KEY_UP:
               menu_driver(menuQuestions, REQ_UP_ITEM);
               break;
            case KEY_NPAGE:
               menu_driver(menuQuestions, REQ_SCR_DPAGE);
               break;
            case KEY_PPAGE:
               menu_driver(menuQuestions, REQ_SCR_UPAGE);
               break;

            case 'g':
               menu_driver(menuQuestions, REQ_FIRST_ITEM);
               break;
            case 'G':
               menu_driver(menuQuestions, REQ_LAST_ITEM);
               break;

            case KEY_ENTER:
            case 10:
               // FIXME move this into a fxn that manages state
               questionsFocused = 0;
               show_panel(panelAnswers);

               // populate answer panel based on the selected question
               itemSelected = current_item(menuQuestions);
               //PopulateAnswers(windowAnswersInner, (SEQuestion*)item_userptr(itemSelected));
               PopulateAnswers(padAnswersContent, (SEQuestion*)item_userptr(itemSelected));
               pos_menu_cursor(menuQuestions);

               // reset the position counter
               answersTop = 0;

               //touchwin(windowAnswers);
               // int prefresh(WINDOW *pad, int pminrow, int pmincol,
               //              int sminrow, int smincol, int smaxrow, int smaxcol);
               //prefresh(padAnswers, 0, 0, 0, 0, LINES, 100);
               //prefresh(padAnswers, 0, 0, 0, 0, LINES, COLS);

               break;

            // ways to quit
            case 'q':
               flag = 1;
               break;

            default:
               break;
         }
      }
      else {
         // operate answers pad
         switch(c)
         {
            // movement
            case 'j':
            case KEY_DOWN:
               answersTop++;
               break;

            case 'k':
            case KEY_UP:
               answersTop = (answersTop - 1 < 0) ? answersTop : answersTop - 1;
               break;

            case KEY_NPAGE:
               break;
            case KEY_PPAGE:
               break;

            case 'g':
               break;
            case 'G':
               break;

            // ways to quit
            case 'q':
               // FIXME move this into a fxn that manages state
               show_panel(panelQuestions);
               questionsFocused = 1;
               break;

            default:
               break;
         }

         // draw the answers pad
         //int prefresh(WINDOW *pad, int pminrow, int pmincol,
         //       int sminrow, int smincol, int smaxrow, int smaxcol);
         //prefresh(padAnswers, answersTop, 0, 0, 0, LINES, COLS);
         //prefresh(padAnswers, answersTop, 0,
         //       0, WIDTH_THIRD * 2, LINES, COLS);
         //prefresh(padAnswers, answersTop, 0, answersTop, 0, LINES, COLS);

         // pad, top y/x of conent to display, top left of screen, bottom right of screen (relative to screen start coords)
         // all are row, col
         //pnoutrefresh(padAnswers, 0, 0, 0, 200, 50, 50);
         touchwin(padAnswersContent);
         int res;
         res = pnoutrefresh(padAnswersViewport, answersTop, 0,
               minY, minX, maxY, maxX);
         //int res = pnoutrefresh(padAnswersViewport, 0, 0, 0, 0, 50, 50);

         wprintw(windowQuestions, " Code %d\n", res);
         wprintw(windowQuestions, "min (x %d, y %d)\n", minX, minY);
         wprintw(windowQuestions, "max (x %d, y %d)\n", maxX, maxY);
      }

      //wprintw(windowAnswers, "answers top = %d\n", answersTop);

      // Update the panel stacking order
      update_panels();

      // FIXME needed?
      wnoutrefresh(stdscr);


      //wprintw(windowQuestions, "top left src (x %d, y %d)\n", COLS-maxX, LINES-maxY);

      // Show it on the screen
      doupdate();

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

   // init stuff
   // full height, 2/3 of screen
   windowQuestions = newwin(LINES, 2 * WIDTH_THIRD, 0, 0);
   windowAnswers = newwin(LINES, 2 * WIDTH_THIRD, 0, WIDTH_THIRD);

   keypad(windowQuestions, TRUE);
   keypad(windowAnswers, TRUE);

   // not sure we want this
   //scrollok(windowAnswers, TRUE);

   // build panels too
   panelQuestions = new_panel(windowQuestions);
   panelAnswers = new_panel(windowAnswers);

   // bring the question panel to the top
   show_panel(panelQuestions);

   // FIXME do this here?
   box(windowQuestions, 0, 0);
   box(windowAnswers, 0, 0);

   // Update the stacking order. 2nd panel will be on top
   update_panels();

   // Show it on the screen
   doupdate();


   g_initialized = 1;

   return DISP_OK;
}

DispError DisplayCleanup() {
   if(!g_initialized) {
      return DISP_OK;
   }

   g_initialized = 0;

   endwin();

   return DISP_OK;
}
