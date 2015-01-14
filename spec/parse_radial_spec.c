
//
// specifying djan
//
// Sun Jun 29 10:42:47 JST 2014
//

#include "djan.h"


context "parsing radial"
{
  before each
  {
    fdja_value *v = NULL;
  }
  after each
  {
    fdja_value_free(v);
  }

  describe "fdja_parse_radial()"
  {
    it "returns NULL when failing to parse"
    {
      // another method for pointing at syntax errors?

      v = fdja_parse_radial("12[3");

      ensure(v == NULL);
    }

    it "parses a single line"
    {
      v = fdja_dparse_radial(
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
      v = fdja_dparse_radial(
        "sequence\n"
        "  participant 'bravo'"
      );

      ensure(v != NULL);
      ensure(v->type == 'a');

      ensure(fdja_to_json(v) ===f ""
        "[\"sequence\",{},["
          "[\"participant\",{\"_0\":\"bravo\"},[]]"
        "]]"
      );
    }

    it "parses a tree of lines"
    {
      v = fdja_dparse_radial(
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
          "[\"participant\",{\"_0\":\"alpha\"},[]],"
          "[\"concurrence\",{},["
            "[\"participant\",{\"_0\":\"bravo\"},[]],"
            "[\"participant\",{\"_0\":\"charly\"},[]]"
          "]],"
          "[\"participant\",{\"_0\":\"delta\"},[]]"
        "]]"
      );
    }

    it "accepts json/djan as first attribute"
    {
      v = fdja_dparse_radial(
        "iterate [\n"
        "  1 2 3 ]\n"
        "  bravo"
      );

      ensure(v != NULL);

      ensure(fdja_to_json(v) ===f ""
        "[\"iterate\",{\"_0\":[1,2,3]},["
          "[\"bravo\",{},[]]"
        "]]"
      );
    }

    it "accepts attributes"
    {
      v = fdja_dparse_radial(
        "participant charly a: 0, b: one c: true, d: [ four ]"
      );

      ensure(v != NULL);

      ensure(fdja_to_json(v) ===f ""
        "["
          "\"participant\","
          "{"
            "\"_0\":\"charly\","
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
      v = fdja_dparse_radial(
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
            "\"_0\":\"charly\","
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
      v = fdja_dparse_radial(
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
            "\"_0\":\"charly\","
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
      v = fdja_dparse_radial(
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
            "\"_0\":\"charly\","
            "\"aa\":0,"
            "\"bb\":\"one\""
          "},"
          "[]"
        "]"
      );
    }

    it "accepts unkeyed attributes"
    {
      v = fdja_dparse_radial("nada aa bb d: 2, e: 3");

      ensure(v != NULL);

      ensure(fdja_to_json(v) ===f ""
        "[\"nada\",{\"_0\":\"aa\",\"_1\":\"bb\",\"d\":2,\"e\":3},[]]");
    }

    it "preserves the attribute order"
    {
      v = fdja_dparse_radial("nada d: 0 e: 1 aa bb");

      ensure(v != NULL);

      ensure(fdja_tod(v) ===f ""
        "[ nada, { d: 0, e: 1, _2: aa, _3: bb }, [] ]");
    }

    it "accepts comments at the end of the radial lines"
    {
      v = fdja_dparse_radial(
        "sequence\n"
        "  participant toto # blind\n"
        "  participant tutu # deaf"
      );

      ensure(v != NULL);

      ensure(fdja_to_json(v) ===f ""
        "[\"sequence\",{},["
          "[\"participant\",{\"_0\":\"toto\"},[]],"
          "[\"participant\",{\"_0\":\"tutu\"},[]]"
        "]]"
      );
    }

    it "accepts comments before the radial lines"
    {
      v = fdja_dparse_radial(
        "# Tue Jul  8 05:50:28 JST 2014\n"
        "sequence\n"
        "  participant toto"
      );

      ensure(v != NULL);

      ensure(fdja_to_json(v) ===f ""
        "[\"sequence\",{},["
          "[\"participant\",{\"_0\":\"toto\"},[]]"
        "]]"
      );
    }

    it "accepts words as rad names"
    {
      v = fdja_dparse_radial(
        "sequence\n"
        "  git://github.com/flon-io/tst x b: 0\n"
        //"  git://github.com/flon-io/tst x y a: 0, b: 1\n"
        //"  git://github.com/flon-io/tst a: 0, b: 1\n"
      );

      ensure(v != NULL);

      ensure(fdja_to_json(v) ===f ""
        "[\"sequence\",{},["
          "[\"git://github.com/flon-io/tst\",{\"_0\":\"x\",\"b\":0},[]]"
        "]]"
      );
    }

    it "accepts single values as radial lines"
    {
      v = fdja_dparse_radial(
        "sequence\n"
        "  3\n"
        "  null\n"
        "  quatre\n"
      );

      ensure(v != NULL);

      ensure(fdja_tod(v) ===f ""
        "[ sequence, {}, ["
          " 3, null, [ quatre, {}, [] ] "
        "] ]"
      );
    }

    context "and () groups"
    {
      it "reads 'p' groups"
      {
        v = fdja_dparse_radial(
          "if (a > b)\n"
        );

        ensure(v != NULL);

        ensure(fdja_tod(v) ===f ""
          "[ if, {}, [] ]"
        );
      }
    }
  }

  describe "fdja_parse_radial_f()"
  {
    it "reads files"
    {
      v = fdja_parse_radial_f("../spec/_test2.rad");

      //puts(fdja_to_json(v));

      //expect(v->source != NULL);
      expect(v->slen == 0);
      expect(fdja_lookup_string(v, "0", NULL) ===f "define");
      expect(fdja_lookup_string(v, "2.0.0", NULL) ===f "mail");
      expect(fdja_lookup_string(v, "2.0.1.to", NULL) ===f "$(user.email)");
    }

    it "composes filenames"
    {
      v = fdja_parse_radial_f("../spec/_%s2.rad", "test");

      expect(fdja_lookup_string(v, "2.0.1.to", NULL) ===f "$(user.email)");
    }

    it "returns NULL when it fails to read"
    {
      v = fdja_parse_radial_f("../spec/_test3.bad");

      expect(v == NULL);
    }
  }

  describe "fdja_fparse_radial()"
  {
    it "reads files"
    {
      FILE *f = fopen("../spec/_test2.rad", "r");
      v = fdja_fparse_radial(f);

      //puts(fdja_to_json(v));

      expect(v != NULL);
      expect(v->slen == 0);
      expect(fdja_lookup_string(v, "0", NULL) ===f "define");
      expect(fdja_lookup_string(v, "2.0.0", NULL) ===f "mail");
      expect(fdja_lookup_string(v, "2.0.1.to", NULL) ===f "$(user.email)");

      fclose(f);
    }
}

