
//
// specifying djan
//
// Wed Oct  1 09:53:00 JST 2014
//

#include "djan.h"


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
      v = fdja_dparse("[ 0, 1, 2 ]");
      fdja_value *r = fdja_push(v, fdja_v("3"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "[0,1,2,3]");
    }

    it "pushes into empty arrays"
    {
      v = fdja_dparse("[]");
      fdja_value *r = fdja_push(v, fdja_v("\"hello\""));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "[\"hello\"]");
    }

    it "returns 0 if the target isn't an array"
    {
      v = fdja_dparse("{}");
      vv = fdja_v("false");
      fdja_value *r = fdja_push(v, vv);

      expect(r == NULL);
    }

    it "pushes null values"
    {
      v = fdja_dparse("[]");
      fdja_value *r = fdja_push(v, fdja_v("null"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "[null]");
    }
  }

  describe "fdja_set()"
  {
    it "sets a value"
    {
      v = fdja_dparse("{ a: 0, b: 1, c: 2 }");
      fdja_value *r = fdja_set(v, "d", fdja_v("3"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"a\":0,\"b\":1,\"c\":2,\"d\":3}");
      expect(fdja_tod(r) ===f "3");
    }

    it "sets a value into an empty object"
    {
      v = fdja_dparse("{}");
      fdja_value *r = fdja_set(v, "e", fdja_v("4"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"e\":4}");
      expect(fdja_tod(r) ===f "4");
    }

    it "returns NULL if the target isn't an object"
    {
      v = fdja_dparse("[]");
      vv = fdja_v("false");
      fdja_value *r = fdja_set(v, "a", vv);

      expect(r == NULL);
    }

    it "re-sets values"
    {
      v = fdja_dparse("{ a: 0, b: 1, c: 2 }");
      fdja_value *r = fdja_set(v, "b", fdja_v("\"BB\""));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"a\":0,\"b\":\"BB\",\"c\":2}");
    }

    it "sets null values"
    {
      v = fdja_dparse("{ a: 0, b: 1 }");
      fdja_value *r = fdja_set(v, "c", fdja_v("null"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"a\":0,\"b\":1,\"c\":null}");
    }

    it "unsets when v is NULL"
    {
      v = fdja_dparse("{ a: 0, b: 1, c: 2 }");
      fdja_value *r = fdja_set(v, "b", NULL);

      expect(r == NULL);
      expect(fdja_to_json(v) ===f "{\"a\":0,\"c\":2}");
    }

    it "sets at the beginning of the object when key starts with backslash-b"
    {
      v = fdja_dparse("{ a: 0, b: 1, c: 2 }");
      fdja_value *r = fdja_set(v, "\bz", fdja_s("zorro"));

      expect(r != NULL);
      expect(fdja_tod(v) ===f "{ z: zorro, a: 0, b: 1, c: 2 }");
    }

    it "sets at the beginning of an empty object with backslash-b"
    {
      v = fdja_dparse("{}");
      fdja_value *r = fdja_set(v, "\bz", fdja_s("zorro"));

      expect(r != NULL);
      expect(fdja_tod(v) ===f "{ z: zorro }");
    }

    it "moves to the front when already present and backslash-b"
    {
      v = fdja_dparse("{ a: 0, b: 1 }");
      fdja_value *r = fdja_set(v, "\bb", fdja_v("2"));

      expect(r != NULL);
      expect(fdja_tod(v) ===f "{ b: 2, a: 0 }");
    }

    it "doesn't care about backlash-b if the value is NULL, it unsets"
    {
      v = fdja_dparse("{ a: 0, b: 1 }");
      fdja_value *r = fdja_set(v, "\bb", NULL);

      expect(r == NULL);
      expect(fdja_tod(v) ===f "{ a: 0 }");
    }

    it "composes its key"
    {
      v = fdja_dparse("{ a: 0, b: 1, c: 2 }");
      fdja_value *r = fdja_set(v, "%sd", "d", fdja_v("3d"));

      expect(r != NULL);
      expect(fdja_tod(v) ===f "{ a: 0, b: 1, c: 2, dd: 3d }");
      expect(fdja_tod(r) ===f "3d");
    }
  }

  describe "fdja_merge()"
  {
    it "merges two objects"
    {
      v = fdja_v("{ a: 0 }");
      vv = fdja_v("{ b : 1}");

      int r = fdja_merge(v, vv);

      expect(r == 1);
      expect(fdja_to_json(v) ===f "{\"a\":0,\"b\":1}");
      expect(fdja_to_json(vv) ===f "{\"b\":1}");
    }

    it "returns 0 if one of the arg is not an object"
    {
      v = fdja_v("{ a: 0 }");
      vv = fdja_v("[]");

      expect(fdja_merge(v, vv) == 0);
      expect(fdja_merge(vv, v) == 0);
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

    it "understands negative indexes"
    {
      v = fdja_v("[ 0, 1, 2 ]");
      int r = fdja_splice(v, -2, 2, fdja_v("one"), NULL);

      expect(r == 1);
      expect(fdja_to_json(v) ===f "[0,\"one\"]");
    }

    it "doesn't accept very negative indexes"
    {
      v = fdja_v("[ 0, 1, 2 ]");
      int r = fdja_splice(v, -4, 2, NULL);

      expect(r == 0);
    }
  }

  describe "fdja_pset()"
  {
    it "sets here if the path is immediate"
    {
      v = fdja_dparse("{ a: 0, b: 1, c: 2 }");
      fdja_value *r = fdja_pset(v, "b", fdja_v("\"B\""));
      //int r = fdja_pset(v, "b", fdja_s("B"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"a\":0,\"b\":\"B\",\"c\":2}");
    }

    it "sets in an object"
    {
      v = fdja_dparse("{ a: {} }");
      fdja_value *r = fdja_pset(v, "a.a", fdja_v("2"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"a\":{\"a\":2}}");
    }

    it "sets in arrays"
    {
      v = fdja_dparse("{ a: [ 0 ] }");
      fdja_value *r = fdja_pset(v, "a.0", fdja_v("1"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"a\":[1]}");
    }

    it "returns 0 when outside of array reach"
    {
      v = fdja_dparse("{ a: [ 0 ] }");
      fdja_value *r = fdja_pset(v, "a.1", NULL);

      expect(r == NULL);
    }

    it "appends to the array when index is ']'"
    {
      v = fdja_dparse("{ a: [ 0 ] }");
      fdja_value *r = fdja_pset(v, "a.]", fdja_v("1"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"a\":[0,1]}");
    }

    it "understands negative indexes"
    {
      v = fdja_dparse("{ a: [ 0, 1 ] }");
      fdja_value *r = fdja_pset(v, "a.-1", fdja_v("-1"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"a\":[0,-1]}");
    }

    it "composes its path"
    {
      v = fdja_dparse("{ a: [ 0, 1 ] }");
      fdja_value *r = fdja_pset(v, "a.%i", 0, fdja_v("zero"));

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"a\":[\"zero\",1]}");
    }

    it "accepts escaped . in its path"
    {
      v = fdja_dparse("{ a: { \"b.b\": {}} }");

      fdja_value *r = fdja_pset(v, "a.b\\.b.c", fdja_v("C"));

      expect(r != NULL);
      expect(fdja_tod(v) ===f "{ a: { b.b: { c: C } } }");
      expect(fdja_tod(r) ===f "C");
    }
  }

  describe "fdja_psetv()"
  {
    it "sets"
    {
      v = fdja_dparse("{ a: {} }");

      fdja_value *r = fdja_psetv(v, "a.type-%i", 0, "%s-car", "blue");

      expect(r != NULL);
      expect(fdja_to_json(v) ===f "{\"a\":{\"type-0\":\"blue-car\"}}");
    }

    it "sets over"
    {
      v = fdja_dparse("{ a: { b: 0 } }");

      fdja_value *r = fdja_psetv(v, "a.b", "1");

      expect(r != NULL);
      expect(fdja_tod(v) ===f "{ a: { b: 1 } }");
      expect(fdja_tod(r) ===f "1");
    }

    it "returns NULL when it cannot set"
    {
      v = fdja_dparse("{ a: [] }");

      fdja_value *r = fdja_psetv(v, "a.type-%i", 0, "%s-car", "red");

      expect(r == NULL);
      expect(fdja_to_json(v) ===f "{\"a\":[]}");
    }

    it "accepts backslash-b to set at the beginning of objects"
    {
      v = fdja_dparse("{ a: { t1: here } }");

      fdja_value *r = fdja_psetv(v, "a.\btype-%i", 0, "%s-car", "blue");

      expect(r != NULL);
      expect(fdja_tod(v) ===f "{ a: { type-0: blue-car, t1: here } }");
    }

    it "accepts escaped . in its path"
    {
      v = fdja_dparse("{ a: { \"b.b\": {}} }");

      fdja_value *r = fdja_psetv(v, "a.b\\.b.c", "C%s", "ok");

      expect(r != NULL);
      expect(fdja_tod(v) ===f "{ a: { b.b: { c: Cok } } }");
      expect(fdja_tod(r) ===f "Cok");
    }
  }

  describe "fdja_replace()"
  {
    it "replaces old with new"
    {
      v = fdja_dparse("{ a: { b: c } }");

      fdja_value *b = fdja_l(v, "a.b");

      fdja_replace(b, fdja_v("d"));

      expect(fdja_to_json(v) ===F fdja_vj("{ a: { b: d } }"));
    }
  }
}

