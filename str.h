/* str.h: string library */

#pragma once

void    str_lower(const char *s, char *w);
void    str_upper(const char *s, char *w);
void    str_title(const char *s, char *w);
void    str_rstrip(const char *s, const char *chars, char *w);
void    str_delete(const char *s, const char *chars, char *w);
void    str_translate(const char *s, const char *from, const char *to, char *w);

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
