
//
// specifying djan
//
// Wed May 28 06:07:18 JST 2014
//

#include "djan.h"


context "dja_value"
{
  before each
  {
    dja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) dja_value_free(v);
  }

  describe "dja_size()"
  {
    it "returns how many children a value has"
    {
      v = dja_parse("[ 0, 7, 14, 28, 35 ]");

      ensure(dja_size(v) == 5);
    }

    it "returns 0 for an atom"
    {
      v = dja_parse("true");

      ensure(dja_size(v) == 0);
    }
  }

  describe "dja_lookup()"
  {
    it "looks up in arrays"
    {
      v = dja_parse("[ 0, 7, 14, 28, 35 ]");

      ensure(dja_to_int(dja_lookup(v, "0")) == 0);
      ensure(dja_to_int(dja_lookup(v, "4")) == 35);
      ensure(dja_lookup(v, "5") == NULL);
    }

    it "counts from the end of the array when the index is negative"
    {
      v = dja_parse("[ 0, 7, 14, 28, 35 ]");

      ensure(dja_to_int(dja_lookup(v, "-1")) == 35);
      ensure(dja_to_int(dja_lookup(v, "-2")) == 28);
      ensure(dja_lookup(v, "-6") == NULL);
    }

    it "looks up in objects"
    {
      v = dja_parse("{ type: car, color: blue, ids: [ 123, 456 ] }");

      ensure(dja_string(dja_lookup(v, "type")) ===f "car");
      ensure(dja_string(dja_lookup(v, "color")) ===f "blue");
      ensure(dja_to_int(dja_lookup(v, "ids.1")) == 456);
      ensure(dja_to_int(dja_lookup(v, "ids.-1")) == 456);
      ensure(dja_to_int(dja_lookup(v, "ids.-2")) == 123);
      ensure(dja_lookup(v, "ids.-3") == NULL);
      ensure(dja_lookup(v, "type.name") == NULL);
      ensure(dja_lookup(v, "nada") == NULL);
    }
  }
}

