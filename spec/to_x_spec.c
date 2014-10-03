
//
// specifying djan
//
// Thu Oct  2 20:54:34 JST 2014
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

  describe "fdja_to_json()"
  {
    it "turns a fdja_value to a json string"
    {
      v = fdja_v("{ type: car, make: mitsubishi, id: 2 }");

      expect(fdja_to_json(v) ===f ""
        "{\"type\":\"car\",\"make\":\"mitsubishi\",\"id\":2}");
    }
  }

  describe "fdja_to_djan()"
  {
    it "turns a fdja_value to a djan string"
    {
      v = fdja_v(
        "{"
          "type: car, "
          "\"make/brand\": mitsubishi, "
          "id: 2, "
          "ok: true"
          "suppliers: [ ]"
        "}"
      );

      expect(fdja_to_djan(v) ===f ""
        "{ "
          "type: car, "
          "\"make/brand\": mitsubishi, "
          "id: 2, "
          "ok: true, "
          "suppliers: []"
        " }"
      );
    }
  }
}

