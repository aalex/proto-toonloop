/**
 * Clutter-GTK prototype
 */
#include "config.h"
#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <math.h>
#include <glib/gi18n.h>
#include <clutter-gtk/clutter-gtk.h>

#define NHANDS  2
#define WINWIDTH 400
#define WINHEIGHT 400
#define RADIUS 150

void proto_setup();

typedef struct SuperOH
{
  ClutterActor *hand[NHANDS];
  ClutterGroup   *group;
  GdkPixbuf      *bgpixb;
} SuperOH; 

static gboolean fullscreen = FALSE;

/* input handler */
void on_key_or_button(ClutterStage *stage, ClutterEvent *event, gpointer data)
{
    if (event->type == CLUTTER_KEY_PRESS)
    {
        g_print ("*** key press event (key:%c) ***\n", clutter_event_get_key_symbol (event));
        if (clutter_event_get_key_symbol(event) == CLUTTER_q)
	        gtk_main_quit();
    }
}

/* Timeline handler */
void frame_cb(ClutterTimeline *timeline, gint msecs, gpointer data)
{
//     SuperOH *oh = (SuperOH *)data;
//     gint i;
//     guint rotation = clutter_timeline_get_progress(timeline) * 360.0f;
//   
//     /* Rotate everything clockwise about stage center*/
//     clutter_actor_set_rotation(CLUTTER_ACTOR(oh->group), CLUTTER_Z_AXIS, rotation, WINWIDTH / 2, WINHEIGHT / 2, 0);
//   
//     for (i = 0; i < NHANDS; i++)
//     {
//         /* rotate each hand around there centers */
//         clutter_actor_set_rotation(
//             oh->hand[i],
//             CLUTTER_Z_AXIS,
//             - 6.0 * rotation,
//             clutter_actor_get_width(oh->hand[i]) / 2,
//             clutter_actor_get_height(oh->hand[i]) / 2,
//             0);
//     }
}

static void on_fullscreen_clicked(GtkButton *button, GtkWindow *window)
{
    if (!fullscreen)
    {
        gtk_window_fullscreen(window);
        fullscreen = TRUE;
    } else {
        gtk_window_unfullscreen(window);
        fullscreen = FALSE;
    }
}

void on_window_destroyed()
{
    gtk_main_quit();
}

void proto_setup()
{
  ClutterTimeline *timeline;
  ClutterActor *stage;
  ClutterColor stage_color = { 0x61, 0x64, 0x8c, 0xff };
  GtkWidget *window, *clutter;
  GtkWidget *label, *button, *vbox;
  GdkPixbuf *pixbuf;
  SuperOH *oh;
  gint i;
  pixbuf = gdk_pixbuf_new_from_file("/usr/share/icons/Human/48x48/stock/generic/folder-new.png", NULL);

  if (!pixbuf)
      g_error("pixbuf load failed");

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroyed), NULL);

  vbox = gtk_vbox_new(FALSE, 6);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  clutter = gtk_clutter_embed_new();
  gtk_widget_set_size_request(clutter, WINWIDTH, WINHEIGHT);

  gtk_container_add(GTK_CONTAINER(vbox), clutter);

  stage = gtk_clutter_embed_get_stage(GTK_CLUTTER_EMBED(clutter));
  // label:
  label = gtk_label_new("This is a label");
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
  // fullscreen button:
  button = gtk_button_new_with_label("Fullscreen");
  gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_FULLSCREEN, GTK_ICON_SIZE_BUTTON));
  g_signal_connect(button, "clicked", G_CALLBACK(on_fullscreen_clicked), window);
  gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
  // quit button:
  button = gtk_button_new_from_stock(GTK_STOCK_QUIT);
  g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), window);
  gtk_box_pack_end(GTK_BOX (vbox), button, FALSE, FALSE, 0);
  
  /* and its background color */

  clutter_stage_set_color(CLUTTER_STAGE(stage), &stage_color);

  oh = g_new(SuperOH, 1);

  /* create a new group to hold multiple actors in a group */
  oh->group = CLUTTER_GROUP(clutter_group_new ());
  
  for (i = 0; i < NHANDS; i++)
    {
      gint x, y, w, h;
#if 1
      /* Create a texture from pixbuf, then clone in to same resources */
      if (i == 0)
       oh->hand[i] = gtk_clutter_texture_new_from_pixbuf(pixbuf);
     else
       oh->hand[i] = clutter_clone_new(oh->hand[0]);
#else
      ClutterColor colour = { 255, 0, 0, 255 };

      oh->hand[i] = clutter_rectangle_new_with_color(&colour);
      clutter_actor_set_size(oh->hand[i], 50, 50);
#endif
      /* Place around a circle */
      w = clutter_actor_get_width(oh->hand[0]);
      h = clutter_actor_get_height(oh->hand[0]);

      x = WINWIDTH/2 + RADIUS * cos(i * M_PI / (NHANDS/2)) - w/2;
      y = WINHEIGHT/2 + RADIUS * sin(i * M_PI / (NHANDS/2)) - h/2;

      clutter_actor_set_position(oh->hand[i], x, y);

      /* Add to our group group */
      clutter_group_add(oh->group, oh->hand[i]);
    }

  /* Add the group to the stage */
  clutter_container_add_actor(CLUTTER_CONTAINER(stage), CLUTTER_ACTOR(oh->group));

  g_signal_connect (stage, "key-release-event", G_CALLBACK(on_key_or_button), oh);

  gtk_widget_show_all(window);

  /* Only show the actors after parent show otherwise it will just be
   * unrealized when the clutter foreign window is set. widget_show
   * will call show on the stage.
   */
  clutter_actor_show_all(CLUTTER_ACTOR(oh->group));

  /* Create a timeline to manage animation */
  timeline = clutter_timeline_new(6000);
  g_object_set(timeline, "loop", TRUE, NULL);   /* have it loop */

  /* fire a callback for frame change */
  g_signal_connect(timeline, "new-frame", G_CALLBACK(frame_cb), oh);

  /* and start it */
  clutter_timeline_start(timeline);
}

int main(int argc, char *argv[])
{
  GError *error;

  error = NULL;
  gtk_clutter_init_with_args (&argc, &argv, NULL, NULL, NULL, &error);
  if (error)
      g_error ("Unable to initialize: %s", error->message);

  proto_setup();
  gtk_main();
  return 0;
}

