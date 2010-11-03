#include <stdio.h>
#include <glib.h>
#include <glib/gprintf.h>

int main (int argc, char **argv) {
    GList *tmp, *list = NULL;
    gint i, *element;
    
    /* Appends 4 integers into list */
    for (i = 1; i < 5; i++) 
    {
        element = (gint *) g_malloc(sizeof(gint));
        g_printf("Enter Number %d: ", i);
        scanf("%d", element);
        list = g_list_append(list, element);
    }
    
    /* Prints data in list */
    tmp = list;
    while (tmp) 
    {
        element = (gint *) tmp->data;
        g_printf("%d\n", *element);
        tmp = g_list_next(tmp);
    }
    
    /* Frees the data in list */
    tmp = list;
    while (tmp)
    {
        g_free(tmp->data);
        tmp = g_list_next(tmp);
    }
    
    /* Frees the list structures */
    g_list_free(list);
    return 0;
}
