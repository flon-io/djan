
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
      abr_n_string("true", "true"),
      abr_n_string("false", "false"),
      abr_n_string("null", "null"),
      NULL);
}

dja_value *dja_extract(char *input, abr_tree *t)
{
  if (t->result != 1) return NULL;
  //if (t->name == NULL) return NULL;

  if (strcmp(t->name, "value") == 0)
  {
    for(size_t i = 0; ; i++)
    {
      if (t->children[i] == NULL) break;
      if (t->children[i]->result != 1) continue;
      return dja_extract(input, t->children[i]);
    }
  }

  char ty = '-';

  if (strcmp(t->name, "string") == 0) ty = 's';
  else if (strcmp(t->name, "number") == 0) ty = 'n';
  else if (strcmp(t->name, "true") == 0) ty = 't';
  else if (strcmp(t->name, "false") == 0) ty = 'f';
  else if (strcmp(t->name, "null") == 0) ty = '0';

  if (ty != '-') return dja_value_malloc(ty, input, t->offset, t->length);

  return NULL;
}

dja_value *dja_parse(char *input)
{
  dja_parser_init();

  abr_tree *t = abr_parse_all(input, 0, dja_parser);
  // TODO: deal with errors (t->result < 0)

  //printf(">%s<\n", input);
  //printf("%s\n", abr_tree_to_string(t));

  dja_value *v = dja_extract(input, t);
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

  return strndup(v->source + v->soff + 1, v->slen - 2);
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

