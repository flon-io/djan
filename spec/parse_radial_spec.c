
//
// specifying djan
//
// Sun Jun 29 10:42:47 JST 2014
//

#include "djan.h"


describe "fdja_parse_radial()"
{
  before each
  {
    fdja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) fdja_value_free(v);
  }

  it "returns NULL when failing to parse"
  {
    // another method for pointing at syntax errors?

    v = fdja_parse_radial("123");

    ensure(v == NULL);
  }

  it "parses a single line"
  {
    v = fdja_parse_radial(""
      "sequence"
    );

    ensure(v != NULL);
    ensure(v->type == 'a');

    ensure(fdja_to_json(v) ===f ""
      "[\"sequence\",{},[]]"
    );
  }

  it "parses a couple of lines"
  {
    v = fdja_parse_radial(""
      "sequence\n"
      "  participant 'bravo'"
    );

    ensure(v != NULL);
    ensure(v->type == 'a');

    ensure(fdja_to_json(v) ===f ""
      "[\"sequence\",{},["
        "[\"participant\",{\"_a\":\"bravo\"},[]]"
      "]]"
    );
  }

  it "parses a tree of lines"
  {
    v = fdja_parse_radial(""
      "sequence\n"
      "  participant 'alpha'\n"
      "  concurrence\n"
      "    participant 'bravo'\n"
      "    participant 'charly'\n"
      "  participant 'delta'"
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "[\"sequence\",{},["
        "[\"participant\",{\"_a\":\"alpha\"},[]],"
        "[\"concurrence\",{},["
          "[\"participant\",{\"_a\":\"bravo\"},[]],"
          "[\"participant\",{\"_a\":\"charly\"},[]]"
        "]],"
        "[\"participant\",{\"_a\":\"delta\"},[]]"
      "]]"
    );
  }

  it "accepts json/djan as first attribute"
  {
    v = fdja_parse_radial(""
      "iterate [\n"
      "  1 2 3 ]\n"
      "  bravo"
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "[\"iterate\",{\"_a\":[1,2,3]},["
        "[\"bravo\",{},[]]"
      "]]"
    );
  }

  it "accepts attributes"
  {
    v = fdja_parse_radial(""
      "participant charly a: 0, b: one c: true, d: [ four ]"
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "["
        "\"participant\","
        "{"
          "\"_a\":\"charly\","
          "\"a\":0,"
          "\"b\":\"one\","
          "\"c\":true,"
          "\"d\":[\"four\"]"
        "},"
        "[]"
      "]"
    );
  }

  it "accepts attributes (and some newlines)"
  {
    v = fdja_parse_radial(""
      "participant charly,\n"
      "  aa: 0,\n"
      "  bb: one,\n"
      "  cc: true,\n"
      "  dd: [ four ]"
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "["
        "\"participant\","
        "{"
          "\"_a\":\"charly\","
          "\"aa\":0,"
          "\"bb\":\"one\","
          "\"cc\":true,"
          "\"dd\":[\"four\"]"
        "},"
        "[]"
      "]"
    );
  }

  it "accepts comments in the attributes"
  {
    v = fdja_parse_radial(""
      "participant charly, # charlie\n"
      "  aa: 0, # zero\n"
      "  bb: one, # one\n"
      "  cc: true, # three\n"
      "  dd: [ four ] # four"
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "["
        "\"participant\","
        "{"
          "\"_a\":\"charly\","
          "\"aa\":0,"
          "\"bb\":\"one\","
          "\"cc\":true,"
          "\"dd\":[\"four\"]"
        "},"
        "[]"
      "]"
    );
  }

  it "accepts comments in the attributes (after the colon)"
  {
    v = fdja_parse_radial(""
      "participant charly, # charlie\n"
      "  aa:     # zero\n"
      "    0,    # zero indeed\n"
      "  bb: one # one\n"
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "["
        "\"participant\","
        "{"
          "\"_a\":\"charly\","
          "\"aa\":0,"
          "\"bb\":\"one\""
        "},"
        "[]"
      "]"
    );
  }

  it "accepts comments at the end of the radial lines"
  {
    v = fdja_parse_radial(""
      "sequence\n"
      "  participant toto # blind\n"
      "  participant tutu # deaf"
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "[\"sequence\",{},["
        "[\"participant\",{\"_a\":\"toto\"},[]],"
        "[\"participant\",{\"_a\":\"tutu\"},[]]"
      "]]"
    );
  }

  it "accepts comments before the radial lines"
  {
    v = fdja_parse_radial(""
      "# Tue Jul  8 05:50:28 JST 2014\n"
      "sequence\n"
      "  participant toto"
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "[\"sequence\",{},["
        "[\"participant\",{\"_a\":\"toto\"},[]]"
      "]]"
    );
  }
}

