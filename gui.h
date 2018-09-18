#ifndef GUI_H
#define GUI_H

#include <stdbool.h>

#include "settings.h"
#include "gui/xcore.h"
#include "gui/xdraw.h"
#include "stats/stats.h"

/*
 * Plan - i'm slowly moving more of this logic to xdraw/xcore components.
 * Goal is to make oxbarui/gui just a container of widgets that knows how to
 * call their render methods and stores their order, setting-up and passing
 * xcontext & xinfo to them and knowing their order. That's it.
 */
typedef struct oxbarui {
   /* not packed */
   xinfo_t          *xinfo;
   xdraw_context_t  *xcontext;

   /* core settings (static after init) */
   settings_t *settings;

   /* state */
   int      space_width;   /* width of a space " " in the font */
} oxbarui_t;

oxbarui_t* ui_init(settings_t *s);
void ui_free(oxbarui_t *ui);

/* the main draw method - renders the whole display */
void ui_draw(oxbarui_t *ui);

/*
 * These form the rendering pipeline w/ double buffering.
 * Start a full draw with ui_clear() to clear the display in a new buffer.
 * End with ui_flush() to flush all draw commands to the buffer and swap to
 * show that one.
 */
void ui_clear(oxbarui_t *ui);
void ui_flush(oxbarui_t *ui);

#endif
