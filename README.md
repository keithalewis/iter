# iter (alpha)
Functional C++20 iterator library.
[Godbolt demo](https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:1,endLineNumber:29,positionColumn:1,positionLineNumber:29,selectionStartColumn:1,selectionStartLineNumber:29,startColumn:1,startLineNumber:29),source:'%23include+%22https://tinyurl.com/libiter%22%0A%0A%23include+%3Carray%3E%0A%0A//+Auto-vectorized+/+zero-overhead%0A//+Interop+with+std::array%0A//+Interop+with+range-based+for+loop%0A%0Avoid+multiply(std::array%3Cfloat,+64%3E+const%26+x,+std::array%3Cfloat,+64%3E+const%26+y,+std::array%3Cfloat,+64%3E%26+z)+%7B%0A++for+(auto+%5Ba,+b,+c%5D+:+iter::zip(x,+y,+z))+%7B%0A++++c+%3D+a+*+b%3B%0A++%7D%0A%7D%0A%0A//+Pipe+syntax%0A//+Still+auto-vectorized%0A%0Afloat+weighted_sum(std::array%3Cfloat,+64%3E+const%26+a)+%7B%0A++return+a%0A++++%7C+iter::enumerate()%0A++++%7C+iter::map+%7C+%5B%5D(auto+ai)+%7B%0A++++++++auto%26+%5Ba,+i%5D+%3D+ai%3B%0A++++++++return+a+*+i%3B+%7D%0A++++%7C+iter::sum()%3B%0A%7D%0A%0Ausing+namespace+iter%3B%0Ausing+namespace+xtd::literals%3B%0A%0A//+Constexpr+friendly%0A%0Astatic+constexpr+int+sum_0_to_9+%3D+indices+%7C+take(_,+10)+%7C+sum()%3B%0Astatic_assert(sum_0_to_9+%3D%3D+45)%3B'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:52.80187823291271,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((g:!((h:compiler,i:(compiler:g103,filters:(b:'0',binary:'1',commentOnly:'0',demangle:'0',directives:'0',execute:'1',intel:'0',libraryCode:'0',trim:'1'),fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!(),options:'-std%3Dc%2B%2B20+-Ofast',selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:'5',n:'0',o:'x86-64+gcc+10.3+(Editor+%231,+Compiler+%231)+C%2B%2B',t:'0')),k:47.19812176708729,l:'4',m:50,n:'0',o:'',s:0,t:'0'),(g:!((h:output,i:(compiler:1,editor:1,fontScale:14,fontUsePx:'0',wrap:'1'),l:'5',n:'0',o:'%231+with+x86-64+gcc+10.3',t:'0')),header:(),l:'4',m:50,n:'0',o:'',s:0,t:'0')),k:47.19812176708729,l:'3',n:'0',o:'',t:'0')),l:'2',n:'0',o:'',t:'0')),version:4)

