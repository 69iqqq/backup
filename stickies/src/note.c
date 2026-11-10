#include "note.h"
#include "color.h"
#include "ui_utils.h"
#include "storage.h"
#include "save_dialog.h"
#include <string.h>

static GList *notes_list = NULL;

GList *note_get_all(void) {
  return notes_list;
}

static void update_header_visibility(StickyNote *n) {
  if (!n->header_box) return;

  if (n->collapsed) {
    gtk_widget_remove_css_class(n->header_box, "header-hidden");
    return;
  }

  /* Keep header visible if the color/options popover is currently open */
  if (n->menu_popover && gtk_widget_get_visible(n->menu_popover)) {
    gtk_widget_remove_css_class(n->header_box, "header-hidden");
    return;
  }

  gboolean is_active = n->window && gtk_window_is_active(GTK_WINDOW(n->window));
  if (!is_active || n->header_hidden) {
    gtk_widget_add_css_class(n->header_box, "header-hidden");
  } else {
    gtk_widget_remove_css_class(n->header_box, "header-hidden");
  }
}

void apply_note_color(StickyNote *n, const char *color_name) {
  if (!n || !color_name) return;
  for (int i = 0; i < NUM_COLORS; i++) {
    char cls[32];
    g_snprintf(cls, sizeof(cls), "color-%s", COLORS[i].name);
    gtk_widget_remove_css_class(n->main_box, cls);
    gtk_widget_remove_css_class(n->window, cls);
  }
  char new_cls[32];
  g_snprintf(new_cls, sizeof(new_cls), "color-%s", color_name);
  gtk_widget_add_css_class(n->main_box, new_cls);
  gtk_widget_add_css_class(n->window, new_cls);
  char *new_col = g_strdup(color_name);
  g_free(n->color);
  n->color = new_col;
  n->is_dirty = TRUE;

  for (int i = 0; i < NUM_COLORS; i++) {
    gboolean active = (g_ascii_strcasecmp(color_name, COLORS[i].name) == 0);
    if (n->color_swatches[i] && GTK_IS_WIDGET(n->color_swatches[i])) {
      if (active) {
        gtk_widget_add_css_class(n->color_swatches[i], "active");
      } else {
        gtk_widget_remove_css_class(n->color_swatches[i], "active");
      }
    }
    if (n->color_checks[i] && GTK_IS_LABEL(n->color_checks[i])) {
      gtk_label_set_text(GTK_LABEL(n->color_checks[i]), active ? "✓" : "");
    }
  }
}

void update_collapse_ui(StickyNote *n) {
  if (n->collapsed) {
    int w = gtk_widget_get_width(n->window);
    int h = gtk_widget_get_height(n->window);
    if (w > 50 && h > 40) {
      n->width = w;
      n->height = h;
    }
    gtk_widget_set_visible(n->scrolled, FALSE);
    if (n->menu_btn) {
      gtk_widget_set_visible(n->menu_btn, FALSE);
    }
    /* While minimizing, the title should NOT show any note contents */
    gtk_label_set_text(GTK_LABEL(n->title_label), "");
    gtk_window_set_default_size(GTK_WINDOW(n->window), n->width, 16);
    update_header_visibility(n);
  } else {
    gtk_widget_set_visible(n->scrolled, TRUE);
    if (n->menu_btn) {
      gtk_widget_set_visible(n->menu_btn, TRUE);
    }
    gtk_label_set_text(GTK_LABEL(n->title_label), "");
    gtk_window_set_default_size(GTK_WINDOW(n->window), n->width, n->height);
    update_header_visibility(n);
  }
}

void toggle_collapse(StickyNote *n) {
  n->collapsed = !n->collapsed;
  update_collapse_ui(n);
  n->is_dirty = TRUE;
}

void toggle_zoom(StickyNote *n) {
  if (n->collapsed)
    toggle_collapse(n);
  if (!n->zoomed) {
    n->saved_w = n->width;
    n->saved_h = n->height;
    n->width = 580;
    n->height = 420;
    n->zoomed = TRUE;
  } else {
    n->width = n->saved_w;
    n->height = n->saved_h;
    n->zoomed = FALSE;
  }
  gtk_window_set_default_size(GTK_WINDOW(n->window), n->width, n->height);
  n->is_dirty = TRUE;
}

void note_set_font_size(StickyNote *n, int size) {
  if (!n) return;
  if (size < 8) size = 8;
  if (size > 54) size = 54;
  n->font_size = size;

  if (n->textview && GTK_IS_WIDGET(n->textview)) {
    if (!n->font_css_provider) {
      n->font_css_provider = gtk_css_provider_new();
      GdkDisplay *display = gtk_widget_get_display(n->textview);
      if (!display) display = gdk_display_get_default();
      gtk_style_context_add_provider_for_display(
          display,
          GTK_STYLE_PROVIDER(n->font_css_provider),
          GTK_STYLE_PROVIDER_PRIORITY_USER + 10);
    }
    char css[256];
    g_snprintf(css, sizeof(css),
               "#note-tv-%s, #note-tv-%s text { font-size: %dpx; }",
               n->id, n->id, size);
    gtk_css_provider_load_from_string(n->font_css_provider, css);
  }
  n->is_dirty = TRUE;
}

