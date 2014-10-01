
//
// specifying djan
//
// Wed Oct  1 09:53:00 JST 2014
//

#include "djan.h"

//int dja_push(dja_value *array, dja_value *v);
//int dja_set(dja_value *object, const char *key, dja_value *v);

context "update"
{
  before each
  {
    dja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) dja_value_free(v);
  }

  describe "dja_push()"
  {
    it "pushes at the end of the target array"
    {
      v = dja_parse("[ 0, 1, 2 ]");
      int r = dja_push(v, dja_v("3"));

      expect(r == 1);
      expect(dja_to_json(v) ===f "[0,1,2,3]");
    }

    it "pushes into empty arrays"
    {
      v = dja_parse("[]");
      int r = dja_push(v, dja_v("\"hello\""));

      expect(r == 1);
      expect(dja_to_json(v) ===f "[\"hello\"]");
    }

    it "returns 0 if the target isn't an array"
    {
      v = dja_parse("{}");
      int r = dja_push(v, dja_v("\"nada\""));

      expect(r == 0);
    }

    it "pushes null values"
    {
      v = dja_parse("[]");
      int r = dja_push(v, dja_v("null"));

      expect(r == 1);
      expect(dja_to_json(v) ===f "[null]");
    }
  }

  describe "dja_set()"
  {
    it "sets a value"
    {
      v = dja_parse("{ a: 0, b: 1, c: 2 }");
      int r = dja_set(v, "d", dja_v("3"));

      expect(r == 1);
      expect(dja_to_json(v) ===f "{\"a\":0,\"b\":1,\"c\":2,\"d\":3}");
    }

    it "sets a value into an empty object"
    {
      v = dja_parse("{}");
      int r = dja_set(v, "e", dja_v("4"));

      expect(r == 1);
      expect(dja_to_json(v) ===f "{\"e\":4}");
    }

    it "returns 0 if the target isn't an object"
    {
      v = dja_parse("[]");
      int r = dja_set(v, "a", dja_v("false"));

      expect(r == 0);
    }

    it "re-sets values"
    {
      v = dja_parse("{ a: 0, b: 1, c: 2 }");
      int r = dja_set(v, "b", dja_v("\"BB\""));

      expect(r == 1);
      expect(dja_to_json(v) ===f "{\"a\":0,\"b\":\"BB\",\"c\":2}");
    }

    it "sets null values"
    {
      v = dja_parse("{ a: 0, b: 1 }");
      int r = dja_set(v, "c", dja_v("null"));

      expect(r == 1);
      expect(dja_to_json(v) ===f "{\"a\":0,\"b\":1,\"c\":null}");
    }

    it "unsets when v is NULL"
    {
      v = dja_parse("{ a: 0, b: 1, c: 2 }");
      int r = dja_set(v, "b", NULL);

      expect(r == 1);
      expect(dja_to_json(v) ===f "{\"a\":0,\"c\":2}");
    }
  }
}

