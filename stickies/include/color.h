#ifndef COLOR_H
#define COLOR_H

typedef struct {
  const char *name;
  const char *bg;
  const char *header;
  const char *border;
} StickyColor;

extern const StickyColor COLORS[];
extern const int NUM_COLORS;

#endif /* COLOR_H */
