#include <clutter/clutter.h>

#define PKGDATADIR "./data/"
#define IMGFILE "example.jpg" // works with svg too


/**
 * Loads a fragment shader source from a file.
 */
static gboolean toon_load_fragment_source_file(ClutterShader *shader, gchar *dir_name, gchar *file_name)
{
    gchar *full_path = g_build_filename(dir_name, file_name, NULL);
    gchar *contents = NULL;
    gsize length;
    GError *error = NULL;

    if(! g_file_get_contents(full_path, &contents, &length, &error)) 
    {
        g_free(full_path);
        g_error_free(error);
        error = NULL;
        return FALSE;
    }
    //g_print("Finished reading %s\n", full_path);
    clutter_shader_set_fragment_source(shader, contents, -1);
    //g_print("Contents: %s\n", contents);
    g_free(contents);
    g_free(full_path);
    return TRUE;
}

static ClutterActor *load_custom_image(int width, int height)
{
    ClutterActor *actor = NULL;
    actor = clutter_texture_new ();
    gboolean success;
    GError *error = NULL;
    gchar *file_name = PKGDATADIR IMGFILE;
    success = clutter_texture_set_from_file(CLUTTER_TEXTURE(actor), file_name, &error);
    if (! success)
        g_print("Could not load image %s.\n", file_name);
    if (error)
        g_error("Error loading image %s: %s\n", file_name, error->message);
    return actor;
}

static void setup_custom_shader(ClutterActor *actor)
{
    ClutterShader *shader = NULL;
    shader = clutter_shader_new();
    //clutter_shader_set_fragment_source(shader, frag_source, -1);
    toon_load_fragment_source_file(shader, PKGDATADIR, "frag.brcosa.glsl");
    //toon_load_fragment_source_file(shader, PKGDATADIR, "frag.test.glsl");
    GError *error = NULL;
    clutter_shader_compile(shader, &error);
    if (error)
    {
        g_warning("Unable to init shader: %s", error->message);
        g_error_free(error);
    }
    else
    {
        clutter_actor_set_shader(actor, shader);
        // set uniform variables:
        clutter_actor_set_shader_param_float(actor, "saturation", 0.0);
        clutter_actor_set_shader_param_float(actor, "contrast", 1.0);
        clutter_actor_set_shader_param_float(actor, "brightness", 1.0);
        clutter_actor_set_shader_param_float(actor, "alpha", 1.0);
        clutter_actor_set_shader_param_float(actor, "opacity", 0.0);
        clutter_actor_set_shader_param_int(actor, "image", 0);
    }
}

static gboolean on_key_pressed(ClutterActor *actor, ClutterEvent *event, gpointer data)
{
    ClutterBindingPool *pool = NULL;
    pool = clutter_binding_pool_find (G_OBJECT_TYPE_NAME (actor));
    return clutter_binding_pool_activate(pool, clutter_event_get_key_symbol(event), clutter_event_get_state(event), G_OBJECT(actor));
}

static void setup_shortcuts(ClutterStage *stage)
{
    ClutterBindingPool *binding_pool = NULL;
    GObjectClass *stage_class = NULL;

    stage_class = (GObjectClass*) CLUTTER_STAGE_GET_CLASS(stage);
    binding_pool = clutter_binding_pool_get_for_class(stage_class);
    clutter_binding_pool_install_action(binding_pool, "escape-to-quit", CLUTTER_Escape, 0 /* no modifiers */, G_CALLBACK(clutter_main_quit), NULL, NULL);
    clutter_binding_pool_install_action(binding_pool, "control-q-to-quit", CLUTTER_KEY_q, CLUTTER_CONTROL_MASK, G_CALLBACK(clutter_main_quit), NULL, NULL);
    g_signal_connect(stage, "key-press-event", G_CALLBACK(on_key_pressed), NULL);
}

int main(int argc, char *argv[])
{
    clutter_init(&argc, &argv);
    ClutterActor *stage = NULL;
    stage = clutter_stage_get_default();
    ClutterColor stage_color = { 0x99, 0x99, 0x99, 0xff };
    int width = 640;
    int height = 480;
    clutter_actor_set_size(stage, width, height);
    clutter_stage_set_title(CLUTTER_STAGE(stage), "Shader Prototype");
    clutter_stage_set_color(CLUTTER_STAGE(stage), &stage_color);
    
    ClutterActor *image = load_custom_image(width, height);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage), image);
    setup_custom_shader(image);
    clutter_actor_show_all(stage);
    setup_shortcuts(CLUTTER_STAGE(stage));
    
    clutter_main();
    return 0;
}
