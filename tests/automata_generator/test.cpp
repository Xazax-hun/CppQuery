#include <string>
#include <set>

#include <gtest/gtest.h>

#include "automata_generator/matcher_traits.hpp"
#include "automata_generator/automata_generator.hpp"

namespace {

// --- Test interface ---

struct A {};
struct B : A {};
struct C {};
struct E : B {};

A *func1(A *) { return 0; }
B *func2(A *, B *) { return 0; }
C *func3(A *) { return 0; }

struct D {
  E *operator()(A *) { return 0; }
};

// For Instantiation test
struct F {
  A *operator()(A *) { return 0; }
  virtual int getTypeID() { return 1; }
  virtual ~F() {}
};

struct G : F {
  A *operator()(A *) { return 0; }
  virtual int getTypeID() { return 2; }
};

struct GetInstancePolicy {
  typedef F *ResultType;

  template <typename T> static ResultType Create() { return new T; }
};

// --- Test interface ---

TEST(SimpleFunctionInterface, First) {
  typedef typename Automata<FUNCTION(func1), FUNCTION(func2),
                            FUNCTION(func3)>::result GeneratedAutomata;
  GeneratedAutomata automata;

  std::set<std::string> expected{ "func1", "func2" };

  auto tmp = automata.GetComposables("func1");

  std::set<std::string> result(tmp.begin(), tmp.end());

  EXPECT_EQ(expected, result);
}

TEST(SimpleFunctionInterfaceWithFunctors, First) {
  typedef typename Automata<FUNCTION(func1), FUNCTION(func2), FUNCTION(func3),
                            FUNCTOR(D)>::result GeneratedAutomata;

  GeneratedAutomata automata;

  std::set<std::string> expected{ "func1", "func2", "D" };

  auto tmp = automata.GetComposables("func1");

  std::set<std::string> result(tmp.begin(), tmp.end());

  EXPECT_EQ(expected, result);
}

TEST(OptimizedMetaString, First) {
  std::string s1 = MetaString<_S("csiga biga")>::GetRuntimeString();
  std::string s2 = MetaString<_S("csiga biga\0\0\0")>::GetRuntimeString();

  EXPECT_EQ(s1, s2);
}

TEST(SimpleFunctionInterfaceWithMultipleParams, First) {
  typedef typename Automata<FUNCTION(func1), FUNCTION(func2), FUNCTION(func3),
                            FUNCTOR(D)>::result GeneratedAutomata;
  GeneratedAutomata automata;

  std::set<std::string> expected{};

  auto tmp = automata.GetComposables("func1", 1);

  std::set<std::string> result(tmp.begin(), tmp.end());

  // Func1 has no second parameter... expect empty result
  EXPECT_EQ(expected, result);
}

TEST(SimpleFunctionInterfaceWithMultipleParams, Second) {
  typedef typename Automata<FUNCTION(func1), FUNCTION(func2), FUNCTION(func3),
                            FUNCTOR(D)>::result GeneratedAutomata;
  GeneratedAutomata automata;

  std::set<std::string> expected{ "func2", "D" };

  auto tmp = automata.GetComposables("func2", 1);

  std::set<std::string> result(tmp.begin(), tmp.end());

  EXPECT_EQ(expected, result);
}

/*TEST(SimpleInstantiationTest, First)
{
  typedef typename Automata<
      FUNCTION(func1),
      FUNCTION(func2),
      FUNCTION(func3),
      FUNCTOR(D)
    >::result GeneratedAutomata;

  GeneratedAutomata automata;

  auto func2 = automata.GetInstance("func2");

  EXPECT_EQ(0, func2(func2(0)));
}*/

/*
TEST(SimpleFunctorInterfaceWithInstantiation, First) {
  typedef typename AutomataWithGetInst<GetInstancePolicy, FUNCTOR(F),
                                       FUNCTOR(G)>::result GeneratedAutomata;

  auto tmp = GeneratedAutomata::GetInstance("G");

  EXPECT_EQ(2, tmp->getTypeID());

  delete tmp;
}
*/

// Matcher related tests
using namespace clang::ast_matchers;

TEST(SimpleMatcherInterfaceTest, First) {
  typedef typename Automata<MATCHER(methodDecl), MATCHER(hasName),
                            MATCHER(namedDecl),
                            MATCHER(qualType)>::result GeneratedAutomata;
  GeneratedAutomata automata;
  std::set<std::string> expected{ "hasName", "namedDecl", "methodDecl" };
  auto tmp = automata.GetComposables("namedDecl");

  std::set<std::string> result(tmp.begin(), tmp.end());

  EXPECT_EQ(expected, result);
}

} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
