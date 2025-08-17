#ifndef PARSER_H
#define PARSER_H

void set_stream(FILE *f);
FILE *get_stream();
void tag(const char *name, const char *content_fmt, ...);
void tag_open(const char *name, const char *attrs);
void tag_close(const char *name);
void tag_inline(const char *name, const char *attrs, const char *text);
void tag_inline_open(const char *name, const char *attrs, const char *text);
void cmtag_css_link(const char *filename);

#endif