void note_zoom_in(StickyNote *n) {
  if (!n) return;
  int new_size;
  if (n->font_size < 13)
    new_size = n->font_size + 1;
  else if (n->font_size < 20)
    new_size = n->font_size + 2;
  else if (n->font_size < 32)
    new_size = n->font_size + 3;
  else if (n->font_size < 54)
    new_size = n->font_size + 4;
  else
    return;
  note_set_font_size(n, new_size);
}

void note_zoom_out(StickyNote *n) {
  if (!n) return;
  int new_size;
  if (n->font_size > 32)
    new_size = n->font_size - 4;
  else if (n->font_size > 20)
    new_size = n->font_size - 3;
  else if (n->font_size > 13)
    new_size = n->font_size - 2;
  else if (n->font_size > 8)
    new_size = n->font_size - 1;
  else
    return;
  note_set_font_size(n, new_size);
}

void note_zoom_reset(StickyNote *n) {
  if (!n) return;
  note_set_font_size(n, DEFAULT_FONT_SIZE);
}

void free_sticky_note(StickyNote *n) {
  if (n->typing_timeout_id > 0) {
    g_source_remove(n->typing_timeout_id);
    n->typing_timeout_id = 0;
  }
  if (n->font_css_provider) {
    gtk_style_context_remove_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(n->font_css_provider));
    g_object_unref(n->font_css_provider);
    n->font_css_provider = NULL;
  }
  notes_list = g_list_remove(notes_list, n);
  g_free(n->id);
  g_free(n->color);
  g_free(n->cached_text);
  g_free(n->cached_tags);
  g_free(n);
}

static void clear_note_widgets(StickyNote *n) {
  if (n->typing_timeout_id > 0) {
    g_source_remove(n->typing_timeout_id);
    n->typing_timeout_id = 0;
  }
  if (n->font_css_provider) {
    gtk_style_context_remove_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(n->font_css_provider));
    g_object_unref(n->font_css_provider);
    n->font_css_provider = NULL;
  }
  n->window = NULL;
  n->buffer = NULL;
  n->handle = NULL;
  n->header_box = NULL;
  n->menu_btn = NULL;
  n->menu_popover = NULL;
  memset(n->color_swatches, 0, sizeof(n->color_swatches));
  memset(n->color_checks, 0, sizeof(n->color_checks));
}

void delete_sticky_note(StickyNote *n) {
  GtkWidget *win = n->window;
  clear_note_widgets(n);
  free_sticky_note(n);
  save_all_notes_now();
  if (win && GTK_IS_WINDOW(win)) {
    gtk_window_destroy(GTK_WINDOW(win));
  }
}

static void on_save_dialog_choice(SaveDialogResponse response, gpointer user_data) {
  StickyNote *n = (StickyNote *)user_data;
  if (response == SAVE_DIALOG_RESPONSE_SAVE) {
    /* Save and close */
    sync_note_cache(n);
    n->is_dirty = FALSE;
    save_all_notes_now();
    GtkWidget *win = n->window;
    clear_note_widgets(n);
    free_sticky_note(n);
    if (win && GTK_IS_WINDOW(win)) {
      gtk_window_destroy(GTK_WINDOW(win));
    }
  } else if (response == SAVE_DIALOG_RESPONSE_DISCARD) {
    /* Discard note changes and close */
    GtkWidget *win = n->window;
    clear_note_widgets(n);
    free_sticky_note(n);
    save_all_notes_now();
    if (win && GTK_IS_WINDOW(win)) {
      gtk_window_destroy(GTK_WINDOW(win));
    }
  }
  /* If response == SAVE_DIALOG_RESPONSE_CANCEL: do nothing, stay open */
}

void request_close_note(StickyNote *n) {
  if (n->is_dirty) {
    if (n->collapsed) {
      toggle_collapse(n);
    }
    if (n->width < 390 || n->height < 260) {
      int w = n->width < 390 ? 390 : n->width;
      int h = n->height < 260 ? 260 : n->height;
      gtk_window_set_default_size(GTK_WINDOW(n->window), w, h);
    }
    show_save_changes_dialog(n, on_save_dialog_choice, n);
  } else {
    /* Already saved or empty: close cleanly */
    GtkWidget *win = n->window;
    clear_note_widgets(n);
    free_sticky_note(n);
    if (win && GTK_IS_WINDOW(win)) {
      gtk_window_destroy(GTK_WINDOW(win));
    }
  }
}

static gboolean on_window_close_request(GtkWindow *win, gpointer data) {
  (void)win;
  StickyNote *n = (StickyNote *)data;
  request_close_note(n);
  return TRUE; /* Intercept window destroy so confirmation can run */
}

static void on_header_dbl_click(GtkGestureClick *gesture, int n_press, double x,
                                double y, gpointer data) {
  (void)gesture;
  (void)x;
  (void)y;
  if (n_press == 2) {
    toggle_collapse((StickyNote *)data);
  }
}

static void on_resize_pressed(GtkGestureClick *gesture, int n_press, double x,
                              double y, gpointer data) {
  (void)n_press;
  StickyNote *n = (StickyNote *)data;
  GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(n->window));
  if (GDK_IS_TOPLEVEL(surface)) {
    GdkDevice *device = gtk_gesture_get_device(GTK_GESTURE(gesture));
    guint button =
        gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    guint32 timestamp = gtk_event_controller_get_current_event_time(
        GTK_EVENT_CONTROLLER(gesture));
    gdk_toplevel_begin_resize(GDK_TOPLEVEL(surface),
                              GDK_SURFACE_EDGE_SOUTH_EAST, device, button, x, y,
                              timestamp);
  }
}

