#include "storage.h"
#include "note.h"
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>

static char *get_config_path(void) {
  char *dir = g_build_filename(g_get_user_config_dir(), "stickies", NULL);
  g_mkdir_with_parents(dir, 0755);
  char *path = g_build_filename(dir, "notes.json", NULL);
  if (!g_file_test(path, G_FILE_TEST_EXISTS)) {
    char *old_dir = g_build_filename(g_get_user_config_dir(), "mac-stickies", NULL);
    char *old_path = g_build_filename(old_dir, "notes.json", NULL);
    if (g_file_test(old_path, G_FILE_TEST_EXISTS)) {
      char *contents = NULL;
      gsize len = 0;
      if (g_file_get_contents(old_path, &contents, &len, NULL)) {
        g_file_set_contents(path, contents, len, NULL);
        g_free(contents);
      }
    }
    g_free(old_path);
    g_free(old_dir);
  }
  g_free(dir);
  return path;
}

static char *get_legacy_ini_path(void) {
  char *dir = g_build_filename(g_get_user_config_dir(), "stickies", NULL);
  char *path = g_build_filename(dir, "notes.ini", NULL);
  if (!g_file_test(path, G_FILE_TEST_EXISTS)) {
    char *old_dir = g_build_filename(g_get_user_config_dir(), "mac-stickies", NULL);
    char *old_path = g_build_filename(old_dir, "notes.ini", NULL);
    if (g_file_test(old_path, G_FILE_TEST_EXISTS)) {
      g_free(path);
      path = old_path;
      g_free(old_dir);
      g_free(dir);
      return path;
    }
    g_free(old_path);
    g_free(old_dir);
  }
  g_free(dir);
  return path;
}

char *serialize_tags(GtkTextBuffer *buffer) {
  if (!GTK_IS_TEXT_BUFFER(buffer))
    return g_strdup("");
  GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);
  if (!GTK_IS_TEXT_TAG_TABLE(table))
    return g_strdup("");

  GString *str = g_string_new("");
  const char *tag_names[] = {"bold", "italic", "underline", "strikethrough", "large", "red"};

  for (int i = 0; i < 6; i++) {
    GtkTextTag *tag = gtk_text_tag_table_lookup(table, tag_names[i]);
    if (!tag)
      continue;

    GtkTextIter iter;
    gtk_text_buffer_get_start_iter(buffer, &iter);
    while (!gtk_text_iter_is_end(&iter)) {
      if (gtk_text_iter_starts_tag(&iter, tag)) {
        int start_off = gtk_text_iter_get_offset(&iter);
        GtkTextIter end_iter = iter;
        if (!gtk_text_iter_forward_to_tag_toggle(&end_iter, tag)) {
          gtk_text_buffer_get_end_iter(buffer, &end_iter);
        }
        int end_off = gtk_text_iter_get_offset(&end_iter);
        g_string_append_printf(str, "%s:%d:%d;", tag_names[i], start_off,
                               end_off);
        iter = end_iter;
      } else {
        if (!gtk_text_iter_forward_to_tag_toggle(&iter, tag))
          break;
      }
    }
  }
  return g_string_free(str, FALSE);
}

void deserialize_tags(GtkTextBuffer *buffer, const char *tags_str) {
  if (!tags_str || !*tags_str)
    return;
  char **items = g_strsplit(tags_str, ";", -1);
  for (int i = 0; items[i] != NULL && *items[i] != '\0'; i++) {
    char **parts = g_strsplit(items[i], ":", 3);
    if (parts[0] && parts[1] && parts[2]) {
      int start_off = atoi(parts[1]);
      int end_off = atoi(parts[2]);
      GtkTextIter start, end;
      gtk_text_buffer_get_iter_at_offset(buffer, &start, start_off);
      gtk_text_buffer_get_iter_at_offset(buffer, &end, end_off);
      gtk_text_buffer_apply_tag_by_name(buffer, parts[0], &start, &end);
    }
    g_strfreev(parts);
  }
  g_strfreev(items);
}

