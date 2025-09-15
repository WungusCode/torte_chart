#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>

#include "custom_pie_widget.h"

#define WIDTH   720
#define HEIGHT  480


int
main (int argc, char **argv)
{
    GtkWidget *window;

    GtkWidget *piMy, *box ,*boxh;

    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), WIDTH, HEIGHT);
    gtk_window_set_title (GTK_WINDOW (window), "Graph test");

    //g_signal_connect (window, "delete_event", gtk_main_quit, NULL);
    g_signal_connect (G_OBJECT (window), "destroy", gtk_main_quit, NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_container_add(GTK_CONTAINER(window), box);

    boxh = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);

    gtk_box_pack_start(GTK_BOX(box), boxh, TRUE, TRUE, 0);
    piMy = pie_widget_new();


    pie_widget_set_text ( (PieWidget *)piMy , PIE_TITLE_T , "NEW Title" );

    pie_widget_add_slice_to_pie ( (PieWidget *) piMy , 50, "#F00000", "Physics");
    pie_widget_add_slice_to_pie ( (PieWidget *) piMy , 10, "#FF6484", "Math");
    pie_widget_add_slice_to_pie ( (PieWidget *) piMy , 20, "#FFC686", "History");
    pie_widget_add_slice_to_pie ( (PieWidget *) piMy , 15, "#36A282", "Biology");
    pie_widget_add_slice_to_pie ( (PieWidget *) piMy ,  5, "#1010A0", "French");

    gtk_box_pack_start( GTK_BOX( boxh ), piMy, FALSE, TRUE, 0);

    gtk_widget_show_all (window);
    gtk_main ();

    return 0;
}

