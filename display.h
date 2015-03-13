/*
 * display arbitrary block of text
 * display arbitrary block of markdown
 * init...?
 *    add windows, arrange things?
 * show questions?
 */

#ifndef __DISP_H__
#define __DISP_H__

#include "stackexchange.h"

typedef enum {
   DISP_OK = 0,
   DISP_NOT_INIT,
   DISP_BAD_PARAM,
   DISP_ERROR
} DispError;

/**
 * Init function. Call this at the top.
 */
DispError DisplayInit();

/**
 * Final cleanup function. Call it at the end end. Do not expect
 * any display stuff to work after this.
 */
DispError DisplayCleanup();

/**
 * Blocking runloop to process all display things.
 * It will return when the user has exited the UI.
 */
DispError DoDisplay(SEQuestion** questions, int numQuestions);

#endif // __DISP_H__
