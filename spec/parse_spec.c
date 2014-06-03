
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
      v = dja_parse(rdz_strdup("1"));

      ensure(v != NULL);
      ensure(v->type == 'n');
      ensure(v->soff == 0);
      ensure(v->slen == 1);
      ensure(dja_to_int(v) == 1);
    }
    it "parses \"-1\""
    {
      v = dja_parse(rdz_strdup("-1"));

      ensure(v != NULL);
      ensure(v->type == 'n');
      ensure(v->soff == 0);
      ensure(v->slen == 2);
      ensure(dja_to_int(v) == -1);
    }

    it "parses \"0.0\""
    {
      v = dja_parse(rdz_strdup("0.0"));

      ensure(v != NULL);
      ensure(v->type == 'n');
      ensure(v->soff == 0);
      ensure(v->slen == 3);
      ensure(dja_to_double(v) == 0.0);
      ensure(dja_to_string(v) ===f "0.0");
    }

    it "parses \"1.5e1\""
    {
      v = dja_parse(rdz_strdup("1.5e1"));

      ensure(v != NULL);
      ensure(v->type == 'n');
      ensure(v->soff == 0);
      ensure(v->slen == 5);
      ensure(dja_to_double(v) == 15.0);
    }
  }

  context "strings"
  {
    it "parses strings"
    {
      v = dja_parse(rdz_strdup("\"hello\""));

      ensure(v != NULL);
      ensure(v->type == 's');
      ensure(dja_to_string(v) ===f "hello");
    }
  }

  context "booleans"
  {
    it "parses \"true\""
    {
      v = dja_parse(rdz_strdup("true"));

      ensure(v != NULL);
      ensure(v->type == 't');
      ensure(dja_to_int(v) == 1);
    }

    it "parses \"false\""
    {
      v = dja_parse(rdz_strdup("false"));

      ensure(v != NULL);
      ensure(v->type == 'f');
      ensure(dja_to_int(v) == 0);
    }
  }

  context "null"
  {
    it "parses \"null\""
    {
      v = dja_parse(rdz_strdup("null"));

      ensure(v != NULL);
      ensure(v->type == '0');
    }
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

