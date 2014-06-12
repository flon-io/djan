
//
// Copyright (c) 2013-2014, John Mettraux, jmettraux+flon@gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// Made in Japan.
//

#define _POSIX_C_SOURCE 200809L

#include "djan.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "aabro.h"
#include "flutil.h"


//
// dja_value malloc/free

dja_value *dja_value_malloc(char type, char *input, size_t off, size_t len)
{
  dja_value *v = calloc(1, sizeof(dja_value));
  v->type = type;
  v->source = input;
  v->soff = off;
  v->slen = len;

  return v;
}

void dja_value_free(dja_value *v)
{
  if (v->key != NULL) free(v->key);
  if (v->slen == 0 && v->source != NULL) free(v->source);

  if (v->children != NULL)
  {
    for (size_t i = 0; ; i++)
    {
      dja_value *c = v->children[i];
      if (c == NULL) break;
      dja_value_free(c);
    }
    free(v->children);
  }

  free(v);
}


//
// parsing

abr_parser *dja_parser = NULL;

void dja_parser_init()
{
  if (dja_parser != NULL) return;

  // TODO: move away from the "values" model...
  abr_parser *entries =
    abr_n_rep(
      "entries",
      abr_seq(
        abr_n("value"),
        abr_rep(
          abr_seq(abr_string(","), abr_n("value"), NULL),
          0, -1),
        NULL
      ),
      0, 1);

  abr_parser *object =
    abr_n_seq("object", abr_string("{"), entries, abr_string("}"), NULL);

  // array == `[` + (value + (`,` + value) * 0) * 0 + `]`
  abr_parser *values =
    abr_n_rep(
      "values",
      abr_seq(
        abr_n("value"),
        abr_rep(
          abr_seq(abr_string(","), abr_n("value"), NULL),
          0, -1),
        NULL
      ),
      0, 1);

  abr_parser *array =
    abr_n_seq("array", abr_string("["), values, abr_string("]"), NULL);

  dja_parser =
    abr_n_alt(
      "value",
      abr_n_regex(
        "string",
        "^\"("
          //"\\\\." "|"
          "\\\\[\"\\/\\\\bfnrt]" "|"
          "\\\\u[0-9a-fA-F]{4}" "|"
          "[^\"\\]"
        ")*\""),
      abr_n_regex("number", "^-?[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?"),
      object,
      array,
      abr_n_string("true", "true"),
      abr_n_string("false", "false"),
      abr_n_string("null", "null"),
      NULL);
}

// forward declaration...
dja_value *dja_extract_value(char *input, abr_tree *t);

dja_value **dja_extract_entries(char *input, abr_tree *t)
{
  return calloc(1, sizeof(dja_value *));
}

//typedef int abr_tree_func(abr_tree *);
//
int dja_atree_is_value(abr_tree *t)
{
  // -1: fail, do not continue
  //  0: fail, please check my children
  //  1: success

  if (t->result != 1) return -1;
  return t->name && strcmp(t->name, "value") == 0;
}

dja_value **dja_extract_values(char *input, abr_tree *t)
{
  //printf("%s\n", abr_tree_to_string(t));

  abr_tree **ts = abr_tree_collect(t, dja_atree_is_value);
  size_t l = 0; while (ts[l] != NULL) l++;

  dja_value **vs = calloc(l + 1, sizeof(dja_value *));

  for (size_t i = 0; i < l; i++)
  {
    //printf("** %s\n", abr_tree_to_string(ts[i]));
    vs[i] = dja_extract_value(input, ts[i]);
  }

  return vs;
}

dja_value *dja_extract_value(char *input, abr_tree *t)
{
  if (t->result != 1) return NULL;
  //if (t->name == NULL) return NULL;

  if (strcmp(t->name, "value") == 0)
  {
    for (size_t i = 0; ; i++)
    {
      if (t->children[i] == NULL) break;
      if (t->children[i]->result != 1) continue;
      return dja_extract_value(input, t->children[i]);
    }
  }

  char ty = '-';

  if (strcmp(t->name, "string") == 0) ty = 's';
  else if (strcmp(t->name, "number") == 0) ty = 'n';
  else if (strcmp(t->name, "true") == 0) ty = 't';
  else if (strcmp(t->name, "false") == 0) ty = 'f';
  else if (strcmp(t->name, "null") == 0) ty = '0';
  else if (strcmp(t->name, "array") == 0) ty = 'a';
  else if (strcmp(t->name, "object") == 0) ty = 'o';

  if (ty == '-') return NULL;

  dja_value *v = dja_value_malloc(ty, input, t->offset, t->length);

  if (ty == 'o') v->children = dja_extract_entries(input, t->children[1]);
  else if (ty == 'a') v->children = dja_extract_values(input, t->children[1]);

  return v;
}

