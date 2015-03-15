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

#define SCREEN_WIDTH_THIRD (COLS / 3)
// asking windows for their bounds when they are not at the
// top of the panel stack gives you the current bounds, not
// their top-of-the-stack bounds
#define WIN_FULL_WIDTH     (2 * SCREEN_WIDTH_THIRD)

static int g_initialized = 0;

// yep, this module is using globals for now. Sorry 'bout that
static WINDOW* windowQuestions;
static WINDOW* windowAnswers;

static PANEL* panelQuestions;
static PANEL* panelAnswers;

// populate the answer panel from this question
static void PopulateAnswers(WINDOW* window, SEQuestion* question) {
   // clear out any existing contents
   wclear(window);

   if(question->answerCount == 0) {
      wprintw(window, "No answers to this question\n");
      return;
   }

   // print the question
   wprintw(window, "%d - %s\n", question->score, question->title);
   wprintw(window, "%s\n", question->body);

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
}

DispError DoDisplay(SEQuestion** questions, int numQuestions) {
   if(!g_initialized) {
      return DISP_NOT_INIT;
   }
   if(!questions || !*questions) {
      return DISP_BAD_PARAM;
   }

   MENU* menuQuestions = NULL;
   InitQuestionsWindow(&menuQuestions, questions, numQuestions);

   int minX, minY;
   getbegyx(windowAnswers, minY, minX);

   // protect the borders
   minX++;
   minY++;

   // these coords are where we will draw the pad. but we don't need a window
   // FIXME #lines? there is a max content length
   WINDOW* padAnswersContent = newpad(999, WIN_FULL_WIDTH - 2);

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
               PopulateAnswers(padAnswersContent, (SEQuestion*)item_userptr(itemSelected));
               pos_menu_cursor(menuQuestions);

               // reset the text position counter
               answersTop = 0;

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
               //FIXME pagesize macro?
               answersTop += (LINES - 2);
               break;
            case KEY_PPAGE:
               answersTop -= (LINES - 2);
               answersTop = (answersTop < 0) ? 0 : answersTop;
               break;

            case 'g':
               answersTop = 0;
               break;

            case 'G':
               // not sure how to easily find the bottom row of text
               answersTop = 999 - (LINES - 2);
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
      }

      // Update the panel stacking order
      update_panels();

      if(!questionsFocused) {
         // draw the answers pad, if it should be shown
         prefresh(padAnswersContent, answersTop, 0,
               minY, minX, LINES - 2, COLS - 2);
      }

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
   // each full height, 2/3 of screen
   windowQuestions = newwin(LINES, WIN_FULL_WIDTH, 0, 0);
   windowAnswers = newwin(LINES, WIN_FULL_WIDTH, 0, SCREEN_WIDTH_THIRD);

   keypad(windowQuestions, TRUE);
   keypad(windowAnswers, TRUE);

   // not sure we want this. the menu itself handles scroll
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
