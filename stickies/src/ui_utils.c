#include "ui_utils.h"
#include <math.h>

static void draw_glyph(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                       gpointer data) {
  (void)area;
  (void)w;
  (void)h;
  const char *type = (const char *)data;

  cairo_set_source_rgba(cr, 0.18, 0.18, 0.18, 0.85);
  cairo_set_line_width(cr, 1.15);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);

  if (g_strcmp0(type, "close") == 0) {
    /* Classic Mac Stickies square close glyph - 1.4x scale */
    cairo_rectangle(cr, 1.8, 1.8, 8.4, 8.4);
    cairo_stroke(cr);
  } else if (g_strcmp0(type, "circle") == 0 || g_strcmp0(type, "color") == 0) {
    /* Simple circular glyph button matching the other buttons - 1.4x scale */
    cairo_arc(cr, 6.0, 6.0, 4.2, 0, 2 * M_PI);
    cairo_stroke(cr);
  } else if (g_strcmp0(type, "zoom") == 0) {
    /* Classic Mac Stickies diagonal zoom triangle - 1.4x scale */
    cairo_move_to(cr, 1.8, 10.2);
    cairo_line_to(cr, 10.2, 1.8);
    cairo_line_to(cr, 10.2, 10.2);
    cairo_close_path(cr);
    cairo_stroke(cr);
  } else if (g_strcmp0(type, "collapse") == 0) {
    /* Classic Mac Stickies collapse bar box - 1.4x scale */
    cairo_rectangle(cr, 1.8, 3.9, 8.4, 4.2);
    cairo_stroke(cr);
  }
}

GtkWidget *make_glyph_button(const char *type, const char *tooltip) {
  GtkWidget *btn = gtk_button_new();
  gtk_button_set_has_frame(GTK_BUTTON(btn), FALSE);
  gtk_widget_add_css_class(btn, "flat");
  gtk_widget_add_css_class(btn, "glyph-btn");
  gtk_widget_set_valign(btn, GTK_ALIGN_CENTER);
  gtk_widget_set_tooltip_text(btn, tooltip);

  GtkWidget *area = gtk_drawing_area_new();
  gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area), 12);
  gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area), 12);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_glyph,
                                 (gpointer)type, NULL);
  gtk_button_set_child(GTK_BUTTON(btn), area);
  return btn;
}
