#include <GL/glew.h>
#include <gst/gst.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gtk/gtkgl.h>
/*** Use OpenGL extensions. ***/
/* #include <gdk/gdkglglext.h> */
#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <gdk/gdkkeysyms.h>

#include "gltools.h"
#include "pipeline.h"
#include "draw.h"
#include "gui.h"
#include "application.h"

#define TIMEOUT_INTERVAL 10


/**************************************************************************
 * Global variable declarations.
 **************************************************************************/
// TODO: remove them
static gboolean animate = FALSE;
static guint timeout_id = 0;

/**************************************************************************
 * The following section contains the function prototype declarations.
 **************************************************************************/
// TODO: move to the header file
static void timeout_add(GtkWidget* widget);
static void timeout_remove(GtkWidget* widget);
static void toggle_animation(GtkWidget* widget);
static gboolean timeout (GtkWidget* widget);

/***
 *** The timeout function. Often in animations,
 *** timeout functions are suitable for continous
 *** frame updates.
 ***/
static gboolean timeout(GtkWidget *widget)
{
    //g_print (".");
    /*** Fill in the details here ***/
    /* Invalidate the whole window. */
    gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
    /* Update synchronously. */
    gdk_window_process_updates (widget->window, FALSE);
    return TRUE;
}


/**************************************************************************
 * The following section contains the timeout function management routines.
 **************************************************************************/

/***
 *** Helper functions to add or remove the timeout function.
 ***/

static void
timeout_add (GtkWidget *widget)
{
  if (timeout_id == 0)
    {
      timeout_id = g_timeout_add(TIMEOUT_INTERVAL, (GSourceFunc) timeout, widget);
    }
}

static void
timeout_remove (GtkWidget *widget)
{
  if (timeout_id != 0)
    {
      g_source_remove(timeout_id);
      timeout_id = 0;
    }
}

/***
 *** The "map_event" signal handler. Any processing required when the
 *** OpenGL-capable drawing area is mapped should be done here.
 ***/
static gboolean map_event(GtkWidget *widget, GdkEvent  *event, gpointer   data)
{
    g_print ("%s: \"map_event\":\n", gtk_widget_get_name (widget));
    if (animate)
        timeout_add (widget);
  
    return TRUE;
}

/***
 *** The "unmap_event" signal handler. Any processing required when the
 *** OpenGL-capable drawing area is unmapped should be done here.
 ***/
static gboolean unmap_event(GtkWidget *widget, GdkEvent  *event, gpointer   data)
{
    g_print ("%s: \"unmap_event\":\n", gtk_widget_get_name (widget));
    timeout_remove (widget);
    return TRUE;
}

/***
 *** Toggle animation.
 ***/
static void toggle_animation(GtkWidget *widget)
{
  animate = !animate;

  if (animate)
    {
      timeout_add (widget);
    }
  else
    {
      timeout_remove (widget);
      gdk_window_invalidate_rect (widget->window, &widget->allocation, FALSE);
    }
}

/**
 * Called when the window has become fullscreen or not.
 */
gboolean Gui::onWindowStateEvent(GtkWidget* widget, GdkEventWindowState *event, gpointer data)
{
    Gui *context = static_cast<Gui*>(data);
    context->isFullscreen_ = (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN);
    if (context->isFullscreen_)
        context->hideCursor();
    else
        context->showCursor();
    return TRUE;
}

/**
 * The "visibility_notify_event" signal handler. Any processing required
 * when the OpenGL-capable drawing area is visually obscured should be
 * done here.
 */
static gboolean visibility_notify_event(GtkWidget *widget, GdkEventVisibility *event, gpointer data)
{
  if (animate)
    {
      if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
	timeout_remove(widget);
      else
	timeout_add(widget);
    }
  return TRUE;
}

void Gui::hideCursor()
{
    // FIXME: this is because gtk doesn't support GDK_BLANK_CURSOR before gtk-2.16
    char invisible_cursor_bits[] = { 0x0 };
    static GdkCursor* cursor = 0;
    if (drawing_area_ != NULL) {
        if (cursor == 0) {
            static GdkBitmap *empty_bitmap;
            const static GdkColor color = {0, 0, 0, 0};
            empty_bitmap = gdk_bitmap_create_from_data(GDK_WINDOW(drawing_area_->window), invisible_cursor_bits, 1, 1);
            cursor = gdk_cursor_new_from_pixmap(empty_bitmap, empty_bitmap, &color, &color, 0, 0);
        }
        gdk_window_set_cursor(GDK_WINDOW(drawing_area_->window), cursor);
    }
}

void Gui::showCursor()
{
    /// sets to default
    if (drawing_area_ != NULL) {
        gdk_window_set_cursor(GDK_WINDOW(drawing_area_->window), NULL);
    }
}

