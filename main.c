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
        clutter_actor_set_shader_param(actor, "radius", &value);
        g_value_unset(&value);
    }
}

int main(int argc, char *argv[])
{
    clutter_init(&argc, &argv);
    ClutterActor *stage = NULL;
    stage = clutter_stage_get_default();
    int width = 640;
    int height = 480;
    clutter_actor_set_size(stage, width, height);
    
    print_file_contents(PKGDATADIR, "dummy.frag");
        
    ClutterActor *image = load_image(width, height);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage), image);
    setup_shader(image);
    clutter_actor_show_all(stage);
    clutter_main();
    return 0;
}
