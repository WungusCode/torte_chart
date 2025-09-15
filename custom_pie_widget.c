#include "custom_pie_widget.h"
#include <math.h>
#include <stdlib.h>

// Based on Zetcode's custom widget ( which doesn't compile under GTK+3 :(

int dbg = 0;

struct chart_slice_t
{
 double   value;  // percentage of circle this slice to be
 GdkRGBA  color;  // color of the slice
 gchar   *label;  // what label to apply to slice
};

/* Private data structure */
struct _PieWidgetPrivate
{
  gdouble    value;
  gdouble    maxValue;
  gdouble    minValue;
  GdkWindow *window;

  gchar      *page_title_text;   // rename to chart_title_text later !

  GSList     *slice_list;
};

/* Internal API */
static void     pie_widget_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);
static void     pie_widget_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);
static void     pie_widget_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static void     pie_widget_realize(GtkWidget *widget);
static gboolean pie_widget_draw(GtkWidget *widget, cairo_t *cr);
static void     pie_widget_get_preferred_height(GtkWidget *widget, gint *minimum_height, gint *natural_height);
static void     pie_widget_get_preferred_width(GtkWidget *widget, gint *minimum_width, gint *natural_width);

/* Define type */
G_DEFINE_TYPE_WITH_PRIVATE(PieWidget, pie_widget, GTK_TYPE_WIDGET);

