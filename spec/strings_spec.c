
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
        it "accepts slashes"
        {
          expect(fdja_vj("\"hello/honolulu\"") ===f "\"hello/honolulu\"");
        }
      }
      describe "fdja_s()"
      {
        it "escapes its input"
        {
          fdja_value *v = NULL;

          v = fdja_s("hello s");
          expect(fdja_to_json(v) ===f "\"hello s\"");
          fdja_free(v);

          v = fdja_s("hello\ns");
          expect(fdja_to_json(v) ===f "\"hello\\ns\"");
          fdja_free(v);
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
        it "accepts slashes"
        {
          expect(fdja_vj("'hello/honolulu'") ===f "\"hello/honolulu\"");
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
        it "accepts slashes"
        {
          expect(fdja_vj("hello/honolulu") ===f "\"hello/honolulu\"");
        }
      }
    }
  }

  //context "output"
  //{
  //  context "double-quoted strings"
  //  {
  //    it "flips burgers"
  //  }
  //  context "single-quoted strings"
  //  {
  //    it "flips burgers"
  //  }
  //  context "symbols"
  //  {
  //    it "flips burgers"
  //  }
  //}
    // dealt with above...
}

