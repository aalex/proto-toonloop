/*
 * g++ -Wall -Werror $(pkg-config --libs --cflags clutter-gst-0.10) -o out gui2.cpp
 *
 * Little video pipeline and a Clutter GUI.
 */

#include <clutter-gst/clutter-gst.h>

void size_change(ClutterTexture *texture, gfloat width, gfloat height, gpointer user_data) {
    ClutterActor *stage;
    gfloat texture_x, texture_y, texture_width, texture_height;
    gfloat stage_width, stage_height;
  
    stage = clutter_actor_get_stage(CLUTTER_ACTOR(texture));
    if (stage == NULL)
        return;
    clutter_actor_get_size(stage, &stage_width, &stage_height);
    texture_height = (height * stage_width) / width;
    if (texture_height <= stage_height) {
        texture_width = stage_width;
        texture_x = 0;
        texture_y = (stage_height - texture_height) / 2;
    } else {
        texture_width  = (width * stage_height) / height;
        texture_height = stage_height;
        texture_x = (stage_width - texture_width) / 2;
        texture_y = 0;
    }
    clutter_actor_set_position(CLUTTER_ACTOR(texture), texture_x, texture_y);
    clutter_actor_set_size(CLUTTER_ACTOR(texture), texture_width, texture_height);
}

int main (int argc, char *argv[]) {
    ClutterTimeline  *timeline;
    ClutterActor *stage;
    ClutterActor *texture;
    GstPipeline *pipeline;
    GstElement *src;
    GstElement *warp;
    GstElement *colorspace;
    GstElement *sink;
  
    if (argc < 1) {
        g_error("Usage: %s", argv[0]);
        return 1;
    }
  
    clutter_init(&argc, &argv);
    gst_init(&argc, &argv);
  
    stage = clutter_stage_get_default();
  
    /* Make a timeline */
    timeline = clutter_timeline_new(1000); 
    g_object_set(timeline, "loop", TRUE, NULL);
  
    /* We need to set certain props on the target texture currently for
     * efficient/corrent playback onto the texture (which sucks a bit)  
    */
    texture = (ClutterActor *) g_object_new(CLUTTER_TYPE_TEXTURE, "sync-size", FALSE, "disable-slicing", TRUE, NULL);
    g_signal_connect(CLUTTER_TEXTURE (texture), "size-change", G_CALLBACK (size_change), NULL);
  
    /* Set up pipeline */
    pipeline = GST_PIPELINE(gst_pipeline_new (NULL));
    src = gst_element_factory_make("videotestsrc", NULL);
    warp = gst_element_factory_make("warptv", NULL);
    colorspace = gst_element_factory_make("ffmpegcolorspace", NULL);
    sink = clutter_gst_video_sink_new(CLUTTER_TEXTURE (texture));
    gst_bin_add_many(GST_BIN(pipeline), src, warp, colorspace, sink, NULL);
    gst_element_link_many(src, warp, colorspace, sink, NULL);
    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING);
  
    /* start the timeline */
    clutter_timeline_start(timeline);
    clutter_group_add(CLUTTER_GROUP(stage), texture);
    clutter_actor_show_all(stage);
  
    clutter_main();
    return 0;
}
