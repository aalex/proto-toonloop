/*
 * g++ -Wall -Werror $(pkg-config --libs --cflags clutter-gst-0.10) -o out gui2.cpp
 *
 * Little video pipeline and a Clutter GUI.
 */

#include <clutter-gst/clutter-gst.h>
// #TODO:2010-08-01:aalex:Store image size in the gui class, or so.
// class GuiData {
//     public:
//         gfloat image_width;
//         gfloat image_height;
//         ClutterActor *stage_;
//         ClutterActor *texture_;
// };
// 
// void resize_texture_in_stage(ClutterTexture *texture, ClutterActor *stage, gfloat image_width, gfloat image_height, gfloat stage_width, gfloat stage_height) {
//     
// }

void on_texture_size_changed(ClutterTexture *texture, gfloat width, gfloat height, gpointer user_data) {
    ClutterActor *stage;
    gfloat set_x, set_y, set_width, set_height;
    gfloat stage_width, stage_height;
  
    stage = clutter_actor_get_stage(CLUTTER_ACTOR(texture));
    if (stage == NULL)
        return;
    clutter_actor_get_size(stage, &stage_width, &stage_height);
    set_height = (height * stage_width) / width;
    if (set_height <= stage_height) {
        set_width = stage_width;
        set_x = 0;
        set_y = (stage_height - set_height) / 2;
    } else {
        set_width  = (width * stage_height) / height;
        set_height = stage_height;
        set_x = (stage_width - set_width) / 2;
        set_y = 0;
    }
    clutter_actor_set_position(CLUTTER_ACTOR(texture), set_x, set_y);
    clutter_actor_set_size(CLUTTER_ACTOR(texture), set_width, set_height);
}

void on_stage_allocation_changed(ClutterActor *stage, ClutterActorBox *box, ClutterAllocationFlags *flags, gpointer user_data) {
    g_print("on_stage_allocation_changed\n");
}

int main (int argc, char *argv[]) {
    ClutterTimeline  *timeline;
    ClutterActor *stage;
    ClutterColor stage_color = { 0x00, 0x00, 0x00, 0xff };
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
    clutter_stage_set_user_resizable(CLUTTER_STAGE(stage), TRUE);
    g_signal_connect(stage, "allocation-changed", G_CALLBACK(on_stage_allocation_changed), NULL);
    clutter_stage_set_minimum_size(CLUTTER_STAGE(stage), 320, 240);
    clutter_stage_set_color(CLUTTER_STAGE(stage), &stage_color);
  
    /* Make a timeline */
    timeline = clutter_timeline_new(1000); 
    g_object_set(timeline, "loop", TRUE, NULL);
  
    /* We need to set certain props on the target texture currently for
     * efficient/corrent playback onto the texture (which sucks a bit)  
    */
    texture = (ClutterActor *) g_object_new(CLUTTER_TYPE_TEXTURE, "sync-size", FALSE, "disable-slicing", TRUE, NULL);
    g_signal_connect(CLUTTER_TEXTURE(texture), "size-change", G_CALLBACK(on_texture_size_changed), NULL);
  
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
