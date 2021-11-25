#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "json.hpp"

using nlohmann::json;

namespace ns {
struct person {
  std::string name;
  std::string address;
  int age;
};
// void to_json(json& j, const person& p) {
//   j = json{{"name", p.name}, {"address", p.address}, {"age", p.age}};
// }
// void from_json(const json& j, person& p) {
//   j.at("name").get_to(p.name);
//   j.at("address").get_to(p.address);
//   j.at("age").get_to(p.age);
// }
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(person, name, address, age)

class address {
 private:
  std::string street;
  int housenumber;
  int postcode;

 public:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(address, street, housenumber, postcode)
};
}  // namespace ns

enum TaskState1 {
  TS_STOPPED1,
};
enum TaskState {
  TS_STOPPED,
  TS_RUNNING,
  TS_COMPLETED,
  TS_INVALID = -1,
};
NLOHMANN_JSON_SERIALIZE_ENUM(TaskState, {
                                            {TS_INVALID, nullptr},
                                            {TS_STOPPED, "stopped"},
                                            {TS_RUNNING, "running"},
                                            {TS_COMPLETED, "completed"},
                                        })

int main() {
  {
    json j;
    j["int"] = 5;
    j["float"] = 3.14;
    j["bool"] = true;
    j["string"] = "hello nlohmann json";
    j["nothing"] = nullptr;
    j["answer"]["everything"] = 42;
    j["array"] = {0, 1, 3};
    j["object"] = {{"str", "str in object"}, {"value", 48}};

    std::cout << j.dump(2) << std::endl;
    /*
    {
      "answer": {
        "everything": 42
      },
      "array": [
        0,
        1,
        3
      ],
      "bool": true,
      "float": 3.14,
      "int": 5,
      "nothing": null,
      "object": {
        "str": "str in object",
        "value": 48
      },
      "string": "hello nlohmann json"
    }
    */
  }

  {
    json j = {{"pi", 3.141},
              {"happy", true},
              {"name", "Niels"},
              {"nothing", nullptr},
              {"answer", {{"everything", 42}}},
              {"list", {1, 0, 2}},
              {"object", {{"currency", "USD"}, {"value", 42.99}}}};
    std::cout << j.dump() << std::endl;
    // {"answer":{"everything":42},"happy":true,"list":[1,0,2],"name":"Niels","nothing":null,"object":{"currency":"USD","value":42.99},"pi":3.141}
  }

  {
    json empty_array_explicit = json::array();
    std::cout << empty_array_explicit.dump() << std::endl;  // []
    json empty_object_implicit = json({});
    std::cout << empty_object_implicit.dump() << std::endl;  // {}
    json empty_object_explicit = json::object();
    std::cout << empty_object_explicit.dump() << std::endl;  // {}
    json array_not_object =
        json::array({{"currency", "USD"}, {"value", 42.99}});
    std::cout << array_not_object.dump() << std::endl;
    // [["currency","USD"],["value",42.99]]
  }

  {  // Serialization，from strings
    json j1 = "{ \"happy\": true, \"pi\": 3.141 }"_json;
    std::cout << j1.dump() << std::endl;  // {"happy":true,"pi":3.141}

    auto j2 = R"(
  {
    "happy": true,
    "pi": 3.141
  }
)"_json;
    std::cout << j2.dump() << std::endl;  // {"happy":true,"pi":3.141}

    json j3 = "{ \"happy\": true, \"pi\": 3.141 }";  // as string，not a object
    std::cout << j3.dump() << std::endl;
    // "{ \"happy\": true, \"pi\": 3.141 }"

    json j4 = json::parse("{ \"happy\": true, \"pi\": 3.141 }");
    std::cout << j4.dump() << std::endl;  // {"happy":true,"pi":3.141}

    json j5 = json::parse(R"({ "happy": true, "pi": 3.141 })");
    std::cout << j5.dump() << std::endl;  // {"happy":true,"pi":3.141}
  }

  {  // Deserialization, to string
    json j_string = "this is a string";
    auto cpp_string = j_string.get<std::string>();

    std::string cpp_string2;
    j_string.get_to(cpp_string2);

    std::string serialized_string = j_string.dump();

    std::cout << cpp_string << " == " << cpp_string2
              << " == " << j_string.get<std::string>() << std::endl;
    // this is a string == this is a string == this is a string
    std::cout << j_string << " == " << serialized_string << std::endl;
    // "this is a string" == "this is a string"
  }

  {
      // Serialization To/from streams
      // json j;
      // std::cin >> j;                // {"nihao":4}
      // std::cout << j << std::endl;  // {"nihao":4}
      // std::cout << std::setw(4) << j << std::endl;
      /*
      {
          "nihao": 4
      }
      */
  }

  {
    std::ifstream i("demo.json");
    json j;
    i >> j;
    std::cout << j << std::endl;  // {"nihao":4}
    j["tahao"] = "xxx";
    std::ofstream o("pretty.json");
    o << std::setw(4) << j << std::endl;
  }

  {
    std::vector<std::uint8_t> v = {'t', 'r', 'u', 'e'};
    json j1 = json::parse(v.begin(), v.end());
    std::cout << j1 << std::endl;

    std::list<std::uint8_t> l = {'f', 'a', 'l', 's', 'e'};
    json j2 = json::parse(l);
    std::cout << j2 << std::endl;
  }

  {  // STL-like access
    json j;
    j.push_back("foo");
    j.push_back(1);
    j.push_back(true);
    j.emplace_back(1.78);
    std::cout << j << std::endl;  // ["foo",1,true,1.78]

    for (json::iterator it = j.begin(); it != j.end(); ++it) {
      std::cout << *it << ',';
    }
    std::cout << std::endl;  // "foo",1,true,1.78,

    for (auto& element : j) {
      std::cout << element << ',';
    }
    std::cout << std::endl;  // "foo",1,true,1.78,

    const auto tmp = j[0].get<std::string>();
    j[1] = 42;
    bool foo = j.at(2);
    std::cout << j << std::endl;                  // ["foo",42,true,1.78]
    std::cout << tmp << " " << foo << std::endl;  // foo 1

    j == R"(["foo", 1, true, 1.78])"_json;                  // true
    std::cout << j.size() << std::endl;                     // 4
    std::cout << std::boolalpha << j.empty() << std::endl;  // false
    std::cout << static_cast<int>(j.type())
              << std::endl;  // 2 : json::value_t::array
    j.clear();
    std::cout << j.size() << std::endl;                     // 0
    std::cout << std::boolalpha << j.empty() << std::endl;  // true

    // convenience type checkers
    std::cout << std::boolalpha << j.is_null() << std::endl;     // false
    std::cout << std::boolalpha << j.is_boolean() << std::endl;  // false
    std::cout << std::boolalpha << j.is_number() << std::endl;   // false
    std::cout << std::boolalpha << j.is_object() << std::endl;   // false
    std::cout << std::boolalpha << j.is_array() << std::endl;    // true
    std::cout << std::boolalpha << j.is_string() << std::endl;   // false

    // create an object
    json o;
    o["foo"] = 23;
    o["bar"] = false;
    o["baz"] = 3.141;
    o.emplace("weather", "sunny");

    for (json::iterator it = o.begin(); it != o.end(); ++it) {
      std::cout << it.key() << " : " << it.value() << "\n";
    }
    // bar : false
    // baz : 3.141
    // foo : 23
    // weather : "sunny"

    for (auto& el : o.items()) {
      std::cout << el.key() << " : " << el.value() << "\n";
    }
    // bar : false
    // baz : 3.141
    // foo : 23
    // weather : "sunny"

    // even easier with structured bindings (C++17)
    for (auto& [key, value] : o.items()) {
      std::cout << key << " : " << value << "\n";
    }
    // bar : false
    // baz : 3.141
    // foo : 23
    // weather : "sunny"

    if (o.contains("foo")) {
      std::cout << "found" << std::endl;  // found
    }
    if (o.find("foo") != o.end()) {
      std::cout << "found" << std::endl;  // found
    }

    std::cout << o.count("foo") << std::endl;  // 1
    std::cout << o.count("fob") << std::endl;  // 0
    o.erase("foo");
    std::cout << o.count("foo") << std::endl;  // 0
  }

  {  // Conversion from STL containers
    std::vector<int> c_vector{1, 2, 3, 4};
    json j_vec(c_vector);
    std::cout << j_vec << std::endl;  // [1,2,3,4]

    std::deque<double> c_deque{1.2, 2.3, 3.4, 5.6};
    json j_deque(c_deque);
    std::cout << j_deque << std::endl;  // [1.2,2.3,3.4,5.6]

    std::list<bool> c_list{true, true, false, true};
    json j_list(c_list);
    std::cout << j_list << std::endl;  // [true,true,false,true]

    std::forward_list<int64_t> c_flist{12345678909876, 23456789098765,
                                       34567890987654, 45678909876543};
    json j_flist(c_flist);
    std::cout << j_flist << std::endl;
    // [12345678909876,23456789098765,34567890987654,45678909876543]

    std::array<unsigned long, 4> c_array{{1, 2, 3, 4}};
    json j_array(c_array);
    std::cout << j_array << std::endl;  // [1,2,3,4]

    std::set<std::string> c_set{"one", "two", "three", "four", "one"};
    json j_set(c_set);
    std::cout << j_set << std::endl;  // ["four","one","three","two"]

    std::unordered_set<std::string> c_uset{"one", "two", "three", "four",
                                           "one"};
    json j_uset(c_uset);
    std::cout << j_uset << std::endl;  // ["four","three","two","one"]

    std::multiset<std::string> c_mset{"one", "two", "one", "four"};
    json j_mset(c_mset);
    std::cout << j_mset << std::endl;  // ["four","one","one","two"]

    std::unordered_multiset<std::string> c_umset{"one", "two", "one", "four"};
    json j_umset(c_umset);
    std::cout << j_umset << std::endl;  // ["four","two","one","one"]

    std::map<std::string, int> c_map{{"one", 1}, {"two", 2}, {"three", 3}};
    json j_map(c_map);
    std::cout << j_map << std::endl;  // {"one":1,"three":3,"two":2}

    std::unordered_map<const char*, double> c_umap{
        {"one", 1.2}, {"two", 2.3}, {"three", 3.4}};
    json j_umap(c_umap);
    std::cout << j_umap << std::endl;  // {"one":1.2,"three":3.4,"two":2.3}

    std::multimap<std::string, bool> c_mmap{
        {"one", true}, {"two", true}, {"three", false}, {"three", true}};
    json j_mmap(c_mmap);
    std::cout << j_mmap << std::endl;  // {"one":true,"three":false,"two":true}

    std::unordered_multimap<std::string, bool> c_ummap{
        {"one", true}, {"two", true}, {"three", false}, {"three", true}};
    json j_ummap(c_ummap);
    std::cout << j_ummap << std::endl;  // {"one":true,"three":true,"two":true}
  }

  {  // JSON Pointer and JSON Patch
    json j_original = R"({
  "baz": ["one", "two", "three"],
  "foo": "bar"
})"_json;

    std::cout << j_original["/baz/1"_json_pointer] << std::endl;  // "two"

    json j_patch = R"([
  { "op": "replace", "path": "/baz", "value": "boo" },
  { "op": "add", "path": "/hello", "value": ["world"] },
  { "op": "remove", "path": "/foo"}
])"_json;

    json j_result = j_original.patch(j_patch);
    std::cout << j_result << std::endl;  // {"baz":"boo","hello":["world"]}

    std::cout << json::diff(j_result, j_original) << std::endl;
    // [
    //   { "op":" replace", "path": "/baz", "value": ["one", "two", "three"] },
    //   { "op": "remove","path": "/hello" },
    //   { "op": "add", "path": "/foo", "value": "bar" }
    // ]

    json j_document = R"({
        "a": "b",
        "c": {
        "d": "e",
        "f": "g"
    }
    })"_json;
    json j_patch1 = R"({
        "a":"z",
        "c": {
        "f": null
    }
    })"_json;
    j_document.merge_patch(j_patch1);
    std::cout << j_document << std::endl;  // {"a":"z","c":{"d":"e"}}
  }

  {  // Implicit conversions
    // strings
    std::string s1 = "Hello, world!";
    json js = s1;
    auto s2 = js.get<std::string>();
    std::string s3 = js;
    std::string s4;
    s4 = js;

    // Booleans
    bool b1 = true;
    json jb = b1;
    auto b2 = jb.get<bool>();
    bool b3 = jb;
    bool b4;
    b4 = jb;

    // numbers
    int i = 42;
    json jn = i;
    auto f = jn.get<double>();
    double f2 = jn;
    double f3;
    f3 = jn;

    char ch = 'A';
    json j_default = ch;
    std::cout << j_default << std::endl;  // 65
    json j_string = std::string(1, ch);
    std::cout << j_string << std::endl;  // "A"
  }

  {  // Arbitrary types conversions
    // struct person {
    //   std::string name;
    //   std::string address;
    //   int age;
    // };
    ns::person p = {"Ned Flanders", "744 Evergreen Terrace", 60};
    json j;
    j["name"] = p.name;
    j["address"] = p.address;
    j["age"] = p.age;
    std::cout << j << std::endl;
    // {"address":"744 Evergreen Terrace","age":60,"name":"Ned Flanders"}
    ns::person per{j["name"].get<std::string>(),
                   j["address"].get<std::string>(), j["age"].get<int>()};

    json j2 = p;
    std::cout << j2 << std::endl;
    // {"address":"744 Evergreen Terrace","age":60,"name":"Ned Flanders"}
    auto per2 = j2.get<ns::person>();
  }

  {  // Specializing enum conversion

    json j1 = TaskState1::TS_STOPPED1;
    std::cout << j1 << std::endl;  // 0
    json j2 = TaskState::TS_STOPPED;
    std::cout << j2 << std::endl;  // "stopped"

    json j3 = "running";
    if (j3.get<TaskState>() == TS_RUNNING) {
      std::cout << "ok" << std::endl;  // ok
    }
    json jPi = 3.14;
    if (jPi.get<TaskState>() == TS_INVALID) {
      std::cout << "ok" << std::endl;  // ok
    }
  }

  {  // Binary formats
    json j = R"({"compact": true, "schema": 0})"_json;
    std::vector<std::uint8_t> v_bson = json::to_bson(j);
    // 0x1B, 0x00, 0x00, 0x00, 0x08, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74,
    // 0x00, 0x01, 0x10, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x00, 0x00, 0x00,
    // 0x00, 0x00, 0x00

    json j_from_bson = json::from_bson(v_bson);
    std::vector<std::uint8_t> v_cbor = json::to_cbor(j);
    // 0xA2, 0x67, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0xF5, 0x66, 0x73,
    // 0x63, 0x68, 0x65, 0x6D, 0x61, 0x00

    json j_from_cbor = json::from_cbor(v_cbor);
    std::vector<std::uint8_t> v_msgpack = json::to_msgpack(j);
    // 0x82, 0xA7, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0xC3, 0xA6, 0x73,
    // 0x63, 0x68, 0x65, 0x6D, 0x61, 0x00

    json j_from_msgpack = json::from_msgpack(v_msgpack);
    std::vector<std::uint8_t> v_ubjson = json::to_ubjson(j);
    // 0x7B, 0x69, 0x07, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x63, 0x74, 0x54, 0x69,
    // 0x06, 0x73, 0x63, 0x68, 0x65, 0x6D, 0x61, 0x69, 0x00, 0x7D

    json j_from_ubjson = json::from_ubjson(v_ubjson);
  }

  {
    std::vector<std::uint8_t> v = {0x42, 0xCA, 0xFE};
    json j = json::from_cbor(v);
    j.is_binary();  // true
    auto& binary = j.get_binary();
    binary.has_subtype();  // false
    binary.size();         // 2
    binary[0];             // 0xCA
    binary[1];             // 0xFE
    binary.set_subtype(0x10);
    auto cbor = json::to_msgpack(j);  // 0xD5 (fixext2), 0x10, 0xCA, 0xFE
  }

  return 0;
}