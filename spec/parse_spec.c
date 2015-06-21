
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
    fdja_value_free(v);
  }

  describe "fdja_parse()"
  {
    context "numbers"
    {
      it "parses \"1\""
      {
        v = fdja_dparse("1");

        ensure(v != NULL);
        ensure(v->type == 'n');
        ensure(v->soff == 0);
        ensure(v->slen > 0);
        ensure(fdja_to_int(v) == 1);
        ensure(fdja_to_json(v) ===f "1");
      }

      it "parses \"-1\""
      {
        v = fdja_dparse("-1");

        ensure(v != NULL);
        ensure(v->type == 'n');
        ensure(v->soff == 0);
        ensure(v->slen > 0);
        ensure(fdja_to_int(v) == -1);
        ensure(fdja_to_json(v) ===f "-1");
      }

      it "parses \"0.0\""
      {
        v = fdja_dparse("0.0");

        ensure(v != NULL);
        ensure(v->type == 'n');
        ensure(v->soff == 0);
        ensure(v->slen > 0);
        ensure(fdja_to_double(v) == 0.0);
        ensure(fdja_string(v) ===f "0.0");
        ensure(fdja_to_string(v) ===f "0.0");
        ensure(fdja_to_json(v) ===f "0.0");
      }

      it "parses \"1.5e2\""
      {
        v = fdja_dparse("1.5e2");

        ensure(v != NULL);
        ensure(v->type == 'n');
        ensure(v->soff == 0);
        ensure(v->slen > 0);
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
        v = fdja_dparse("\"hello\"");

        ensure(v != NULL);
        ensure(v->type == 's');
        ensure(fdja_string(v) ===f "\"hello\"");
        ensure(fdja_to_string(v) ===f "hello");
        ensure(fdja_to_json(v) ===f "\"hello\"");
      }

      it "parses \"hello \\\"old bore\\\"\""
      {
        v = fdja_dparse("\"hello \\\"old bore\\\"\"");

        ensure(v != NULL);
        ensure(v->type == 's');
        ensure(fdja_string(v) ===f "\"hello \\\"old bore\\\"\"");
        ensure(fdja_to_string(v) ===f "hello \"old bore\"");
        ensure(fdja_to_json(v) ===f "\"hello \\\"old bore\\\"\"");
      }

      it "parses \\t"
      {
        v = fdja_dparse("\"hello\\ttab\"");

        ensure(v != NULL);
        ensure(v->type == 's');
        ensure(fdja_string(v) ===f "\"hello\\ttab\"");
        ensure(fdja_to_string(v) ===f "hello\ttab");
        ensure(fdja_to_json(v) ===f "\"hello\\ttab\"");
      }

      it "parses unicode escape sequences"
      {
        v = fdja_dparse("\"hello \\u0066ool\"");

        ensure(v != NULL);
        ensure(v->type == 's');
        ensure(fdja_string(v) ===f "\"hello \\u0066ool\"");
        ensure(fdja_to_string(v) ===f "hello fool");
        ensure(fdja_to_json(v) ===f "\"hello \\u0066ool\"");
      }

      it "doesn't parse unknown escapes"
      {
        v = fdja_parse("\"hello \\z\"");
          // no need for a dparse here...

        ensure(v == NULL);
      }

      it "rejects control characters"
      {
        v = fdja_parse("\"hello\nnada\"");

        ensure(v == NULL);
      }

      context "single quotes"
      {
        it "parses single quotes strings"
        {
          v = fdja_dparse("'hello \"world\"'");

          ensure(v != NULL);
          ensure(v->type == 'q');
          ensure(fdja_string(v) ===f "'hello \"world\"'");
          ensure(fdja_to_string(v) ===f "hello \"world\"");
          ensure(fdja_to_json(v) ===f "\"hello \"world\"\"");
        }
        it "lets a quote being escaped"
        {
          v = fdja_dparse("'aujourd\\'hui'");

          ensure(v != NULL);
          ensure(v->type == 'q');
          ensure(fdja_string(v) ===f "'aujourd\\'hui'");
          ensure(fdja_to_string(v) ===f "aujourd'hui");
          ensure(fdja_to_json(v) ===f "\"aujourd'hui\"");
        }
      }

      context "escapes"
      {
        it "accepts reverse soliduses"
        {
          v = fdja_dparse("\"$(dol\\\\)lar)\"");

          expect(v != NULL);
          expect(v->type == 's');
          expect(fdja_string(v) ===f "\"$(dol\\\\)lar)\"");
          expect(fdja_to_string(v) ===f "$(dol\\)lar)");
          expect(fdja_to_json(v) ===f "\"$(dol\\\\)lar)\"");
        }
      }
    }

    context "booleans"
    {
      it "parses \"true\""
      {
        v = fdja_dparse("true");

        ensure(v != NULL);
        ensure(v->type == 't');
        ensure(fdja_string(v) ===f "true");
        ensure(fdja_to_int(v) == 1);
        ensure(fdja_to_json(v) ===f "true");
      }

      it "parses \"false\""
      {
        v = fdja_dparse("false");

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
        v = fdja_dparse("null");

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
        v = fdja_dparse("[]");

        ensure(v != NULL);
        ensure(v->type == 'a');
        ensure(v->child == NULL);
        ensure(fdja_to_json(v) ===f "[]");
      }

      it "parses [ ]"
      {
        v = fdja_dparse("[ ]");

        ensure(v != NULL);
        ensure(v->type == 'a');
        ensure(v->child == NULL);
        ensure(fdja_to_json(v) ===f "[]");
      }

      it "parses [1,2,3]"
      {
        v = fdja_dparse("[1,2,3]");

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
        v = fdja_dparse("[ 10 20,30, 40 50\t51\n52]");

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
        v = fdja_dparse("[1,2,]");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "[1,2]");
      }

      it "parses [1,,3]"
      {
        v = fdja_dparse("[1,,3]");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "[1,3]");
      }

      it "parses [<lf>]"
      {
        v = fdja_dparse("[\n]");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "[]");
      }

      it "parses [ 1 # comment \\n 3 ]"
      {
        v = fdja_dparse("[ 1 # comment \n 3 ]");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "[1,3]");
      }
    }

    context "objects"
    {
      it "parses {}"
      {
        v = fdja_dparse("{}");

        ensure(v != NULL);
        ensure(v->type == 'o');
        ensure(fdja_value_at(v, 0) == NULL);
        ensure(fdja_to_json(v) ===f "{}");
      }

      it "parses {\"a\":0,\"bb\":null,\"cc c\":true}"
      {
        v = fdja_dparse("{\"a\":0,\"bb\":null,\"cc c\":true}");

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
        v = fdja_dparse("{ a_a: 0, bb_: null, c3:\"three\" }");

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
        v = fdja_dparse("{ 'a_a': 0, 'bb_': null }");

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
        v = fdja_dparse("{ a_a: 0, bb_: null \"c\": true\nd: [ 1, 2 ] }");

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
        v = fdja_dparse("{a:0,b:1,}");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "{\"a\":0,\"b\":1}");
      }

      it "parses {a:0,,c:2}"
      {
        v = fdja_dparse("{a:0,,c:2}");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "{\"a\":0,\"c\":2}");
      }

      it "parses {<lf>}"
      {
        v = fdja_dparse("{\n}");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "{}");
      }

      it "doesn't mind empty lines before the object"
      {
        v = fdja_dparse("\n{}");

        expect(v != NULL);
        expect(fdja_to_json(v) ===f "{}");
      }
    }

    context "symbols"
    {
      it "accepts symbols in lieu of strings"
      {
        v = fdja_dparse("sk8park");

        ensure(v != NULL);
        ensure(v->type == 'y');
        ensure(fdja_string(v) ===f "sk8park");
        ensure(fdja_to_string(v) ===f "sk8park");
        ensure(fdja_to_json(v) ===f "\"sk8park\"");
      }

      it "accepts symbols (nested)"
      {
        v = fdja_dparse("[ mi6, cia, kgb c64 ]");

        ensure(v != NULL);
        ensure(v->type == 'a');

        ensure(fdja_to_json(v) ===f "[\"mi6\",\"cia\",\"kgb\",\"c64\"]");
      }

      it "defaults to symbols (leaf)"
      {
        v = fdja_dparse("123.456.abc");

        ensure(v != NULL);
        ensure(fdja_to_json(v) ===f "\"123.456.abc\"");
      }

      it "defaults to symbols (in array)"
      {
        v = fdja_dparse("[ 123.456.789 ]");

        ensure(v != NULL);
        ensure(fdja_to_json(v) ===f "[\"123.456.789\"]");
      }

      it "defaults to symbols (object key)"
      {
        v = fdja_dparse("{ 123.456.abc: true }");

        ensure(v != NULL);
        ensure(fdja_to_json(v) ===f "{\"123.456.abc\":true}");
      }

      it "defaults to symbols (object value)"
      {
        v = fdja_dparse("{ smurf: 123.456.789 }");

        ensure(v != NULL);
        ensure(fdja_to_json(v) ===f "{\"smurf\":\"123.456.789\"}");
      }
    }

    context "whitespaces"
    {
      it "accepts whitespace around values"
      {
        v = fdja_dparse(" 77.0 ");

        ensure(v != NULL);
        ensure(v->type == 'n');
        ensure(fdja_to_string(v) ===f "77.0");
        ensure(fdja_to_json(v) ===f "77.0");
      }

      it "accepts whitespace inside of arrays"
      {
        v = fdja_dparse(" [1, 2,\n\t3 ] ");

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
        v = fdja_dparse("\n{\n\"a\": 0, \"bb\": null, \"cc c\": true\n}\n");

        ensure(v != NULL);
        ensure(v->type c== 'o');
        ensure(fdja_at(v, 0) != NULL);
        ensure(fdja_at(v, 1) != NULL);
        ensure(fdja_at(v, 2) != NULL);
        ensure(fdja_at(v, 3) == NULL);
        ensure(fdja_at(v, 0)->key === "a");
        ensure(fdja_at(v, 1)->key === "bb");
        ensure(fdja_at(v, 2)->key === "cc c");
        ensure(fdja_at(v, 0)->type c== 'n');
        ensure(fdja_at(v, 1)->type c== '0');
        ensure(fdja_at(v, 2)->type c== 't');
        ensure(fdja_li(v, "a") i== 0);
        ensure(fdja_to_int(fdja_at(v, 2)) i== 1);
        ensure(fdja_tod(v) ===f "{ a: 0, bb: null, \"cc c\": true }");
      }

      it "accepts comments at the end of lines"
      {
        v = fdja_dparse(""
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

    it "parses fine"
    {
      v = fdja_parse(
        "{\n"
          "domain: org.example\n"
          "execute: [ invoke, { _0: stamp }, [] ]\n"
          "payload: {}\n"
        "}\n");

      expect(v != NULL);

      v->sowner = 0;

      expect(fdja_ls(v, "domain", NULL) ===f "org.example");
      expect(fdja_ls(v, "execute.1._0", NULL) ===f "stamp");
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

    it "returns NULL when it fails to read"
    {
      v = fdja_parse_f("../spec/_test3.bad");

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

  describe "fdja_fparse()"
  {
    it "reads JSON from a FILE"
    {
      FILE *f = fopen("../spec/_test1.json", "r");
      v = fdja_fparse(f);

      expect(fdja_lookup_int(v, "id", -1) == 12345);
      expect(fdja_lookup_string(v, "type", NULL) ===f "test");

      fclose(f);
    }
  }
}

