
//
// specifying djan
//
// Thu Oct  2 20:54:34 JST 2014
//

#include "flutil.h"
#include "djan.h"


context "to_x"
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

    it "doesn't print the key when printing an entry on its own"
    {
      v = fdja_v("{ type: car, make: mitsubishi, id: 2 }");
      fdja_value *vv = fdja_lookup(v, "type");

      expect(fdja_to_json(vv) ===f "\"car\"");
    }
  }

  describe "fdja_to_json_f()"
  {
    it "writes a value to a file"
    {
      v = fdja_v("{ type: car, make: subaru, id: 3 }");

      int r = fdja_to_json_f(v, "./to_json_f.json");

      expect(r == 1);
      char *s = flu_readall("./to_json_f.json");

      expect(s ===f ""
        "{\"type\":\"car\",\"make\":\"subaru\",\"id\":3}");

      expect(unlink("./to_json_f.json") == 0);
    }

    it "composes filenames"
    {
      v = fdja_v("{ type: car, make: daihatsu, id: 4 }");

      fdja_to_json_f(v, "./to_json_f_%i.json", 2);

      expect(flu_readall("./to_json_f_2.json") ===f ""
        "{\"type\":\"car\",\"make\":\"daihatsu\",\"id\":4}");

      expect(unlink("./to_json_f_2.json") == 0);
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
          "\"suppliers,\": [ ]"
          "stuff: nada"
          "'branding': fail,"
          "x: \"4\""
        "}"
      );

      expect(fdja_to_djan(v) ===f ""
        "{ "
          "type: car, "
          "make/brand: mitsubishi, "
          "id: 2, "
          "ok: true, "
          "\"suppliers,\": [], "
          "stuff: nada, "
          "branding: fail, "
          "x: \"4\""
        " }"
      );
    }

    it "doesn't print the key when the entry is on its own"
    {
      v = fdja_v("{ type: car, make: mitsubishi, id: 2 }");
      fdja_value *vv = fdja_lookup(v, "type");

      expect(fdja_to_djan(vv) ===f "car");
    }
  }

  describe "fdja_to_pretty_djan()"
  {
    it "turns a fdja_value to a pretty djan string"
    {
      v = fdja_v(
        "{"
          "type: car, "
          "\"make/brand\": mitsubishi, "
          "id: 2, "
          "ok: true"
          "\"suppliers,\": [ alpha, bravo, charly, \"4\", 3 ]"
          "stuff: nada"
          "'branding': fail"
        "}"
      );

      expect(fdja_to_pretty_djan(v) ===f ""
        "{\n"
        "  type: car\n"
        "  make/brand: mitsubishi\n"
        "  id: 2\n"
        "  ok: true\n"
        "  \"suppliers,\": [\n"
        "    alpha\n"
        "    bravo\n"
        "    charly\n"
        "    \"4\"\n"
        "    3\n"
        "  ]\n"
        "  stuff: nada\n"
        "  branding: fail\n"
        "}"
      );
    }
  }
}

