
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
      dja_value *vv = NULL;
      v = dja_parse("[ 0, 7, 14, 28, 35 ]");

      vv = dja_lookup(v, "0");
      ensure(dja_to_int(vv) == 0);

      vv = dja_lookup(v, "4");
      ensure(dja_to_int(vv) == 35);

      vv = dja_lookup(v, "5");
      ensure(vv == NULL);
    }

    it "counts from the end of the array when the index is negative"
    {
      dja_value *vv = NULL;
      v = dja_parse("[ 0, 7, 14, 28, 35 ]");

      vv = dja_lookup(v, "-1");
      ensure(dja_to_int(vv) == 35);

      vv = dja_lookup(v, "-2");
      ensure(dja_to_int(vv) == 28);

      vv = dja_lookup(v, "-6");
      ensure(vv == NULL);
    }
  }
}

