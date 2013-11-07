# Either
Implementation is in [Either.hh] and provides the `Either<Left, Right>` class template.

## Introduction

Either is a class template inspired strongly by Haskell's `Either` type. It models a variant that has two possibilities, `Either<Left, Right>` either a `Left` or a `Right`.

It's intended use case is similar to `{std,boost}::optional`, in that it represents the result of a computation that may fail, however it allows additional information to be transferred in the failing case.

By convention the "left" type is used for the error and the "right" (e.g. correct) type is used for the successful value.

## Synopsis

```C++
namespace funky {

enum EmplaceLeftTag { EmplaceLeft };
enum EmplaceRightTag { EmplaceRight };

template <class LeftT, class RightT>
class Either {
public:

  Either(Either const &e);
  Either(Either &&e);

  Either(LeftT const &l);
  Either(RightT const &r);

  Either(LeftT &&l);
  Either(RightT &&r);

  template <class... Args> Either(EmplaceLeftTag, Args&&... args);
  template <class... Args> Either(EmplaceRightTag, Args&&... args);


  Either &operator=(Either const &other);
  Either &operator=(Either &&other);

  Either &operator=(LeftT const &l);
  Either &operator=(RightT const &r);

  Either &operator=(LeftT &&l);
  Either &operator=(RightT &&r);

  void set(Either const &e);
  void set(Either &&e);

  void set(LeftT const &l);
  void set(RightT const &r);

  void set(LeftT &&l);
  void set(RightT &&r);

  template <class... Args> void emplaceLeft(Args&&... args);
  template <class... Args> void emplaceRight(Args&&... args);

  LeftT const &left() const &;
  LeftT       &left()       &;
  LeftT      &&left()      &&;

  RightT const &right() const &
  RightT       &right()       &
  RightT      &&right()      &&

  bool isLeft() const;
  bool isRight() const;

  LeftT       *getLeftPointer();
  LeftT const *getLeftPointer() const;

  RightT       *getRightPointer();
  RightT const *getRightPointer() const;

  bool operator==(Either const &e) const;
  bool operator!=(Either const &e) const;


  template <class T> bool is() const;

  template <class T, class... Args> void emplace(Args&&... args);

  template <class T> T       *getPointer();
  template <class T> T const *getPointer() const;

  template <class T> T       &get() &;
  template <class T> T const &get() const &;
  template <class T> T      &&get() &&;

  template <class LF, class RF> auto either(LF leftFn, RF rightFn) const;
  template <class LF, class RF> auto either(LF leftFn, RF rightFn);

};

template <class L, class R>
void swap(Either<L, R> &a, Either<L, R> b);


} // namespace funky

```


## Details

```C++
Either(Either const &e);
Either &operator=(Either const &other);
```

Copy-construct or copy-assign an Either to another.

---

```C++
Either(Either &&e);
Either &operator=(Either &&other);
```

Move-construct or move-assign an Either to another.