static void toggle_tag_on_selection(GtkTextBuffer *buffer,
                                    const char *tag_name,
                                    StickyNote *n) {
  GtkTextIter start, end;
  if (!gtk_text_buffer_get_selection_bounds(buffer, &start, &end))
    return;
  GtkTextTag *tag = gtk_text_tag_table_lookup(
      gtk_text_buffer_get_tag_table(buffer), tag_name);
  if (!tag)
    return;

  if (gtk_text_iter_has_tag(&start, tag)) {
    gtk_text_buffer_remove_tag(buffer, tag, &start, &end);
  } else {
    gtk_text_buffer_apply_tag(buffer, tag, &start, &end);
  }
  if (n) {
    n->is_dirty = TRUE;
  }
}

static gboolean on_key_pressed(GtkEventControllerKey *controller, guint keyval,
                               guint keycode, GdkModifierType state,
                               gpointer data) {
  (void)controller;
  (void)keycode;
  StickyNote *n = (StickyNote *)data;
  gboolean ctrl = (state & GDK_CONTROL_MASK) != 0;
  gboolean alt = (state & GDK_ALT_MASK) != 0;

  if (ctrl && (keyval == GDK_KEY_n || keyval == GDK_KEY_N)) {
    /* Ctrl+N (or Ctrl+Shift+N): New Note Application Window */
    char *id = g_uuid_string_random();
    StickyNote *new_n = create_sticky_note(n->app, id, "yellow", 360, 240, FALSE, "", "", n->font_size);
    if (new_n) {
      new_n->is_dirty = FALSE;
    }
    g_free(id);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_m || keyval == GDK_KEY_M)) {
    /* Ctrl+M: Toggle Minimize / Maximize (Collapse) */
    toggle_collapse(n);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_s || keyval == GDK_KEY_S)) {
    /* Ctrl+S: Save note in JSON file */
    sync_note_cache(n);
    save_all_notes_now();
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_w || keyval == GDK_KEY_W)) {
    /* Ctrl+W: Close Note (with prompt if unsaved) */
    request_close_note(n);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_d || keyval == GDK_KEY_D)) {
    /* Ctrl+D: Delete Note */
    delete_sticky_note(n);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_q || keyval == GDK_KEY_Q)) {
    /* Ctrl+Q: Quit Application */
    g_application_quit(G_APPLICATION(n->app));
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_t || keyval == GDK_KEY_T)) {
    double op = gtk_widget_get_opacity(n->window);
    gtk_widget_set_opacity(n->window, (op > 0.9) ? 0.78 : 1.0);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_b || keyval == GDK_KEY_B)) {
    toggle_tag_on_selection(n->buffer, "bold", n);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_i || keyval == GDK_KEY_I)) {
    toggle_tag_on_selection(n->buffer, "italic", n);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_u || keyval == GDK_KEY_U)) {
    toggle_tag_on_selection(n->buffer, "underline", n);
    return TRUE;
  } else if ((ctrl && (keyval == GDK_KEY_k || keyval == GDK_KEY_K)) ||
             (alt && (keyval == GDK_KEY_s || keyval == GDK_KEY_S))) {
    /* Ctrl+K or Alt+S: Strikethrough */
    toggle_tag_on_selection(n->buffer, "strikethrough", n);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_equal || keyval == GDK_KEY_plus ||
                      keyval == GDK_KEY_KP_Add)) {
    /* Ctrl + (or Ctrl =): Zoom In (increase font size) */
    note_zoom_in(n);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_minus || keyval == GDK_KEY_underscore ||
                      keyval == GDK_KEY_KP_Subtract)) {
    /* Ctrl -: Zoom Out (decrease font size) */
    note_zoom_out(n);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_0 || keyval == GDK_KEY_KP_0)) {
    /* Ctrl 0: Reset Zoom (default font size) */
    note_zoom_reset(n);
    return TRUE;
  } else if (ctrl && (keyval == GDK_KEY_r || keyval == GDK_KEY_R)) {
    toggle_tag_on_selection(n->buffer, "red", n);
    return TRUE;
  } else if (alt && keyval == GDK_KEY_8) {
    gtk_text_buffer_insert_at_cursor(n->buffer, "• ", -1);
    n->is_dirty = TRUE;
    return TRUE;
  }
  return FALSE;
}

static void on_zoom_in_action(GSimpleAction *action, GVariant *param,
                              gpointer data) {
  (void)action;
  (void)param;
  StickyNote *n = (StickyNote *)data;
  note_zoom_in(n);
}

static void on_zoom_out_action(GSimpleAction *action, GVariant *param,
                               gpointer data) {
  (void)action;
  (void)param;
  StickyNote *n = (StickyNote *)data;
  note_zoom_out(n);
}

static void on_zoom_reset_action(GSimpleAction *action, GVariant *param,
                                 gpointer data) {
  (void)action;
  (void)param;
  StickyNote *n = (StickyNote *)data;
  note_zoom_reset(n);
}

