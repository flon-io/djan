
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
  it "parses floats"
  it "parses e numbers"

  it "parses strings"

  it "parses true"
  it "parses false"

  it "parses null"
  {
    v = dja_parse("null");

    ensure(v->type == '0');
  }
}

