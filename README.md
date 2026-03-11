# SSL - Simple Scripting Language

SSL is a lightweight, dynamically typed, functional, embeddable scripting language.

> Note: A stable embedding API is planned. Currently, embedding requires replicating the interpreter's internal setup. See [`src/main.c`](src/main.c) for reference.

## Features

- **Pattern matching**
- [**Curried functions**](https://en.wikipedia.org/wiki/Currying)
- [**Tail-call optimized**](https://en.wikipedia.org/wiki/Tail_call)
- **Bytecode compiled**

## Quick Example

```ssl
range = n. ns.
  | n         = range (n - 1) (n : ns)
  | otherwise = ns
  ;

foldl = f. acc. xs.
  match xs
  | []     = acc
  | x : xs = foldl f (f acc x) xs
  ;

# Point-free function
reverse = foldl (a. b. b : a) [];

# Create a list of numbers ranging from 1 to 10000; reverese it; print it.
printl (reverse (range 10000 []));
```

## Language Overview

### Hello, world!

```ssl
# This is a comment!

printl "Hello, world!"; # The language requires semicolons!
```

### Value Binding

```ssl
a = 10; # Binds the value 10 to a.

printl a; # Prints 10.

a = 20; # Binds the value 20 to a. This shadows the previous a.

printl a; # Prints 20.
```

### Basic Arithmetic

```ssl
a = 10; # Binds the value 10 to a.
b = 20; # Binds the value 20 to b.

c = a + b; # Applies (+) to a and b.

printl c; # Prints 30.
```

### Abstraction & Application

```ssl
square = n. n * n; # Binds the value (n. n * n) to square.
```

A lambda abstraction has the form `x. E`, where `x` is an identifier and `E` is an expression. It defines an anonymous function with parameter `x` and body `E`.

```ssl
printl (square 5); # Prints 25.
```

Application can be thought of as a juxtaposition operator, which applies the left-hand side to the right-hand side.

In the above example, `square` is first applied to the value `5`, producing `25`, which is passed to `printl`.

### Currying

Since every lambda abstraction takes exactly one parameter, functions of multiple arguments are expressed through currying; a technique where a function returns another function.

```ssl
add = m. n. m + n;

printl (add 10 20); # Prints 30
```

In the above example, `add 10 20` is evaluated left-to-right as `(add 10) 20`. Applying `add` to `10` returns an intermediate function `n. 10 + n`, which is then applied to `20`, yielding `30`.

### Partial Application

```ssl
add = m. n. m + n;

add3 = add 3; # The intermediate function `n. 3 + n` is bound to `add3`

printl (add3 10); # Prints 13
printl (add3 20); # Prints 23
printl (add3 30); # Prints 33
```

### Lists

A list is an ordered sequence of values. They may contain values of any type, including other lists.

```ssl
printl [1, 1.5, "Hello", [1, 2, 3, 4]]; # Prints [1,1.5,"Hello",[1,2,3,4]]
```

A list is either empty (`[]`), or a `head` and a `tail`, where the `head` is the first element and the `tail` is the remaining list.

```ssl
printl (head [1, 2, 3, 4, 5]); # Prints 1
printl (tail [1, 2, 3, 4, 5]); # Prints [2,3,4,5]
```

The `:` operator constructs a new list by prepending a value to an existing list.

```ssl
printl (1 : [2, 3]); # Prints [1,2,3]
printl (1 : []);     # Prints [1]
```

This means `[1, 2, 3]` is syntactic shorthand for `1 : 2 : 3 : []`.

### Values

| Type      | Example      | Description                      |
| --------- | ------------ | -------------------------------- |
| Unit      | `()`         | A value carrying no information  |
| Integer   | `10`         | A whole number                   |
| Float     | `3.142`      | A floating-point number          |
| String    | `"Hi!"`      | A sequence of characters         |
| List      | `[1,2]`      | An ordered sequence of values    |
| Lambda    | `(a. a)`     | A function                       |
| Native    | `(+)`        | A function defined natively in C |

### Branches

```ssl
| condition1 = expression1
| condition2 = expression2
| condition3 = expression3
...
```

Branches check a series of conditions, evaluating to the expression of the first one that is truthy.

```ssl
classify = n.
  | n > 0     = "Positive"
  | n < 0     = "Negative"
  | otherwise = "Zero"
  ;

printl (classify (0 - 5)); # Prints "Negative"
printl (classify (0));     # Prints "Zero"
printl (classify (5));     # Prints "Positive"
```

Branch conditions are not limited to booleans.

```ssl
isTruthy = a.
  | a         = "Yes"
  | otherwise = "No"
  ;

printl (isTruthy ()); # Prints "No"

printl (isTruthy 1); # Prints "Yes"
printl (isTruthy 0); # Prints "No"

printl (isTruthy "Hi"); # Prints "Yes"
printl (isTruthy "");   # Prints "No"

printl (isTruthy [1, 2]); # Prints "Yes"
printl (isTruthy []);     # Prints "No"

printl (isTruthy (a. a)); # Prints "Yes"
printl (isTruthy (+));    # Prints "Yes"
```

### Pattern Matching

```ssl
match E
| pattern1 = expression1
| pattern2 = expression2
| pattern3 = expression3
...
```

Pattern matching tests a value against a series of patterns, evaluating to the expression of the first one that matches.

```ssl
describe = n.
  match n
  | 0 = "Zero"
  | 1 = "One"
  | 2 = "Two"
  | _ = "I don't know that number!"
  ;

printl (describe 0); # Prints "Zero"
printl (describe 1); # Prints "One"
printl (describe 2); # Prints "Two"

printl (describe 100); # Prints "I don't know that number!"
```

| Pattern   | Example          | Description                                                 |
| --------- | ---------------- | ----------------------------------------------------------- |
| Wildcard  | `_`              | Matches any value                                           |
| Name      | `x`              | Matches any value, binds it to `x`                          |
| Literal   | `0`, `""`, etc.  | Matches an exact value                                      |
| List      | `[1, 2, 3]`      | Matches an exact list                                       |
| Cons      | `x : xs`         | Matches a non-empty list, splitting it into a head and tail |

Patterns may appear inside other patterns, in any combination.

### Recursion

A function may refer to itself in its own definition, allowing it to express repetition.

```ssl
fibonacci = n.
  match n
  | 0 = 0
  | 1 = 1
  | _ = fibonacci (n - 1) + fibonacci (n - 2)
  ;

printl (fibonacci 10); # Prints 55, the 10th Fibonacci number.
```

The above example computes the nth Fibonacci number. It's rather slow. A tail-recursive version is much faster!

```ssl
fibonacci' = k. a. b.
  | k         = fibonacci' (k - 1) b (a + b)
  | otherwise = a
  ;

fibonacci = n.
  | n         = fibonacci' n 0 1
  | otherwise = 0
  ;

printl (fibonacci 10); # Prints 55, the 10th Fibonacci number.
```

Recursion combined with `:` is the natural way to build lists.

```ssl
collatz = n.
  match n
  | 1 = [1]
  | _ =
    (
    match n % 2
    | 0 = n : collatz (n / 2)
    | 1 = n : collatz (n * 3 + 1)
    )
  ;

printl (collatz 13); # Prints [13,40,20,10,5,16,8,4,2,1]
```

## Prerequisites

- A C compiler (e.g., [GCC](https://gcc.gnu.org/))

### Optional
- [Make](https://www.gnu.org/software/make/) — Required for building via the `Makefile`.

## Build

### With Make

```bash
make
```

### Without Make

```bash
cc -std=c99 -o ssl src/*.c -lm
```

## Usage

```bash
./ssl <file>
```

## License

See [LICENSE](LICENSE) for details.

