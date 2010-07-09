#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <gst/gst.h>
#include <gtk/gtk.h>
#include <GL/glx.h>

class Pipeline
{
    public:
        void stop();
        Pipeline();
        ~Pipeline();
    private:
        GstElement* videosrc_;
        GstElement* appsink0_;
        GstPipeline* pipeline_;
        static void end_stream_cb(GstBus* bus, GstMessage* msg, GstElement* pipeline);
        static void on_new_buffer(GstElement *element, Pipeline *context);
};

#endif // __PIPELINE_H__
