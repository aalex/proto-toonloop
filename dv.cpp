#include <gst/gst.h>
#include <gtk/gtk.h>
#include <string>

void cb_new_src_pad(GstElement *  /*srcElement*/, GstPad * srcPad, gpointer data)
{
    GstElement *sinkElement = static_cast<GstElement*>(data);

    if (std::string("video") == gst_pad_get_name(srcPad))
    {
        g_print("Got video stream from DV\n");
    }
    else if (std::string("audio") == gst_pad_get_name(srcPad))
    {
        g_print("Got audio stream from DV, ignoring\n");
        return; // possible to get audio streams from dv with wanting them
    }
    else {
        g_print("Ignoring unknown stream from DV\n");
        return;
    }

    GstPad *sinkPad;

    sinkPad = gst_element_get_static_pad(sinkElement, "sink");

    if (GST_PAD_IS_LINKED(sinkPad))
    {
        g_object_unref(sinkPad);        // don't link more than once
        return;
    }
    g_print("Dv1394: linking new srcpad to sinkpad.\n");
    GstPadLinkReturn ret = gst_pad_link(srcPad, sinkPad);
    g_assert(ret == GST_PAD_LINK_OK);
    gst_object_unref(sinkPad);
}

int main()
{
    gst_init(0, 0);
    gtk_init(0, 0);
    GstElement *src, *sink, *colorspace, *scale, *dvdemux, *queue, *dvdec;

    src = gst_element_factory_make("dv1394src", 0);
    dvdemux = gst_element_factory_make("dvdemux", 0);
    queue = gst_element_factory_make("queue", 0);
    dvdec = gst_element_factory_make("dvdec", 0);
    colorspace = gst_element_factory_make("ffmpegcolorspace", 0);
    scale = gst_element_factory_make("videoscale", 0);
    sink = gst_element_factory_make("xvimagesink", 0);

    GstElement *pipeline = gst_pipeline_new("fuck");
    gst_bin_add_many(GST_BIN(pipeline), src, dvdemux, queue, dvdec, colorspace, scale, sink, NULL);
    gst_element_link_many(src, dvdemux, NULL);
    gst_element_link_many(queue, dvdec, colorspace, scale, sink, NULL);

    g_signal_connect(dvdemux, "pad-added", 
            G_CALLBACK(cb_new_src_pad), queue);


    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);

    gtk_main();
    
    return 0;
}