gboolean Gui::key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    Gui *context = static_cast<Gui*>(data);
    switch (event->keyval)
    {
        case GDK_Escape:
            context->toggleFullscreen(widget);
            break;
        case GDK_q:
            // Quit application on ctrl-q, this quits the main loop
            // (if there is one)
            if (event->state & GDK_CONTROL_MASK)
            {
                g_print("Ctrl-Q key pressed, quitting.\n");
                Application::get_instance().quit();
            }
            break;
        default:
            break;
    }
    return TRUE;
}
/**
 * Handles the "realize" signal to take any necessary actions when the widget is instantiated on a particular display. (Create GDK resources in response to this signal.) 
 * 
 * The "realize" signal handler. All the OpenGL initialization
 * should be performed here, such as default background colour,
 * certain states etc.
 */
void Gui::on_realize(GtkWidget *widget, gpointer data)
{

    Gui *context = static_cast<Gui*>(data);

    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
  
    /*** OpenGL BEGIN ***/
    if (! gdk_gl_drawable_gl_begin(gldrawable, glcontext))
    {
        return;
    }
    //context->glx_context_ = glXGetCurrentContext();
    glDisable(GL_DEPTH_TEST);
  
    glClearColor(0.0, 0.0, 0.0, 1.0); // black background
    glViewport(0, 0, widget->allocation.width, widget->allocation.height);
    _set_view(widget->allocation.width / float(widget->allocation.height));
    gdk_gl_drawable_gl_end(gldrawable);
    /*** OpenGL END ***/
}

/***
 *** The "unrealize" signal handler. Any processing required when
 *** the OpenGL-capable window is unrealized should be done here.
 ***/
static void
unrealize (GtkWidget *widget,
	   gpointer   data)
{
  GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

  g_print ("%s: \"unrealize\"\n", gtk_widget_get_name (widget));

  /*** OpenGL BEGIN ***/
  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    return;

  /*** Fill in the details here. ***/

  gdk_gl_drawable_gl_end (gldrawable);
  /*** OpenGL END ***/
}

void Gui::on_delete_event(GtkWidget* widget, GdkEvent* event, gpointer data)
{
    Gui *context = static_cast<Gui*>(data);
    g_print("Window has been deleted.\n");
    Application::get_instance().quit();
}

void Gui::toggleFullscreen(GtkWidget *widget)
{
    // toggle fullscreen state
    isFullscreen_ ? makeUnfullscreen(widget) : makeFullscreen(widget);
}

void Gui::makeFullscreen(GtkWidget *widget)
{
    gtk_window_stick(GTK_WINDOW(widget)); // window is visible on all workspaces
    gtk_window_fullscreen(GTK_WINDOW(widget));
}

void Gui::makeUnfullscreen(GtkWidget *widget)
{
    gtk_window_unstick(GTK_WINDOW(widget)); // window is not visible on all workspaces
    gtk_window_unfullscreen(GTK_WINDOW(widget));
}

/**
 * Sets up the orthographic projection.
 * 
 * Makes sure height is always 1.0 in GL modelview coordinates.
 * 
 * Coordinates should give a rendering area height of 1
 * and a width of 1.33, when in 4:3 ratio.
*/
void _set_view(float ratio)
{
    float w = ratio;
    float h = 1.0;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w, w, -h, h, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
/**
 * Handles the "configure_event" signal to take any necessary actions when the widget changes size. 
 * 
 * The "configure_event" signal handler. Any processing required when
 * the OpenGL-capable drawing area is re-configured should be done here.
 * Almost always it will be used to resize the OpenGL viewport when
 * the window is resized.
 */
gboolean Gui::on_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    Gui *context = static_cast<Gui*>(data);
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
    std::cout << "on_configure_event" << std::endl;
    /*** OpenGL BEGIN ***/
    if (! gdk_gl_drawable_gl_begin(gldrawable, glcontext))
    {
        std::cout << "Could not begin OpenGL rendering" << std::endl; 
        return FALSE;
    }
    glViewport(0, 0, widget->allocation.width, widget->allocation.height);
    _set_view(widget->allocation.width / float(widget->allocation.height));
    gdk_gl_drawable_gl_end(gldrawable);
    /*** OpenGL END ***/
    return TRUE;
}
/**
 * Reads the data from the pipeline and updates an OpenGL texture.
 */
// TODO:
bool Gui::create_live_input_texture()
{
    Pipeline pipeline = Application::get_instance().get_pipeline();
    
    if (pipeline.has_new_live_input_data_) {
        std::cout << "has_new_live_input_data_" << std::endl;
        pipeline.has_new_live_input_data_ = false;
        /*
        if (! live_input_texture_created_)
        {
            live_input_texture_created_ = true;
            glGenTextures(1, &live_input_texture_);
        }
        int width = 
        int height = 
        char* buf = 
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, live_input_texture_);
        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buf);
        // TODO: simplify those parameters
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        */
    }
    return true;
}


