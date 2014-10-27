
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
          expect(fdja_parse("\"hello\b/world\"") == NULL);
        }
        it "accepts escaped control characters"
        {
          expect(fdja_vj("\"hello\\nworld\"") ===f "\"hello\\nworld\"");
          expect(fdja_vj("\"hello\\tworld\"") ===f "\"hello\\tworld\"");
          expect(fdja_vj("\"hello\\\\world\"") ===f "\"hello\\\\world\"");
          expect(fdja_vj("\"hello\\bworld\"") ===f "\"hello\\bworld\"");
        }
      }
    }
    context "single-quoted strings"
    {
      describe "fdja_parse()"
      {
        it "rejects control characters"
        {
          expect(fdja_parse("'hello\nworld'") == NULL);
          expect(fdja_parse("'hello\tworld'") == NULL);
          expect(fdja_parse("'hello\\world'") == NULL);
          expect(fdja_parse("'hello\bworld'") == NULL);
        }
        it "accepts escaped control characters"
        {
          expect(fdja_vj("'hello\\nworld'") ===f "\"hello\\nworld\"");
          expect(fdja_vj("'hello\\tworld'") ===f "\"hello\\tworld\"");
          expect(fdja_vj("'hello\\\\world'") ===f "\"hello\\\\world\"");
          expect(fdja_vj("'hello\\bworld'") ===f "\"hello\\bworld\"");
        }
      }
    }
    context "symbols"
    {
      describe "fdja_parse()"
      {
        it "rejects control characters"
        {
          expect(fdja_parse("hello\nworld") == NULL);
          expect(fdja_parse("hello\bworld") == NULL);
        }
        it "rejects escaped control characters"
        {
          expect(fdja_parse("hello\\nworld") == NULL);
          expect(fdja_parse("hello\\/world") == NULL);
        }
      }
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

