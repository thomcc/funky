#ifndef FUNKY_EITHER_HH_INCLUDED
#define FUNKY_EITHER_HH_INCLUDED
// Copyright (c) 2013 Thom Chiovoloni.
// This file is distributed under the terms of the Boost Software License.
// See LICENSE.txt at the root of this distribution for details.

#include <type_traits>
#include <utility>
#include <cassert>

namespace funky {



  /// Either<Left, Right>, inspired by Haskell's `Either`.
  /// Used to represent when values can be one type or another.
  /// A common use case is for when a value can succeed or fail with a message, e.g.
  /// `Either<Foo, std::string>`.
  ///
  /// Caveats:
  /// Left and Right must be distinct types and neither may be a reference type.
  /// If you need to use a reference type, wrapping in a std::reference_wrapper \
  /// is probably your best bet.

  /// pass these to the Either constructor to construct a Left/Right either via
  /// emplacement.

  enum EmplaceRightTag { EmplaceRight };
  enum EmplaceLeftTag { EmplaceLeft };

  template <class LeftT, class RightT>
  class Either {

    static_assert(!std::is_same<LeftT, RightT>::value,
                  "Either<T, T> is disallowed");

    static_assert(!std::is_reference<LeftT>::value &&
                  !std::is_reference<RightT>::value,
                  "Either may not be used with reference types "
                  "(you may want to use std::reference_wrapper)");

    template <class T>
    struct isLeftOrRight : public std::integral_constant<bool
    , std::is_same<T, LeftT>::value || std::is_same<T, RightT>::value
    > {};



  public:

    /// Move construct from another either.
    Either(Either const &e) {
      if (e.isLeft()) {
        construct<LeftT>(e.left());
      } else {
        construct<RightT>(e.right());
      }
      assert(e.isLeft() == isLeft());
    }

    /// Move construct from another either.
    Either(Either &&e) {
      if (e.isLeft()) {
        construct<LeftT>(std::move(e.left()));
      } else {
        construct<RightT>(std::move(e.right()));
      }
      assert(e.isLeft() == isLeft());
    }

    template <class... Args>
    Either(EmplaceLeftTag, Args&&... args) {
      construct<LeftT>(std::forward<Args>(args)...);
      assert(isLeft());
    }

    template <class... Args>
    Either(EmplaceRightTag, Args&&... args) {
      construct<RightT>(std::forward<Args>(args)...);
      assert(isRight());
    }



    /// Construct an Either from a leftT or rightT.
    Either(LeftT const &l) { construct<LeftT>(l); assert(isLeft()); }
    Either(RightT const &r) { construct<RightT>(r); assert(isRight()); }

    /// Construct an Either by moving a leftT or rightT.
    Either(LeftT &&l) { construct<LeftT>(std::move(l)); assert(isLeft()); }
    Either(RightT &&r) { construct<RightT>(std::move(r)); assert(isRight()); }

    ~Either() { destroy(); }

    template <class Arg>
    Either &operator=(Arg &&arg) {
      set(std::forward<Arg>(arg));
      return *this;
    }

    Either &operator=(Either const &other) {
      set(other);
      return *this;
    }

    Either &operator=(Either &&other) {
      set(std::move(other));
      return *this;
    }

    /// Assign another Either to this.
    void set(Either const &e) {
      if (e.isRight()) {
        set(e.right());
      } else {
        set(e.left());
      }
      assert(e.isLeft() == isLeft());
    }

    /// Move assign another Either to this.
    void set(Either &&e) {
      if (e.isRight()) {
        set(std::move(e.right()));
      } else {
        set(e.left());
      }
    }

    /// assign a LeftT const& or RightT const&
    template <class T>
    typename std::enable_if<isLeftOrRight<T>::value, void>::type set(T const &v) {
      if (is<T>()) {
        get<T>() = v;
      } else {
        destroy();
        construct<T>(v);
      }
      assert(is<T>());
    }

    /// move-assign a LeftT&& or RightT&&
    template <class T>
    typename std::enable_if<isLeftOrRight<T>::value, void>::type set(T &&v) {
      if (is<T>()) {
        get<T>() = std::move(v);
      } else {
        destroy();
        construct<T>(std::move(v));
      }
      assert(is<T>());
    }

    /// Construct T in place.
    template <class T, class... Args>
    void emplace(Args&&... args) {
      static_assert(isLeftOrRight<T>::value, "Either<L, R>::emplace<T> where T != L && T != R");
      destroy();
      construct<T>(std::forward<Args>(args)...);
      assert(is<T>());
    }

    /// Construct LeftT in place.
    template <class... Args>
    void emplaceLeft(Args&&... args) {
      destroy();
      construct<LeftT>(std::forward<Args>(args)...);
      assert(isLeft());
    }

