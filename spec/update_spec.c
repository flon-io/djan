
//
// specifying djan
//
// Wed Oct  1 09:53:00 JST 2014
//

#include "djan.h"

//int fdja_push(fdja_value *array, fdja_value *v);
//int fdja_set(fdja_value *object, const char *key, fdja_value *v);

context "update"
{
  before each
  {
    fdja_value *v = NULL;
    fdja_value *vv = NULL;
  }
  after each
  {
    if (v != NULL) fdja_value_free(v);
    if (vv != NULL) fdja_value_free(vv);
  }

  describe "fdja_push()"
  {
    it "pushes at the end of the target array"
    {
      v = fdja_parse("[ 0, 1, 2 ]");
      int r = fdja_push(v, fdja_v("3"));

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[0,1,2,3]");
    }

    it "pushes into empty arrays"
    {
      v = fdja_parse("[]");
      int r = fdja_push(v, fdja_v("\"hello\""));

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[\"hello\"]");
    }

    it "returns 0 if the target isn't an array"
    {
      v = fdja_parse("{}");
      vv = fdja_v("false");
      int r = fdja_push(v, vv);

      expect(r == 0);
    }

    it "pushes null values"
    {
      v = fdja_parse("[]");
      int r = fdja_push(v, fdja_v("null"));

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[null]");
    }
  }

  describe "fdja_set()"
  {
    it "sets a value"
    {
      v = fdja_parse("{ a: 0, b: 1, c: 2 }");
      int r = fdja_set(v, "d", fdja_v("3"));

      expect(r == 1);
      expect(fdja_to_json(v) ===f "{\"a\":0,\"b\":1,\"c\":2,\"d\":3}");
    }

    it "sets a value into an empty object"
    {
      v = fdja_parse("{}");
      int r = fdja_set(v, "e", fdja_v("4"));

      expect(r == 1);
      expect(fdja_to_json(v) ===f "{\"e\":4}");
    }

    it "returns 0 if the target isn't an object"
    {
      v = fdja_parse("[]");
      vv = fdja_v("false");
      int r = fdja_set(v, "a", vv);

      expect(r == 0);
    }

    it "re-sets values"
    {
      v = fdja_parse("{ a: 0, b: 1, c: 2 }");
      int r = fdja_set(v, "b", fdja_v("\"BB\""));

      expect(r == 1);
      expect(fdja_to_json(v) ===f "{\"a\":0,\"b\":\"BB\",\"c\":2}");
    }

    it "sets null values"
    {
      v = fdja_parse("{ a: 0, b: 1 }");
      int r = fdja_set(v, "c", fdja_v("null"));

      expect(r == 1);
      expect(fdja_to_json(v) ===f "{\"a\":0,\"b\":1,\"c\":null}");
    }

    it "unsets when v is NULL"
    {
      v = fdja_parse("{ a: 0, b: 1, c: 2 }");
      int r = fdja_set(v, "b", NULL);

      expect(r == 1);
      expect(fdja_to_json(v) ===f "{\"a\":0,\"c\":2}");
    }
  }

  describe "fdja_splice()"
  {
    it "returns 0 if the target isn't an array"
    {
      v = fdja_v("{}");
      int r = fdja_splice(v, 0, 1, NULL);

      expect(r == 0);
    }

    it "removes values"
    {
      v = fdja_v("[ 0, 1, 2 ]");
      int r = fdja_splice(v, 0, 1, NULL);

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[1,2]");
    }

    it "removes values (2)"
    {
      v = fdja_v("[ 0, 1, 2, 3 ]");
      int r = fdja_splice(v, 1, 2, NULL);

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[0,3]");
    }

    it "removes values (3)"
    {
      v = fdja_v("[ 0, 1, 2, 3 ]");
      int r = fdja_splice(v, 1, 10, NULL);

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[0]");
    }

    it "inserts values"
    {
      v = fdja_v("[]");
      int r = fdja_splice(v, 0, 0, fdja_v("one"), fdja_v("two"), NULL);

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[\"one\",\"two\"]");
    }

    it "inserts values (2)"
    {
      v = fdja_v("[ 3, 4 ]");
      int r = fdja_splice(v, 0, 0, fdja_v("one"), fdja_v("two"), NULL);

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[\"one\",\"two\",3,4]");
    }

    it "inserts values (3)"
    {
      v = fdja_v("[ 0, 1 ]");
      int r = fdja_splice(v, 2, 0, fdja_v("two"), fdja_v("three"), NULL);

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[0,1,\"two\",\"three\"]");
    }

    it "removes and inserts values"
    {
      v = fdja_v("[ 0, 1, 2, 3 ]");
      int r = fdja_splice(v, 1, 2, fdja_v("one"), fdja_v("two"), NULL);

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[0,\"one\",\"two\",3]");
    }

    it "returns 0 if the start index is too far"
    {
      v = fdja_v("[ 0, 1 ]");
      int r = fdja_splice(v, 2, 1, NULL);

      expect(r == 0);
    }
  }
}
