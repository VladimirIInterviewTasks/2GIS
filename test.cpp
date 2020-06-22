#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <array>

#include "node.hpp"
#include "datatype.hpp"

using namespace std::string_literals;

TEST(Node, ShouldPrintHelloWorld) {
  Node n {"Hello World"s};
  std::stringstream ss;
  ss << n;
  EXPECT_EQ(ss.str(),"{Hello World}"s);
}

template<typename... Ts>
std::array<std::byte, sizeof...(Ts)> make_bytes(Ts&&... args) noexcept {
    return{std::byte(std::forward<Ts>(args))...};
}

TEST(Node, ShouldSerializeHelloWorld) {
  Node n {"Hello"s};

  std::vector <std::byte> data;
  auto array = make_bytes(DataType::STRING,0,0,0,5,0,0,0,0,0,0,0,'H','e','l','l','o');
  std::vector <std::byte> comp ( array.begin(), array.end());

  n.serialize(data);
  EXPECT_EQ(data,comp);
}


#define XSTR(s) STR(s)
#define STR(s) std::string(#s)
#define TESTSTRING {7,{{9,{{2.015,{{13}}}}}}}

TEST(Node, ShouldCorectPrintExampe) {
  Node n TESTSTRING;
  std::stringstream ss;
  ss << n;
  EXPECT_EQ(ss.str(),XSTR(TESTSTRING));
}


TEST(Node, ShouldSameSerializeDeserialize) {
  Node original {7,{{13},{4}}};
  Node copy;
  std::vector <std::byte> data;

    std::stringstream ssOriginal, ssCopy;
  
  original.serialize(data);
  copy.deserialize(data);

  ssOriginal << original;
  ssCopy << copy;
  EXPECT_EQ(ssOriginal.str(),ssCopy.str());
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}