static void on_color_action(GSimpleAction *action, GVariant *param,
                            gpointer data) {
  (void)param;
  StickyNote *n = (StickyNote *)data;
  const char *name = g_action_get_name(G_ACTION(action));
  if (g_str_has_prefix(name, "color_")) {
    apply_note_color(n, name + 6);
  }
}

static void on_new_note_action(GSimpleAction *action, GVariant *param,
                               gpointer data) {
  (void)action;
  (void)param;
  StickyNote *n = (StickyNote *)data;
  char *id = g_uuid_string_random();
  StickyNote *new_n = create_sticky_note(n->app, id, "yellow", 360, 240, FALSE, "", "", n->font_size);
  if (new_n) {
    new_n->is_dirty = FALSE;
  }
  g_free(id);
}

static void on_save_action(GSimpleAction *action, GVariant *param,
                           gpointer data) {
  (void)action;
  (void)param;
  StickyNote *n = (StickyNote *)data;
  sync_note_cache(n);
  save_all_notes_now();
}

static void on_toggle_min_action(GSimpleAction *action, GVariant *param,
                                gpointer data) {
  (void)action;
  (void)param;
  StickyNote *n = (StickyNote *)data;
  toggle_collapse(n);
}

static void on_delete_action(GSimpleAction *action, GVariant *param,
                             gpointer data) {
  (void)action;
  (void)param;
  StickyNote *n = (StickyNote *)data;
  delete_sticky_note(n);
}

static void on_format_action(GSimpleAction *action, GVariant *param,
                             gpointer data) {
  (void)param;
  StickyNote *n = (StickyNote *)data;
  const char *name = g_action_get_name(G_ACTION(action));
  if (g_str_has_prefix(name, "format_")) {
    toggle_tag_on_selection(n->buffer, name + 7, n);
  }
}

static gboolean on_typing_timeout(gpointer data) {
  StickyNote *n = (StickyNote *)data;
  n->typing_timeout_id = 0;
  n->header_hidden = FALSE;
  update_header_visibility(n);
  return G_SOURCE_REMOVE;
}

static void on_buffer_changed(GtkTextBuffer *buffer, gpointer data) {
  (void)buffer;
  StickyNote *n = (StickyNote *)data;
  n->is_dirty = TRUE;

  /* When typing, keep top bar hidden */
  if (n->typing_timeout_id > 0) {
    g_source_remove(n->typing_timeout_id);
  }
  n->header_hidden = TRUE;
  update_header_visibility(n);
  n->typing_timeout_id = g_timeout_add(1500, on_typing_timeout, n);
}

static void on_header_enter(GtkEventControllerMotion *controller, double x, double y, gpointer user_data) {
  (void)controller;
  (void)x;
  (void)y;
  StickyNote *n = (StickyNote *)user_data;
  n->header_hidden = FALSE;
  if (n->header_box) {
    gtk_widget_remove_css_class(n->header_box, "header-hidden");
  }
}

static void on_header_leave(GtkEventControllerMotion *controller, gpointer user_data) {
  (void)controller;
  StickyNote *n = (StickyNote *)user_data;
  update_header_visibility(n);
}

static void on_window_motion(GtkEventControllerMotion *controller, double x, double y, gpointer user_data) {
  (void)controller;
  (void)x;
  StickyNote *n = (StickyNote *)user_data;
  if (y <= 30.0) {
    n->header_hidden = FALSE;
    update_header_visibility(n);
  }
}

static void on_window_active_changed(GObject *object, GParamSpec *pspec, gpointer user_data) {
  (void)object;
  (void)pspec;
  StickyNote *n = (StickyNote *)user_data;
  update_header_visibility(n);
}

static void on_popover_visibility_changed(GObject *object, GParamSpec *pspec, gpointer user_data) {
  (void)object;
  (void)pspec;
  StickyNote *n = (StickyNote *)user_data;
  update_header_visibility(n);
}

void sync_note_cache(StickyNote *n) {
  if (n->window && GTK_IS_WIDGET(n->window) && !n->collapsed) {
    int w = gtk_widget_get_width(n->window);
    int h = gtk_widget_get_height(n->window);
    if (w > 50 && h > 40) {
      n->width = w;
      n->height = h;
    }
  }
  if (n->buffer && GTK_IS_TEXT_BUFFER(n->buffer)) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(n->buffer, &start, &end);
    g_free(n->cached_text);
    g_free(n->cached_tags);
    n->cached_text = gtk_text_buffer_get_text(n->buffer, &start, &end, TRUE);
    n->cached_tags = serialize_tags(n->buffer);
  }
}

static void on_swatch_clicked(GtkButton *btn, gpointer user_data) {
  StickyNote *n = (StickyNote *)user_data;
  const char *cname = (const char *)g_object_get_data(G_OBJECT(btn), "color_name");
  if (cname) {
    apply_note_color(n, cname);
  }
}

static void on_menu_btn_clicked(GtkButton *btn, gpointer user_data) {
  (void)btn;
  StickyNote *n = (StickyNote *)user_data;
  if (n->menu_popover && GTK_IS_POPOVER(n->menu_popover)) {
    if (gtk_widget_get_visible(n->menu_popover)) {
      gtk_popover_popdown(GTK_POPOVER(n->menu_popover));
    } else {
      gtk_popover_popup(GTK_POPOVER(n->menu_popover));
    }
  }
}

