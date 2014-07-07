
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

  context "numbers"
  {
    it "parses \"1\""
    {
      v = dja_parse("1");

      ensure(v != NULL);
      ensure(v->type == 'n');
      ensure(v->soff == 0);
      ensure(v->slen == 1);
      ensure(dja_to_int(v) == 1);
      ensure(dja_to_json(v) ===f "1");
    }
    it "parses \"-1\""
    {
      v = dja_parse("-1");

      ensure(v != NULL);
      ensure(v->type == 'n');
      ensure(v->soff == 0);
      ensure(v->slen == 2);
      ensure(dja_to_int(v) == -1);
      ensure(dja_to_json(v) ===f "-1");
    }

    it "parses \"0.0\""
    {
      v = dja_parse("0.0");

      ensure(v != NULL);
      ensure(v->type == 'n');
      ensure(v->soff == 0);
      ensure(v->slen == 3);
      ensure(dja_to_double(v) == 0.0);
      ensure(dja_string(v) ===f "0.0");
      ensure(dja_to_string(v) ===f "0.0");
      ensure(dja_to_json(v) ===f "0.0");
    }

    it "parses \"1.5e2\""
    {
      v = dja_parse("1.5e2");

      ensure(v != NULL);
      ensure(v->type == 'n');
      ensure(v->soff == 0);
      ensure(v->slen == 5);
      ensure(dja_to_double(v) == 150.0);
      ensure(dja_string(v) ===f "1.5e2");
      ensure(dja_to_string(v) ===f "1.5e2");
      ensure(dja_to_json(v) ===f "1.5e2");
    }
  }

  context "strings"
  {
    it "parses \"hello\""
    {
      v = dja_parse("\"hello\"");

      ensure(v != NULL);
      ensure(v->type == 's');
      ensure(dja_string(v) ===f "\"hello\"");
      ensure(dja_to_string(v) ===f "hello");
      ensure(dja_to_json(v) ===f "\"hello\"");
    }
    it "parses \"hello \\\"old bore\\\"\""
    {
      v = dja_parse("\"hello \\\"old bore\\\"\"");

      ensure(v != NULL);
      ensure(v->type == 's');
      ensure(dja_string(v) ===f "\"hello \\\"old bore\\\"\"");
      ensure(dja_to_string(v) ===f "hello \"old bore\"");
      ensure(dja_to_json(v) ===f "\"hello \\\"old bore\\\"\"");
    }
    it "parses \\t"
    {
      v = dja_parse("\"hello\\ttab\"");

      ensure(v != NULL);
      ensure(v->type == 's');
      ensure(dja_string(v) ===f "\"hello\\ttab\"");
      ensure(dja_to_string(v) ===f "hello\ttab");
      ensure(dja_to_json(v) ===f "\"hello\\ttab\"");
    }
    it "parses unicode escape sequences"
    {
      v = dja_parse("\"hello \\u0066ool\"");

      ensure(v != NULL);
      ensure(v->type == 's');
      ensure(dja_string(v) ===f "\"hello \\u0066ool\"");
      ensure(dja_to_string(v) ===f "hello fool");
      ensure(dja_to_json(v) ===f "\"hello \\u0066ool\"");
    }
    it "doesn't parse unknown escapes"
    {
      v = dja_parse("\"hello \\z\"");

      ensure(v == NULL);
    }
    context "single quotes"
    {
      it "parses single quotes strings"
      {
        v = dja_parse("'hello \"world\"'");

        ensure(v != NULL);
        ensure(v->type == 'q');
        ensure(dja_string(v) ===f "'hello \"world\"'");
        ensure(dja_to_string(v) ===f "hello \"world\"");
        ensure(dja_to_json(v) ===f "\"hello \"world\"\"");
      }
      it "lets a quote being escaped"
      {
        v = dja_parse("'aujourd\\'hui'");

        ensure(v != NULL);
        ensure(v->type == 'q');
        ensure(dja_string(v) ===f "'aujourd\\'hui'");
        ensure(dja_to_string(v) ===f "aujourd'hui");
        ensure(dja_to_json(v) ===f "\"aujourd'hui\"");
      }
    }
  }

  context "booleans"
  {
    it "parses \"true\""
    {
      v = dja_parse("true");

      ensure(v != NULL);
      ensure(v->type == 't');
      ensure(dja_string(v) ===f "true");
      ensure(dja_to_int(v) == 1);
      ensure(dja_to_json(v) ===f "true");
    }

    it "parses \"false\""
    {
      v = dja_parse("false");

      ensure(v != NULL);
      ensure(v->type == 'f');
      ensure(dja_string(v) ===f "false");
      ensure(dja_to_int(v) == 0);
      ensure(dja_to_json(v) ===f "false");
    }
  }

  context "null"
  {
    it "parses \"null\""
    {
      v = dja_parse("null");

      ensure(v != NULL);
      ensure(v->type == '0');
      ensure(dja_string(v) ===f "null");
      ensure(dja_to_json(v) ===f "null");
    }
  }

  context "arrays"
  {
    it "parses []"
    {
      v = dja_parse("[]");

      ensure(v != NULL);
      ensure(v->type == 'a');
      ensure(v->child == NULL);
      ensure(dja_to_json(v) ===f "[]");
    }

    it "parses [1,2,3]"
    {
      v = dja_parse("[1,2,3]");

      ensure(v != NULL);
      ensure(v->type == 'a');
      ensure(dja_value_at(v, 0) != NULL);
      ensure(dja_value_at(v, 3) == NULL);
      ensure(dja_value_at(v, 0)->type == 'n');
      ensure(dja_value_at(v, 1)->type == 'n');
      ensure(dja_value_at(v, 2)->type == 'n');
      ensure(dja_to_int(dja_value_at(v, 0)) == 1);
      ensure(dja_to_int(dja_value_at(v, 1)) == 2);
      ensure(dja_to_int(dja_value_at(v, 2)) == 3);
      ensure(dja_to_json(v) ===f "[1,2,3]");
    }

    it "parses with or without commas"
    {
      v = dja_parse("[ 10 20,30, 40 50\t51\n52]");

      ensure(v != NULL);
      ensure(v->type == 'a');
      ensure(dja_to_int(dja_value_at(v, 0)) == 10);
      ensure(dja_to_int(dja_value_at(v, 1)) == 20);
      ensure(dja_to_int(dja_value_at(v, 2)) == 30);
      ensure(dja_to_int(dja_value_at(v, 3)) == 40);
      ensure(dja_to_int(dja_value_at(v, 4)) == 50);
      ensure(dja_to_int(dja_value_at(v, 5)) == 51);
      ensure(dja_to_int(dja_value_at(v, 6)) == 52);
      ensure(dja_value_at(v, 7) == NULL);
      ensure(dja_to_json(v) ===f "[10,20,30,40,50,51,52]");
    }
  }

  context "objects"
  {
    it "parses {}"
    {
      v = dja_parse("{}");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(dja_value_at(v, 0) == NULL);
      ensure(dja_to_json(v) ===f "{}");
    }

    it "parses {\"a\":0,\"bb\":null,\"cc c\":true}"
    {
      v = dja_parse("{\"a\":0,\"bb\":null,\"cc c\":true}");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(dja_value_at(v, 0) != NULL);
      ensure(dja_value_at(v, 1) != NULL);
      ensure(dja_value_at(v, 2) != NULL);
      ensure(dja_value_at(v, 3) == NULL);
      ensure(dja_value_at(v, 0)->key === "a");
      ensure(dja_value_at(v, 1)->key === "bb");
      ensure(dja_value_at(v, 2)->key === "cc c");
      ensure(dja_value_at(v, 0)->type == 'n');
      ensure(dja_value_at(v, 1)->type == '0');
      ensure(dja_value_at(v, 2)->type == 't');
      ensure(dja_to_int(dja_value_at(v, 0)) == 0);
      ensure(dja_to_int(dja_value_at(v, 2)) == 1);
      ensure(dja_to_json(v) ===f "{\"a\":0,\"bb\":null,\"cc c\":true}");
    }

    it "accepts 'symbols' as keys"
    {
      v = dja_parse("{ a_a: 0, bb_: null, c3:\"three\" }");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(dja_value_at(v, 0)->key === "a_a");
      ensure(dja_value_at(v, 1)->key === "bb_");
      ensure(dja_value_at(v, 2)->key === "c3");
      ensure(dja_to_int(dja_value_at(v, 0)) == 0);
      ensure(dja_to_string(dja_value_at(v, 1)) ===f "null");
      ensure(dja_to_string(dja_value_at(v, 2)) ===f "three");
      ensure(dja_value_at(v, 3) == NULL);
      ensure(dja_to_json(v) ===f "{\"a_a\":0,\"bb_\":null,\"c3\":\"three\"}");
    }
    it "accepts 'single quote strings' as keys"
    {
      v = dja_parse("{ 'a_a': 0, 'bb_': null }");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(dja_value_at(v, 0)->key === "a_a");
      ensure(dja_value_at(v, 1)->key === "bb_");
      ensure(dja_to_int(dja_value_at(v, 0)) == 0);
      ensure(dja_to_string(dja_value_at(v, 1)) ===f "null");
      ensure(dja_value_at(v, 2) == NULL);
      ensure(dja_to_json(v) ===f "{\"a_a\":0,\"bb_\":null}");
    }

    it "parses with or without commas"
    {
      v = dja_parse("{ a_a: 0, bb_: null \"c\": true\nd: [ 1, 2 ] }");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(dja_value_at(v, 0)->key === "a_a");
      ensure(dja_value_at(v, 1)->key === "bb_");
      ensure(dja_value_at(v, 2)->key === "c");
      ensure(dja_value_at(v, 3)->key === "d");
      ensure(dja_value_at(v, 4) == NULL);
      ensure(dja_value_at(v, 3)->type == 'a');

      ensure(dja_to_json(v) ===f ""
        "{\"a_a\":0,\"bb_\":null,\"c\":true,\"d\":[1,2]}");
    }
  }

  context "symbols"
  {
    it "accepts symbols in lieu of strings"
    {
      v = dja_parse("sk8park");

      ensure(v != NULL);
      ensure(v->type == 'y');
      ensure(dja_string(v) ===f "sk8park");
      ensure(dja_to_string(v) ===f "sk8park");
      ensure(dja_to_json(v) ===f "\"sk8park\"");
    }
    it "accepts symbols (nested)"
    {
      v = dja_parse("[ mi6, cia, kgb c64 ]");

      ensure(v != NULL);
      ensure(v->type == 'a');

      ensure(dja_to_json(v) ===f "[\"mi6\",\"cia\",\"kgb\",\"c64\"]");
    }
  }

  context "whitespaces"
  {
    it "accepts whitespace around values"
    {
      v = dja_parse(" 77.0 ");

      ensure(v != NULL);
      ensure(v->type == 'n');
      ensure(dja_to_string(v) ===f "77.0");
      ensure(dja_to_json(v) ===f "77.0");
    }

    it "accepts whitespace inside of arrays"
    {
      v = dja_parse(" [1, 2,\n\t3 ] ");

      ensure(v != NULL);
      ensure(v->type == 'a');
      ensure(dja_to_int(dja_value_at(v, 0)) == 1);
      ensure(dja_to_int(dja_value_at(v, 1)) == 2);
      ensure(dja_to_int(dja_value_at(v, 2)) == 3);
      ensure(dja_value_at(v, 3) == NULL);
      ensure(dja_to_json(v) ===f "[1,2,3]");
    }

    it "accepts whitespace inside of objects"
    {
      v = dja_parse("\n{\n\"a\": 0, \"bb\": null, \"cc c\": true\n}\n");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(dja_value_at(v, 0) != NULL);
      ensure(dja_value_at(v, 1) != NULL);
      ensure(dja_value_at(v, 2) != NULL);
      ensure(dja_value_at(v, 3) == NULL);
      ensure(dja_value_at(v, 0)->key === "a");
      ensure(dja_value_at(v, 1)->key === "bb");
      ensure(dja_value_at(v, 2)->key === "cc c");
      ensure(dja_value_at(v, 0)->type == 'n');
      ensure(dja_value_at(v, 1)->type == '0');
      ensure(dja_value_at(v, 2)->type == 't');
      ensure(dja_to_int(dja_value_at(v, 0)) == 0);
      ensure(dja_to_int(dja_value_at(v, 2)) == 1);
      ensure(dja_to_json(v) ===f "{\"a\":0,\"bb\":null,\"cc c\":true}");
    }

    it "accepts comments at the end of lines"
    {
      v = dja_parse(""
        "{\n"
        "  \"a\": 0,      # alpha delta\n"
        "  \"bb\": null,  # bravo johnny\n"
        "  \"cc c\": true # charly\n"
        "} # over"
      );

      ensure(v != NULL);
      ensure(dja_to_json(v) ===f "{\"a\":0,\"bb\":null,\"cc c\":true}");
    }
  }
}

