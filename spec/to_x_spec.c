
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
      v = fdja_v("{ type: car, make: mitsubishi, id: 2, l: [] }");

      expect(fdja_to_json(v) ===f ""
        "{\"type\":\"car\",\"make\":\"mitsubishi\",\"id\":2,\"l\":[]}");
    }

    it "doesn't print the key when printing an entry on its own"
    {
      v = fdja_v("{ type: car, make: mitsubishi, id: 2 }");
      fdja_value *vv = fdja_lookup(v, "type");

      expect(fdja_to_json(vv) ===f "\"car\"");
    }

    it "preserves escapes"
    {
      v = fdja_v("\"na\\nda\"");

      expect(fdja_string(v) ===f "\"na\\nda\"");
      expect(fdja_to_string(v) ===f "na\nda");
      expect(fdja_to_json(v) ===f "\"na\\nda\"");
      expect(fdja_tod(v) ===f "\"na\\nda\"");
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

  describe "fdja_to_djan() oneline"
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
          "list: []"
        "}"
      );

      expect(v != NULL);

      expect(fdja_to_djan(v, FDJA_F_ONELINE) ===f ""
        "{ "
          "type: car, "
          "make/brand: mitsubishi, "
          "id: 2, "
          "ok: true, "
          "\"suppliers,\": [], "
          "stuff: nada, "
          "branding: fail, "
          "x: \"4\", "
          "list: []"
        " }"
      );
    }

    it "doesn't print the key when the entry is on its own"
    {
      v = fdja_v("{ type: car, make: mitsubishi, id: 2 }");
      fdja_value *vv = fdja_lookup(v, "type");

      expect(fdja_to_djan(vv, FDJA_F_ONELINE) ===f "car");
    }
  }

  describe "fdja_to_djan() multiline"
  {
    it "turns a fdja_value to a pretty djan string"
    {
      v = fdja_v(
        "{"
          "type: car, "
          "\"make\\/brand\": mitsubishi, "
          "id: 2, "
          "ok: true"
          "\"suppliers,\": [ alpha, bravo, charly, \"4\", 3 ]"
          "list: []"
          "stuff: nada"
          "'branding': fail "
          "0: ok"
        "}"
      );

      expect(v != NULL);

      expect(fdja_to_djan(v, 0) ===f ""
        "{\n"
        "  type: car\n"
        "  \"make\\/brand\": mitsubishi\n"
        "  id: 2\n"
        "  ok: true\n"
        "  \"suppliers,\": [ alpha, bravo, charly, \"4\", 3 ]\n"
        "  list: []\n"
        "  stuff: nada\n"
        "  branding: fail\n"
        "  0: ok\n"
        "}"
      );
    }
  }

  describe "fdja_to_djan() colour output"
  {
    it "is nice"
    {
      v = fdja_v(
        "{"
          "type: car, "
          "\"make/brand\": mitsubishi, "
          "id: 2, "
          "ok: true"
          "\"suppliers,\": [ alpha, bravo, charly, \"4\", 3 ]"
          "list: []"
          "stuff: nada"
          "'branding': fail "
          "people: { owner: Jeff, driver: Coop, wing: Herman }"
          "0: ok\n"
          "x: false"
        "}"
      );

      expect(v != NULL);

      char *s;

      //s = fdja_tod(v); puts(s); free(s);

      s = fdja_tod(v);
      expect(strstr(s, "0;33m") == NULL);
      expect(strstr(s, "\n") == NULL);
      free(s);

      //s = fdja_todc(v); puts(s); free(s);

      s = fdja_todc(v);
      expect(strstr(s, "0;33m") != NULL);
      expect(strstr(s, "\n") != NULL);
      free(s);
    }
  }

  describe "fdja_f_todc()"
  {
    it "reads a file and outputs it as coloured djan"
    {
      char *s = fdja_f_todc("../spec/%s", "_test1.json");

      //puts(s);
      expect(strstr(s, "0;33m") != NULL);
      expect(strstr(s, "12345") != NULL);
      expect(strstr(s, "\n") == NULL);
      free(s);
    }

    it "returns NULL if it cannot read"
    {
      expect(fdja_f_todc("../spec/_nada.json") == NULL);
    }
  }
}

