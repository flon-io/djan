
//
// specifying djan
//
// Sun Jun 29 10:42:47 JST 2014
//

#include "djan.h"


describe "dja_parse_radial()"
{
  before each
  {
    dja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) dja_value_free(v);
  }

  it "returns NULL when failing to parse"
  {
    // another method for pointing at syntax errors?

    v = dja_parse_radial("123");

    ensure(v == NULL);
  }

  it "parses"
  {
    v = dja_parse_radial(""
      "sequence"
    );

    ensure(v != NULL);
    ensure(v->type == 'a');

    ensure(dja_to_json(v) ===f ""
      "[\"sequence\",{},[]]"
    );
  }

  it "parses"
  {
    v = dja_parse_radial(""
      "sequence\n"
      "  participant 'bravo'"
    );

    ensure(v != NULL);
    ensure(v->type == 'a');

    ensure(dja_to_json(v) ===f ""
      "[\"sequence\",{},["
        "[\"participant\",{\"_a\":\"bravo\"},[]]"
      "]]"
    );
  }
}