dja_value *dja_parse(char *input)
{
  dja_parser_init();

  abr_tree *t = abr_parse_all(input, 0, dja_parser);
  // TODO: deal with errors (t->result < 0)

  //printf(">%s<\n", input);
  //printf("%s\n", abr_tree_to_string(t));

  dja_value *v = dja_extract_value(input, t);
  abr_tree_free(t);

  return v;
}


//
// extracting stuff out of dja_value items

char *dja_string(dja_value *v)
{
  if (v->slen == 0) return strdup(v->source + v->soff);
  return strndup(v->source + v->soff, v->slen);
}

char *dja_to_string(dja_value *v)
{
  if (v->type != 's') return dja_string(v);

  return dja_n_unescape(v->source + v->soff + 1, v->slen - 2);
}

int dja_to_int(dja_value *v)
{
  if (v->type == 't') return 1;
  if (v->type == 'f') return 0;
  if (v->type != 'n') return -1;
  return atoi(v->source + v->soff);
}

double dja_to_double(dja_value *v)
{
  if (v->type != 'n') return 0.0;
  return atof(v->source + v->soff);
}


//
// [un]escape

char *dja_escape(char *s)
{
  return dja_n_escape(s, strlen(s));
}

char *dja_n_escape(char *s, size_t n)
{
  flu_sbuffer *b = flu_sbuffer_malloc();

  for (size_t i; i < n; i++)
  {
    char c = s[i];
    if (c == '\0') break;
    if (c == '\\') flu_sbprintf(b, "\\\\");
    else if (c == '"') flu_sbprintf(b, "\\\"");
    else if (c == '\b') flu_sbprintf(b, "\\b");
    else if (c == '\f') flu_sbprintf(b, "\\f");
    else if (c == '\n') flu_sbprintf(b, "\\n");
    else if (c == '\r') flu_sbprintf(b, "\\r");
    else if (c == '\t') flu_sbprintf(b, "\\t");
    else flu_sbputc(b, c);
  }

  return flu_sbuffer_to_string(b);
}

char *dja_unescape(char *s)
{
  return dja_n_unescape(s, strlen(s));
}

// based on cutef8 by Jeff Bezanson
//
char *dja_n_unescape(char *s, size_t n)
{
  char *d = calloc(n + 1, sizeof(char));

  for (size_t is = 0, id = 0; is < n; is++)
  {
    if (s[is] != '\\') { d[id++] = s[is]; continue; }

    char c = s[is + 1];
    if (c == '\\') d[id++] = '\\';
    else if (c == '"') d[id++] = '"';
    else if (c == 'b') d[id++] = '\b';
    else if (c == 'f') d[id++] = '\f';
    else if (c == 'n') d[id++] = '\n';
    else if (c == 'r') d[id++] = '\r';
    else if (c == 't') d[id++] = '\t';
    else if (c == 'u')
    {
      char *su = strndup(s + is + 2, 4);
      unsigned int u = strtol(su, NULL, 16);
      free(su);
      if (u < 0x80)
      {
        d[id++] = (char)u;
      }
      else if (u < 0x800)
      {
        d[id++] = (u >> 6) | 0xc0;
        d[id++] = (u & 0x3f) | 0x80;
      }
      else //if (u < 0x8000)
      {
        d[id++] = (u >> 12) | 0xe0;
        d[id++] = ((u >> 6) & 0x3f) | 0x80;
        d[id++] = (u & 0x3f) | 0x80;
      }
      is += 4;
    }
    else // leave as is
    {
      d[id++] = '\\'; d[id++] = c;
    }
    is++;
  }

  return d;
}

