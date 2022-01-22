#ifndef CONF_H_
#define CONF_H_
#include <stdint.h>
typedef struct {
  int image_width;
  int image_height;
  uint8_t *pixels;
} conf_t;
#define IMG_WIDTH(c) (c).image_width
#define IMG_HEIGHT(c) (c).image_height
#define PIXELS(c) (c).pixels
typedef void (*apicb_t)(conf_t c);
void main02_2(conf_t c);
void main02_3(conf_t c);
void main03_3(conf_t c);
void main04_2(conf_t c);
void main05_2(conf_t c);
void main06_1(conf_t c);
void main06_7(conf_t c);
void main07_2(conf_t c);
typedef struct {
  apicb_t cb;
  const char *name;
} api_t;
#define DECL(n)                                                                \
  (api_t) { n, #n }
#ifdef CONF_IMPL
api_t apis[] = {
    DECL(main02_2), DECL(main02_3), DECL(main03_3), DECL(main04_2),
    DECL(main05_2), DECL(main06_1), DECL(main06_7), DECL(main07_2),
};
const int napis = sizeof(apis) / sizeof(apis[0]);
#endif /*CONF_IMPL*/
#endif /*CONF_H_*/