Small, single header, feature-rich, functional C++20 iterator library that aims to be simple, composable, and easily extensible. Much of the simplicity in the design is owed to the [extend library](https://github.com/NasalDaemon/extend).

- [x] Single header
- [x] Simple implementation (~2500 source lines all-in)
- [x] Minimal std library dependencies: `<type_traits> <concepts> <functional> <optional> <limits> <memory>`
- [x] `constexpr`/`consteval` friendly
- [x] Interoperable with range-based for loops
- [x] Zero-overhead
- [x] Auto vectorisable
- [x] Seemless interop with std::vector, std::array, std::optional
- [ ] Seemless interop with other std containers
- [x] Predictible API: the function signatures align more-or-less with those of the [rust iterator library](https://doc.rust-lang.org/std/iter/trait.Iterator.html), which itself follows a pretty standard functional nomenclature.
- [ ] Documentation (relies on previous experience with standard iterator libraries in other languages)
- [ ] Full unit test coverage. Only ~50% coverage. Use at your own risk!

## Getting started

### Single header
1. Copy [singleheader/iter.hpp](https://github.com/NasalDaemon/iter/blob/main/singleheader/iter.hpp) into your include directory
1. Enable C++20
1. `#include "iter.hpp"`
### Full
1. `git clone https://github.com/NasalDaemon/iter.git --recurse-submodules`
1. Add `iter/include/` and `iter/extern/extend/include/` to your include path
1. Enable C++20
1. `#include "iter/zip.hpp"` (include whichever headers you need: each iter function is in its own header). `#include "iter.hpp"` to include all iter functions

## Compiler support

GCC 10+, Clang 12+, MSVC 19.27+.

## Propaganda

##### Integrates non-intrusively with existing standard library containers and range-based for loops.
```c++
void multiply(std::vector<float> const& x, std::vector<float> const& y, std::vector<float>& z) {
  for (auto [a, b, c] : iter::zip(x, y, z)) {
    c = a * b;
  }
}
```
##### Supports UFCS-style syntax via the [extend](https://github.com/NasalDaemon/extend) library.

All functions in `iter` namespace can be called in these equivalent ways:
* `iter::fun(like, that, ...)`
* `like | iter::fun()` (for unary calls to `iter::fun` only)
* `like | iter::fun(_, that, ...)`
* `that | iter::fun(like, _, ...)`
* `like | iter::fun | _` (for unary calls to `iter::fun`)
* `like | iter::fun | that` (for binary calls to `iter::fun`)
* `like $(iter::fun) (that, ...)` (after including [macros](https://github.com/NasalDaemon/extend/blob/main/include/extend/dollar_macros/define.hpp) from extend).

```c++
void multiply(std::vector<float> const& a, std::vector<float> const& b, std::vector<float>& c) {
  a | iter::zip | b
    | iter::zip | c // for convenience -- equivalent to: a | iter::zip(_, b, c)
    | iter::foreach | [](auto& abc) {
        auto& [a, b, c] = abc;
        c = a * b; };
}
```
##### Functionality and nomenclature similar by existing functional iterator libraries in other languages, such as rust, python, scala.

`foreach`,`map`,`flatmap`,`range`,`generator`,`compound`,`once`,`repeat`,`chain`,`enumerate`,`zip`,`unzip`,`fold`,`reduce`,`filter`,`take`,`take_while`,`skip`,`skip_while`,`inspect`,`collect`,`partition`,`sorted`,`last`,`min`,`max`,`find_linear`,`any`,`all`,`map_while`,`filter_map`

```c++
float weighted_sum(std::vector<float> const& a) {
  return a
    | iter::enumerate()
    | iter::map | [](auto ai) {
        auto& [a, i] = ai;
        return a * i; }
    | iter::sum();
}
```

## Extending existing classes with iter functionality

There are two fundamental concepts: `iter::iter` and `iter::iterable`.

An `iter` is anything that can be passed as the only argument to `iter::next(...)`, which should return either `std::optional<T>` for an optional value or `T*` for an optional reference.

An `iterable` is anything that can be passed to `iter::to_iter(...)` and returns an `iter`. Anything that is an `iter` is also an `iterable` (since there is a default implementation of `iter::to_iter` for `iter` arguments which simply returns the argument back).

All adaptors and consumers operate on `iterable` or `iter`.

To make any type an `iter`, you simply define the relevant `iter::next` implementation for it.

```c++
struct enumerate_until {
  int max;
  int i = 0;

  using this_t = enumerate_until;
  constexpr auto ITER_IMPL_THIS(next) (this_t& self) {
    return self.i < self.max ? std::optional(self.i++) : std::nullopt;
  }
};
```

To make any type an `iterable`, you simply define the relevant `iter::to_iter` implementation for it.

```c++
// Makes all ints "iterable" via enumerate_until (probably not a good idea)
constexpr auto ITER_IMPL(to_iter) (int max) {
  return enumerate_until{max};
}
```

## Is iter a drop-in replacement for std::ranges?

Not quite, and it doesn't aim to be. iter aims to do one thing, and one thing well: provide a simple functional interface to consume iterable things without adding run-time or compile-time overhead. It does this by avoiding the standard C++ iterator concept in favour of a simpler `iter::iter` concept. On the other hand, std::ranges builds on standard iterators while attempting to hide away their complexities, offering useful concepts and algorithms for existing standard containers and iterators. In other words, std::ranges offers a wider scope of functionality than iter. That wider scope necesarily comes at the cost of complexity, bloat and often poorer performance in the subset of things that iter does specialise in.

The extremely simple `iter::iter` concept, which iter builds upon, has only one requirement: `iter::next(it)` optionally returns the next item of `it`. Items are consumed one after the other until no item is returned. This greatly simplifies the implementation of this library's many components, scales extremely well, and greatly helps the compiler to optimise things away -- but it also precludes implementing certain algorithms. For example, any algorithm that accesses items in non-sequential order cannot be expressed using this concept. This includes algorithms such as: quicksort, in-place partition.

Anything that can use iter should use iter rather than std::ranges: iter has lower compile-time overhead and is also more likely to be better optimised by the compiler. However, anything that can't be represented with the simple `iter::iter` concept is out of scope for this library, and std::ranges deals with it perfectly well. `std::ranges::sort(container)` is simple enough!