static void on_popover_new_clicked(GtkButton *btn, gpointer user_data) {
  (void)btn;
  StickyNote *n = (StickyNote *)user_data;
  if (n->menu_popover && GTK_IS_POPOVER(n->menu_popover)) {
    gtk_popover_popdown(GTK_POPOVER(n->menu_popover));
  }
  char *id = g_uuid_string_random();
  StickyNote *new_n = create_sticky_note(n->app, id, "yellow", 360, 240, FALSE, "", "", n->font_size);
  if (new_n) {
    new_n->is_dirty = FALSE;
  }
  g_free(id);
}

static void on_popover_save_clicked(GtkButton *btn, gpointer user_data) {
  (void)btn;
  StickyNote *n = (StickyNote *)user_data;
  if (n->menu_popover && GTK_IS_POPOVER(n->menu_popover)) {
    gtk_popover_popdown(GTK_POPOVER(n->menu_popover));
  }
  sync_note_cache(n);
  save_all_notes_now();
}

static void on_popover_min_clicked(GtkButton *btn, gpointer user_data) {
  (void)btn;
  StickyNote *n = (StickyNote *)user_data;
  if (n->menu_popover && GTK_IS_POPOVER(n->menu_popover)) {
    gtk_popover_popdown(GTK_POPOVER(n->menu_popover));
  }
  toggle_collapse(n);
}

static void on_popover_zoom_in_clicked(GtkButton *btn, gpointer user_data) {
  (void)btn;
  StickyNote *n = (StickyNote *)user_data;
  if (n->menu_popover && GTK_IS_POPOVER(n->menu_popover)) {
    gtk_popover_popdown(GTK_POPOVER(n->menu_popover));
  }
  note_zoom_in(n);
}

static void on_popover_zoom_out_clicked(GtkButton *btn, gpointer user_data) {
  (void)btn;
  StickyNote *n = (StickyNote *)user_data;
  if (n->menu_popover && GTK_IS_POPOVER(n->menu_popover)) {
    gtk_popover_popdown(GTK_POPOVER(n->menu_popover));
  }
  note_zoom_out(n);
}

static void on_popover_del_clicked(GtkButton *btn, gpointer user_data) {
  (void)btn;
  StickyNote *n = (StickyNote *)user_data;
  if (n->menu_popover && GTK_IS_POPOVER(n->menu_popover)) {
    gtk_popover_popdown(GTK_POPOVER(n->menu_popover));
  }
  delete_sticky_note(n);
}

static GtkWidget *make_popover_action_row(const char *label, const char *shortcut, gboolean is_destr) {
  GtkWidget *btn = gtk_button_new();
  gtk_button_set_has_frame(GTK_BUTTON(btn), FALSE);
  gtk_widget_add_css_class(btn, "flat");
  gtk_widget_add_css_class(btn, "popover-action-row");
  if (is_destr) {
    gtk_widget_add_css_class(btn, "destructive");
  }

  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  GtkWidget *lbl = gtk_label_new(label);
  gtk_label_set_xalign(GTK_LABEL(lbl), 0.0f);
  gtk_widget_set_hexpand(lbl, TRUE);
  gtk_box_append(GTK_BOX(row), lbl);

  if (shortcut) {
    GtkWidget *sc = gtk_label_new(shortcut);
    gtk_widget_add_css_class(sc, "shortcut-text");
    gtk_label_set_xalign(GTK_LABEL(sc), 1.0f);
    gtk_box_append(GTK_BOX(row), sc);
  }

  gtk_button_set_child(GTK_BUTTON(btn), row);
  return btn;
}

