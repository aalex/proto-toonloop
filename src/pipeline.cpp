#include <gtk/gtk.h>
#include <GL/glx.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappbuffer.h>
#include <gdk/gdk.h>
#include <iostream>
#include <cstring> // for memcpy
#include "pipeline.h"
//#include "draw.h"
#include "application.h"
#include <sstream>

/**
 * GST bus signal watch callback 
 */
void Pipeline::end_stream_cb(GstBus* bus, GstMessage* message, GstElement* pipeline)
{
    bool stop_it = true;
    if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_ERROR)
    {
        gchar *debug = NULL;
        GError *err = NULL;
        gst_message_parse_error(message, &err, &debug);
        g_print("Error: %s\n", err->message);
        g_error_free(err);
        if (debug) 
        {
            g_print("Debug details: %s\n", debug);
            g_free(debug);
        }
    } else if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_EOS) {
        std::cout << "EOS: End of stream" << std::endl;
    } else if (GST_MESSAGE_TYPE(message) == GST_MESSAGE_WARNING) {
        gchar *debug = NULL;
        GError *err = NULL;
        gst_message_parse_warning(message, &err, &debug);
        g_print("Warning: %s\n", err->message);
        g_error_free(err);
        if (debug) 
        {
            g_print("Debug details: %s\n", debug);
            g_free(debug);
        }
        stop_it = false;
    }
    if (stop_it)
    {
        g_print("Stopping the stream and quitting.\n");
        //gst_element_set_state(pipeline, GST_STATE_NULL);
        //gst_object_unref(pipeline);
        // TODO: stop()
        Application::get_instance().quit();
        //gtk_main_quit();
    }
}

void Pipeline::stop()
{
    std::cout << "Stopping the GStreamer pipeline." << std::endl;
    gst_element_set_state(GST_ELEMENT(pipeline_), GST_STATE_NULL);
    gst_object_unref(pipeline_);
    // gtk main quit?
}

void Pipeline::on_new_buffer(GstElement *element, Pipeline *context)
{
    GstBuffer *buffer = 0;
    static size_t size = 0;

    /// FIXME: maybe replace with Concurrent queue?
    /* get the buffer from appsink */
    buffer = gst_app_sink_pull_buffer(GST_APP_SINK(element));

    // push the buffer
    size_t newSize = GST_BUFFER_SIZE (buffer);
    //std::cout << "Got a buffer of size: " <<  size << std::endl;

    Pipeline* pipeline = static_cast<Pipeline*>(context);
    // Copy the data into RAM
    if  (newSize != 0) {
        if (size != newSize) {
            size = newSize;
            std::cout << "reallocating the last frame data buffer with size " << size << std::endl;
            delete [] pipeline->last_frame_data_;
            pipeline->last_frame_data_ = new char[size];
        }
        memcpy(pipeline->last_frame_data_, GST_BUFFER_DATA(buffer), size);
        pipeline->has_new_live_input_data_ = true;
    }
    /* we don't need the appsink buffer anymore */
    gst_buffer_unref(buffer);
}

Pipeline::Pipeline()
{
    last_frame_data_ = new char[0]; // IMPORTANT!
    int width = 640;
    int height = 480;
    pipeline_ = NULL;
    
    pipeline_ = GST_PIPELINE(gst_pipeline_new("pipeline"));
    GstElement* glupload;
    
    // videotestsrc element
    videosrc_  = gst_element_factory_make("videotestsrc", "videotestsrc0");
    // capsfilter element #0
        // Possible values for the capture FPS:
        // 25/1 
        // 30000/1001
        // 30/1
    GstElement* capsfilter0 = gst_element_factory_make ("capsfilter", NULL);
    GstCaps *caps = gst_caps_new_simple("video/x-raw-rgb",
                                        "width", G_TYPE_INT, width,
                                        "height", G_TYPE_INT, height,
                                        "framerate", GST_TYPE_FRACTION, 30000, 1001,
                                        NULL); 
    g_object_set(capsfilter0, "caps", caps, NULL);
    gst_caps_unref(caps);
    
    // ffmpegcolorspace
    
    GstElement* ffmpegcolorspace0 = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace0");
    g_assert(ffmpegcolorspace0);
    GstElement* appsink0_ = gst_element_factory_make("appsink", "appsink0");
    g_assert(appsink0_);
    
    // add elements
    if (!videosrc_ or !capsfilter0)
    {
        g_print("one element could not be found \n");
        exit(1);
    }
    
    gst_bin_add(GST_BIN(pipeline_), videosrc_);
    gst_bin_add(GST_BIN(pipeline_), capsfilter0);
    gst_bin_add(GST_BIN(pipeline_), ffmpegcolorspace0);
    gst_bin_add(GST_BIN(pipeline_), appsink0_);
    gboolean is_linked;

    is_linked = gst_element_link(videosrc_, capsfilter0);
    if (!is_linked) { 
        g_print("Could not link %s to %s.\n", "videosrc_", "capsfilter0"); 
        exit(1); 
    }
    is_linked = gst_element_link(capsfilter0, ffmpegcolorspace0);
    if (!is_linked) { 
        g_print("Could not link %s to %s.\n", "capsfilter0", "ffmpegcolorspace0"); 
        exit(1); 
    }
    is_linked = gst_element_link(ffmpegcolorspace0, appsink0_);
    if (!is_linked) { 
        g_print("Could not link %s to %s.\n", "ffmpegcolorspace0", "appsink0_"); 
        exit(1); 
    }
    
    // Preparing the appsink:
    GstCaps *videoCaps; 
    std::ostringstream capsStr;

    /// FIXME: should detect caps from preceding element in pipeline if possible
    capsStr << "video/x-raw-rgb, width=" << width << ", height=" << height << ",bpp=16, depth=16"; 
    videoCaps = gst_caps_from_string(capsStr.str().c_str());

    g_object_set(G_OBJECT(appsink0_), "emit-signals", TRUE, "caps", videoCaps, NULL);
    //g_object_set(sink_, "max-buffers", MAX_BUFFERS, "drop", TRUE, NULL);
    g_signal_connect(appsink0_, "new-buffer", G_CALLBACK(on_new_buffer), this);
    gst_caps_unref(videoCaps);
    
    /* setup bus */
    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline_));
    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message::error", G_CALLBACK(end_stream_cb), this);
    g_signal_connect(bus, "message::warning", G_CALLBACK(end_stream_cb), this);
    g_signal_connect(bus, "message::eos", G_CALLBACK(end_stream_cb), this);
    gst_object_unref(bus);

    // make it play !!

    /* run */
    GstStateChangeReturn ret;
    ret = gst_element_set_state(GST_ELEMENT(pipeline_), GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_print ("Failed to start up pipeline!\n");
        /* check if there is an error message with details on the bus */
        GstMessage* msg = gst_bus_poll (bus, GST_MESSAGE_ERROR, 0);
        if (msg)
        {
          GError *err = NULL;
          gst_message_parse_error (msg, &err, NULL);
          g_print ("ERROR: %s\n", err->message);
          g_error_free (err);
          gst_message_unref (msg);
        }
        exit(1);
    }
}

Pipeline::~Pipeline() {}

