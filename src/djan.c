
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

#include <stdlib.h>

#include "djan.h"


dja_value *dja_value_malloc(char type)
{
  dja_value *v = calloc(1, sizeof(dja_value));
  v->type = type;
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

dja_value *dja_parse(char *input)
{
  return dja_value_malloc('0');
}

