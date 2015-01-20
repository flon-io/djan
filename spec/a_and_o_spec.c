
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
    it "flips burgers"
  }
}

