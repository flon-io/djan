
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
    }
    it "parses \"-1\""
    {
      v = dja_parse("-1");

      ensure(v != NULL);
      ensure(v->type == 'n');
      ensure(v->soff == 0);
      ensure(v->slen == 2);
      ensure(dja_to_int(v) == -1);
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
    }
    it "parses \"hello \\\"old bore\\\"\""
    {
      v = dja_parse("\"hello \\\"old bore\\\"\"");

      ensure(v != NULL);
      ensure(v->type == 's');
      ensure(dja_string(v) ===f "\"hello \\\"old bore\\\"\"");
      ensure(dja_to_string(v) ===f "hello \"old bore\"");
    }
    it "parses \\t"
    {
      v = dja_parse("\"hello\\ttab\"");

      ensure(v != NULL);
      ensure(v->type == 's');
      ensure(dja_string(v) ===f "\"hello\\ttab\"");
      ensure(dja_to_string(v) ===f "hello\ttab");
    }
    it "parses unicode escape sequences"
    {
      v = dja_parse("\"hello \\u0066ool\"");

      ensure(v != NULL);
      ensure(v->type == 's');
      ensure(dja_string(v) ===f "\"hello \\u0066ool\"");
      ensure(dja_to_string(v) ===f "hello fool");
    }
    it "doesn't parse unknown escapes"
    {
      v = dja_parse("\"hello \\z\"");

      ensure(v == NULL);
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
    }

    it "parses \"false\""
    {
      v = dja_parse("false");

      ensure(v != NULL);
      ensure(v->type == 'f');
      ensure(dja_string(v) ===f "false");
      ensure(dja_to_int(v) == 0);
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
    }
  }

  context "arrays"
  {
    it "parses []"
    {
      v = dja_parse("[]");

      ensure(v != NULL);
      ensure(v->type == 'a');
      ensure(v->children[0] == NULL);
    }

    it "parses [1,2,3]"
    {
      v = dja_parse("[1,2,3]");

      ensure(v != NULL);
      ensure(v->type == 'a');
      ensure(v->children[0] != NULL);
      ensure(v->children[3] == NULL);
      ensure(v->children[0]->type == 'n');
      ensure(v->children[1]->type == 'n');
      ensure(v->children[2]->type == 'n');
      ensure(dja_to_int(v->children[0]) == 1);
      ensure(dja_to_int(v->children[1]) == 2);
      ensure(dja_to_int(v->children[2]) == 3);
    }

    it "parses with or without commas"
    {
      v = dja_parse("[ 10 20,30, 40 50\t51\n52]");

      ensure(v != NULL);
      ensure(v->type == 'a');
      ensure(dja_to_int(v->children[0]) == 10);
      ensure(dja_to_int(v->children[1]) == 20);
      ensure(dja_to_int(v->children[2]) == 30);
      ensure(dja_to_int(v->children[3]) == 40);
      ensure(dja_to_int(v->children[4]) == 50);
      ensure(dja_to_int(v->children[5]) == 51);
      ensure(dja_to_int(v->children[6]) == 52);
      ensure(v->children[7] == NULL);
    }
  }

  context "objects"
  {
    it "parses {}"
    {
      v = dja_parse("{}");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(v->children[0] == NULL);
    }

    it "parses {\"a\":0,\"bb\":null,\"cc c\":true}"
    {
      v = dja_parse("{\"a\":0,\"bb\":null,\"cc c\":true}");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(v->children[0] != NULL);
      ensure(v->children[1] != NULL);
      ensure(v->children[2] != NULL);
      ensure(v->children[3] == NULL);
      ensure(v->children[0]->key === "a");
      ensure(v->children[1]->key === "bb");
      ensure(v->children[2]->key === "cc c");
      ensure(v->children[0]->type == 'n');
      ensure(v->children[1]->type == '0');
      ensure(v->children[2]->type == 't');
      ensure(dja_to_int(v->children[0]) == 0);
      ensure(dja_to_int(v->children[2]) == 1);
    }

    it "accepts 'symbols' as keys"
    {
      v = dja_parse("{ a_a: 0, bb_: null }");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(v->children[0]->key === "a_a");
      ensure(v->children[1]->key === "bb_");
      ensure(dja_to_int(v->children[0]) == 0);
      ensure(dja_to_string(v->children[1]) ===f "null");
      ensure(v->children[2] == NULL);
    }

    it "parses with or without commas"
    {
      v = dja_parse("{ a_a: 0, bb_: null \"c\": true\nd: [ 1, 2 ] }");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(v->children[0]->key === "a_a");
      ensure(v->children[1]->key === "bb_");
      ensure(v->children[2]->key === "c");
      ensure(v->children[3]->key === "d");
      ensure(v->children[4] == NULL);
      ensure(v->children[3]->type == 'a');
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
    }
    it "accepts whitespace inside of arrays"
    {
      v = dja_parse(" [1, 2,\n\t3 ] ");

      ensure(v != NULL);
      ensure(v->type == 'a');
      ensure(dja_to_int(v->children[0]) == 1);
      ensure(dja_to_int(v->children[1]) == 2);
      ensure(dja_to_int(v->children[2]) == 3);
      ensure(v->children[3] == NULL);
    }
    it "accepts whitespace inside of objects"
    {
      v = dja_parse("\n{\n\"a\": 0, \"bb\": null, \"cc c\": true\n}\n");

      ensure(v != NULL);
      ensure(v->type == 'o');
      ensure(v->children[0] != NULL);
      ensure(v->children[1] != NULL);
      ensure(v->children[2] != NULL);
      ensure(v->children[3] == NULL);
      ensure(v->children[0]->key === "a");
      ensure(v->children[1]->key === "bb");
      ensure(v->children[2]->key === "cc c");
      ensure(v->children[0]->type == 'n');
      ensure(v->children[1]->type == '0');
      ensure(v->children[2]->type == 't');
      ensure(dja_to_int(v->children[0]) == 0);
      ensure(dja_to_int(v->children[2]) == 1);
    }
  }
}

