
//
// specifying djan
//
// Sun Sep 28 17:44:25 JST 2014
//

#include "djan.h"


describe "dja_parse_obj()"
{
  before each
  {
    dja_value *v = NULL;
  }
  after each
  {
    if (v != NULL) dja_value_free(v);
  }

  it "returns NULL when failing to parse"
  {
    v = dja_parse_obj("123");

    ensure(v == NULL);
  }

  it "parses"
  {
    v = dja_parse_obj(""
      "type: car\n"
      "make: honda\n"
      "parts: [ 123, 456, 789, whiskey ]\n"
      "details: { name: beetle, year: 1969 }"
    );

    ensure(v != NULL);

    ensure(dja_to_json(v) ===f ""
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
    v = dja_parse_obj(""
      "{\n"
      "  type: car\n"
      "  make: honda\n"
      "  parts: [ 123, 456, 789, whiskey ]\n"
      "  details: { name: beetle, year: 1969 }"
      "}"
    );

    ensure(v != NULL);

    ensure(dja_to_json(v) ===f ""
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
    v = dja_parse_obj(""
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

    ensure(dja_to_json(v) ===f ""
      "{"
        "\"type\":\"car\","
        "\"make\":\"honda\","
        "\"parts\":[123,456,789,\"whiskey\"],"
        "\"details\":{\"name\":\"beetle\",\"year\":1969}"
      "}"
    );
  }
}