static void json_append_escaped(GString *str, const char *s) {
  g_string_append_c(str, '"');
  if (s) {
    for (const char *p = s; *p; p++) {
      switch (*p) {
        case '"':  g_string_append(str, "\\\""); break;
        case '\\': g_string_append(str, "\\\\"); break;
        case '\b': g_string_append(str, "\\b"); break;
        case '\f': g_string_append(str, "\\f"); break;
        case '\n': g_string_append(str, "\\n"); break;
        case '\r': g_string_append(str, "\\r"); break;
        case '\t': g_string_append(str, "\\t"); break;
        default:
          if ((unsigned char)*p < 0x20) {
            g_string_append_printf(str, "\\u%04x", (unsigned char)*p);
          } else {
            g_string_append_c(str, *p);
          }
          break;
      }
    }
  }
  g_string_append_c(str, '"');
}

static void skip_ws(const char **p) {
  while (**p && g_ascii_isspace(**p)) (*p)++;
}

static char *parse_json_string(const char **p) {
  skip_ws(p);
  if (**p != '"') return NULL;
  (*p)++;
  GString *res = g_string_new("");
  while (**p && **p != '"') {
    if (**p == '\\') {
      (*p)++;
      if (!**p) break;
      switch (**p) {
        case '"':  g_string_append_c(res, '"'); break;
        case '\\': g_string_append_c(res, '\\'); break;
        case '/':  g_string_append_c(res, '/'); break;
        case 'b':  g_string_append_c(res, '\b'); break;
        case 'f':  g_string_append_c(res, '\f'); break;
        case 'n':  g_string_append_c(res, '\n'); break;
        case 'r':  g_string_append_c(res, '\r'); break;
        case 't':  g_string_append_c(res, '\t'); break;
        case 'u': {
          char hex[5] = {0};
          for (int i = 0; i < 4 && (*p)[1]; i++) {
            (*p)++;
            hex[i] = **p;
          }
          gunichar u = (gunichar)strtoul(hex, NULL, 16);
          g_string_append_unichar(res, u);
          break;
        }
        default: g_string_append_c(res, **p); break;
      }
    } else {
      g_string_append_c(res, **p);
    }
    (*p)++;
  }
  if (**p == '"') (*p)++;
  return g_string_free(res, FALSE);
}

void save_all_notes_now(void) {
  char *path = get_config_path();
  GString *json = g_string_new("[\n");
  gboolean first = TRUE;

  for (GList *l = note_get_all(); l != NULL; l = l->next) {
    StickyNote *n = (StickyNote *)l->data;
    sync_note_cache(n);

    if (!first) {
      g_string_append(json, ",\n");
    }
    first = FALSE;

    g_string_append(json, "  {\n");
    g_string_append(json, "    \"id\": ");
    json_append_escaped(json, n->id);
    g_string_append(json, ",\n    \"color\": ");
    json_append_escaped(json, n->color ? n->color : "yellow");
    g_string_append_printf(json, ",\n    \"width\": %d,\n    \"height\": %d,\n    \"collapsed\": %s,\n    \"font_size\": %d",
                           n->width, n->height, n->collapsed ? "true" : "false",
                           n->font_size > 0 ? n->font_size : DEFAULT_FONT_SIZE);
    g_string_append(json, ",\n    \"text\": ");
    json_append_escaped(json, n->cached_text ? n->cached_text : "");
    g_string_append(json, ",\n    \"tags\": ");
    json_append_escaped(json, n->cached_tags ? n->cached_tags : "");
    g_string_append(json, "\n  }");

    n->is_dirty = FALSE;
  }
  g_string_append(json, "\n]\n");

  g_file_set_contents(path, json->str, (gssize)json->len, NULL);
  g_string_free(json, TRUE);
  g_free(path);
}

