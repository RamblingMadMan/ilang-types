# Infinity Lang Type System
Documentation and reference implementation of the Infinity Lang type system.

All types within the Infinity Language are treated as abstract concepts.

One side-effect of this is that there are no concrete fixed-width types. That isn't to say that types with a fixed bit-width do not exist (e.g. `Int32`, `Real32`, `Nat32`, etc.) but that they simply do not have a single universal underlying type.

This can usually be treated as an implementation detail. E.g. on a system with no support for 16-bit integers, `Int16` might be represented by the C type `int32_t` but on other (more traditional) systems it would more probably be represented by `int16_t`.

## Base Types

There are a number of abstract base types (that may be based on one another) used as the base for complete types.

| Description                                  | Abstract Type | Base     | Example Implementation     |
| -------------------------------------------- | ------------- | -------- | -------------------------- |
| An atomic mathematical unit                  | `Number`      | None     | N/A                        |
| A string of characters                       | `String`      | None     | N/A                        |
| Mapping from keys to values                  | `Map K T`     | None     | N/A                        |
| A list of values of `T`                      | `List T`      | None     | N/A                        |
| A tree structure with nodes of `T`           | `Tree T`      | `List T` | N/A                        |
| A contiguously laid out series of `T` values | `Array T`     | `List T` | N/A                        |
| Counting numbers                             | `Natural`     | `Number` | C `uint32_t` type          |
| Round numbers                                | `Integer`     | `Number` | C `int32_t` type           |
| Real numbers with quotient form              | `Rational`    | `Number` | Pair of an `Integer` type  |
| Real numbers                                 | `Real`        | `Number` | C `float` or `double` type |

## Complete Types

Complete types are those that can not be refined within their concept any further. An example of a complete type is `Natural32`; A natural number that can be represented in 32 bits. This type can not be refined further: it encodes the category and precision of the range of possible values.

> Wherever a type is suffixed by an uppercase `N`, `N` may be replaced by some desired bit-width.

| Complete Type | Abstract Base | Value Syntax                           | Possible C/C++ Type                |
| ------------- | ------------- | -------------------------------------- | ---------------------------------- |
| `NaturalN`    | `Natural`     | `0`, `1`, `2`, `3`, ...                | `uintN_t`                          |
| `IntegerN`    | `Integer`     | `1`, `0`, `-1`, `-2`, ...              | `intN_t`                           |
| `RationalN`   | `Rational`    | `1/3`, `2/7`, `6/21`, ...              | `div_t`, `ldiv_t`, etc.            |
| `RealN`       | `Real`        | `0.1`, `1.1111`, `69.420`, `9001`, ... | `float`, `double` or `long double` |

## Compound Types

Compound types are used to construct more complex types from simple types.

| Name                         | Abstract Base | Type Syntax        | Value Syntax                              | Possible C/C++ Type        |
| ---------------------------- | ------------- | ------------------ | ----------------------------------------- | -------------------------- |
| Static array                 | `Array T`     | `StaticArray T N`  | `[x0, x1, xn...]`                         | `T[N]`                     |
| Ordered map                  | `Map K T`     | `OrderedMap K T`   | `["x0" -> x0, "x1" -> x1, "xn" -> xn...]` | `std::map<K, T>`           |
| Unordered map                | `Map K T`     | `UnorderedMap K T` | Same as above                             | `std::unordered_map<K, T>` |
| Sum type / Union             | N/A           | `T | U...`         | Value of any of the summed types          | `std::variant<T, U...>`    |
| Product type / Tuple         | N/A           | `T * U...`         | `(1, "Hello")`                            | `std::tuple<T, U...>`      |
| Composite data type / Struct | N/A           | `{x: T, xs: U...}` | Constructor based                         | `struct{ T x; U xs...; }`  |

## Type Promotion

Type promotion is only performed to pass lesser-precision values as higher-precision arguments.

Type promotion works in the following directions:

| LHS Type   | RHS Type   | Result Type |
| ---------- | ---------- | ----------- |
| `Integer`  | `Natural`  | `Integer`   |
| `Integer`  | `Rational` | `Rational`  |
| `Integer`  | `Real`     | `Real`      |
| `Rational` | `Natural`  | `Rational`  |
| `Rational` | `Real`     | `Rational`  |
| `Real`     | `Natural`  | `Real`      |

