/*
 * Toonloop
 *
 * Copyright 2010 Alexandre Quessy
 * <alexandre@quessy.net>
 * http://www.toonloop.com
 *
 * Toonloop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Toonloop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the gnu general public license
 * along with Toonloop.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <clutter/clutter.h>

static gboolean on_stage_button_press(ClutterStage *stage, ClutterEvent *event, gpointer data)
{
    gfloat x = 0;
    gfloat y = 0;
    clutter_event_get_coords(event, &x, &y);
    g_print("Stage clicked at (%f, %f)\n", x, y);
    return TRUE; /* Stop further handling of this event. */
}

int main(int argc, char *argv[])
{
    ClutterColor stage_color = { 0x00, 0x00, 0x00, 0xff }; /* Black */
    clutter_init(&argc, &argv);
  
    /* Get the stage and set its size and color: */
    ClutterActor *stage = clutter_stage_get_default();
    clutter_actor_set_size(stage, 200, 200);
    clutter_stage_set_color(CLUTTER_STAGE(stage), &stage_color);
  
    /* Show the stage: */
    clutter_actor_show(stage);
  
    /* Connect a signal handler to handle mouse clicks and key presses on the stage: */ 
    g_signal_connect(stage, "button-press-event",
    G_CALLBACK(on_stage_button_press), NULL);
  
    /* Start the main loop, so we can respond to events: */
    clutter_main();
  
    return 0;
}

