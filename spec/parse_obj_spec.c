
//
// specifying djan
//
// Sun Sep 28 17:44:25 JST 2014
//

#include "flutil.h"
#include "djan.h"


context "parsing obj"
{
  before each
  {
    fdja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) fdja_value_free(v);
  }

  describe "fdja_parse_obj()"
  {
    it "returns NULL when failing to parse"
    {
      v = fdja_parse_obj("123");

      ensure(v == NULL);
    }

    it "parses"
    {
      v = fdja_dparse_obj(""
        "type: car\n"
        "make: honda\n"
        "parts: [ 123, 456, 789, whiskey ]\n"
        "details: { name: beetle, year: 1969 }"
      );

      ensure(v != NULL);

      ensure(fdja_to_json(v) ===f ""
        "{"
          "\"type\":\"car\","
          "\"make\":\"honda\","
          "\"parts\":[123,456,789,\"whiskey\"],"
          "\"details\":{\"name\":\"beetle\",\"year\":1969}"
        "}"
      );
    }

    it "parses with brackets"
    {
      v = fdja_dparse_obj(""
        "{\n"
        "  type: car\n"
        "  make: honda\n"
        "  parts: [ 123, 456, 789, whiskey ]\n"
        "  details: { name: beetle, year: 1969 }"
        "}"
      );

      ensure(v != NULL);

      ensure(fdja_to_json(v) ===f ""
        "{"
          "\"type\":\"car\","
          "\"make\":\"honda\","
          "\"parts\":[123,456,789,\"whiskey\"],"
          "\"details\":{\"name\":\"beetle\",\"year\":1969}"
        "}"
      );
    }

    it "parses with comments"
    {
      v = fdja_dparse_obj(""
        "#\n"
        "# that conf\n"
        "# Sun Sep 28 19:50:14 JST 2014\n"
        "#\n"
        "  # nada\n"
        //"{\n"
        "  type: car\n"
        "  make: honda\n"
        "  parts: [ 123, 456, 789, whiskey ]\n"
        "  details: { name: beetle, year: 1969 }"
        //"}"
        "# over."
      );

      ensure(v != NULL);

      ensure(fdja_to_json(v) ===f ""
        "{"
          "\"type\":\"car\","
          "\"make\":\"honda\","
          "\"parts\":[123,456,789,\"whiskey\"],"
          "\"details\":{\"name\":\"beetle\",\"year\":1969}"
        "}"
      );
    }

    it "parses json objects"
    {
      v = fdja_dparse_obj("{\"name\":\"cinderella\",\"age\":21}");

      expect(v != NULL);

      expect(fdja_to_json(v) ===f "{\"name\":\"cinderella\",\"age\":21}");
    }

    it "parses"
    {
      v = fdja_dparse_obj(
        "execute: [ invoke, { _0: stamp, color: blue }, [] ]\n"
        "id: 20141015.1320.fujutseli\n"
        "payload: {\n"
        "hello: world\n"
        "}\n"
      );

      expect(v != NULL);

      //puts(fdja_to_json(v));
      expect(fdja_lookup_string(v, "id", NULL) ===f "20141015.1320.fujutseli");
    }

    it "doesn't mind empty lines before the obj"
    {
      v = fdja_dparse_obj("\n{ hello: world }");

      expect(v != NULL);
      expect(fdja_tod(v) ===f "{ hello: world }");
    }

    it "doesn't mind empty lines before the obj (no brackets)"
    {
      v = fdja_dparse_obj("\n\n\nhello: world");

      expect(v != NULL);
      expect(fdja_tod(v) ===f "{ hello: world }");
    }

    it "doesn't mind empty lines after the obj"
    {
      v = fdja_dparse_obj("{ hello: world }\n");

      expect(v != NULL);
      expect(fdja_tod(v) ===f "{ hello: world }");
    }

    it "doesn't mind empty lines in the obj"
    {
      v = fdja_dparse_obj("{ hello:\n \n world }");

      expect(v != NULL);
      expect(fdja_tod(v) ===f "{ hello: world }");
    }
  }

  describe "fdja_parse_obj_f()"
  {
    it "loads json directly out of files"
    {
      v = fdja_parse_obj_f("../spec/_test0.jon");

      expect(v != NULL);
      expect(v->slen > 0);
      expect(fdja_to_json(v) ===f "{\"hello\":\"world\"}");

      //free(v->source);
    }

    it "composes the filename"
    {
      v = fdja_parse_obj_f("../spec/_test%i.jon", 0);

      expect(fdja_to_json(v) ===f "{\"hello\":\"world\"}");
    }

    it "returns NULL when it doesn't find the file"
    {
      v = fdja_parse_obj_f("_nada_.json");

      expect(v == NULL);
    }

    it "returns NULL when it fails to read"
    {
      v = fdja_parse_obj_f("../spec/_test3.bad");

      expect(v == NULL);
    }

    it "parses fine (gh-9)"
    {
      flu_writeall(
        "../spec/_gh9.json",
        "{\n"
        "  execute:\n"
        "    [ sequence, {}, [\n"
        "      [ invoke, { _0: stamp, color: blue }, [] ]\n"
        "      [ invoke, { _0: stamp, color: green }, [] ]\n"
        "      [ invoke, { _0: stamp, color: red }, [] ]\n"
        "      #[ invoke, { _0: sgmail, subjet: lila }, [] ]\n"
        "    ] ]\n"
        "  exid: launch.rb-u0-20141104.0729.lotichumeba\n"
        "  payload: {\n"
        "    hello: world\n"
        "  }\n"
        "}\n");

      v = fdja_parse_obj_f("../spec/_gh9.json");

      expect(v != NULL);
      expect(fdja_ls(v, "payload.hello", NULL) ===f "world");

      //flu_putf(fdja_todc(v));
    }
  }

  describe "fdja_fparse_obj()"
  {
    it "parses files"
    {
      FILE *f = fopen("../spec/_test0.jon", "r");
      v = fdja_fparse_obj(f);

      expect(v != NULL);
      expect(v->slen > 0);
      expect(fdja_to_json(v) ===f "{\"hello\":\"world\"}");

      fclose(f);
    }
  }
}

