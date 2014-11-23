
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

  describe "fdja_src()"
  {
    it "returns a pointer to the beginning of the source string of the value"
    {
      v = fdja_v("{\"a\":true,\"b\":[1,2,\"trois\"],\"c\":\"nada\"}");

      fdja_value *v1 = fdja_lookup(v, "b");

      expect(fdja_src(v1) ^== "[1,2,\"trois\"],\"c\":");
        // yes, it's a pointer inside the source
    }
  }
}

