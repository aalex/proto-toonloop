/**
 * Clutter-GTK prototype
 */
#include <iostream>
#include "config.h"
#include <gtk/gtk.h>
#include <clutter/clutter.h>
#include <math.h>
#include <glib/gi18n.h>
#include <clutter-gtk/clutter-gtk.h>

#define NHANDS  2
// Defines the requested window width and height
#define WINWIDTH 640
#define WINHEIGHT 480
#define RADIUS 150

void proto_setup();

typedef struct GuiData {
   ClutterActor *live_input_image_; // actually a ClutterTexture
//  ClutterActor *playback_image_;
//  ClutterGroup   *group;
//  GdkPixbuf      *bgpixb;
} GuiData; 

static gboolean fullscreen = FALSE;

/* input handler */
void on_key_released(ClutterStage *stage, ClutterEvent *event, gpointer data) {
    if (event->type == CLUTTER_KEY_PRESS) {
        g_print ("*** key press event (key:%c) ***\n", clutter_event_get_key_symbol (event));
        if (clutter_event_get_key_symbol(event) == CLUTTER_q)
	        gtk_main_quit();
    }
}

/* Timeline handler */
void on_new_frame(ClutterTimeline *timeline, gint msecs, gpointer data)
{
//    std::cout << "on_new_frame" << std::endl; 
//     GuiData *gui = (GuiData *)data;
}

static void on_fullscreen_clicked(GtkButton *button, GtkWindow *window) {
    if (!fullscreen) {
        gtk_window_fullscreen(window);
        fullscreen = TRUE;
    } else {
        gtk_window_unfullscreen(window);
        fullscreen = FALSE;
    }
}

void on_stage_paint(ClutterStage *stage) {
    std::cout << "on_stage_paint" << std::endl; 
}

void on_window_destroyed() {
    gtk_main_quit();
}

void proto_setup() {
    ClutterTimeline *timeline;
    ClutterActor *stage;
    ClutterColor stage_color = { 0x00, 0x00, 0x00, 0xff };
    GtkWidget *window, *clutter_widget;
    GtkWidget *label, *button, *vbox;
    GdkPixbuf *pixbuf;
    GuiData *gui;
    gint i;

    pixbuf = gdk_pixbuf_new_from_file("/usr/share/icons/Human/48x48/stock/generic/folder-new.png", NULL);
    if (!pixbuf)
        g_error("pixbuf load failed");
    
    // The window contains a vbox which contains the clutter widget:
    // Window:
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroyed), NULL);
    // VBox:
    vbox = gtk_vbox_new(FALSE, 6);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    // Clutter widget:
    clutter_widget = gtk_clutter_embed_new();
    gtk_widget_set_size_request(clutter_widget, WINWIDTH, WINHEIGHT);
    gtk_container_add(GTK_CONTAINER(vbox), clutter_widget);
    stage = gtk_clutter_embed_get_stage(GTK_CLUTTER_EMBED(clutter_widget));
    // label:
    label = gtk_label_new(PACKAGE_STRING);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
    // fullscreen button:
    button = gtk_button_new_with_label("Fullscreen");
    gtk_button_set_image(GTK_BUTTON(button), gtk_image_new_from_stock(GTK_STOCK_FULLSCREEN, GTK_ICON_SIZE_BUTTON));
    g_signal_connect(button, "clicked", G_CALLBACK(on_fullscreen_clicked), window);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    
    // Background color:
    clutter_stage_set_color(CLUTTER_STAGE(stage), &stage_color);

    gui = g_new(GuiData, 1);
    // The live input image:
    gui->live_input_image_ = gtk_clutter_texture_new_from_pixbuf(pixbuf);
    clutter_actor_set_size(gui->live_input_image_, WINWIDTH / 2, WINHEIGHT / 2);
    clutter_actor_set_position(gui->live_input_image_, 0, 0);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage), CLUTTER_ACTOR(gui->live_input_image_));

    g_signal_connect(stage, "key-release-event", G_CALLBACK(on_key_released), gui);
    // g_signal_connect(stage, "paint", G_CALLBACK(on_stage_paint), NULL);

    gtk_widget_show_all(window);

    /* Only show the actors after parent show otherwise it will just be
     * unrealized when the clutter foreign window is set. widget_show
     * will call show on the stage.
     */
    clutter_actor_show_all(CLUTTER_ACTOR(gui->live_input_image_));

    /* Create a timeline to manage animation */
    timeline = clutter_timeline_new(6000);
    g_object_set(timeline, "loop", TRUE, NULL);   /* have it loop */
    /* fire a callback for frame change */
    g_signal_connect(timeline, "new-frame", G_CALLBACK(on_new_frame), gui);
    /* and start it */
    clutter_timeline_start(timeline);
}

int main(int argc, char *argv[])
{
    GError *error;

    error = NULL;
    gtk_clutter_init_with_args (&argc, &argv, NULL, NULL, NULL, &error);
    if (error)
        g_error ("Unable to initialize Clutter: %s", error->message);

    proto_setup();
    gtk_main();
    return 0;
}

