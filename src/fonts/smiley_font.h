#ifndef SMILEY_FONT_H
#define SMILEY_FONT_H

#include <lvgl.h>

#ifndef SMILEY_FONT
#define SMILEY_FONT 1
#endif

#if SMILEY_FONT

#ifdef __cplusplus
extern "C" {
#endif

// Change the name to match the actual font
extern const lv_font_t smiley_font;  // Changed from smiley_font_24

#ifdef __cplusplus
}
#endif

#endif /* SMILEY_FONT */
#endif /* SMILEY_FONT_H */