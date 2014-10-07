
//
// specifying djan
//
// Mon Jun  2 06:44:56 JST 2014
//

#include "djan.h"


context "parsing"
{
  before each
  {
    fdja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) fdja_value_free(v);
  }

  describe "fdja_parse()"
  {
    context "numbers"
    {
      it "parses \"1\""
      {
        v = fdja_parse("1");

        ensure(v != NULL);
        ensure(v->type == 'n');
        ensure(v->soff == 0);
        ensure(v->slen == 1);
        ensure(fdja_to_int(v) == 1);
        ensure(fdja_to_json(v) ===f "1");
      }
      it "parses \"-1\""
      {
        v = fdja_parse("-1");

        ensure(v != NULL);
        ensure(v->type == 'n');
        ensure(v->soff == 0);
        ensure(v->slen == 2);
        ensure(fdja_to_int(v) == -1);
        ensure(fdja_to_json(v) ===f "-1");
      }

      it "parses \"0.0\""
      {
        v = fdja_parse("0.0");

        ensure(v != NULL);
        ensure(v->type == 'n');
        ensure(v->soff == 0);
        ensure(v->slen == 3);
        ensure(fdja_to_double(v) == 0.0);
        ensure(fdja_string(v) ===f "0.0");
        ensure(fdja_to_string(v) ===f "0.0");
        ensure(fdja_to_json(v) ===f "0.0");
      }

      it "parses \"1.5e2\""
      {
        v = fdja_parse("1.5e2");

        ensure(v != NULL);
        ensure(v->type == 'n');
        ensure(v->soff == 0);
        ensure(v->slen == 5);
        ensure(fdja_to_double(v) == 150.0);
        ensure(fdja_string(v) ===f "1.5e2");
        ensure(fdja_to_string(v) ===f "1.5e2");
        ensure(fdja_to_json(v) ===f "1.5e2");
      }
    }

    context "strings"
    {
      it "parses \"hello\""
      {
        v = fdja_parse("\"hello\"");

        ensure(v != NULL);
        ensure(v->type == 's');
        ensure(fdja_string(v) ===f "\"hello\"");
        ensure(fdja_to_string(v) ===f "hello");
        ensure(fdja_to_json(v) ===f "\"hello\"");
      }
      it "parses \"hello \\\"old bore\\\"\""
      {
        v = fdja_parse("\"hello \\\"old bore\\\"\"");

        ensure(v != NULL);
        ensure(v->type == 's');
        ensure(fdja_string(v) ===f "\"hello \\\"old bore\\\"\"");
        ensure(fdja_to_string(v) ===f "hello \"old bore\"");
        ensure(fdja_to_json(v) ===f "\"hello \\\"old bore\\\"\"");
      }
      it "parses \\t"
      {
        v = fdja_parse("\"hello\\ttab\"");

        ensure(v != NULL);
        ensure(v->type == 's');
        ensure(fdja_string(v) ===f "\"hello\\ttab\"");
        ensure(fdja_to_string(v) ===f "hello\ttab");
        ensure(fdja_to_json(v) ===f "\"hello\\ttab\"");
      }
      it "parses unicode escape sequences"
      {
        v = fdja_parse("\"hello \\u0066ool\"");

        ensure(v != NULL);
        ensure(v->type == 's');
        ensure(fdja_string(v) ===f "\"hello \\u0066ool\"");
        ensure(fdja_to_string(v) ===f "hello fool");
        ensure(fdja_to_json(v) ===f "\"hello \\u0066ool\"");
      }
      it "doesn't parse unknown escapes"
      {
        v = fdja_parse("\"hello \\z\"");

        ensure(v == NULL);
      }
      context "single quotes"
      {
        it "parses single quotes strings"
        {
          v = fdja_parse("'hello \"world\"'");

          ensure(v != NULL);
          ensure(v->type == 'q');
          ensure(fdja_string(v) ===f "'hello \"world\"'");
          ensure(fdja_to_string(v) ===f "hello \"world\"");
          ensure(fdja_to_json(v) ===f "\"hello \"world\"\"");
        }
        it "lets a quote being escaped"
        {
          v = fdja_parse("'aujourd\\'hui'");

          ensure(v != NULL);
          ensure(v->type == 'q');
          ensure(fdja_string(v) ===f "'aujourd\\'hui'");
          ensure(fdja_to_string(v) ===f "aujourd'hui");
          ensure(fdja_to_json(v) ===f "\"aujourd'hui\"");
        }
      }
    }

    context "booleans"
    {
      it "parses \"true\""
      {
        v = fdja_parse("true");

        ensure(v != NULL);
        ensure(v->type == 't');
        ensure(fdja_string(v) ===f "true");
        ensure(fdja_to_int(v) == 1);
        ensure(fdja_to_json(v) ===f "true");
      }

      it "parses \"false\""
      {
        v = fdja_parse("false");

        ensure(v != NULL);
        ensure(v->type == 'f');
        ensure(fdja_string(v) ===f "false");
        ensure(fdja_to_int(v) == 0);
        ensure(fdja_to_json(v) ===f "false");
      }
    }

    context "null"
    {
      it "parses \"null\""
      {
        v = fdja_parse("null");

        ensure(v != NULL);
        ensure(v->type == '0');
        ensure(fdja_string(v) ===f "null");
        ensure(fdja_to_json(v) ===f "null");
      }
    }

    context "arrays"
    {
      it "parses []"
      {
        v = fdja_parse("[]");

        ensure(v != NULL);
        ensure(v->type == 'a');
        ensure(v->child == NULL);
        ensure(fdja_to_json(v) ===f "[]");
      }

      it "parses [ ]"
      {
        v = fdja_parse("[ ]");

        ensure(v != NULL);
        ensure(v->type == 'a');
        ensure(v->child == NULL);
        ensure(fdja_to_json(v) ===f "[]");
      }

      it "parses [1,2,3]"
      {
        v = fdja_parse("[1,2,3]");

        ensure(v != NULL);
        ensure(v->type == 'a');
        ensure(fdja_value_at(v, 0) != NULL);
        ensure(fdja_value_at(v, 3) == NULL);
        ensure(fdja_value_at(v, 0)->type == 'n');
        ensure(fdja_value_at(v, 1)->type == 'n');
        ensure(fdja_value_at(v, 2)->type == 'n');
        ensure(fdja_to_int(fdja_value_at(v, 0)) == 1);
        ensure(fdja_to_int(fdja_value_at(v, 1)) == 2);
        ensure(fdja_to_int(fdja_value_at(v, 2)) == 3);
        ensure(fdja_to_json(v) ===f "[1,2,3]");
      }

      it "parses with or without commas"
      {
        v = fdja_parse("[ 10 20,30, 40 50\t51\n52]");

        ensure(v != NULL);
        ensure(v->type == 'a');
        ensure(fdja_to_int(fdja_value_at(v, 0)) == 10);
        ensure(fdja_to_int(fdja_value_at(v, 1)) == 20);
        ensure(fdja_to_int(fdja_value_at(v, 2)) == 30);
        ensure(fdja_to_int(fdja_value_at(v, 3)) == 40);
        ensure(fdja_to_int(fdja_value_at(v, 4)) == 50);
        ensure(fdja_to_int(fdja_value_at(v, 5)) == 51);
        ensure(fdja_to_int(fdja_value_at(v, 6)) == 52);
        ensure(fdja_value_at(v, 7) == NULL);
        ensure(fdja_to_json(v) ===f "[10,20,30,40,50,51,52]");
      }

      it "parses [1,2,]"
      {
        v = fdja_parse("[1,2,]");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "[1,2]");
      }

      it "parses [1,,3]"
      {
        v = fdja_parse("[1,,3]");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "[1,3]");
      }

      it "parses [<lf>]"
      {
        v = fdja_parse("[\n]");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "[]");
      }
    }

    context "objects"
    {
      it "parses {}"
      {
        v = fdja_parse("{}");

        ensure(v != NULL);
        ensure(v->type == 'o');
        ensure(fdja_value_at(v, 0) == NULL);
        ensure(fdja_to_json(v) ===f "{}");
      }

      it "parses {\"a\":0,\"bb\":null,\"cc c\":true}"
      {
        v = fdja_parse("{\"a\":0,\"bb\":null,\"cc c\":true}");

        ensure(v != NULL);
        ensure(v->type == 'o');
        ensure(fdja_value_at(v, 0) != NULL);
        ensure(fdja_value_at(v, 1) != NULL);
        ensure(fdja_value_at(v, 2) != NULL);
        ensure(fdja_value_at(v, 3) == NULL);
        ensure(fdja_value_at(v, 0)->key === "a");
        ensure(fdja_value_at(v, 1)->key === "bb");
        ensure(fdja_value_at(v, 2)->key === "cc c");
        ensure(fdja_value_at(v, 0)->type == 'n');
        ensure(fdja_value_at(v, 1)->type == '0');
        ensure(fdja_value_at(v, 2)->type == 't');
        ensure(fdja_to_int(fdja_value_at(v, 0)) == 0);
        ensure(fdja_to_int(fdja_value_at(v, 2)) == 1);
        ensure(fdja_to_json(v) ===f "{\"a\":0,\"bb\":null,\"cc c\":true}");
      }

      it "accepts 'symbols' as keys"
      {
        v = fdja_parse("{ a_a: 0, bb_: null, c3:\"three\" }");

        ensure(v != NULL);
        ensure(v->type == 'o');
        ensure(fdja_value_at(v, 0)->key === "a_a");
        ensure(fdja_value_at(v, 1)->key === "bb_");
        ensure(fdja_value_at(v, 2)->key === "c3");
        ensure(fdja_to_int(fdja_value_at(v, 0)) == 0);
        ensure(fdja_to_string(fdja_value_at(v, 1)) ===f "null");
        ensure(fdja_to_string(fdja_value_at(v, 2)) ===f "three");
        ensure(fdja_value_at(v, 3) == NULL);
        ensure(fdja_to_json(v) ===f "{\"a_a\":0,\"bb_\":null,\"c3\":\"three\"}");
      }

      it "accepts 'single quote strings' as keys"
      {
        v = fdja_parse("{ 'a_a': 0, 'bb_': null }");

        ensure(v != NULL);
        ensure(v->type == 'o');
        ensure(fdja_value_at(v, 0)->key === "a_a");
        ensure(fdja_value_at(v, 1)->key === "bb_");
        ensure(fdja_to_int(fdja_value_at(v, 0)) == 0);
        ensure(fdja_to_string(fdja_value_at(v, 1)) ===f "null");
        ensure(fdja_value_at(v, 2) == NULL);
        ensure(fdja_to_json(v) ===f "{\"a_a\":0,\"bb_\":null}");
      }

      it "parses with or without commas"
      {
        v = fdja_parse("{ a_a: 0, bb_: null \"c\": true\nd: [ 1, 2 ] }");

        ensure(v != NULL);
        ensure(v->type == 'o');
        ensure(fdja_value_at(v, 0)->key === "a_a");
        ensure(fdja_value_at(v, 1)->key === "bb_");
        ensure(fdja_value_at(v, 2)->key === "c");
        ensure(fdja_value_at(v, 3)->key === "d");
        ensure(fdja_value_at(v, 4) == NULL);
        ensure(fdja_value_at(v, 3)->type == 'a');

        ensure(fdja_to_json(v) ===f ""
          "{\"a_a\":0,\"bb_\":null,\"c\":true,\"d\":[1,2]}");
      }

      it "parses {a:0,b:1,}"
      {
        v = fdja_parse("{a:0,b:1,}");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "{\"a\":0,\"b\":1}");
      }

      it "parses {a:0,,c:2}"
      {
        v = fdja_parse("{a:0,,c:2}");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "{\"a\":0,\"c\":2}");
      }

      it "parses {<lf>}"
      {
        v = fdja_parse("{\n}");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "{}");
      }
    }

    context "symbols"
    {
      it "accepts symbols in lieu of strings"
      {
        v = fdja_parse("sk8park");

        ensure(v != NULL);
        ensure(v->type == 'y');
        ensure(fdja_string(v) ===f "sk8park");
        ensure(fdja_to_string(v) ===f "sk8park");
        ensure(fdja_to_json(v) ===f "\"sk8park\"");
      }
      it "accepts symbols (nested)"
      {
        v = fdja_parse("[ mi6, cia, kgb c64 ]");

        ensure(v != NULL);
        ensure(v->type == 'a');

        ensure(fdja_to_json(v) ===f "[\"mi6\",\"cia\",\"kgb\",\"c64\"]");
      }
    }

    context "whitespaces"
    {
      it "accepts whitespace around values"
      {
        v = fdja_parse(" 77.0 ");

        ensure(v != NULL);
        ensure(v->type == 'n');
        ensure(fdja_to_string(v) ===f "77.0");
        ensure(fdja_to_json(v) ===f "77.0");
      }

      it "accepts whitespace inside of arrays"
      {
        v = fdja_parse(" [1, 2,\n\t3 ] ");

        ensure(v != NULL);
        ensure(v->type == 'a');
        ensure(fdja_to_int(fdja_value_at(v, 0)) == 1);
        ensure(fdja_to_int(fdja_value_at(v, 1)) == 2);
        ensure(fdja_to_int(fdja_value_at(v, 2)) == 3);
        ensure(fdja_value_at(v, 3) == NULL);
        ensure(fdja_to_json(v) ===f "[1,2,3]");
      }

      it "accepts whitespace inside of objects"
      {
        v = fdja_parse("\n{\n\"a\": 0, \"bb\": null, \"cc c\": true\n}\n");

        ensure(v != NULL);
        ensure(v->type == 'o');
        ensure(fdja_value_at(v, 0) != NULL);
        ensure(fdja_value_at(v, 1) != NULL);
        ensure(fdja_value_at(v, 2) != NULL);
        ensure(fdja_value_at(v, 3) == NULL);
        ensure(fdja_value_at(v, 0)->key === "a");
        ensure(fdja_value_at(v, 1)->key === "bb");
        ensure(fdja_value_at(v, 2)->key === "cc c");
        ensure(fdja_value_at(v, 0)->type == 'n');
        ensure(fdja_value_at(v, 1)->type == '0');
        ensure(fdja_value_at(v, 2)->type == 't');
        ensure(fdja_to_int(fdja_value_at(v, 0)) == 0);
        ensure(fdja_to_int(fdja_value_at(v, 2)) == 1);
        ensure(fdja_to_json(v) ===f "{\"a\":0,\"bb\":null,\"cc c\":true}");
      }

      it "accepts comments at the end of lines"
      {
        v = fdja_parse(""
          "{\n"
          "  \"a\": 0,      # alpha delta\n"
          "  \"bb\": null,  # bravo johnny\n"
          "  \"cc c\": true # charly\n"
          "} # over"
        );

        ensure(v != NULL);
        ensure(fdja_to_json(v) ===f "{\"a\":0,\"bb\":null,\"cc c\":true}");
      }
    }
  }

  describe "fdja_parse_f()"
  {
    it "reads JSON from a file"
    {
      v = fdja_parse_f("../spec/_test1.json");

      expect(fdja_lookup_int(v, "id", -1) == 12345);
      expect(fdja_lookup_string(v, "type", NULL) ===f "test");
    }

    it "composes the file name"
    {
      v = fdja_parse_f("../spec/_test%i.json", 1);

      expect(fdja_lookup_int(v, "id", -1) == 12345);
      expect(fdja_lookup_string(v, "type", NULL) ===f "test");
    }
  }
}

