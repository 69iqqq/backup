#ifndef SAVE_DIALOG_H
#define SAVE_DIALOG_H

#include <gtk/gtk.h>

typedef enum {
  SAVE_DIALOG_RESPONSE_SAVE,
  SAVE_DIALOG_RESPONSE_DISCARD,
  SAVE_DIALOG_RESPONSE_CANCEL
} SaveDialogResponse;

typedef void (*SaveDialogCallback)(SaveDialogResponse response, gpointer user_data);

typedef struct _StickyNote StickyNote;

void show_save_changes_dialog(StickyNote *n,
                              SaveDialogCallback callback,
                              gpointer user_data);

#endif /* SAVE_DIALOG_H */
