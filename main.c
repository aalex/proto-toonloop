#include <clutter/clutter.h>

#define PKGDATADIR "./data/"
#define IMGFILE "example.jpg" // works with svg too

static ClutterActor *load_image(int width, int height)
{
    ClutterActor *actor = NULL;
    actor = clutter_texture_new ();
    gboolean success;
    GError *error = NULL;
    success = clutter_texture_set_from_file(CLUTTER_TEXTURE(actor), PKGDATADIR IMGFILE, &error);
    if (! success)
        g_print("Could not load image.\n");
    if (error)
        g_error("Error loading image: %s\n", error->message);
    return actor;
}

int main(int argc, char *argv[])
{
    clutter_init(&argc, &argv);
    ClutterActor *stage = NULL;
    stage = clutter_stage_get_default();
    int width = 640;
    int height = 480;
    clutter_actor_set_size(stage, width, height);
    
    ClutterActor *image = load_image(width, height);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage), image);
    clutter_actor_show_all(stage);
    clutter_main();
    return 0;
}
