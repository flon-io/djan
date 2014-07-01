
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
#include <stdint.h>

#include "aabro.h"
#include "flutil.h"


//
// dja_value malloc/free

static dja_value *dja_value_malloc(
  char type, char *input, size_t off, size_t len)
{
  dja_value *v = calloc(1, sizeof(dja_value));
  v->type = type;
  v->source = input;
  v->soff = off;
  v->slen = len;
  v->sibling = NULL;
  v->child = NULL;

  return v;
}

void dja_value_free(dja_value *v)
{
  if (v->key != NULL) free(v->key);
  if (v->slen == 0 && v->source != NULL) free(v->source);

  for (dja_value *c = v->child, *next = NULL; c != NULL; )
  {
    next = c->sibling;
    dja_value_free(c);
    c = next;
  }

  free(v);
}


//
// parsing

static abr_parser *dja_parser = NULL;
static abr_parser *dja_radial_parser = NULL;

static void dja_parser_init()
{
  if (dja_parser != NULL) return;

  // djan (JSON & co)

  abr_parser *blanks =
    abr_regex("^[ \t\n\r]*");

  abr_parser *string =
    abr_n_regex(
      "string",
      "^\"("
        //"\\\\." "|"
        "\\\\[\"\\/\\\\bfnrt]" "|"
        "\\\\u[0-9a-fA-F]{4}" "|"
        "[^\"\\]"
      ")*\"");
  abr_parser *sqstring =
    abr_n_regex("sqstring", "^'(\\\\'|[^'])*'");

  abr_parser *symbol =
    abr_n_regex(
      "symbol",
      "^[a-zA-Z_][a-zA-Z_0-9]*");

  abr_parser *entry =
    abr_n_seq(
      "entry",
      blanks,
      abr_n_alt("key", string, sqstring, symbol, NULL),
      blanks,
      abr_string(":"),
      abr_n("value"),
      NULL);

  abr_parser *entries =
    abr_n_rep(
      "entries",
      abr_seq(
        entry,
        abr_rep(
          abr_seq(abr_regex("^,?"), entry, NULL),
          0, -1),
        NULL
      ),
      0, 1);

  abr_parser *object =
    abr_n_seq("object", abr_string("{"), entries, abr_string("}"), NULL);

  abr_parser *values =
    abr_n_rep(
      "values",
      abr_seq(
        abr_n("value"),
        abr_rep(
          abr_seq(abr_regex("^,?"), abr_n("value"), NULL),
          0, -1),
        NULL
      ),
      0, 1);

  abr_parser *array =
    abr_n_seq("array", abr_string("["), values, abr_string("]"), NULL);

  abr_parser *pure_value =
    abr_alt(
      string,
      sqstring,
      abr_n_regex("number", "^-?[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?"),
      object,
      array,
      abr_n_string("true", "true"),
      abr_n_string("false", "false"),
      abr_n_string("null", "null"),
      symbol,
      NULL);

  dja_parser =
    abr_n_seq("value", blanks, pure_value, blanks, NULL);

  // radial

  //abr_parser *radas = abr_rep

  //abr_parser *exparg =
  //  abr_n_rep(
  //    "expa", dja_, 0, 1);

  //abr_parser *expatts =
  //  abr_n_rep(
  //    "expas",
  //    abr_seq(
  //      dja_parser,
  //      abr_rep(
  //        abr_seq(abr_regex("^,?"), dja_parser, NULL),
  //        0, -1),
  //      NULL
  //    ),
  //    0, 1);

  //abr_parser *line =
  //  abr_n_seq("line", expindent, expname, exparg, expatts, NULL);

  abr_parser *rad_blank = abr_regex("^[ \t]*");

  abr_parser *rad_i = abr_n_regex("rad_i", "^[ \t]*");
  abr_parser *rad_n = abr_name("rad_n", symbol);

  abr_parser *rad_a =
    abr_rep(abr_n_seq("rad_a", rad_blank, pure_value, NULL), 0, 1);

  abr_parser *rad_eol =
    abr_regex("^[ \t]*(#[^\n\r]*)?");

  abr_parser *rad_l =
    abr_n_seq("rad_l", rad_i, rad_n, rad_a, NULL);

  abr_parser *rad_line =
    abr_seq(abr_rep(rad_l, 0, 1), rad_eol, NULL);

  dja_radial_parser =
    abr_seq(
      abr_regex("^[\n\r]*"),
      rad_line,
      abr_rep(
        abr_seq(
          abr_regex("^[\n\r]+"),
          rad_line,
          NULL),
        0, -1),
      NULL);
}

