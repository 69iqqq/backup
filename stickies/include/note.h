#ifndef NOTE_H
#define NOTE_H

#include <gtk/gtk.h>

#define DEFAULT_FONT_SIZE 13

typedef struct _StickyNote {
  GtkApplication *app;
  GtkWidget *window;
  GtkWidget *overlay;
  GtkWidget *main_box;
  GtkWidget *handle;
  GtkWidget *header_box;
  GtkWidget *scrolled;
  GtkWidget *textview;
  GtkWidget *title_label;
  GtkWidget *menu_btn;
  GtkWidget *menu_popover;
  GtkWidget *color_swatches[8];
  GtkWidget *color_checks[8];
  GtkTextBuffer *buffer;
  char *id;
  char *color;
  int width;
  int height;
  gboolean collapsed;
  gboolean zoomed;
  int saved_w;
  int saved_h;
  char *cached_text;
  char *cached_tags;
  gboolean is_dirty;
  gboolean header_hidden;
  guint typing_timeout_id;
  int font_size;
  GtkCssProvider *font_css_provider;
} StickyNote;

StickyNote *create_sticky_note(GtkApplication *app, const char *id,
                              const char *color, int w, int h,
                              gboolean collapsed, const char *text,
                              const char *tags_str, int font_size);
void free_sticky_note(StickyNote *n);
void sync_note_cache(StickyNote *n);
void apply_note_color(StickyNote *n, const char *color_name);
void note_set_font_size(StickyNote *n, int size);
void note_zoom_in(StickyNote *n);
void note_zoom_out(StickyNote *n);
void note_zoom_reset(StickyNote *n);
void toggle_collapse(StickyNote *n);
void toggle_zoom(StickyNote *n);
void update_collapse_ui(StickyNote *n);
void request_close_note(StickyNote *n);
void delete_sticky_note(StickyNote *n);

GList *note_get_all(void);

#endif /* NOTE_H */
