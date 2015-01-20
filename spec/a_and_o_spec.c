
//
// specifying djan
//
// Tue Jan 20 13:59:12 JST 2015
//

#include "djan.h"


context "fdja_a(), fdja_o(), ..."
{
  before each
  {
    fdja_value *v = NULL;
  }
  after each
  {
    fdja_value_free(v);
  }

  describe "fdja_a()"
  {
    it "composes an empty array"
    {
      v = fdja_a(NULL);

      expect(fdja_tod(v) ===f "[]");
    }

    it "composes an array (1 element)"
    {
      v = fdja_a(fdja_v("toto"), NULL);

      expect(fdja_tod(v) ===f "[ toto ]");
    }

    it "composes an array"
    {
      v = fdja_a(fdja_v("toto"), fdja_object_malloc(), NULL);

      expect(fdja_tod(v) ===f "[ toto, {} ]");
    }
  }

  describe "fdja_o()"
  {
    it "composes an empty object"
    {
      v = fdja_o(NULL);

      expect(fdja_tod(v) ===f "{}");
    }

    it "composes an object (1 entry)"
    {
      v = fdja_o("k0", fdja_v("val0"), NULL);

      expect(fdja_tod(v) ===f "{ k0: val0 }");
    }

    it "composes an object"
    {
      v = fdja_o("k0", fdja_v("val0"), "k1", fdja_v("null"), NULL);

      expect(fdja_tod(v) ===f "{ k0: val0, k1: null }");
    }

    it "composes its keys"
    {
      v = fdja_o("k0", fdja_v("val0"), "k%d", 1, fdja_v("hello"), NULL);

      expect(fdja_tod(v) ===f "{ k0: val0, k1: hello }");
    }
  }
}

