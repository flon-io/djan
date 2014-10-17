
//
// specifying djan
//
// Wed May 28 06:07:18 JST 2014
//

#include "djan.h"


context "fdja_value"
{
  before each
  {
    fdja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) fdja_value_free(v);
  }

  describe "fdja_size()"
  {
    it "returns how many children a value has"
    {
      v = fdja_dparse("[ 0, 7, 14, 28, 35 ]");

      ensure(fdja_size(v) == 5);
    }

    it "returns 0 for an atom"
    {
      v = fdja_dparse("true");

      ensure(fdja_size(v) == 0);
    }
  }

  describe "fdja_lookup()"
  {
    it "looks up in arrays"
    {
      v = fdja_dparse("[ 0, 7, 14, 28, 35 ]");

      ensure(fdja_to_int(fdja_lookup(v, "0")) == 0);
      ensure(fdja_to_int(fdja_lookup(v, "4")) == 35);
      ensure(fdja_lookup(v, "5") == NULL);
    }

    it "counts from the end of the array when the index is negative"
    {
      v = fdja_dparse("[ 0, 7, 14, 28, 35 ]");

      ensure(fdja_to_int(fdja_lookup(v, "-1")) == 35);
      ensure(fdja_to_int(fdja_lookup(v, "-2")) == 28);
      ensure(fdja_lookup(v, "-6") == NULL);
    }

    it "looks up in objects"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      ensure(fdja_string(fdja_lookup(v, "type")) ===f "car");
      ensure(fdja_string(fdja_lookup(v, "color")) ===f "blue");
      ensure(fdja_to_int(fdja_lookup(v, "ids.1")) == 456);
      ensure(fdja_to_int(fdja_lookup(v, "ids.-1")) == 456);
      ensure(fdja_to_int(fdja_lookup(v, "ids.-2")) == 123);
      ensure(fdja_lookup(v, "ids.-3") == NULL);
      ensure(fdja_lookup(v, "type.name") == NULL);
      ensure(fdja_lookup(v, "nada") == NULL);
    }

    it "composes its path"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      ensure(fdja_to_int(fdja_lookup(v, "%s.1", "ids")) == 456);
    }
  }

  describe "fdja_lookup_c()"
  {
    before each
    {
      fdja_value *vv = NULL;
    }
    after each
    {
      if (vv) fdja_free(vv);
    }

    it "lookups a value and returns a clone of it"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      vv = fdja_lookup_c(v, "ids");

      expect(vv != NULL);
      expect(vv->sowner == 1);
      expect(vv->source === "[123,456]");
      expect(vv->soff == 0);
      expect(vv->slen == 9);
    }

    it "returns NULL if it doesn't find the value"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      vv = fdja_lookup_c(v, "nada");

      expect(vv == NULL);
    }

    it "composes its path"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      vv = fdja_lookup_c(v, "ids.%i", -1);

      ensure(fdja_to_int(vv) == 456);
    }
  }

  describe "fdja_lookup_string()"
  {
    it "returns a copy of the string value when it finds"
    {
      v = fdja_dparse("{ type: car, parts: [ carburator, wheel ] }");

      expect(fdja_lookup_string(v, "type", NULL) ===f "car");
      expect(fdja_lookup_string(v, "parts.0", NULL) ===f "carburator");
      expect(fdja_lookup_string(v, "parts.1", NULL) ===f "wheel");
      expect(fdja_lookup_string(v, "parts.2", NULL) == NULL);
    }

    it "returns the default when key is not found"
    {
      v = fdja_dparse("{}");

      expect(fdja_lookup_string(v, "type", NULL) === NULL);
      expect(fdja_lookup_string(v, "type", "nada") === "nada");
    }
  }

  describe "fdja_lookup_int()"
  {
    before each
    {
      v = fdja_v("{ a: 1234 }");
    }

    it "returns an int"
    {
      expect(fdja_lookup_int(v, "a", 0) == 1234);
    }

    it "returns the default value if it doesn't find"
    {
      expect(fdja_lookup_int(v, "z", 0) == 0);
    }
  }

  describe "fdja_lookup_bool()"
  {
    before each
    {
      v = fdja_v(""
        "{"
          "t0: true, t1: TRUE, t2: yes,"
          "f0: false, f1: FALSE, f2: no,"
          "x: nada"
        "}");
    }

    it "0, 1 or the default"
    {
      expect(fdja_lookup_bool(v, "t0", -1) == 1);
      expect(fdja_lookup_bool(v, "t1", -1) == 1);
      expect(fdja_lookup_bool(v, "t2", -1) == 1);
      expect(fdja_lookup_bool(v, "f0", -1) == 0);
      expect(fdja_lookup_bool(v, "f1", -1) == 0);
      expect(fdja_lookup_bool(v, "f2", -1) == 0);
      expect(fdja_lookup_bool(v, "x", -1) == -1);
      expect(fdja_lookup_bool(v, "z", -1) == -1);
    }
  }

  describe "fdja_lookup_boolean()"
  {
    before each
    {
      v = fdja_v("{ a: true, b: false, c: nada }");
    }

    it "returns 0, 1 or the default"
    {
      expect(fdja_lookup_boolean(v, "a", -1) == 1);
      expect(fdja_lookup_boolean(v, "b", -1) == 0);
      expect(fdja_lookup_boolean(v, "c", -1) == -1);
      expect(fdja_lookup_boolean(v, "d", -1) == -1);
    }
  }

  describe "fdja_clone()"
  {
    before each
    {
      fdja_value *v2 = NULL;
    }
    after each
    {
      if (v2) fdja_value_free(v2);
    }

    it "copies entirely a fdja_value"
    {
      v = fdja_v("{\"a\":true,\"b\":[1,2,\"trois\"],\"c\":\"nada\"}");

      v2 = fdja_clone(v);

      expect(v2 != NULL);
      expect(v->source != v2->source);
      expect(v->source === v2->source);
      expect(v2->sowner == 1);
    }

    it "copies entirely a fdja_value (not owner)"
    {
      v = fdja_v("{\"a\":true,\"b\":[1,2,\"trois\"],\"c\":\"nada\"}");

      fdja_value *v1 = fdja_lookup(v, "b");

      v2 = fdja_clone(v1);

      expect(v2 != NULL);
      //expect(v1->source != v2->source); // pointless
      expect(fdja_to_json(v1) ===f v2->source);
      expect(v1->sowner == 0);
      expect(v2->sowner == 1);
    }

    it "returns NULL if the input is NULL"
    {
      expect(fdja_clone(NULL) == NULL);
    }
  }

  describe "fdja_free()"
  {
    it "is an alias to fdja_value_free()"
    {
      fdja_value *vv = fdja_v("{}");
      fdja_free(vv);

      // Valgrind to the rescue...
    }
  }
}

