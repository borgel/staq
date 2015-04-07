/**
 *
 */

#include <curses.h>

#include "libsoldout/markdown.h"

int RenderMarkdown(WINDOW* window, char* text) {
   //TODO do the markdown
   //see mkd2term.c in libsoldout to see how its done. can I graft that here?
   //can I take its printing fxns verbatim and run them against window?
   //start at its main, and shove those guts here. then work outward
   return 0;
}
