#ifndef STORAGE_H
#define STORAGE_H

#include <gtk/gtk.h>

char *serialize_tags(GtkTextBuffer *buffer);
void deserialize_tags(GtkTextBuffer *buffer, const char *tags_str);

void save_all_notes_now(void);
void load_or_create_default_notes(GtkApplication *app);

#endif /* STORAGE_H */