// forward declarations
static dja_value *dja_extract_value(char *input, abr_tree *t);

//typedef int abr_tree_func(abr_tree *);
//
// -1: fail, do not continue
//  0: fail, please check my children
//  1: success
//
static short dja_atree_is_value(abr_tree *t)
{
  if (t->result != 1) return -1;
  return t->name && strcmp(t->name, "value") == 0;
}
static short dja_atree_is_entry(abr_tree *t)
{
  if (t->result != 1) return -1;
  return t->name && strcmp(t->name, "entry") == 0;
}

static char *dja_sq_unescape(const char *s, size_t n)
{
  char *r = calloc(n + 1, sizeof(char));
  for (size_t i = 0, j = 0; i < n; i++)
  {
    char c = s[i];
    if (c == '\0') break;
    if (c != '\\') r[j++] = c;
  }
  return r;
}

static char *dja_extract_key(char *input, abr_tree *t)
{
  //printf("tk\n%s\n", abr_tree_to_string_with_leaves(input, t));

  abr_tree *c = t->child;

  if (c->result == 1)
    return flu_n_unescape(input + c->offset + 1, c->length - 2);

  c = c->sibling;

  if (c->result == 1)
    return dja_sq_unescape(input + c->offset + 1, c->length - 2);

  c = c->sibling;

  return strndup(input + c->offset, c->length);
}

static dja_value *dja_extract_entries(char *input, abr_tree *t)
{
  //printf("%s\n", abr_tree_to_string_with_leaves(input, t));

  flu_list *ts = abr_tree_list(t, dja_atree_is_entry);

  dja_value *first = NULL;
  dja_value *child = NULL;

  for (flu_node *n = ts->first; n != NULL; n = n->next)
  {
    abr_tree *tt = (abr_tree *)n->item;
    //printf("**\n%s\n", abr_tree_to_string_with_leaves(input, tt));
    dja_value *v = dja_extract_value(input, abr_t_child(tt, 4));
    v->key = dja_extract_key(input, abr_t_child(tt, 1));
    if (first == NULL) first = v;
    if (child != NULL) child->sibling = v;
    child = v;
  }

  flu_list_free(ts);

  return first;
}

static dja_value *dja_extract_values(char *input, abr_tree *t)
{
  //printf("%s\n", abr_tree_to_string(t));

  flu_list *ts = abr_tree_list(t, dja_atree_is_value);

  dja_value *first = NULL;
  dja_value *child = NULL;

  for (flu_node *n = ts->first; n != NULL; n = n->next)
  {
    //printf("** %s\n", abr_tree_to_string(ts[i]));
    dja_value *v = dja_extract_value(input, (abr_tree *)n->item);
    if (first == NULL) first = v;
    if (child != NULL) child->sibling = v;
    child = v;
  }

  flu_list_free(ts);

  return first;
}

static dja_value *dja_extract_v(char *input, abr_tree *t)
{
  //printf("%s\n", abr_tree_to_string_with_leaves(input, t));

  char ty = '-';

  if (strcmp(t->name, "string") == 0) ty = 's';
  else if (strcmp(t->name, "sqstring") == 0) ty = 'q';
  else if (strcmp(t->name, "symbol") == 0) ty = 'y';
  else if (strcmp(t->name, "number") == 0) ty = 'n';
  else if (strcmp(t->name, "true") == 0) ty = 't';
  else if (strcmp(t->name, "false") == 0) ty = 'f';
  else if (strcmp(t->name, "null") == 0) ty = '0';
  else if (strcmp(t->name, "array") == 0) ty = 'a';
  else if (strcmp(t->name, "object") == 0) ty = 'o';

  if (ty == '-') return NULL;

  dja_value *v = dja_value_malloc(ty, input, t->offset, t->length);

  if (ty == 'o') v->child = dja_extract_entries(input, abr_t_child(t, 1));
  else if (ty == 'a') v->child = dja_extract_values(input, abr_t_child(t, 1));

  return v;
}

