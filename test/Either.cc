#include "gtest/gtest.h"
#include "funky/Either.hh"

#include <memory>
#include <string>

using namespace funky;

namespace {

  Either<int, std::unique_ptr<int>> Eint1() {
    return 1;
  }

  Either<int, std::unique_ptr<int>> EUptr30() {
    return std::unique_ptr<int>{new int(30)};
  }


  TEST(Either, Basics) {

    Either<int, std::unique_ptr<int>> v1{Eint1()};

    EXPECT_TRUE(v1.isLeft());

    EXPECT_EQ(1, v1.left());

    v1 = EUptr30();

    EXPECT_TRUE(v1.isRight());
    EXPECT_EQ(30, *v1.right());

    Either<int, std::unique_ptr<int>> v2{std::move(v1)};

    EXPECT_TRUE(v2.isRight());
    EXPECT_TRUE(v1.isRight()); // still should be Right.

    EXPECT_EQ(nullptr, v1.getLeftPointer());
    EXPECT_EQ(nullptr, v2.getLeftPointer());

    EXPECT_EQ(nullptr, v1.right().get()); // should have been moved.

    EXPECT_EQ(30, *v2.right());

    v2 = 90;

    EXPECT_TRUE(v2.isLeft());

    EXPECT_EQ(90, v2.left());

  }


  TEST(Either, Emplacement) {

    Either<int, std::unique_ptr<int>> v0{0};

    v0.emplaceRight(new int(10));
    EXPECT_TRUE(v0.isRight());
    EXPECT_EQ(10, *v0.right());

    v0.emplace<std::unique_ptr<int>>(new int(2));
    EXPECT_TRUE(v0.isRight());
    EXPECT_EQ(2, *v0.right());

    Either<int, std::unique_ptr<int>> v1{RightTag{}, new int(5)};
    EXPECT_TRUE(v1.isRight());
    EXPECT_EQ(5, *v1.right());

  }

}
