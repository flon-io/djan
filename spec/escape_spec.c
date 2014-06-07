
//
// specifying djan
//
// Sat Jun  7 09:41:22 JST 2014
//

#include "djan.h"


context "escapes"
{
  describe "dja_escape()"
  {
    it "escapes"
  }

  describe "dja_n_escape()"
  {
    it "escapes"
  }

  describe "dja_unescape()"
  {
    it "unescapes tabs"
    {
      ensure(dja_unescape("a\\ttab") ===f "a\ttab");
    }
  }

  describe "dja_n_unescape()"
  {
    it "unescapes"
  }
}

