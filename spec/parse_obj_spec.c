
//
// specifying djan
//
// Sun Sep 28 17:44:25 JST 2014
//

#include "djan.h"


describe "fdja_parse_obj()"
{
  before each
  {
    fdja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) fdja_value_free(v);
  }

  it "returns NULL when failing to parse"
  {
    v = fdja_parse_obj("123");

    ensure(v == NULL);
  }

  it "parses"
  {
    v = fdja_parse_obj(""
      "type: car\n"
      "make: honda\n"
      "parts: [ 123, 456, 789, whiskey ]\n"
      "details: { name: beetle, year: 1969 }"
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "{"
        "\"type\":\"car\","
        "\"make\":\"honda\","
        "\"parts\":[123,456,789,\"whiskey\"],"
        "\"details\":{\"name\":\"beetle\",\"year\":1969}"
      "}"
    );
  }

  it "parses with brackets"
  {
    v = fdja_parse_obj(""
      "{\n"
      "  type: car\n"
      "  make: honda\n"
      "  parts: [ 123, 456, 789, whiskey ]\n"
      "  details: { name: beetle, year: 1969 }"
      "}"
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "{"
        "\"type\":\"car\","
        "\"make\":\"honda\","
        "\"parts\":[123,456,789,\"whiskey\"],"
        "\"details\":{\"name\":\"beetle\",\"year\":1969}"
      "}"
    );
  }

  it "parses with comments"
  {
    v = fdja_parse_obj(""
      "#\n"
      "# that conf\n"
      "# Sun Sep 28 19:50:14 JST 2014\n"
      "#\n"
      "  # nada\n"
      //"{\n"
      "  type: car\n"
      "  make: honda\n"
      "  parts: [ 123, 456, 789, whiskey ]\n"
      "  details: { name: beetle, year: 1969 }"
      //"}"
      "# over."
    );

    ensure(v != NULL);

    ensure(fdja_to_json(v) ===f ""
      "{"
        "\"type\":\"car\","
        "\"make\":\"honda\","
        "\"parts\":[123,456,789,\"whiskey\"],"
        "\"details\":{\"name\":\"beetle\",\"year\":1969}"
      "}"
    );
  }

  it "parses json objects"
  {
    v = fdja_parse_obj("{\"name\":\"cinderella\",\"age\":21}");

    expect(v != NULL);

    expect(fdja_to_json(v) ===f "{\"name\":\"cinderella\",\"age\":21}");
  }
}

describe "fdja_parse_obj_f()"
{
  before each
  {
    fdja_value *v = NULL;
  }
  after each
  {
    if (v) fdja_value_free(v);
  }

  it "loads json directly out of files"
  {
    v = fdja_parse_obj_f("../spec/test0.jon");

    expect(v != NULL);
    expect(fdja_to_json(v) ===f "{\"hello\":\"world\"}");

    //free(v->source);
  }

  it "returns NULL in case of problem"
  {
    v = fdja_parse_obj_f("_nada_.json");

    expect(v == NULL);
  }
}