/* Initialization */
static void pie_widget_class_init(PieWidgetClass *klass)
{
    GObjectClass   *g_class;
    GtkWidgetClass *w_class;
    GParamSpec     *pspec;

    GParamSpec *colorstyle;

    g_class = G_OBJECT_CLASS(klass);
    /* Override widget class methods */
    g_class->set_property         = pie_widget_set_property;
    g_class->get_property         = pie_widget_get_property;

    w_class                       = GTK_WIDGET_CLASS(klass);
    /* Override widget class methods */
    w_class->realize              = pie_widget_realize;
    w_class->get_preferred_height = pie_widget_get_preferred_height;
    w_class->get_preferred_width  = pie_widget_get_preferred_width;

    // w_class->size_request=pie_widget_size_request;
    w_class->size_allocate        = pie_widget_size_allocate;
    w_class->draw                 = pie_widget_draw;

    /* Install Property */
    pspec = g_param_spec_double("pie-value", "Value", "Value will show", -G_MINDOUBLE, G_MAXDOUBLE, 0.0, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_class, P_VALUE, pspec);
    pspec = g_param_spec_double("pie-max-value", "MaxValue", "Max value will hold", -G_MINDOUBLE, G_MAXDOUBLE, 100.0, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_class, P_MAX_VALUE, pspec);

    pspec = g_param_spec_double("pie-min-value", "MinValue", "Min value will hold", -G_MINDOUBLE, G_MAXDOUBLE, 0.0, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
    g_object_class_install_property(g_class, P_MIN_VALUE, pspec);

    colorstyle = g_param_spec_boxed("value-color", "Value Color", "The color of value gauge", GDK_TYPE_RGBA, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
    gtk_widget_class_install_style_property(w_class, colorstyle);

    colorstyle = g_param_spec_boxed("base-fill-color", "Base Fill Color", "Fill color of base", GDK_TYPE_RGBA, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
    gtk_widget_class_install_style_property(w_class, colorstyle);

    colorstyle = g_param_spec_boxed("base-stroke-color", "Base Stroke Color", "Stroke color of base", GDK_TYPE_RGBA, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
    gtk_widget_class_install_style_property(w_class, colorstyle);

    colorstyle = g_param_spec_boxed("value-text-color", "Value text Color", "Text color of value", GDK_TYPE_RGBA, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);
    gtk_widget_class_install_style_property(w_class, colorstyle);
    gtk_widget_class_set_css_name(w_class, "pie-gauge");

    pspec =  g_param_spec_string ("text-title-main", "Graph Top Title", "Title at top of graph on the X axis",
                                                   "<big><b>Top Title</b></big>",  G_PARAM_WRITABLE);
    g_object_class_install_property ( g_class,  PROP_GRAPH_TITLE, pspec );

}

static void pie_widget_init(PieWidget *widget)
{
    PieWidgetPrivate *priv;


    // priv = G_TYPE_INSTANCE_GET_PRIVATE(widget, PIE_TYPE_WIDGET, PieWidgetPrivate);//deprecated
    priv = pie_widget_get_instance_private(widget);
    gtk_widget_set_has_window(GTK_WIDGET(widget), TRUE);

    //set default value
    priv->value = 0;
    priv->maxValue = 100;
    priv->minValue = 0;

    priv->page_title_text = g_strdup ( "MY TITLE" );

    priv->slice_list = NULL;

    //create cache for faster access
    widget->priv = priv;
}

/* Override Methods */
static void pie_widget_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    PieWidget *widget = PIE_WIDGET(object);
    switch (prop_id)
    {
    case P_VALUE:
        pie_widget_set_value(widget, g_value_get_double(value));
        break;
    case P_MAX_VALUE:
        pie_widget_set_max_value(widget, g_value_get_double(value));
        break;
    case P_MIN_VALUE:
        pie_widget_set_min_value(widget, g_value_get_double(value));
        break;
    case PROP_GRAPH_TITLE:
      pie_widget_set_text ( widget, PIE_TITLE_T, g_value_get_string(value) );
      break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}
static void pie_widget_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    PieWidget *widget = PIE_WIDGET(object);
    switch (prop_id)
    {
    case P_VALUE:
        g_value_set_double(value, widget->priv->value);
        break;
    case P_MAX_VALUE:
        g_value_set_double(value, widget->priv->maxValue);
        break;
    case P_MIN_VALUE:
        g_value_set_double(value, widget->priv->minValue);
        break;
    case PROP_GRAPH_TITLE:
        g_debug( " Can't get title !" );
      break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}
static void pie_widget_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
    PieWidgetPrivate *priv = PIE_WIDGET(widget)->priv;
    gtk_widget_set_allocation(widget, allocation);
    if (gtk_widget_get_realized(widget))
    {
        gdk_window_move_resize(priv->window, allocation->x, allocation->y, allocation->width, allocation->height);
    }
}
static void pie_widget_realize(GtkWidget *widget)
{
    PieWidgetPrivate *priv = PIE_WIDGET(widget)->priv;
    GtkAllocation alloc;
    GdkWindowAttr attrs;
    gint attrs_mask;

    gtk_widget_set_realized(widget, TRUE);
    gtk_widget_get_allocation(widget, &alloc);
    attrs.x           = alloc.x;
    attrs.y           = alloc.y;
    attrs.width       = alloc.width;
    attrs.height      = alloc.height;
    attrs.window_type = GDK_WINDOW_CHILD;
    attrs.wclass      = GDK_INPUT_OUTPUT;
    attrs.event_mask  = gtk_widget_get_events(widget) | GDK_EXPOSURE_MASK;

    attrs_mask = GDK_WA_X | GDK_WA_Y;

    priv->window = gdk_window_new(gtk_widget_get_parent_window(widget), &attrs, attrs_mask);
    gdk_window_set_user_data(priv->window, widget);
    gtk_widget_set_window(widget, priv->window);
    //style
}

static gboolean pie_widget_draw(GtkWidget *widget, cairo_t *cr) {

  GdkRGBA *val_color = NULL;
  GValue    styleVal = G_VALUE_INIT;

  GtkAllocation         alloc;
  cairo_text_extents_t  extent;
  PieWidgetPrivate     *self;

  PieWidgetPrivate *priv = PIE_WIDGET(widget)->priv;
  PieWidget       *graph = (PieWidget *)widget;
  GSList          *l;

  int ii;
  float w,h;
  double cx,cy;
  double radius;

  double start_angle = 0.0;
  double total       = 0.0;

  self = graph->priv;

  gtk_widget_get_allocation(widget, &alloc);

  guint size=(alloc.width<=alloc.height)*alloc.width+(alloc.width>alloc.height)*alloc.height;

  cairo_set_antialias (cr, CAIRO_ANTIALIAS_FAST);

  w = alloc.width;
  h = alloc.height;

  printf( "  Dimensions : w = %4d h=%4d \n" , (int)w, (int)h );

  // Center of chart
  cx = w  / 2.0;
  cy = h / 2.0;

  radius = MIN( w, h ) / 2.5; // margin

  for(l = self->slice_list; l != NULL; l = l->next) {
    struct chart_slice_t *slice = l->data;
    total += slice->value;
  }

  if(total <= 0.0)  {
    cairo_destroy(cr);
    return FALSE;
  }

  ii =0;
  for (l = self->slice_list; l != NULL; l = l->next) {
    struct chart_slice_t *slice = l->data;

    // Angle of the slice proportional to its value
    double slice_angle = (slice->value / total) * 2.0 * G_PI;

    cairo_set_source_rgba(cr, slice->color.red, slice->color.green, slice->color.blue, slice->color.alpha);

    cairo_move_to(cr, w / 2, h / 2);
    cairo_arc(cr, w / 2, h / 2, radius, start_angle, start_angle + slice_angle);
    cairo_close_path(cr);
    cairo_fill(cr);

    if(slice->label != NULL) {
      double middle   = start_angle + slice_angle / 2.0;
      double distance = radius + 10; // Position outside of the slide radius

      double lx = cx + cos(middle) * distance; // X = cx + cos(0) * radius
      double ly = cy + sin(middle) * distance; // Y = cy + cos(0) * radius

      cairo_set_source_rgba(cr, slice->color.red, slice->color.green, slice->color.blue, slice->color.alpha);

      cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

      cairo_set_font_size(cr, 12);
      cairo_text_extents(cr, slice->label, &extent);

      // Adjust x position if angle is between 90º (PI/2) and 270º (3PI/2)
      if(middle > G_PI / 2 && middle < 3 * G_PI / 2) {
        lx -= extent.width;
      }
      printf( "  %2d label at x=%4.1f y=%4.1f   %s \n" , ii, lx , ly , slice->label );
      cairo_move_to(cr, lx, ly);
      cairo_show_text(cr, slice->label);
    } // if slice-label

    start_angle += slice_angle;
    ii++;
  } // for
  printf( "\n" );

  // gdk_cairo_set_source_rgba (cr, &self->text_color);
  g_value_init(&styleVal, GDK_TYPE_RGBA);
  gtk_widget_style_get_property(widget, "base-fill-color", &styleVal);
  if (G_VALUE_HOLDS(&styleVal, GDK_TYPE_RGBA)) {
    val_color = g_value_get_boxed(&styleVal);
  }

  if (val_color != NULL) {
    gdk_cairo_set_source_rgba(cr, val_color);
  }
  else {
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
  }

  cairo_select_font_face (cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

  if ( priv->page_title_text != NULL ) {
    // cairo_text_extents_t extent;
    char *val;
    int lngth = strlen( priv->page_title_text );

    if ( dbg ) printf( "  Title len = %3d ... " , lngth );
    val = (char *)malloc(sizeof(char) * lngth+1 );
    strcpy( val , priv->page_title_text );

    cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 9 * size / 75);

    cairo_move_to(cr, 40 , 40 );
    cairo_text_path(cr, val);
    free(val);
    if ( dbg ) printf( " done draw title \n" );
  }

  //draw text value
  gtk_widget_style_get_property(widget, "value-text-color", &styleVal);
  if (G_VALUE_HOLDS(&styleVal, GDK_TYPE_RGBA)) {
      val_color = g_value_get_boxed(&styleVal);
  }

  if (val_color != NULL) {
    gdk_cairo_set_source_rgba(cr, val_color);
  }
  else {
    cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 1);
  }
  cairo_fill(cr);

  return FALSE;
} // pie_widget_draw

static void pie_widget_get_preferred_height(GtkWidget *widget, gint *minimum_height, gint *natural_height)
{
    (void)widget;
    *minimum_height = 120;
    *natural_height = 400;
}
static void pie_widget_get_preferred_width(GtkWidget *widget, gint *minimum_width, gint *natural_width)
{
    (void)widget;
    *minimum_width = 120;
    *natural_width = 400;
}

/* Public API */
GtkWidget *pie_widget_new(void)
{
    return (g_object_new(PIE_TYPE_WIDGET, NULL));
}

gdouble pie_widget_get_value(PieWidget *widget)
{
    g_return_val_if_fail(PIE_IS_WIDGET(widget), 0);
    return (widget->priv->value);
}

void pie_widget_set_value(PieWidget *widget, gdouble value)
{
    g_return_if_fail(PIE_IS_WIDGET(widget));
    if (value < widget->priv->minValue)
    {
        value = widget->priv->minValue;
    }
    else if (value > widget->priv->maxValue)
    {
        value = widget->priv->maxValue;
    }
    widget->priv->value = value;
    gtk_widget_queue_draw(GTK_WIDGET(widget));
}

void pie_widget_set_max_value(PieWidget *widget, gdouble maxValue)
{
    g_return_if_fail(PIE_IS_WIDGET(widget));
    widget->priv->maxValue = maxValue;
    gtk_widget_queue_draw(GTK_WIDGET(widget));
}
gdouble pie_widget_get_max_value(PieWidget *widget)
{
    g_return_val_if_fail(PIE_IS_WIDGET(widget), 0);
    return (widget->priv->maxValue);
}

void pie_widget_set_min_value(PieWidget *widget, gdouble minValue)
{
    g_return_if_fail(PIE_IS_WIDGET(widget));
    widget->priv->minValue = minValue;
    gtk_widget_queue_draw(GTK_WIDGET(widget));
}

gdouble pie_widget_get_min_value(PieWidget *widget)
{
    g_return_val_if_fail(PIE_IS_WIDGET(widget), 0);
    return (widget->priv->minValue);
}

gboolean pie_widget_set_text (PieWidget *graph, PIE_ElementID element, const gchar *pch_text)
{
  PieWidgetPrivate *priv;
  gchar      *pch = NULL;
  gboolean   rc = TRUE;

  g_debug ("    ===> pie_widget_set_text(entered)");
  printf( " >> E %s \n" , __FUNCTION__ );

  if ( PIE_IS_WIDGET ( graph ) ) {
    printf( "     passed in widget %p IS GAUGE !\n" , graph );
  }
  else  printf( "     passed in widget %p IS NOT GAUGE !\n", graph );

  g_return_val_if_fail ( PIE_IS_WIDGET ( graph ), FALSE);
  g_return_val_if_fail (pch_text != NULL, FALSE);

  priv = graph->priv;

  pch = g_strdup (pch_text);

  printf( " pch = %s !\n" , pch );

  switch ( element ) {
   case PIE_TITLE_T :
     if (priv->page_title_text != NULL) {
       g_free (priv->page_title_text);
     }
     priv->page_title_text = pch;
     break;
   default:
      g_message ("pie_widget_set_text(): Invalid Element ID = %2d " , element );
      g_free ( pch );
      rc = FALSE;
    break;
  } // switch element

  //g_debug ("    ===> pie_widget_set_text(exited)");
  printf( " << Lv  %s  , rc = %d\n" , __FUNCTION__ , rc );
  return rc;
}

gboolean pie_widget_add_slice_to_pie ( PieWidget *graph  , double value, const char *color, const char *label) {

  PieWidgetPrivate *priv;
  gboolean   rc = TRUE;

  g_debug ("    ===> pie_widget_add_slice_to_pie(entered)");
  if ( dbg ) printf( " >> E %s \n" , __FUNCTION__ );

  if ( PIE_IS_WIDGET ( graph ) ) {
    if ( dbg ) printf( "     passed in widget %p IS GAUGE !\n" , graph );
  }
  else {
    if ( dbg ) printf( "     passed in widget %p IS NOT GAUGE !\n", graph );
  }
  g_return_val_if_fail ( PIE_IS_WIDGET ( graph ), FALSE);

  priv = graph->priv;

  // Allocate memory for new slice
  struct chart_slice_t *slice = g_new0(struct chart_slice_t, 1);
  slice->value = value;

  gdk_rgba_parse(&slice->color, color);
  if(label != NULL) slice->label = g_strdup(label);

  // Add slice to list to be drawn
  priv->slice_list = g_slist_append( priv->slice_list, slice);

  // Queue draw of widget
  if (GTK_IS_WIDGET( graph )) {
    gtk_widget_queue_draw( GTK_WIDGET( graph ) );
  }
  if ( dbg ) printf( " << LV  %s \n" , __FUNCTION__ );

  return rc;
} // pie_widget_add_slice_to_pie