static void setup_note_menu_popover(StickyNote *n) {
  n->menu_popover = gtk_popover_new();
  gtk_widget_set_parent(n->menu_popover, n->menu_btn);
  gtk_popover_set_has_arrow(GTK_POPOVER(n->menu_popover), TRUE);
  gtk_widget_add_css_class(n->menu_popover, "sticky-menu-popover");

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);

  /* Color swatches row - Windows Sticky Notes style */
  GtkWidget *swatches_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  gtk_widget_add_css_class(swatches_box, "color-swatches-box");
  gtk_widget_set_halign(swatches_box, GTK_ALIGN_CENTER);

  for (int i = 0; i < NUM_COLORS; i++) {
    GtkWidget *swatch = gtk_button_new();
    gtk_button_set_has_frame(GTK_BUTTON(swatch), FALSE);
    gtk_widget_add_css_class(swatch, "flat");
    gtk_widget_add_css_class(swatch, "color-swatch");
    char cls[32];
    g_snprintf(cls, sizeof(cls), "color-swatch-%s", COLORS[i].name);
    gtk_widget_add_css_class(swatch, cls);

    char cap[32];
    g_snprintf(cap, sizeof(cap), "%c%s", g_ascii_toupper(COLORS[i].name[0]), COLORS[i].name + 1);
    gtk_widget_set_tooltip_text(swatch, cap);

    gboolean is_active = (g_ascii_strcasecmp(n->color ? n->color : "", COLORS[i].name) == 0);
    GtkWidget *check_lbl = gtk_label_new(is_active ? "✓" : "");
    gtk_widget_add_css_class(check_lbl, "swatch-check");
    gtk_button_set_child(GTK_BUTTON(swatch), check_lbl);

    if (is_active) {
      gtk_widget_add_css_class(swatch, "active");
    }

    n->color_swatches[i] = swatch;
    n->color_checks[i] = check_lbl;

    g_object_set_data(G_OBJECT(swatch), "color_name", (gpointer)COLORS[i].name);
    g_signal_connect(swatch, "clicked", G_CALLBACK(on_swatch_clicked), n);
    gtk_box_append(GTK_BOX(swatches_box), swatch);
  }
  gtk_box_append(GTK_BOX(vbox), swatches_box);

  /* Separator */
  GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_add_css_class(sep, "popover-separator");
  gtk_box_append(GTK_BOX(vbox), sep);

  /* Merged Action items */
  GtkWidget *new_item = make_popover_action_row("New Note", "Ctrl+N", FALSE);
  g_signal_connect(new_item, "clicked", G_CALLBACK(on_popover_new_clicked), n);
  gtk_box_append(GTK_BOX(vbox), new_item);

  GtkWidget *save_item = make_popover_action_row("Save Note", "Ctrl+S", FALSE);
  g_signal_connect(save_item, "clicked", G_CALLBACK(on_popover_save_clicked), n);
  gtk_box_append(GTK_BOX(vbox), save_item);

  GtkWidget *min_item = make_popover_action_row("Minimize / Maximize", "Ctrl+M", FALSE);
  g_signal_connect(min_item, "clicked", G_CALLBACK(on_popover_min_clicked), n);
  gtk_box_append(GTK_BOX(vbox), min_item);

  GtkWidget *zin_item = make_popover_action_row("Zoom In", "Ctrl++", FALSE);
  g_signal_connect(zin_item, "clicked", G_CALLBACK(on_popover_zoom_in_clicked), n);
  gtk_box_append(GTK_BOX(vbox), zin_item);

  GtkWidget *zout_item = make_popover_action_row("Zoom Out", "Ctrl+-", FALSE);
  g_signal_connect(zout_item, "clicked", G_CALLBACK(on_popover_zoom_out_clicked), n);
  gtk_box_append(GTK_BOX(vbox), zout_item);

  GtkWidget *del_item = make_popover_action_row("Delete Note", "Ctrl+D", TRUE);
  g_signal_connect(del_item, "clicked", G_CALLBACK(on_popover_del_clicked), n);
  gtk_box_append(GTK_BOX(vbox), del_item);

  gtk_popover_set_child(GTK_POPOVER(n->menu_popover), vbox);
  g_signal_connect(n->menu_popover, "notify::visible",
                   G_CALLBACK(on_popover_visibility_changed), n);
}