    /// Equivalent to emplace<Right>(args...)
    template <class... Args>
    void emplaceRight(Args&&... args) {
      destroy();
      construct<RightT>(std::forward<Args>(args)...);
      assert(isRight());
    }

    /// Get a {const,non-const,rvalue} reference to our {LeftT,RightT}. asserts is{LeftT,RightT}();
    LeftT const &left() const & { assert(isLeft()); return *rawGetPtr<LeftT>(); }
    LeftT       &left()       & { assert(isLeft()); return *rawGetPtr<LeftT>(); }
    LeftT      &&left()      && { assert(isLeft()); return std::move(*rawGetPtr<LeftT>()); }

    RightT const &right() const & { assert(isRight()); return *rawGetPtr<RightT>(); }
    RightT       &right()       & { assert(isRight()); return *rawGetPtr<RightT>(); }
    RightT      &&right()      && { assert(isRight()); return std::move(*rawGetPtr<RightT>()); }


    /// Do we hold a {Left,Right}?
    bool isLeft() const { return isLeft_; }
    bool isRight() const { return !isLeft_; }

    /// Comparison of eithers
    bool operator==(Either const &e) const {
      return (isLeft() == e.isLeft() && (isLeft() ? left() == e.left() : right() == e.right()));
    }

    bool operator!=(Either const &e) const {
      return !operator==(e);
    }

    /// Do we hold a T? static_asserts that T is LeftT or RightT.
    template <class T>
    bool is() const {
      // hm... should it just be false?
      static_assert(isLeftOrRight<T>::value, "Either<L, R>::is<T> where T != L && T != R");
      return std::is_same<LeftT, T>::value ? isLeft() : isRight();
    }

    /// If is<T>(), get a pointer to our T. otherwise, return nullptr.
    template <class T> T *getPointer() {
      static_assert(isLeftOrRight<T>::value, "Either<L, R>::as<T> where T != L && T != R");
      return is<T>() ? rawGetPtr<T>() : nullptr;
    }

    /// If is<T>(), get a const pointer to our T. otherwise, return nullptr.
    template <class T> T const *getPointer() const {
      static_assert(isLeftOrRight<T>::value, "Either<L, R>::as<T> where T != L && T != R");
      return is<T>() ? rawGetPtr<T>() : nullptr;
    }

    LeftT       *getLeftPointer()       { return getPointer<LeftT>(); }
    LeftT const *getLeftPointer() const { return getPointer<LeftT>(); }

    RightT       *getRightPointer()       { return getPointer<RightT>(); }
    RightT const *getRightPointer() const { return getPointer<RightT>(); }

    /// templated versions of left() and right().
    template <class T> T       &get()       & { assert(is<T>()); return *rawGetPtr<T>(); }
    template <class T> T const &get() const & { assert(is<T>()); return *rawGetPtr<T>(); }
    template <class T> T      &&get()      && { assert(is<T>()); return std::move(*rawGetPtr<T>()); }



    /// either(leftfn, rightfn):
    /// if we're a left, call leftfn(left()),
    /// otherwise call rightfn(right()).
    template <class LeftFn, class RightFn>
    auto either(LeftFn lf, RightFn rf) const -> decltype(isRight() ? rf(right()) : lf(left())) {
      return isRight() ? rf(right()) : lf(left());
    }

    /// as above, but non-const.
    template <class LeftFn, class RightFn>
    auto either(LeftFn lf, RightFn rf) -> decltype(isRight() ? rf(right()) : lf(left())) {
      return isRight() ? rf(right()) : lf(left());
    }

  private:

    // storage type. If this is changed (e.g. if std::aligned_union isn't well supported)
    // we should only need to change the implementation of rawGetPtr and construct.
    typedef typename std::aligned_union<0, LeftT, RightT>::type Storage;

    Storage storage_;
    bool isLeft_;

    template <class T> T       *rawGetPtr()       { return reinterpret_cast<T*>(&storage_); }
    template <class T> T const *rawGetPtr() const { return reinterpret_cast<T const*>(&storage_); }

    template <class T, class... Args>
    void construct(Args&&... args) {
      static_assert(isLeftOrRight<T>::value, "bug");
      void const *ptr = &storage_; // use a const void to allow const LeftT or RightTs
      new (const_cast<void*>(ptr)) T(std::forward<Args>(args)...);
      isLeft_ = std::is_same<T, LeftT>::value;
    }

    void destroy() {
      if (isLeft()) {
        rawGetPtr<LeftT>()->~LeftT();
      } else {
        rawGetPtr<RightT>()->~RightT();
      }
    }

  };


  template <class L, class R>
  void swap(Either<L, R> &a, Either<L, R> &b) {
   if (a.isLeft() && b.isLeft()) {
     using std::swap;
     swap(a.left(), b.left());
   } else if (a.isRight() && b.isRight()) {
     using std::swap;
     swap(a.right(), b.right());
   } else {
     std::swap(a, b);
   }
  }

}


#endif
