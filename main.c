#include <clutter/clutter.h>

#define PKGDATADIR "./data/"
#define IMGFILE "example.jpg" // works with svg too

static gchar *frag_source = "uniform float radius ;"
        "uniform sampler2DRect rectTexture;"
        ""
        "void main()"
        "{"
        "    vec4 color = texture2DRect(rectTexture, gl_TexCoord[0].st);"
        "    float u;"
        "    float v;"
        "    int count = 1;"
        "    for (u=-radius;u<radius;u++)"
        "      for (v=-radius;v<radius;v++)"
        "        {"
        "          color += texture2DRect(rectTexture, vec2(gl_TexCoord[0].s + u * 2, gl_TexCoord[0].t +v * 2));"
        "          count ++;"
        "        }"
        ""
        "    gl_FragColor = color / count;"
        "}" ;

gboolean print_file_contents(gchar *dir_name, gchar *file_name)
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
    g_print("Finished reading %s\n", full_path);
    g_print("Contents: %s\n", contents);
    g_free(contents);
    g_free(full_path);
    return TRUE;
}

static ClutterActor *load_image(int width, int height)
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

static void setup_shader(ClutterActor *actor)
{
    ClutterShader *shader = NULL;
    shader = clutter_shader_new();
    clutter_shader_set_fragment_source(shader, frag_source, -1);
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
        GValue value = { 0, };
        g_value_init(&value, G_TYPE_FLOAT);
        g_value_set_float(&value, 5.0);
        //clutter_actor_set_shader_param(actor, "radius", &value);
        clutter_actor_set_shader_param_float(actor, "radius", 5.0);
        clutter_actor_set_shader_param_int(actor, "rectTexture", 0);
        g_value_unset(&value);
    }
}
static gboolean on_key_pressed(ClutterActor *actor, ClutterEvent *event, gpointer data)
{
    if (event) /* There is no event for the first triggering */
    {
        switch (clutter_event_get_key_symbol(event))
        {
            case CLUTTER_Escape:
                clutter_main_quit();
                break;
            default:
                break;
        }
    }
    return FALSE;
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
    
    //print_file_contents(PKGDATADIR, "dummy.frag");
        
    ClutterActor *image = load_image(width, height);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage), image);
    setup_shader(image);
    clutter_actor_show_all(stage);
    g_signal_connect(stage, "key-press-event", G_CALLBACK(on_key_pressed), NULL);
    g_print("Press Escape to quit.\n");

    clutter_main();
    return 0;
}