// draws the stuff
void _draw()
{
    // DRAW STUFF HERE
    std::cout << "drawing" << std::endl;
    glDisable(GL_TEXTURE_RECTANGLE_ARB);
    glColor4f(1.0, 0.8, 0.2, 1.0);
    glPushMatrix();
    glScalef(0.5, 0.5, 1.0);
    draw::draw_square();
    glPopMatrix();

    glColor4f(1.0, 1.0, 0.0, 0.8);
    int num = 64;
    float x;
    for (int i = 0; i < num; i++)
    {
        x = (i / float(num)) * 4 - 2;
        draw::draw_line(float(x), -2.0, float(x), 2.0);
        draw::draw_line(-2.0, float(x), 2.0, float(x));
    }
    //TODO: glEnable(GL_TEXTURE_RECTANGLE_ARB);
}
/**
 * Handles the "expose_event" signal to redraw the contents of the widget.
 * 
 * The "expose_event" signal handler. All the OpenGL re-drawing should
 * be done here. This is repeatedly called as the painting routine
 * every time the 'expose'/'draw' event is signalled.
 */
gboolean Gui::on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
    Gui *gui = static_cast<Gui*>(data);
    /*** OpenGL BEGIN ***/
    if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext))
    {
      return FALSE;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _draw();

    if (gdk_gl_drawable_is_double_buffered(gldrawable))
    {
        gdk_gl_drawable_swap_buffers(gldrawable);
    } else {
        glFlush();
    }
    gdk_gl_drawable_gl_end(gldrawable);
    /*** OpenGL END ***/
    return TRUE;
}

/**
 * Exits the application if OpenGL needs are not met.
 */
Gui::Gui() :
    isFullscreen_(false)
{
    live_input_texture_created_ = false;
    //glx_context_ = NULL;
    gint major; 
    gint minor;
    gdk_gl_query_version(&major, &minor);
    g_print("\nOpenGL extension version - %d.%d\n", major, minor);
    /* Try double-buffered visual */

    GdkGLConfig* glconfig;
    // the line above does not work in C++ if the cast is not there.
    glconfig = gdk_gl_config_new_by_mode(static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGB | GDK_GL_MODE_DOUBLE));
    if (glconfig == NULL)
    {
        g_print("*** Cannot find the double-buffered visual.\n");
        g_print("*** Trying single-buffered visual.\n");
        /* Try single-buffered visual */
        glconfig = gdk_gl_config_new_by_mode(static_cast<GdkGLConfigMode>(GDK_GL_MODE_RGB));
        if (glconfig == NULL)
        {
            g_print ("*** No appropriate OpenGL-capable visual found.\n");
            exit(1);
        }
    }
    gltools::examine_gl_config_attrib(glconfig);
    // Main GTK window
    window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window_, 640, 480);
    gtk_window_move (GTK_WINDOW (window_), 300, 10);
    gtk_window_set_title(GTK_WINDOW (window_), "Toonloop 1.11 experimental");
    GdkGeometry geometry;
    geometry.min_width = 1;
    geometry.min_height = 1;
    geometry.max_width = -1;
    geometry.max_height = -1;
    gtk_window_set_geometry_hints(GTK_WINDOW(window_), window_, &geometry, GDK_HINT_MIN_SIZE);
    // connect window signals:
    
    drawing_area_ = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window_), drawing_area_);
    
    /* Set OpenGL-capability to the widget. */
    gtk_widget_set_gl_capability(drawing_area_, glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);
    
    g_signal_connect_after(G_OBJECT(drawing_area_), "realize", G_CALLBACK(on_realize), this);
    g_signal_connect(G_OBJECT(drawing_area_), "configure_event", G_CALLBACK(on_configure_event), this);
    g_signal_connect(G_OBJECT(drawing_area_), "expose_event", G_CALLBACK(on_expose_event), this);
    g_signal_connect (G_OBJECT(drawing_area_), "unrealize", G_CALLBACK(unrealize), NULL);

    /* For timeout function. */
    g_signal_connect (G_OBJECT (drawing_area_), "map_event", G_CALLBACK(map_event), NULL);
    g_signal_connect (G_OBJECT (drawing_area_), "unmap_event", G_CALLBACK(unmap_event), NULL);
    g_signal_connect (G_OBJECT (drawing_area_), "visibility_notify_event", G_CALLBACK(visibility_notify_event), NULL);
    
    g_signal_connect(G_OBJECT(window_), "delete-event", G_CALLBACK(on_delete_event), this);
    g_signal_connect(G_OBJECT(window_), "key-press-event", G_CALLBACK(key_press_event), this);
    // add listener for window-state-event to detect fullscreenness
    g_signal_connect(G_OBJECT(window_), "window-state-event", G_CALLBACK(onWindowStateEvent), this);

    //area where the video is drawn

    //avoid flickering when resizing or obscuring the main window
    gtk_widget_realize(drawing_area_);
    gdk_window_set_back_pixmap(drawing_area_->window, NULL, FALSE);
    gtk_widget_set_app_paintable(drawing_area_, TRUE);
    gtk_widget_set_double_buffered(drawing_area_, FALSE);
  
    gtk_widget_show_all(window_);
    // Let's start the constant rendering.
    toggle_animation(drawing_area_);
}

