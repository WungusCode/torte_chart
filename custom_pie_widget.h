#ifndef __CUSTOM_PIE_WIDGET_H__
#define __CUSTOME_PIE_WIDGET_H__

#include <gtk/gtk.h>

#include <cairo.h> //for drawing

G_BEGIN_DECLS

#define PIE_WIDGET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), PIE_WIDGET, _PieWidgetPrivate ) )

typedef enum _PIE_Graph_Elements {
  /* enable chart flags and title keys */
  PIE_ELEMENT_NONE = 1 << 0,
  PIE_TITLE_X      = 1 << 1,
  PIE_NO_TITLE_X   = 0 << 1,
  PIE_TITLE_Y      = 1 << 2,
  PIE_NO_TITLE_Y   = 0 << 2,
  PIE_TITLE_T      = 1 << 3,
  PIE_NO_TITLE_T   = 0 << 3,

  /* enable chart attributes flag */
  PIE_GRID_LABELS_X    = 1 << 4,
  PIE_NO_GRID_LABELS_X = 0 << 4,
  PIE_GRID_LABELS_Y    = 1 << 5,
  PIE_NO_GRID_LABELS_Y = 0 << 5,

  /* enable tooltip flag */
  PIE_TOOLTIP    = 1 << 6,
  PIE_NO_TOOLTIP = 0 << 6,

  /* enabled chart attributes */
  PIE_GRID_LINES      = 1 << 7,
  PIE_NO_GRID_LINES   = 0 << 7,
  PIE_GRID_MINOR_X    = 1 << 8,
  PIE_NO_GRID_MINOR_X = 0 << 8,
  PIE_GRID_MAJOR_X    = 1 << 9,
  PIE_NO_GRID_MAJOR_X = 0 << 9,
  PIE_GRID_MINOR_Y    = 1 << 10,
  PIE_NO_GRID_MINOR_Y = 0 << 10,
  PIE_GRID_MAJOR_Y    = 1 << 11,
  PIE_NO_GRID_MAJOR_Y = 0 << 11,

  /* chart color key -- used to change window color only */
  PIE_SCALE   = 1 << 12,
  PIE_TITLE   = 1 << 13,
  PIE_WINDOW  = 1 << 14,
  PIE_CHART   = 1 << 15,

  /* Reserved */
  PIE_RESERVED_OFF = 0 << 16,
  PIE_RESERVED_ON  = 1 << 16
} PIE_ElementID;

/* Properties enum*/
typedef enum _PIE_PROPERTY_ID {
  P_0,     // for padding
  P_VALUE, // for value
  P_MAX_VALUE,
  P_MIN_VALUE,

  // ADDED
  P_CHART_TITLE,
  // from gline
  PROP_GRAPH_DRAWING_TYPE,
  PROP_GRAPH_TITLE,
  PROP_GRAPH_TITLE_X,
  PROP_GRAPH_TITLE_Y,
  PROP_GRAPH_LINE_WIDTH,
  PROP_GRAPH_ELEMENTS,
  PROP_GRAPH_TITLE_COLOR,
  PROP_GRAPH_SCALE_COLOR,
  PROP_GRAPH_CHART_COLOR,
  PROP_GRAPH_WINDOW_COLOR,
  PROP_TICK_MINOR_X,
  PROP_TICK_MAJOR_X,
  PROP_SCALE_MINOR_X,
  PROP_SCALE_MAJOR_X,
  PROP_TICK_MINOR_Y,
  PROP_TICK_MAJOR_Y,
  PROP_SCALE_MINOR_Y,
  PROP_SCALE_MAJOR_Y

} PIE_PROPERTY_ID;

/* Standart GObject macros */
#define PIE_TYPE_WIDGET ( pie_widget_get_type())
#define PIE_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), PIE_TYPE_WIDGET, PieWidget))
#define PIE_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PIE_TYPE_WIDGET, PieWidgetClass))
#define PIE_IS_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), PIE_TYPE_WIDGET))
#define PIE_IS_WIDGET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), PIE_TYPE_WIDGET))
#define PIE_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), PIE_TYPE_WIDGET, PieWidgetClass))

/* Type definition */
typedef struct _PieWidget PieWidget;
typedef struct _PieWidgetClass PieWidgetClass;
typedef struct _PieWidgetPrivate PieWidgetPrivate;

struct _PieWidget
{
    GtkWidget parent;
    /*< Private >*/
    PieWidgetPrivate *priv;
};

struct _PieWidgetClass
{
    GtkWidgetClass parent_class;
};

/* public API */
GType pie_widget_get_type(void) G_GNUC_CONST;
GtkWidget *pie_widget_new(void);

//set value widget
void pie_widget_set_value(PieWidget *widget, gdouble value);
//get value widget
gdouble pie_widget_get_value(PieWidget *widget);

//set max value
void pie_widget_set_max_value(PieWidget *widget, gdouble maxValue);
//get max value
gdouble pie_widget_get_max_value(PieWidget *widget);

//set min value
void pie_widget_set_min_value(PieWidget *widget, gdouble minValue);
//get min value
gdouble pie_widget_get_min_value(PieWidget *widget);

extern gboolean pie_widget_set_text (PieWidget *graph, PIE_ElementID element, const gchar *pch_text);
extern gboolean pie_widget_add_slice_to_pie ( PieWidget *graph  , double value, const char *color, const char *label);

G_END_DECLS

#endif /* __CUSTOME_PIE_WIDGET_H_ */
