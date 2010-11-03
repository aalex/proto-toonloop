#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


class MidiBinder
{
    private:
    public:
        void do_some_stuff();
        gchar *current_animal_noise;
};

/*
 * Called for open tags <foo bar="baz"> 
 */
static void start_element(
        GMarkupParseContext *context,
        const gchar *element_name,
        const gchar **attribute_names,
        const gchar **attribute_values,
        gpointer user_data,
        GError **error) 
{
    MidiBinder *self = static_cast<MidiBinder *>(user_data);
    const gchar **name_cursor = attribute_names;
    const gchar **value_cursor = attribute_values;
    while (*name_cursor) 
    {
        if (g_strcmp0(*name_cursor, "noise") == 0)
            self->current_animal_noise = g_strdup(*value_cursor);
        name_cursor++;
        value_cursor++;
    }
}

/**
 * Called when some text inside a tag is encountered.
 */
static void text(
        GMarkupParseContext *context,
        const gchar         *text,
        gsize                text_len,
        gpointer             user_data,
        GError             **error)
{
    MidiBinder *self = static_cast<MidiBinder *>(user_data);
    // Note that "text" is not a regular C string: it is
    // not null-terminated. This is the reason for the
    // unusual %*s format below.
    if (self->current_animal_noise)
        printf("I am a %*s and I go %s. Can you do it?\n", text_len, text, self->current_animal_noise);
}

/**
 * Called for close tags </foo> 
 */
static void end_element(
        GMarkupParseContext *context,
        const gchar *element_name,
        gpointer user_data,
        GError **error)
{
    MidiBinder *self = static_cast<MidiBinder *>(user_data);
    if (self->current_animal_noise)
    {
        g_free(self->current_animal_noise);
        self->current_animal_noise = NULL;
    }
}


  /* Called on error, including one set by other
   * methods in the vtable. The GError should not be freed.
   */
static void on_error(GMarkupParseContext *context, GError *error, gpointer user_data)
{
    //MidiBinder *self = static_cast<MidiBinder *>(user_data);
    g_print("Error parsing XML markup: %s\n", error->message);
}

/* Code to grab the file into memory and parse it. */

void MidiBinder::do_some_stuff()
{
    /* The list of what handler does what. */
    GMarkupParser parser = {
        start_element,
        end_element,
        text,
        NULL,
        on_error
    };
    gpointer user_data = (gpointer) this;
    current_animal_noise = NULL;
    GMarkupParseContext *context = g_markup_parse_context_new(&parser, G_MARKUP_TREAT_CDATA_AS_TEXT, user_data, NULL);
    /* seriously crummy error checking */
    char *xml_text;
    gsize length;
    if (g_file_get_contents("simple.xml", &xml_text, &length, NULL) == FALSE) 
    {
        printf("Couldn't load XML\n");
        exit(255);
    }
    if (g_markup_parse_context_parse(context, xml_text, length, NULL) == FALSE) 
    {
        printf("Parse failed\n");
        exit(255);
    }
    g_free(xml_text);
    g_markup_parse_context_free(context);
}

int main() 
{
    MidiBinder binder = MidiBinder();
    binder.do_some_stuff();
    return 0;
}