NOTE: The type of the moved either does not change. See [Caveats](#Caveats) for details.

---

```C++
template <class... Args> Either(EmplaceLeftTag, Args&&... args);
template <class... Args> Either(EmplaceRightTag, Args&&... args);
```

Construct via emplacement.

Example:

```C++
Either<bool, std::unique_ptr<int>> righty{ EmplaceRight, new int(4) };
Either<std::unique_ptr<int>, bool> lefty{ EmplaceLeft, new int(4) };
```

---

```C++
Either(LeftT const &l);
Either(RightT const &r);
Either(LeftT &&l);
Either(RightT &&r);
```

Construct an `Either` by copying or moving a `LeftT` or `RightT`.

---

```C++
Either &operator=(LeftT const &l);
Either &operator=(LeftT &&l);
Either &operator=(RightT const &r);
Either &operator=(RightT &&r);
```

Assign a `LeftT` or `RightT` to an `Either`.

If the assigned type matches our current type, then we use the `T::operator=` to do the assignment.  Otherwise we destroy our existing value before using the new type's copy/move constructor.

---

```C++
void set(Either const &e);
void set(Either &&e);

void set(LeftT const &l);
void set(RightT const &r);

void set(LeftT &&l);
void set(RightT &&r);
```

These are equivalent to using `Either::operator=`, and only differ in return type.

---

```C++
template <class... Args> void emplaceLeft(Args&&... args);
template <class... Args> void emplaceRight(Args&&... args);
```

Construct `LeftT` or `RightT` in place.

---

```C++
LeftT const &left() const &;
LeftT       &left()       &;
LeftT      &&left()      &&;
RightT const &right() const &
RightT       &right()       &
RightT      &&right()      &&
```

Get a const or non-const lvalue, or rvalue reference to our `LeftT` or `RightT`. Asserts that we actually have the type that you're getting.

---

```C++
bool isLeft() const;
bool isRight() const;
```

Returns true if we contain the requested type.

---

```C++
LeftT       *getLeftPointer();
LeftT const *getLeftPointer() const;
RightT       *getRightPointer();
RightT const *getRightPointer() const;
```

If `this->is{Left,Right}()`, get return a pointer to our `{Left,Right}T`. Otherwise return `nullptr`. This is intended to be used in conditionals, like so:

```C++
Either<int, std::string> foobar{0};

if (int *value = foobar->getLeftPointer()) {
  // use *value.
}
```

---

```C++
bool operator==(Either const &e) const;
bool operator!=(Either const &e) const;
```

Eithers are equal iff they hold the same type and their stored `LeftT`/`RightT`'s are considered equal.

---

```C++
// Do we hold a T?
template <class T> bool is() const;

/// Construct T in place.
template <class T, class... Args> void emplace(Args&&... args);

/// If is<T>(), get a pointer to our T. otherwise, return nullptr.
template <class T> T       *getPointer();
template <class T> T const *getPointer() const;

/// Get a reference to our T. Asserts that `this->is<T>()`.
template <class T> T       &get() &;
template <class T> T const &get() const &;
template <class T> T      &&get() &&;
```
These are intended to simplify generic code. (They're more awkward for general use.) `T` is statically asserted to be either `LeftT` or `RightT`.

---

```C++
template <class L, class R>
void swap(Either<L, R> &a, Either<L, R> b);
```

Swap a with b. If a and b have the same type, `swap(Type&,Type&)` is used unqualified but with std::swap introduced in scope. Otherwise, the default swap is used.

---

```C++
template <class LF, class RF>
auto either(LF leftFn, RF rightFn) const
  -> decltype(isRight() ? rightFn(right()) : leftFn(left()));

template <class LF, class RF>
auto either(LF leftFn, RF rightFn)
  -> decltype(isRight() ? rightFn(right()) : leftFn(left()))
```

Returns `isLeft() ? leftFn(left()) : rightFn(right())`. This is inspired by the Haskell `either` function.


## Caveats

### Moving Eithers

It's important to note that after moving an `Either` to another, the type of the moved `Either` does **not** change (what would it change to?). For example, the following code compiles and runs fine.

```C++
Either<bool, std::unique_ptr<int>> foo{EmplaceRight, new int(4)};

// Demonstrate that the initialization worked.
assert(foo->isRight());
assert(foo->right() != nullptr);
assert(*foo->right() == 4);

// the actual pointer value, for later comparison
int *pointer = foo->right().get();

// Move foo to bar
Either<bool, std::unique_ptr<int>> bar{std::move(foo)};

// foo is still a right()
assert(foo->isRight());

// our unique_ptr was moved to bar, but we still have one,
// it's just nullptr now.
assert(foo->right() == nullptr);

// show that bar has our unique_ptr now.
assert(bar->isRight());
assert(bar->right() != nullptr);
assert(*bar->right() == 4);

// check against the earlier saved pointer
assert(bar->right().get() == pointer);
```

[Either.hh]: include/funky/Either.hh