StickyNote *create_sticky_note(GtkApplication *app, const char *id,
                              const char *color, int w, int h,
                              gboolean collapsed, const char *text,
                              const char *tags_str, int font_size) {
  StickyNote *n = g_malloc0(sizeof(StickyNote));
  n->app = app;
  n->id = g_strdup(id);
  n->color = g_strdup(color ? color : "yellow");
  n->width = w > 50 ? w : 420;
  n->height = h > 40 ? h : 260;
  n->collapsed = collapsed;
  n->zoomed = FALSE;
  n->cached_text = g_strdup(text ? text : "");
  n->cached_tags = g_strdup(tags_str ? tags_str : "");
  n->font_size = (font_size > 0) ? font_size : DEFAULT_FONT_SIZE;
  n->font_css_provider = NULL;
  n->is_dirty = FALSE;
  n->header_hidden = FALSE;

  n->window = gtk_application_window_new(app);
  gtk_window_set_decorated(GTK_WINDOW(n->window), FALSE);
  gtk_window_set_icon_name(GTK_WINDOW(n->window), "stickies");
  gtk_widget_add_css_class(n->window, "sticky-window");
  gtk_window_set_default_size(GTK_WINDOW(n->window), n->width, n->height);
  g_signal_connect(n->window, "close-request",
                   G_CALLBACK(on_window_close_request), n);
  g_signal_connect(n->window, "notify::is-active",
                   G_CALLBACK(on_window_active_changed), n);

  GtkEventController *win_motion = gtk_event_controller_motion_new();
  g_signal_connect(win_motion, "motion", G_CALLBACK(on_window_motion), n);
  gtk_widget_add_controller(n->window, win_motion);

  GtkWidget *overlay = gtk_overlay_new();
  n->overlay = overlay;
  gtk_window_set_child(GTK_WINDOW(n->window), overlay);

  n->main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_add_css_class(n->main_box, "sticky-frame");
  apply_note_color(n, n->color);
  n->is_dirty = FALSE; /* Reset dirty flag on initial setup */
  gtk_overlay_set_child(GTK_OVERLAY(overlay), n->main_box);

  /* Ultra-thin 14px Mac Stickies draggable header bar */
  GtkWidget *handle = gtk_window_handle_new();
  n->handle = handle;
  GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  n->header_box = header_box;
  gtk_widget_add_css_class(header_box, "sticky-header");
  gtk_window_handle_set_child(GTK_WINDOW_HANDLE(handle), header_box);
  gtk_box_append(GTK_BOX(n->main_box), handle);

  GtkGesture *dbl_click = gtk_gesture_click_new();
  gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(dbl_click), 1);
  g_signal_connect(dbl_click, "pressed", G_CALLBACK(on_header_dbl_click), n);
  gtk_widget_add_controller(handle, GTK_EVENT_CONTROLLER(dbl_click));

  GtkEventController *header_motion = gtk_event_controller_motion_new();
  g_signal_connect(header_motion, "enter", G_CALLBACK(on_header_enter), n);
  g_signal_connect(header_motion, "leave", G_CALLBACK(on_header_leave), n);
  gtk_widget_add_controller(header_box, header_motion);

  GtkWidget *close_btn = make_glyph_button("close", "Close Note (Ctrl+W)");
  g_signal_connect_swapped(close_btn, "clicked",
                           G_CALLBACK(request_close_note), n);
  gtk_box_append(GTK_BOX(header_box), close_btn);

  n->title_label = gtk_label_new("");
  gtk_widget_add_css_class(n->title_label, "collapsed-title");
  gtk_widget_set_hexpand(n->title_label, TRUE);
  gtk_label_set_xalign(GTK_LABEL(n->title_label), 0.0f);
  gtk_widget_set_margin_start(n->title_label, 6);
  gtk_label_set_ellipsize(GTK_LABEL(n->title_label), PANGO_ELLIPSIZE_END);
  gtk_box_append(GTK_BOX(header_box), n->title_label);

  /* Simple circular button like all other buttons */
  n->menu_btn = make_glyph_button("circle", "Note Color & Options");
  g_signal_connect(n->menu_btn, "clicked", G_CALLBACK(on_menu_btn_clicked), n);
  gtk_box_append(GTK_BOX(header_box), n->menu_btn);

  GtkWidget *zoom_btn = make_glyph_button("zoom", "Zoom Note");
  g_signal_connect_swapped(zoom_btn, "clicked", G_CALLBACK(toggle_zoom), n);
  gtk_box_append(GTK_BOX(header_box), zoom_btn);

  GtkWidget *collapse_btn = make_glyph_button("collapse", "Minimize / Maximize Note (Ctrl+M)");
  g_signal_connect_swapped(collapse_btn, "clicked", G_CALLBACK(toggle_collapse),
                           n);
  gtk_box_append(GTK_BOX(header_box), collapse_btn);

  setup_note_menu_popover(n);

  /* Rich text area */
  n->scrolled = gtk_scrolled_window_new();
  gtk_widget_set_vexpand(n->scrolled, TRUE);
  gtk_widget_set_hexpand(n->scrolled, TRUE);
  gtk_box_append(GTK_BOX(n->main_box), n->scrolled);

  n->textview = gtk_text_view_new();
  char widget_name[64];
  g_snprintf(widget_name, sizeof(widget_name), "note-tv-%s", n->id);
  gtk_widget_set_name(n->textview, widget_name);
  gtk_widget_add_css_class(n->textview, "sticky-text");
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(n->textview), GTK_WRAP_WORD_CHAR);
  gtk_text_view_set_top_margin(GTK_TEXT_VIEW(n->textview), 8);
  gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(n->textview), 10);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(n->textview), 10);
  gtk_text_view_set_right_margin(GTK_TEXT_VIEW(n->textview), 10);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(n->scrolled), n->textview);

  note_set_font_size(n, n->font_size);

  n->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(n->textview));
  gtk_text_buffer_create_tag(n->buffer, "bold", "weight", PANGO_WEIGHT_BOLD,
                             NULL);
  gtk_text_buffer_create_tag(n->buffer, "italic", "style", PANGO_STYLE_ITALIC,
                             NULL);
  gtk_text_buffer_create_tag(n->buffer, "underline", "underline",
                             PANGO_UNDERLINE_SINGLE, NULL);
  gtk_text_buffer_create_tag(n->buffer, "strikethrough", "strikethrough", TRUE,
                             NULL);
  gtk_text_buffer_create_tag(n->buffer, "large", "scale", 1.35, NULL);
  gtk_text_buffer_create_tag(n->buffer, "red", "foreground", "#d61a1a", NULL);

  gtk_text_buffer_set_text(n->buffer, text ? text : "", -1);
  deserialize_tags(n->buffer, tags_str);
  sync_note_cache(n);
  n->is_dirty = FALSE; /* Initial state after load is clean */
  g_signal_connect(n->buffer, "changed", G_CALLBACK(on_buffer_changed), n);

  /* Right-click context menu */
  GSimpleActionGroup *ag = g_simple_action_group_new();
  GMenu *menu = g_menu_new();
  GMenu *color_menu = g_menu_new();
  GMenu *format_menu = g_menu_new();

  GSimpleAction *new_act = g_simple_action_new("new_note", NULL);
  g_signal_connect(new_act, "activate", G_CALLBACK(on_new_note_action), n);
  g_action_map_add_action(G_ACTION_MAP(ag), G_ACTION(new_act));
  g_menu_append(menu, "New Sticky Note (Ctrl+N)", "note.new_note");

  GSimpleAction *save_act = g_simple_action_new("save_note", NULL);
  g_signal_connect(save_act, "activate", G_CALLBACK(on_save_action), n);
  g_action_map_add_action(G_ACTION_MAP(ag), G_ACTION(save_act));
  g_menu_append(menu, "Save Note (Ctrl+S)", "note.save_note");

  GSimpleAction *toggle_min_act = g_simple_action_new("toggle_min", NULL);
  g_signal_connect(toggle_min_act, "activate", G_CALLBACK(on_toggle_min_action), n);
  g_action_map_add_action(G_ACTION_MAP(ag), G_ACTION(toggle_min_act));
  g_menu_append(menu, "Minimize / Maximize (Ctrl+M)", "note.toggle_min");

  GSimpleAction *zin_act = g_simple_action_new("zoom_in", NULL);
  g_signal_connect(zin_act, "activate", G_CALLBACK(on_zoom_in_action), n);
  g_action_map_add_action(G_ACTION_MAP(ag), G_ACTION(zin_act));
  g_menu_append(menu, "Zoom In (Ctrl++)", "note.zoom_in");

  GSimpleAction *zout_act = g_simple_action_new("zoom_out", NULL);
  g_signal_connect(zout_act, "activate", G_CALLBACK(on_zoom_out_action), n);
  g_action_map_add_action(G_ACTION_MAP(ag), G_ACTION(zout_act));
  g_menu_append(menu, "Zoom Out (Ctrl+-)", "note.zoom_out");

  GSimpleAction *zreset_act = g_simple_action_new("zoom_reset", NULL);
  g_signal_connect(zreset_act, "activate", G_CALLBACK(on_zoom_reset_action), n);
  g_action_map_add_action(G_ACTION_MAP(ag), G_ACTION(zreset_act));
  g_menu_append(menu, "Reset Zoom (Ctrl+0)", "note.zoom_reset");

  GSimpleAction *del_act = g_simple_action_new("delete_note", NULL);
  g_signal_connect(del_act, "activate", G_CALLBACK(on_delete_action), n);
  g_action_map_add_action(G_ACTION_MAP(ag), G_ACTION(del_act));
  g_menu_append(menu, "Delete Note (Ctrl+D)", "note.delete_note");

  /* Color submenu */
  for (int i = 0; i < NUM_COLORS; i++) {
    char act_name[32], full_act[40], label[32];
    g_snprintf(act_name, sizeof(act_name), "color_%s", COLORS[i].name);
    g_snprintf(full_act, sizeof(full_act), "note.%s", act_name);
    g_snprintf(label, sizeof(label), "%c%s", g_ascii_toupper(COLORS[i].name[0]),
               COLORS[i].name + 1);

    GSimpleAction *c_act = g_simple_action_new(act_name, NULL);
    g_signal_connect(c_act, "activate", G_CALLBACK(on_color_action), n);
    g_action_map_add_action(G_ACTION_MAP(ag), G_ACTION(c_act));
    g_menu_append(color_menu, label, full_act);
  }
  g_menu_append_submenu(menu, "Color", G_MENU_MODEL(color_menu));

  /* Format submenu */
  const char *formats[][2] = {
      {"format_bold", "Bold (Ctrl+B)"},
      {"format_italic", "Italic (Ctrl+I)"},
      {"format_underline", "Underline (Ctrl+U)"},
      {"format_strikethrough", "Strikethrough (Ctrl+K)"},
      {"format_large", "Large Text"},
      {"format_red", "Red Color (Ctrl+R)"}
  };
  for (size_t i = 0; i < G_N_ELEMENTS(formats); i++) {
    char full_act[40];
    g_snprintf(full_act, sizeof(full_act), "note.%s", formats[i][0]);
    GSimpleAction *f_act = g_simple_action_new(formats[i][0], NULL);
    g_signal_connect(f_act, "activate", G_CALLBACK(on_format_action), n);
    g_action_map_add_action(G_ACTION_MAP(ag), G_ACTION(f_act));
    g_menu_append(format_menu, formats[i][1], full_act);
  }
  g_menu_append_submenu(menu, "Format", G_MENU_MODEL(format_menu));

  gtk_widget_insert_action_group(n->window, "note", G_ACTION_GROUP(ag));
  gtk_text_view_set_extra_menu(GTK_TEXT_VIEW(n->textview), G_MENU_MODEL(menu));

  /* Invisible bottom-right corner resize hotspot */
  GtkWidget *corner = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(corner, 14, 14);
  gtk_widget_set_halign(corner, GTK_ALIGN_END);
  gtk_widget_set_valign(corner, GTK_ALIGN_END);
  gtk_widget_set_cursor_from_name(corner, "se-resize");
  GtkGesture *resize_click = gtk_gesture_click_new();
  gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(resize_click), 1);
  g_signal_connect(resize_click, "pressed", G_CALLBACK(on_resize_pressed), n);
  gtk_widget_add_controller(corner, GTK_EVENT_CONTROLLER(resize_click));
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay), corner);

  /* Keyboard controller */
  GtkEventController *key_ctrl = gtk_event_controller_key_new();
  g_signal_connect(key_ctrl, "key-pressed", G_CALLBACK(on_key_pressed), n);
  gtk_widget_add_controller(n->window, key_ctrl);

  notes_list = g_list_append(notes_list, n);
  if (n->collapsed) {
    update_collapse_ui(n);
  } else {
    update_header_visibility(n);
  }
  gtk_window_present(GTK_WINDOW(n->window));
  return n;
}