void load_or_create_default_notes(GtkApplication *app) {
  char *path = get_config_path();
  char *json_content = NULL;
  gsize length = 0;
  int note_count = 0;

  if (g_file_get_contents(path, &json_content, &length, NULL) && json_content) {
    const char *p = json_content;
    skip_ws(&p);
    if (*p == '[') {
      p++;
      while (*p && *p != ']') {
        skip_ws(&p);
        if (*p == '{') {
          p++;
          char *id = NULL;
          char *color = NULL;
          int width = 360;
          int height = 240;
          gboolean collapsed = FALSE;
          char *text = NULL;
          char *tags = NULL;

          int font_size = DEFAULT_FONT_SIZE;

          while (*p && *p != '}') {
            char *key = parse_json_string(&p);
            skip_ws(&p);
            if (*p == ':') p++;
            skip_ws(&p);
            if (g_strcmp0(key, "id") == 0) {
              id = parse_json_string(&p);
            } else if (g_strcmp0(key, "color") == 0) {
              color = parse_json_string(&p);
            } else if (g_strcmp0(key, "text") == 0) {
              text = parse_json_string(&p);
            } else if (g_strcmp0(key, "tags") == 0) {
              tags = parse_json_string(&p);
            } else if (g_strcmp0(key, "width") == 0) {
              width = (int)strtol(p, (char **)&p, 10);
            } else if (g_strcmp0(key, "height") == 0) {
              height = (int)strtol(p, (char **)&p, 10);
            } else if (g_strcmp0(key, "font_size") == 0) {
              font_size = (int)strtol(p, (char **)&p, 10);
            } else if (g_strcmp0(key, "collapsed") == 0) {
              if (strncmp(p, "true", 4) == 0) { collapsed = TRUE; p += 4; }
              else if (strncmp(p, "false", 5) == 0) { collapsed = FALSE; p += 5; }
            } else {
              if (*p == '"') {
                char *dummy = parse_json_string(&p);
                g_free(dummy);
              } else {
                while (*p && *p != ',' && *p != '}') p++;
              }
            }
            g_free(key);
            skip_ws(&p);
            if (*p == ',') p++;
            skip_ws(&p);
          }
          if (*p == '}') p++;

          if (id) {
            StickyNote *n = create_sticky_note(app, id, color ? color : "yellow",
                                               width, height, collapsed,
                                               text ? text : "", tags ? tags : "",
                                               font_size);
            if (n) {
              n->is_dirty = FALSE;
            }
            note_count++;
          }
          g_free(id);
          g_free(color);
          g_free(text);
          g_free(tags);
        }
        skip_ws(&p);
        if (*p == ',') p++;
        skip_ws(&p);
      }
    }
    g_free(json_content);
  }

  g_free(path);

  if (note_count > 0) {
    return;
  }

  /* Fallback: check legacy notes.ini if notes.json was empty or absent */
  char *legacy_path = get_legacy_ini_path();
  GKeyFile *kf = g_key_file_new();
  if (g_key_file_load_from_file(kf, legacy_path, G_KEY_FILE_NONE, NULL)) {
    gsize len = 0;
    char **groups = g_key_file_get_groups(kf, &len);
    for (gsize i = 0; i < len; i++) {
      char *color = g_key_file_get_string(kf, groups[i], "color", NULL);
      int w = g_key_file_get_integer(kf, groups[i], "width", NULL);
      int h = g_key_file_get_integer(kf, groups[i], "height", NULL);
      gboolean col = g_key_file_get_boolean(kf, groups[i], "collapsed", NULL);
      char *text = g_key_file_get_string(kf, groups[i], "text", NULL);
      char *tags = g_key_file_get_string(kf, groups[i], "tags", NULL);
      int font_size = g_key_file_get_integer(kf, groups[i], "font_size", NULL);
      if (font_size <= 0) font_size = DEFAULT_FONT_SIZE;

      /* Filter out old demo text if migrating from legacy notes.ini */
      if (text && g_str_has_prefix(text, "It's easy to customize your notes")) {
        g_free(color);
        g_free(text);
        g_free(tags);
        continue;
      }

      StickyNote *n = create_sticky_note(app, groups[i], color, w, h, col, text, tags, font_size);
      if (n) {
        n->is_dirty = FALSE;
      }
      note_count++;
      g_free(color);
      g_free(text);
      g_free(tags);
    }
    g_strfreev(groups);
  }
  g_key_file_free(kf);
  g_free(legacy_path);

  if (note_count > 0) {
    save_all_notes_now();
    return;
  }

  /* Default brand-new setup: create single blank note with NO demo text */
  char *id = g_uuid_string_random();
  StickyNote *n = create_sticky_note(app, id, "yellow", 360, 240, FALSE, "", "", DEFAULT_FONT_SIZE);
  if (n) {
    n->is_dirty = FALSE;
  }
  g_free(id);
}
