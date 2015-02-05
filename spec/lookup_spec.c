
//
// specifying djan
//
// Fri Oct 17 15:43:23 JST 2014
//

#include "djan.h"


context "fdja_lookup..."
{
  before each
  {
    fdja_value *v = NULL;
  }
  after each
  {
    fdja_value_free(v);
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
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ], 0: nemo }");

      ensure(fdja_string(fdja_lookup(v, "type")) ===f "car");
      ensure(fdja_string(fdja_lookup(v, "color")) ===f "blue");
      ensure(fdja_string(fdja_lookup(v, "0")) ===f "nemo");
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

    it "is aliased to fdja_l()"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      ensure(fdja_to_int(fdja_l(v, "%s.1", "ids")) == 456);
    }

    it "is ok with keys that begin with a digit"
    {
      v = fdja_dparse("{ nodes: { 0_0: nada } }");

      ensure(fdja_lj(v, "nodes.0_0") ===F fdja_vj("nada"));
    }

    it "accepts keys as long as they don't contain tabs, dots or newlines"
    {
      v = fdja_dparse("{ \"group a\": { 0_0-f: sruf } }");

      ensure(fdja_lj(v, "group a.0_0-f") ===F fdja_vj("sruf"));
    }

    it "accepts an escaped . in the key"
    {
      v = fdja_v("{ a: { \"b.b\": { c: 0 }, \"d'd\": e } }");

      ensure(fdja_lj(v, "a.b\\.b.c") ===F fdja_vj("0"));
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

    it "is aliased to fdja_lc()"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      vv = fdja_lc(v, "ids.%i", -1);

      ensure(fdja_to_int(vv) == 456);
    }
  }

  describe "fdja_lj()"
  {
    it "lookups a value and returns it turned into JSON"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      expect(fdja_lj(v, "ids") ===F fdja_vj("[ 123, 456 ]"));
      expect(fdja_lj(v, "color") ===F fdja_vj("\"blue\""));
    }

    it "returns NULL if it doesn't find"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      expect(fdja_lj(v, "nada") == NULL);
    }
  }

  describe "fdja_ld()"
  {
    it "lookups a value and returns it turned into djan"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      expect(fdja_ld(v, "ids") ===f "[ 123, 456 ]");
      expect(fdja_ld(v, "color") ===f "blue");
    }

    it "returns NULL if it doesn't find"
    {
      v = fdja_dparse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      expect(fdja_ld(v, "nada") == NULL);
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

    it "composes its path"
    {
      v = fdja_dparse("{ type: car, parts: [ carburator, wheel ] }");

      expect(fdja_lookup_string(v, "parts.%i", 1, NULL) ===f "wheel");
      expect(fdja_lookup_string(v, "parts.%i", 2, NULL) == NULL);
      expect(fdja_lookup_string(v, "parts.%i", 2, "none") === "none");
    }

    it "is aliased to fdja_ls()"
    {
      v = fdja_dparse("{ type: car, parts: [ carburator, wheel ] }");

      expect(fdja_ls(v, "parts.%i", 1, NULL) ===f "wheel");
    }
  }

  describe "fdja_lookup_string_dup_default()"
  {
    it "looks up"
    {
      v = fdja_dparse("{ type: car, parts: [ carburator, wheel ] }");

      expect(fdja_lsd(v, "parts.%i", 1, NULL) ===f "wheel");
    }

    it "returns a dup of the default it has to return the default"
    {
      v = fdja_dparse("{ type: car, parts: [ carburator, wheel ] }");

      char *r = fdja_lsd(v, "parts.%i", 2, "gastank");

      expect(r === "gastank");

      free(r); // no crash
    }

    it "doesn't dup NULL"
    {
      v = fdja_dparse("{ type: car, parts: [ carburator, wheel ] }");

      expect(fdja_lsd(v, "parts.%i", 2, NULL) == NULL);
    }
  }

  describe "fdja_lookup_int()"
  {
    it "returns an int"
    {
      v = fdja_v("{ a: 1234 }");

      expect(fdja_lookup_int(v, "a", 0) == 1234);
    }

    it "returns the default value if it doesn't find"
    {
      v = fdja_v("{ a: 1234 }");

      expect(fdja_lookup_int(v, "z", 0) == 0);
    }

    it "returns the default value if it doesn't find (2)"
    {
      v = fdja_v("{ point: terminated, nid: \"0\", from: \"\" }");
      fdja_set(v, "payload", fdja_v("{\"ret\":42}"));

      expect(fdja_lookup_int(v, "ret", 0) lli== 0);
      expect(fdja_lookup_int(v, "payload.ret", 0) lli== 42); // extra
    }

    it "composes its path"
    {
      v = fdja_v("{ a: { b: 77 } }");

      expect(fdja_lookup_int(v, "a.%s", "b", 0) == 77);
      expect(fdja_lookup_int(v, "a.%s", "c", 0) == 0);
    }

    it "is aliased to fdja_li()"
    {
      v = fdja_v("{ a: { b: 77 } }");

      expect(fdja_li(v, "a.%s", "b", 0) == 77);
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

    it "composes its path"
    {
      expect(fdja_lookup_bool(v, "%s", "t0", -1) == 1);
      expect(fdja_lookup_bool(v, "%s", "z", -1) == -1);
    }

    it "is aliased to fdja_lb()"
    {
      expect(fdja_lb(v, "%s", "t0", -1) == 1);
      expect(fdja_lb(v, "%s", "z", -1) == -1);
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

    it "composes its path"
    {
      expect(fdja_lookup_boolean(v, "%s", "a", -1) == 1);
      expect(fdja_lookup_boolean(v, "%s", "b", -1) == 0);
      expect(fdja_lookup_boolean(v, "%s", "c", -1) == -1);
      expect(fdja_lookup_boolean(v, "%s", "d", -1) == -1);
    }
  }
}

