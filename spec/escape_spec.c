
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
    it "unescapes back slashes"
    {
      ensure(dja_unescape("back\\\\slash") ===f "back\\slash");
    }
    it "unescapes double quotes"
    {
      ensure(dja_unescape("double\\\"quote") ===f "double\"quote");
    }
    it "unescapes tabs"
    {
      ensure(dja_unescape("a\\ttab") ===f "a\ttab");
    }
    it "unescapes \\uxxxx sequences"
    {
      ensure(dja_unescape("fuji\\u5c71") ===f "fuji山");
    }
    it "unescapes low \\uxxxx sequences"
    {
      ensure(dja_unescape("old \\u0066ool") ===f "old fool");
    }
    it "leaves unknown escapes as is"
    {
      ensure(dja_unescape("that's \\zorro") ===f "that's \\zorro");
    }
  }

  describe "dja_n_unescape()"
  {
    it "unescapes"
  }
}

