#include "style.h"
#include "color.h"
#include <gtk/gtk.h>

void style_init(void) {
  /* High-priority CSS override that strips rounded corners and shadows ONLY from
   * the sticky notes, leaving Libadwaita dialogs, alerts, and system widgets
   * 100% styled by the active GNOME theme. */
  GString *css = g_string_new(
      "window.sticky-window, "
      "window.sticky-window.background, "
      "window.sticky-window.csd, "
      "window.sticky-window.solid-csd {"
      "  border-radius: 0px;"
      "  box-shadow: none;"
      "  outline: none;"
      "  margin: 0px;"
      "  padding: 0px;"
      "  border: none;"
      "}"
      ".sticky-frame {"
      "  border-radius: 0px;"
      "  border-width: 1px;"
      "  border-style: solid;"
      "  box-shadow: none;"
      "  margin: 0px;"
      "}"
      "window.sticky-window .sticky-header {"
      "  min-height: 16px; padding: 1px 4px; border-bottom-width: 1px; "
      "border-bottom-style: solid; transition: opacity 200ms ease, border-bottom-color 200ms ease;"
      "  opacity: 1.0;"
      "}"
      "window.sticky-window .sticky-header.header-hidden {"
      "  opacity: 0.0; border-bottom-color: transparent;"
      "}"
      "window.sticky-window .sticky-header.header-hidden:hover {"
      "  opacity: 1.0;"
      "}"
      "window.sticky-window .sticky-header .glyph-btn, "
      "window.sticky-window .sticky-header .glyph-btn:focus, "
      "window.sticky-window .sticky-header .glyph-btn:active, "
      "window.sticky-window .sticky-header .glyph-btn:focus-visible {"
      "  min-width: 14px; min-height: 14px; padding: 1px 2px; margin: 0px 2px;"
      "  border-width: 0px; border-style: none; outline-style: none; box-shadow: none;"
      "  background-color: transparent; background-image: none; border-radius: 3px; opacity: 0.65;"
      "}"
      "window.sticky-window .sticky-header .glyph-btn:hover {"
      "  opacity: 1.0; background-color: rgba(0, 0, 0, 0.08);"
      "  border-width: 0px; border-style: none; outline-style: none; box-shadow: none;"
      "}"
      "window.sticky-window .sticky-header .glyph-btn:active {"
      "  background-color: rgba(0, 0, 0, 0.16);"
      "}"
      ".collapsed-title { font-size: 10px; color: rgba(0,0,0,0.75); font-weight: bold; }"
      "popover.sticky-menu-popover contents {"
      "  padding: 6px 8px; border-radius: 10px; background-color: rgba(255, 255, 255, 0.96);"
      "  border: 1px solid rgba(0, 0, 0, 0.12); box-shadow: 0 4px 18px rgba(0, 0, 0, 0.18);"
      "}"
      ".color-swatches-box { padding: 4px 2px; }"
      ".color-swatch {"
      "  min-width: 24px; min-height: 24px; border-radius: 12px; padding: 0px; margin: 2px 3px;"
      "  border: 1px solid rgba(0, 0, 0, 0.18); box-shadow: 0 1px 2px rgba(0, 0, 0, 0.10);"
      "  outline-style: none;"
      "}"
      ".color-swatch:hover {"
      "  box-shadow: 0 2px 6px rgba(0, 0, 0, 0.25);"
      "}"
      ".color-swatch.active {"
      "  border: 2px solid rgba(0, 0, 0, 0.75);"
      "}"
      ".swatch-check { font-size: 13px; font-weight: 800; color: rgba(0, 0, 0, 0.75); }"
      ".popover-separator { margin: 5px 0px; opacity: 0.20; }"
      ".popover-action-row {"
      "  min-height: 26px; padding: 4px 8px; margin: 1px 0px; border-radius: 5px;"
      "  font-size: 13px; font-weight: 500; border-width: 0px; outline-style: none;"
      "  box-shadow: none; background-color: transparent; color: rgba(0, 0, 0, 0.85);"
      "}"
      ".popover-action-row:hover {"
      "  background-color: rgba(0, 0, 0, 0.08); color: #000000;"
      "}"
      ".popover-action-row.destructive { color: #d32f2f; }"
      ".popover-action-row.destructive:hover {"
      "  background-color: rgba(211, 47, 47, 0.10); color: #d32f2f;"
      "}"
      ".shortcut-text { font-size: 11px; opacity: 0.50; font-weight: 400; }"
      "textview.sticky-text, textview.sticky-text text {"
      "  background: transparent; color: #000000;"
      "  font-family: 'Helvetica Neue', 'Inter', 'Cantarell', sans-serif; "
      "font-size: 13px;"
      "}");

  for (int i = 0; i < NUM_COLORS; i++) {
    g_string_append_printf(
        css,
        "window.color-%s, .color-%s { background-color: %s; border-color: %s; }"
        ".color-%s .sticky-header { background-color: %s; border-bottom-color: %s; }"
        "button.color-swatch-%s { background-color: %s; }",
        COLORS[i].name, COLORS[i].name, COLORS[i].bg, COLORS[i].border,
        COLORS[i].name, COLORS[i].header, COLORS[i].border,
        COLORS[i].name, COLORS[i].bg);
  }

  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_string(provider, css->str);
  gtk_style_context_add_provider_for_display(
      gdk_display_get_default(), GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_USER
  );
  g_string_free(css, TRUE);
  g_object_unref(provider);
}