static dja_value *dja_extract_value(char *input, abr_tree *t)
{
  //printf("%s\n", abr_tree_to_string_with_leaves(input, t));

  if (t->result != 1) return NULL;

  t = abr_t_child(t, 1);

  for (abr_tree *c = t->child; c != NULL; c = c->sibling)
  {
    if (c->result == 1) return dja_extract_v(input, c);
  }

  return NULL;
}

dja_value *dja_parse(char *input)
{
  dja_parser_init();

  abr_tree *t = abr_parse_all(input, 0, dja_parser);
  // TODO: deal with errors (t->result < 0)

  //printf(">%s<\n", input);
  //puts(abr_tree_to_string_with_leaves(input, t));

  dja_value *v = dja_extract_value(input, t);
  abr_tree_free(t);

  return v;
}

static short dja_atree_is_radl(abr_tree *t)
{
  if (t->result != 1) return -1;
  return t->name && strcmp(t->name, "rad_l") == 0;
}

static dja_value *dja_parse_radl(
  char *input, abr_tree *radl, dja_value *current)
{
  printf("**\n");
  abr_tree *radi = abr_tree_lookup(radl, "rad_i");
  printf("indent: %zu\n", radi->length);
  printf("%s\n", abr_tree_to_string_with_leaves(input, radl));
  return NULL;
}

dja_value *dja_parse_radial(char *input)
{
  dja_parser_init();

  abr_tree *t = abr_parse_all(input, 0, dja_radial_parser);
  // TODO: deal with errors (t->result < 0)

  //printf(">%s<\n", input);
  //puts(abr_tree_to_string_with_leaves(input, t));

  flu_list *ls = abr_tree_list(t, dja_atree_is_radl);

  dja_value *root = NULL;
  dja_value *current = NULL;

  if (ls->size > 0) for (flu_node *n = ls->first; n != NULL; n = n->next)
  {
    current = dja_parse_radl(input, (abr_tree *)n->item, current);
    if (root == NULL) root = current;
  }

  flu_list_free(ls);
  abr_tree_free(t);

  return root;
}


//
// outputting

static void dja_to_j(flu_sbuffer *b, dja_value *v)
{
  if (v->key != NULL)
  {
    flu_sbprintf(b, "\"%s\":", v->key);
  }

  if (v->type == 'q')
  {
    char *s = dja_to_string(v);
    flu_sbprintf(b, "\"%s\"", s);
    free(s);
  }
  else if (v->type == 'y')
  {
    char *s = dja_string(v);
    flu_sbprintf(b, "\"%s\"", s);
    free(s);
  }
  else if (v->type == 'a')
  {
    flu_sbputc(b, '[');
    for (dja_value *c = v->child; c != NULL; c = c->sibling)
    {
      dja_to_j(b, c);
      if (c->sibling != NULL) flu_sbputc(b, ',');
    }
    flu_sbputc(b, ']');
  }
  else if (v->type == 'o')
  {
    flu_sbputc(b, '{');
    for (dja_value *c = v->child; c != NULL; c = c->sibling)
    {
      dja_to_j(b, c);
      if (c->sibling != NULL) flu_sbputc(b, ',');
    }
    flu_sbputc(b, '}');
  }
  else if (v->slen == 0) flu_sbputs(b, v->source + v->soff);
  else flu_sbputs_n(b, v->source + v->soff, v->slen);
}

char *dja_to_json(dja_value *v)
{
  flu_sbuffer *b = flu_sbuffer_malloc();
  dja_to_j(b, v);

  return flu_sbuffer_to_string(b);
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
  if (v->type == 's')
  {
    return flu_n_unescape(v->source + v->soff + 1, v->slen - 2);
  }
  if (v->type == 'q')
  {
    return dja_sq_unescape(v->source + v->soff + 1, v->slen - 2);
  }
  return dja_string(v);
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

size_t dja_size(dja_value *v)
{
  size_t i = 0; for (dja_value *c = v->child; ; c = c->sibling)
  {
    if (c == NULL || i == SIZE_MAX) break;
    i++;
  }
  return i;
}

dja_value *dja_value_at(dja_value *v, long n)
{
  if (n < 0)
  {
    n = dja_size(v) + n;
    if (n < 0) return NULL;
  }

  size_t i = 0; for (dja_value *c = v->child; c != NULL; c = c->sibling)
  {
    if (i++ == n) return c;
  }

  return NULL;
}

dja_value *dja_lookup(dja_value *v, const char *path)
{
  long index = atol(path);

  return dja_value_at(v, index);
}

