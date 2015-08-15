/***************************************************************************
 *
 * Multitouch X driver
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
 * Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
 *
 * Gestures
 * Copyright (C) 2008 Henrik Rydberg <rydberg@euromail.se>
 * Copyright (C) 2010 Arturo Castro <mail@arturocastro.net>
 * Copyright (C) 2011 Ryan Bourgeois <bluedragonx@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 **************************************************************************/

#include "gestures.h"
#include "mtouch.h"
#include "trig.h"

static void update_touches(struct Gestures* gs,
            const struct MConfig* cfg,
            struct MTState* ms)
{
    int i, j, dist;

    gs->move_dx = 0;
    gs->move_dy = 0;

    if (cfg->trackpad_disable >= 1)
        return;

    if ((!GETBIT(ms->touch_used, gs->pointer_touch_plus1-1))
                || (GETBIT(ms->touch[gs->pointer_touch_plus1-1].state, MT_INVALID))
                || (GETBIT(ms->touch[gs->pointer_touch_plus1-1].state, MT_RELEASED))) {
        gs->pointer_touch_plus1 = 0;
        #ifdef DEBUG_GESTURES
            xf86Msg(X_INFO, "update_touches: pointer touch removed\n");
        #endif
    }

    if ((!GETBIT(ms->touch_used, gs->button_touch_plus1-1))
                || (GETBIT(ms->touch[gs->button_touch_plus1-1].state, MT_INVALID))
                || (GETBIT(ms->touch[gs->button_touch_plus1-1].state, MT_RELEASED))) {
        gs->button_touch_plus1 = 0;
        gs->buttons = 0;
        #ifdef DEBUG_GESTURES
            xf86Msg(X_INFO, "update_touches: button touch removed\n");
        #endif
    }

    if (gs->pointer_touch_plus1) {
        gs->move_dx = (int)(ms->touch[gs->pointer_touch_plus1-1].dx*cfg->sensitivity);
        gs->move_dy = (int)(ms->touch[gs->pointer_touch_plus1-1].dy*cfg->sensitivity);
    }

    foreach_bit(i, ms->touch_used) {
        if ((!GETBIT(ms->touch[i].state, MT_INVALID))
                    && GETBIT(ms->touch[i].state, MT_NEW)) {
            if (!gs->pointer_touch_plus1) {
                if (gs->button_touch_plus1 != i+1) {
                    gs->pointer_touch_plus1 = i+1;
                    #ifdef DEBUG_GESTURES
                        xf86Msg(X_INFO, "update_touches: touch %d controls pointer\n", i);
                    #endif
                }
            } else if (!gs->button_touch_plus1) {
                if (gs->pointer_touch_plus1 != i+1) {
                    gs->button_touch_plus1 = i+1;
                    gs->buttons = 1;
                    foreach_bit(j, ms->touch_used) {
                        if (GETBIT(ms->touch[j].state, MT_INVALID)) {
                            gs->buttons = 4;
                        }
                    }
                    #ifdef DEBUG_GESTURES
                        xf86Msg(X_INFO, "update_touches: touch %d is a button press (mask = %d)\n", i, gs->buttons);
                    #endif
                }
            }
        }
    }
}

void gestures_init(struct MTouch* mt)
{
    memset(&mt->gs, 0, sizeof(struct Gestures));
}

void gestures_extract(struct MTouch* mt)
{
    update_touches(&mt->gs, &mt->cfg, &mt->state);
}

static int gestures_sleep(struct MTouch* mt, const struct timeval* sleep)
{
    return 0;
}

int gestures_delayed(struct MTouch* mt)
{
    return 0;
}

