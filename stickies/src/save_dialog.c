#include "save_dialog.h"
#include "note.h"
#include <dlfcn.h>
#include <string.h>

typedef void *(*fn_adw_alert_dialog_new)(const char *heading, const char *body);
typedef void (*fn_adw_alert_dialog_add_response)(void *self, const char *response, const char *label);
typedef void (*fn_adw_alert_dialog_set_response_appearance)(void *self, const char *response, int appearance);
typedef void (*fn_adw_alert_dialog_set_default_response)(void *self, const char *response);
typedef void (*fn_adw_alert_dialog_set_close_response)(void *self, const char *response);
typedef const char *(*fn_adw_alert_dialog_choose_finish)(void *self, GAsyncResult *result);
typedef void (*fn_adw_alert_dialog_choose)(void *self, GtkWidget *parent, GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data);

typedef struct {
  StickyNote *note;
  SaveDialogCallback callback;
  gpointer user_data;
} SaveDialogClosure;

static fn_adw_alert_dialog_choose_finish s_choose_finish = NULL;

static void on_adw_choose_callback(GObject *source, GAsyncResult *res, gpointer data) {
  SaveDialogClosure *closure = (SaveDialogClosure *)data;
  SaveDialogCallback cb = closure->callback;
  gpointer udata = closure->user_data;
  g_free(closure);

  const char *resp = "cancel";
  if (s_choose_finish) {
    resp = s_choose_finish(source, res);
  }

  if (g_strcmp0(resp, "save") == 0) {
    if (cb) cb(SAVE_DIALOG_RESPONSE_SAVE, udata);
  } else if (g_strcmp0(resp, "discard") == 0) {
    if (cb) cb(SAVE_DIALOG_RESPONSE_DISCARD, udata);
  } else {
    if (cb) cb(SAVE_DIALOG_RESPONSE_CANCEL, udata);
  }
}

void show_save_changes_dialog(StickyNote *n,
                              SaveDialogCallback callback,
                              gpointer user_data) {
  static gboolean adw_attempted = FALSE;
  static void *h = NULL;
  static fn_adw_alert_dialog_new s_alert_dialog_new = NULL;
  static fn_adw_alert_dialog_add_response s_add_response = NULL;
  static fn_adw_alert_dialog_set_response_appearance s_set_appearance = NULL;
  static fn_adw_alert_dialog_set_default_response s_set_default = NULL;
  static fn_adw_alert_dialog_set_close_response s_set_close = NULL;
  static fn_adw_alert_dialog_choose s_choose = NULL;

  if (!adw_attempted) {
    adw_attempted = TRUE;
    h = dlopen("libadwaita-1.so.0", RTLD_NOW | RTLD_GLOBAL);
    if (h) {
      void (*adw_init)(void) = dlsym(h, "adw_init");
      if (adw_init) {
        adw_init();
      }
      s_alert_dialog_new = (fn_adw_alert_dialog_new)dlsym(h, "adw_alert_dialog_new");
      s_add_response = (fn_adw_alert_dialog_add_response)dlsym(h, "adw_alert_dialog_add_response");
      s_set_appearance = (fn_adw_alert_dialog_set_response_appearance)dlsym(h, "adw_alert_dialog_set_response_appearance");
      s_set_default = (fn_adw_alert_dialog_set_default_response)dlsym(h, "adw_alert_dialog_set_default_response");
      s_set_close = (fn_adw_alert_dialog_set_close_response)dlsym(h, "adw_alert_dialog_set_close_response");
      s_choose = (fn_adw_alert_dialog_choose)dlsym(h, "adw_alert_dialog_choose");
      s_choose_finish = (fn_adw_alert_dialog_choose_finish)dlsym(h, "adw_alert_dialog_choose_finish");
    }
  }

  if (s_alert_dialog_new && s_add_response && s_choose) {
    void *dialog = s_alert_dialog_new(
        "Save Changes?",
        "Open note contains unsaved changes. Changes which are not saved will be permanently lost.");
    s_add_response(dialog, "cancel", "Cancel");
    s_add_response(dialog, "discard", "Discard");
    if (s_set_appearance) {
      s_set_appearance(dialog, "discard", 2 /* ADW_RESPONSE_DESTRUCTIVE */);
    }
    s_add_response(dialog, "save", "Save");
    if (s_set_appearance) {
      s_set_appearance(dialog, "save", 1 /* ADW_RESPONSE_SUGGESTED */);
    }
    if (s_set_default) {
      s_set_default(dialog, "save");
    }
    if (s_set_close) {
      s_set_close(dialog, "cancel");
    }

    SaveDialogClosure *closure = g_malloc0(sizeof(SaveDialogClosure));
    closure->note = n;
    closure->callback = callback;
    closure->user_data = user_data;

    s_choose(dialog, GTK_WIDGET(n->window), NULL, on_adw_choose_callback, closure);
    return;
  }

  /* Safe fallback if libadwaita was somehow unavailable */
  if (callback) {
    callback(SAVE_DIALOG_RESPONSE_SAVE, user_data);
  }
}
