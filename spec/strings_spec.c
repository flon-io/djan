
//
// specifying djan
//
// Mon Oct 27 09:15:08 JST 2014
//

#include "djan.h"


context "strings, single quoted strings and symbols"
{
  context "input"
  {
    context "double-quoted strings"
    {
      describe "fdja_parse()"
      {
        it "rejects control characters"
        {
          expect(fdja_parse("\"hello\nworld\"") == NULL);
          expect(fdja_parse("\"hello\tworld\"") == NULL);
          expect(fdja_parse("\"hello\\world\"") == NULL);
        }
        it "accepts escaped control characters"
        {
          expect(fdja_vj("\"hello\\nworld\"") ===f "\"hello\\nworld\"");
          expect(fdja_vj("\"hello\\tworld\"") ===f "\"hello\\tworld\"");
          expect(fdja_vj("\"hello\\\\world\"") ===f "\"hello\\\\world\"");
        }
      }
    }
    context "single-quoted strings"
    {
      it "rejects control characters"
    }
    context "symbols"
    {
      it "rejects control characters"
    }
  }

  context "output"
  {
    context "double-quoted strings"
    {
      it "flips burgers"
    }
    context "single-quoted strings"
    {
      it "flips burgers"
    }
    context "symbols"
    {
      it "flips burgers"
    }
  }
}

