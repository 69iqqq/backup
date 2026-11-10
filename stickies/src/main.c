#include <gtk/gtk.h>
#include <dlfcn.h>
#include "style.h"
#include "storage.h"

static void init_adwaita_if_available(void) {
  void *h = dlopen("libadwaita-1.so.0", RTLD_NOW | RTLD_GLOBAL);
  if (h) {
    void (*adw_init)(void) = dlsym(h, "adw_init");
    if (adw_init) {
      adw_init();
    }
  }
}

static void on_activate(GtkApplication *app, gpointer user_data) {
  (void)user_data;
  GdkDisplay *display = gdk_display_get_default();
  if (display) {
    GtkIconTheme *theme = gtk_icon_theme_get_for_display(display);
    gtk_icon_theme_add_search_path(theme, "/home/abir/stickies");
    gtk_icon_theme_add_search_path(theme, ".");
  }
  gtk_window_set_default_icon_name("stickies");

  style_init();
  load_or_create_default_notes(app);
}

int main(int argc, char **argv) {
  init_adwaita_if_available();

  g_setenv("GSK_RENDERER", "cairo", TRUE);
  g_set_prgname("stickies");
  g_set_application_name("Stickies");

  GtkApplication *app = gtk_application_new("org.custom.Stickies",
                                            G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
