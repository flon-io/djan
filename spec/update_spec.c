
//
// specifying djan
//
// Wed Oct  1 09:53:00 JST 2014
//

#include "djan.h"

//int dja_push(dja_value *array, dja_value *v);
//int dja_set(dja_value *object, const char *key, dja_value *v);

context "update"
{
  before each
  {
    dja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) dja_value_free(v);
  }

  describe "dja_push()"
  {
    it "pushes at the end of the target array"
    {
      v = dja_parse("[ 0, 1, 2 ]");
      int r = dja_push(v, dja_v("3"));

      expect(r == 1);
      expect(dja_to_json(v) ===f "[0,1,2,3]");
    }

    it "pushes into empty arrays"
    {
      v = dja_parse("[]");
      int r = dja_push(v, dja_v("\"hello\""));

      expect(r == 1);
      expect(dja_to_json(v) ===f "[\"hello\"]");
    }

    it "returns 0 if the target isn't an array"
    {
      v = dja_parse("{}");
      int r = dja_push(v, dja_v("\"nada\""));

      expect(r == 0);
    }
  }

  describe "dja_set()"
  {
    it "works"
    {
      v = dja_parse("{ a: 0, b: 1, c: 2 }");

      pending();
    }
  }
}

