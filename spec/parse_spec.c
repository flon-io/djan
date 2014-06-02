
//
// specifying djan
//
// Mon Jun  2 06:44:56 JST 2014
//

#include "djan.h"


describe "dja_parse()"
{
  before each
  {
    dja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) dja_value_free(v);
  }

  it "parses integers"
  {
    v = dja_parse(strdup("1"));

    ensure(v != NULL);
    ensure(v->type == 'n');
    ensure(dja_to_int(v) == 1);
  }

  it "parses floats"
  it "parses e numbers"

  it "parses strings"
  {
    v = dja_parse(strdup("\"hello\""));

    ensure(v != NULL);
    ensure(v->type == 's');
    ensure(dja_to_string(v) ===f "hello");
  }

  it "parses true"
  {
    v = dja_parse(strdup("true"));

    ensure(v != NULL);
    ensure(v->type == 't');
    ensure(dja_to_int(v) == 1);
  }

  it "parses false"
  {
    v = dja_parse(strdup("false"));

    ensure(v != NULL);
    ensure(v->type == 'f');
    ensure(dja_to_int(v) == 0);
  }

  it "parses null"
  {
    v = dja_parse(strdup("null"));

    ensure(v != NULL);
    ensure(v->type == '0');
  }

  context "arrays"
  {
    it "parses []"
  }

  context "objects"
  {
    it "parses {}"
  }
}

