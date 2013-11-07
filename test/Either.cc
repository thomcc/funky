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

    EXPECT_NE(nullptr, v1.getLeftPointer());
    EXPECT_EQ(nullptr, v1.getRightPointer());

    v1 = EUptr30();

    EXPECT_TRUE(v1.isRight());
    EXPECT_EQ(30, *v1.right());

    Either<int, std::unique_ptr<int>> v2{std::move(v1)};

    EXPECT_TRUE(v2.isRight());
    EXPECT_TRUE(v1.isRight()); // still should be Right.

    EXPECT_EQ(nullptr, v1.getLeftPointer());
    EXPECT_EQ(nullptr, v2.getLeftPointer());

    EXPECT_NE(nullptr, v1.getRightPointer());
    EXPECT_NE(nullptr, v2.getRightPointer());

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

    Either<int, std::unique_ptr<int>> v1{EmplaceRight, new int(5)};
    EXPECT_TRUE(v1.isRight());
    EXPECT_EQ(5, *v1.right());

  }

  TEST(Either, Movement) {

    Either<bool, std::unique_ptr<int>> foo{EmplaceRight, new int(4)};

    EXPECT_TRUE(foo.isRight());
    EXPECT_NE(foo.right(), nullptr);
    EXPECT_EQ(*foo.right(), 4);

    int *pointer = foo.right().get();

    // check move construction
    Either<bool, std::unique_ptr<int>> bar{std::move(foo)};

    EXPECT_TRUE(foo.isRight());
    EXPECT_EQ(foo.right().get(), nullptr);

    EXPECT_TRUE(bar.isRight());
    EXPECT_NE(nullptr, bar.right());

    EXPECT_EQ(*bar.right(), 4);

    EXPECT_EQ(bar.right().get(), pointer);

    // check move assignment
    foo = std::move(bar);

    EXPECT_TRUE(bar.isRight());
    EXPECT_EQ(bar.right().get(), nullptr);

    EXPECT_TRUE(foo.isRight());
    EXPECT_NE(foo.right(), nullptr);
    EXPECT_EQ(*foo.right(), 4);

    EXPECT_EQ(foo.right().get(), pointer);



  }

  TEST(Either, EitherFn) {
    const double DoubleValue = 4.0;
    const bool BoolValue = true;

    Either<bool, double> lefty{BoolValue};
    Either<bool, double> const righty{DoubleValue};

    EXPECT_TRUE(righty.isRight());
    EXPECT_TRUE(lefty.isLeft());

    enum Result { RanRight, RanLeft };

    auto leftFn = [BoolValue](bool b) -> Result {
      EXPECT_EQ(b, BoolValue);
      return RanLeft;
    };

    auto rightFn = [DoubleValue](double d) -> Result {
      EXPECT_EQ(d, DoubleValue);
      return RanRight;
    };

    EXPECT_EQ(RanLeft, lefty.either(leftFn, rightFn));
    EXPECT_EQ(RanRight, righty.either(leftFn, rightFn));

  }

  TEST(Either, Assign) {
    const double DoubleValue = 4.0;
    const bool BoolValue = true;

    Either<bool, double> e0{BoolValue};
    Either<bool, double> e1{DoubleValue};

    EXPECT_TRUE(e1.isRight());
    EXPECT_EQ(e1.right(), DoubleValue);

    EXPECT_NE(e1.getRightPointer(), nullptr);
    EXPECT_EQ(e1.getLeftPointer(), nullptr);


    EXPECT_TRUE(e0.isLeft());
    EXPECT_EQ(e0.left(), BoolValue);

    EXPECT_NE(e0.getLeftPointer(), nullptr);
    EXPECT_EQ(e0.getRightPointer(), nullptr);

    e1 = e0;

    EXPECT_TRUE(e1.isLeft());
    EXPECT_EQ(e1.left(), BoolValue);

    EXPECT_NE(e1.getLeftPointer(), nullptr);
    EXPECT_EQ(e1.getRightPointer(), nullptr);

    EXPECT_EQ(e1, e0);


    e1 = DoubleValue;

    EXPECT_TRUE(e1.isRight());
    EXPECT_EQ(e1.right(), DoubleValue);

    EXPECT_NE(e1.getRightPointer(), nullptr);
    EXPECT_EQ(e1.getLeftPointer(), nullptr);


    e1 = BoolValue;

    EXPECT_TRUE(e1.isLeft());
    EXPECT_EQ(e1.left(), BoolValue);

    EXPECT_NE(e1.getLeftPointer(), nullptr);
    EXPECT_EQ(e1.getRightPointer(), nullptr);

    e1 = DoubleValue;


    e0 = e1;

    EXPECT_TRUE(e0.isRight());
    EXPECT_EQ(e0.right(), DoubleValue);

    EXPECT_NE(e0.getRightPointer(), nullptr);
    EXPECT_EQ(e0.getLeftPointer(), nullptr);

    EXPECT_EQ(e0, e1);

  }





}
