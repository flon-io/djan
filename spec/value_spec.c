
//
// specifying djan
//
// Wed May 28 06:07:18 JST 2014
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
    fdja_value_free(v);
  }

  describe "fdja_size()"
  {
    it "returns how many children a value has"
    {
      v = fdja_dparse("[ 0, 7, 14, 28, 35 ]");

      ensure(fdja_size(v) == 5);
    }

    it "returns 0 for an atom"
    {
      v = fdja_dparse("true");

      ensure(fdja_size(v) == 0);
    }
  }

  describe "fdja_clone()"
  {
    before each
    {
      fdja_value *v2 = NULL;
    }
    after each
    {
      fdja_value_free(v2);
    }

    it "copies entirely a fdja_value"
    {
      v = fdja_v("{\"a\":true,\"b\":[1,2,\"trois\"],\"c\":\"nada\"}");

      v2 = fdja_clone(v);

      expect(v2 != NULL);
      expect(v->source != v2->source);
      expect(v->source === v2->source);
      expect(v2->sowner == 1);
    }

    it "copies entirely a fdja_value (not owner)"
    {
      v = fdja_v("{\"a\":true,\"b\":[1,2,\"trois\"],\"c\":\"nada\"}");

      fdja_value *v1 = fdja_lookup(v, "b");

      v2 = fdja_clone(v1);

      expect(v2 != NULL);
      //expect(v1->source != v2->source); // pointless
      expect(fdja_to_json(v1) ===f v2->source);
      expect(v1->sowner == 0);
      expect(v2->sowner == 1);
    }

    it "returns NULL if the input is NULL"
    {
      expect(fdja_clone(NULL) == NULL);
    }
  }

  describe "fdja_free()"
  {
    it "is an alias to fdja_value_free()"
    {
      fdja_value *vv = fdja_v("{}");
      fdja_free(vv);

      // Valgrind to the rescue...
    }

    it "doesn't mind NULL fdja_value pointers"
    {
      fdja_value_free(NULL);
      fdja_free(NULL);
    }
  }

  describe "fdja_src()"
  {
    it "returns a pointer to the beginning of the source string of the value"
    {
      v = fdja_v("{\"a\":true,\"b\":[1,2,\"trois\"],\"c\":\"nada\"}");

      fdja_value *v1 = fdja_lookup(v, "b");

      expect(fdja_src(v1) ^== "[1,2,\"trois\"],\"c\":");
        // yes, it's a pointer inside the source
    }
  }

  describe "fdja_srk()"
  {
    it "behaves like fjda_src() but skips the initial \" or ' if present"
    {
      v = fdja_v("{\"a\":true,\"b\":[1,2,\"trois\"],\"c\":\"nada\"}");

      fdja_value *v1 = fdja_lookup(v, "b.2");

      expect(fdja_srk(v1) ^== "trois\"],\"c\":");
        // yes, it's a pointer inside the source

      v1 = fdja_lookup(v, "b");

      expect(fdja_srk(v1) ^== "[1,2,\"trois\"],\"c\":");
    }
  }

  describe "fdja_cmp()"
  {
    before each
    {
      fdja_value *v1 = NULL;
    }
    after each
    {
      fdja_value_free(v1);
    }

    it "returns 0 if a and b are the same"
    {
      v = fdja_v("{ a: true }");

      expect(fdja_cmp(v, v) i== 0);
    }

    it "returns 0 if a and b have the same JSON representation"
    {
      v = fdja_v("{ a: true }");
      v1 = fdja_v("{ 'a': true }");

      expect(fdja_cmp(v, v1) i== 0);
    }

    it "returns some non-zero value else"
    {
      v = fdja_v("{ a: true }");
      v1 = fdja_v("{ b: true }");

      expect(fdja_cmp(v, v1) < 0);
    }

    it "returns 0 if both values point to NULL"
    {
      expect(fdja_cmp(NULL, NULL) i== 0);
    }

    it "returns -1 if 1! of the values point to NULL"
    {
      v = fdja_v("true");

      expect(fdja_cmp(v, NULL) i== -1);
      expect(fdja_cmp(NULL, v) i== -1);
    }
  }

  describe "fdja_strcmp()"
  {
    it "returns -1 if the value isn't a string or symbol"
    {
      fdja_free(v); v = fdja_v("true");
      expect(fdja_strcmp(v, "true") i== -1);

      fdja_free(v); v = fdja_v("[]");
      expect(fdja_strcmp(v, "[]") i== -1);
    }

    it "returns 0 in case of match"
    {
      fdja_free(v); v = fdja_v("\"chf\"");
      expect(fdja_strcmp(v, "chf") i== 0);

      fdja_free(v); v = fdja_v("'eur'");
      expect(fdja_strcmp(v, "eur") i== 0);

      fdja_free(v); v = fdja_v("jpy");
      expect(fdja_strcmp(v, "jpy") i== 0);
    }

    it "returns some non zero int else"
    {
      fdja_free(v); v = fdja_v("\"chf\"");
      expect(fdja_strcmp(v, "Chf") i!= 0);

      fdja_free(v); v = fdja_v("'eur'");
      expect(fdja_strcmp(v, "Eur") i!= 0);

      fdja_free(v); v = fdja_v("jpy");
      expect(fdja_strcmp(v, "Jpy") i!= 0);
    }

    it "doesn't overflow the source"
    {
      v = fdja_v("chff");
      v->slen = 3;

      expect(fdja_strcmp(v, "chff") i!= 0);
    }

    it "considers the whole source span"
    {
      v = fdja_v("chff");

      expect(fdja_strcmp(v, "chf") i!= 0);
    }
  }

  describe "fdja_strncmp()"
  {
    it "returns -1 if the value isn't a string or symbol"
    {
      fdja_free(v); v = fdja_v("true");
      expect(fdja_strncmp(v, "tr", 2) i== -1);

      fdja_free(v); v = fdja_v("[]");
      expect(fdja_strncmp(v, "[", 1) i== -1);
    }

    it "flips burgers"
  }

  describe "fdja_is_stringy()"
  {
    it "returns 1 if v is a string, a single quote string or a symbol"
    {
      fdja_free(v); v = fdja_v("\"nada\"");
      expect(fdja_is_stringy(v) i== 1);

      fdja_free(v); v = fdja_v("'nada'");
      expect(fdja_is_stringy(v) i== 1);

      fdja_free(v); v = fdja_v("nada");
      expect(fdja_is_stringy(v) i== 1);
    }

    it "returns 0 else"
    {
      fdja_free(v); v = fdja_v("null");
      expect(fdja_is_stringy(v) i== 0);

      fdja_free(v); v = fdja_v("[ \"nada\" ]");
      expect(fdja_is_stringy(v) i== 0);

      fdja_free(v); v = fdja_v("{ a: 0 }");
      expect(fdja_is_stringy(v) i== 0);

      fdja_free(v); v = fdja_v("1.0");
      expect(fdja_is_stringy(v) i== 0);

      fdja_free(v); v = fdja_v("true");
      expect(fdja_is_stringy(v) i== 0);

      fdja_free(v); v = fdja_v("false");
      expect(fdja_is_stringy(v) i== 0);
    }
  }
}

