
//
// specifying djan
//
// Tue Oct  7 11:56:17 JST 2014
//

#include "djan.h"


context "fdja_v, fdja_s, ..."
{
  before each
  {
    fdja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) fdja_value_free(v);
  }

  describe "fdja_v()"
  {
    it "turns a string into a fdja_value"
    {
      v = fdja_v("{ type: car, make: mitsubishi, id: 2 }");

      expect(fdja_to_json(v) ===f ""
        "{\"type\":\"car\",\"make\":\"mitsubishi\",\"id\":2}");
    }

    it "behaves like printf"
    {
      v = fdja_v("{ type: %s, make: %s, id: %i }", "car", "nissan", 3);

      expect(fdja_to_json(v) ===f ""
        "{\"type\":\"car\",\"make\":\"nissan\",\"id\":3}");
    }
  }

  describe "fdja_s()"
  {
    it "wraps a string in a fdja_value"
    {
      v = fdja_s("hello world");

      expect(v->key == NULL);
      expect(v->type == 'y');
      expect(v->soff == 0);
      expect(v->slen == 0);
      expect(v->source === "hello world");
      expect(v->child == NULL);
      expect(v->sibling == NULL);
      expect(fdja_to_json(v) ===f "\"hello world\"");
    }

    it "accepts arguments"
    {
      v = fdja_s("hello %s", "world");

      expect(v->key == NULL);
      expect(v->type == 'y');
      expect(v->soff == 0);
      expect(v->slen == 0);
      expect(v->source === "hello world");
      expect(v->child == NULL);
      expect(v->sibling == NULL);
      expect(fdja_to_json(v) ===f "\"hello world\"");
    }
  }

  describe "fdja_c()"
  {
    it "parses loose objects (confs)"
    {
      v = fdja_c(
        "host: tsubaki\n"
        "whitelist: [ crane, turtle ]\n"
      );

      expect(fdja_to_json(v) ===f ""
        "{\"host\":\"tsubaki\",\"whitelist\":[\"crane\",\"turtle\"]}");
    }

    it "behave like printf"
    {
      v = fdja_c(
        "host: %s\n"
        "whitelist: [ %s ]\n",
        "heliopolis", "crane"
      );

      expect(fdja_to_json(v) ===f ""
        "{\"host\":\"heliopolis\",\"whitelist\":[\"crane\"]}");
    }
  }
}

