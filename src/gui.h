#ifndef __GUI_H__
#define __GUI_H__

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <GL/glx.h>

void _set_view(int width, int height);

class Gui
{
    public:
        Gui(); 
        ~Gui() {};
        void toggleFullscreen() { toggleFullscreen(window_); } // no argument version of the same method below.
        GLuint live_input_texture_;

    private:
        bool live_input_texture_created_;
        GtkWidget *drawing_area_;
        GtkWidget *window_;
        //GLXContext glx_context_;
        bool create_live_input_texture();
        static void on_realize(GtkWidget *widget, gpointer data);
        static void on_delete_event(GtkWidget* widget, GdkEvent* event, gpointer data);
        static gboolean key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data);

        static gboolean on_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
        static gboolean on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data);
        static int onWindowStateEvent(_GtkWidget *widget, _GdkEventWindowState *event, void *data);
        void toggleFullscreen(GtkWidget* widget);
        void makeFullscreen(GtkWidget* widget);
        void makeUnfullscreen(GtkWidget* widget);
        void hideCursor();
        void showCursor();
        bool isFullscreen_;
};

#endif // __GUI_H__

