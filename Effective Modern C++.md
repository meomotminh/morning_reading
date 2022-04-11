C++11 most pervasive feature is move semantics: rvalues and lvalues
- rvalues indicates objects eligible for move operations, correspond to temporaty objects returned from functions
- lvalues dont, correspond to objects you can refer to, either by name or by following a pointer (has address)

```
    class Widget {
        public:
            Widget(Widget&& rhs);   // rhs is a lvalue, though it has an rvalue reference type
    }
```

all parameters are lvalues

conventions:
- the class name is Widget -> refer to an arbitrary user-defined type
- use parameter rhs for the move operations and copy operations
- apply special formating to parts of code or parts of comment to draw attention
- "…" to indicate "orther code could go here"/ wide ellipsis ("...") used in source code C++11 variadic templates

```
    template<typename... Ts>
    void processVals(const Ts&... params){
        …
    }
```
when an object is initialized with another object of the same type, the new object is said to be a copy of the initializing object, even if the copy was created via the move constructor. 

```
    void someFunc(Widget w);    // someFunc's parameter w is passed by value
    Widget wid;                 // wid is some Widget
    someFunc(wid);              // in this call to someFunc, w is a copy of wid that's created via copy construction
    someFunc(std::move(wid));   // in this call to someFunc, w is a copy of wid that's created via move construction
```

Copies of rvalues are generally move constructed, while copies of lvalues are usually copy constructed. An implication is that if you know only that an object is a copy of another object, it's nbot possible to say how expensive it was to construct the copy

In a function call, the expressions passed at the call site are the functions's arguments, used to initialize the function's parameter-> wid is argument, std::move(wid) is argument. Parameter is w. Parameters are lvalues but the arguments whit which they are initialized may be rvalues or lvalues.

Process of *perfect forwarding*, whereby an argument passed to a function is passed to a second function such that the original argumnent's rvalueness or lvalueness is preserved. 

well-designed functions are *exception safe*, meaning they offer at least the basic exception safety guarantee
- *basic guarantee*: if an exception is thrown, program invariants remain intact and no resources are leaked
- *strong guarantee*: if an exception arises, the state of the program remains as it was prior to the call

*function object* mean an object of a type supporting an operator() member function.

*callable object* member function pointers

*function object* created through lambda expression known as *closures*

*Declaration* introduce names and types without giving details, such as where storage is located and how things are implemented

```
    extern int x;   // object declaration

    class Widget;   // class declaration

    bool func(const Widget& w); // function declaration

    enum class Color;      // scoped enum declaration
```

*Definition* provide the storage locations or implementation details:

```
    int x;  // object definition

    class Widget { // class definition

    };  

    bool func(const Widget& w){ // function definition
        return w.size() < 10;
    }

    enum class Color {  // scoped enum definition
        Yellow, Red, Blue
    };

```

A definition also qualifies as a declaration

function's *signature* is the part of its declaration that specifies parameter and return types // bool(const Widget&)
Elements of a functions's declaration other than its parameter and return types are excluded

str::auto_ptr is deprecated in C++11 because std::unique_ptr does the same job, only better

*undefined behavior* means that runtime behavior is unpredictable, and it should go without saying that you want to steer clear of such uncertainty. // [] to index beyond the bounds of std::vector, dereferencing an uninitilized iterator or engaging in a data race

built-in pointers, such as those returned form new, *raw pointers*. *Smart pointer* overload the pointer dereferencing operator (operator-> and operator*).

# Chapter 1: Deducing Types
type deduction: function template. C++11 modifies and add auto and decltype
C++14 extends the usage contexts in which auto and decltype may be employed

-> C++ software more adaptable, because changing a type at one point in the source code auto propagates through type deduction to other locations.

Explain how template type deduction works, how auto builds on that, and how decltype goes its own way, how you can force compilers to make the results of their type deduction visible

## Item1 : Understand template type deduction
```
    template<typename T>
    void f(ParamType param);

    f(expr);        // call f with some expression
```

During compilation, compilers use *expr* to deduce two types: one for T and one for *paramType*

```
    template<typename T>
    void f(const T& param);     // paramType is const T&

    int x = 0;
    f(x);       // call f with an int
```
T is deduced to be int, but paramType is deduced to be const int&. it's natural to expect that the type deduced for T is the same as the type of the argument passed to the function (T is the type of *expr*). Three cases:
- *ParamType* is a pointer or reference type, but not a universal reference
- *ParamType* is a universal reference
- *ParamType* is neither a pointer nor a reference

### Case 1: ParamType is a Reference or Pointer, but not a Universal Reference
- If *expr* type is a reference, ignore the reference part
- then pattern-match *expr*'s type agains *ParamType* to determine T

```
    template<typename T>
    void f(T& param);   // param is a reference

    int x = 27; // x is an int
    const int cx = x;   // cx is a const int
    const int& rx = x;  // rx is a reference to x as a const int

    f(x);   // T is int, param's type  is int&
    f(cx);  // T is const int, param's type ist const int&
    f(rx);  // T is const int, param's type is const int&
```

In the second and third calls, cx and rx designate const values, T is deduced to be const int, thus yielding a parameter type of const int&. When they  passa const obj to a reference parameter, they expect that obj to remain unmodifiable. That's why passing a const obj to a template taking a T& parameter is safe: the constness of the obj becomes part of the type deduced for T


IN the 3, note that even though rx's type is a reference, T is deduced to be a non-reference. That because rx's referenceness is ignore during type deduction.

These examples show lvalue reference parameters, but type deduction work exactly the same for rvalue reference parameters. Of course, only rvalue arguments may be passed to rvalue reference parameter, but that restriction has nothing to do with type deduction

if change f's parameter type from T& to const T&, thing change a little, the constness of cx and rx continue to be respected, but we assume that param is a reference-to-const, no longer need for const to be deduced as part of T

```
    template<typename T>
    void f(const T& param); // param is now a ref-to-const

    int x = 27; 
    const int cx = x;
    const int& rx = x;

    f(x);   // T is int, param's type is const int&
    f(cx);  // T is int, param's type is const int&
    f(rx);  // T is int, param's type is const int&
```

as before, rx's referenceness is ignored during type deduction

if param were a pointer (or a pointer to const) instead of a reference, things work the same way

```
    template<typename T>
    void f(T* param);   // param is now a pointer

    int x = 27;
    const int *px = &x; // px is a ptr to x as a const int

    f(&x);  // T is int, param's type is int*
    f(px);  // T is const int, param's type is const int*
```

### Case 2: ParamType is a Universal Reference

template taking universal reference parameters, such parameters are declared like rvalue reference (T&&) but they behave differently when lvalue arguments are passed in

- if expr is an lvalue, both T and ParamType are deduced to be lvalue references. ONly situation in template type deduction where T is deduced to be a reference, second although ParamType is declared using the syntax for an rvalue reference, its deduced type is an lvalue reference

- if expr is an rvalue, the normal rules apply

```
    template<typename T>
    void f(T&& param);  // param is now a universal reference

    int x = 27;
    const int cx = x;
    const int& rx = x;

    f(x);   // x is lvalue, so T is int&, param type is int&
    f(cx);  // cx is lvalue, so T is const int&, param type is const int&
    f(rx);  // rx is lvalue, so T is const int&, param type is const int&
    f(27);  // 27 is rvalue, so T is int, param type is int&&
```

The key point is that the type deduction rules for universal reference parameters are different from those for parameters that are lvalue references or rvalue reference


### Case 3: ParamType is neither a Pointer nor a Reference

deal with pass-by-value, means param will be a copy of whatever is passed in-a completely new object.
- if expr's type is a reference, ignore the reference part
- if after ignoring expr's referenceness, expr is const, ignore that too. If it's volatile, also ignore that.

```
    template<typename T>
    void f(T param);

    int x = 27;
    const int cx = x;
    const int& rx = x;

    f(x);   // T is int, param's type is int
    f(cx);  // T is int, param's type is int
    f(rx);  // T is int, param's type is int   
```

Note that, even though cx and rx represent const values, param isn't const. param is an object that completely independent of cx and rx-> the fact that cx and rx can't be modified say nothing about whether param can be. It is important to recognize that const is ignored only for by-value parameters. For parameters that are references-to or pointers-to const, the constness of expr is preserved during type deduction

expr is a const pointer to a const obj, and expr is passed to a by-value param
```
    template<typename T>
    void f(T param);    // param is passed by value

    const char* const ptr = "fun with pointer"; // ptr is const ptr to const obj

    f(ptr); // pass arg of type const char* const
```

here the const to the right of * declare ptr to be const: ptr can't be made to point to a different location, nor can it be set to null. when ptr is passed to f, the bits making up the pointer are copied into param. As such, the pointer itself (ptr) will be passed by value. The constness of ptr will be ignored, and the type deduced for param will be const char* (a modifiable pointer to a const char string). The constness of what ptr point to is preserved during type deduction, but the constness of ptr itself is ignored when copying it to create the new pointer, param



### Array Arguments
array types are different from pointer types, even though they sometimes seem to be interchangeable.

```
    const char name[] = "J. P. Briggs"; // name type is const char[13]

    const char* ptrToName = name;   // array decays to pointer
```

ptrToName is initialized with name (cont char* <> const char[13]) but because of the array-to-pointer decay rule, the code compile

```
    template<typename T>
    void f(T param);

    f(name); // name is array, but T deduced as const char*
```
because array parameter declarations are treated as if they were pointer parameters, the type of an array that's passed to a template function by value is deduced to be a pointer type.

although function can't declare parameters that are truly arrays, they can declare parameters that are reference to arrays.

```
   template<typename T>     
   void f(T& param);    // template with by-reference parameter

    f(name);    // pass array to f
```
The type deduced for T is the actual type of the array includes size of the array // T: const char [13] and param: const char& [13]

```
    // return size of an array as a compile-time constant ( The array parameter has no name, because we care only about the number of elements it contains)
    template<typename T, std::size_t N>
    constexpr   std::size_t arraySize(T (&)[N]) noexcept    
    {
        return N;
    }
```
The function *constexpr* makes its result available during compilation-> possible to declare an array with the same number of elements as a second array whose size is computed from a braced initializer

```
    int keyVals[] = {1,3,7,9,11,22,35}; 

    int mappedVals[arraySize(keyVals)];
```

prefer a std::array to a built-in array
```
    std::array<int, arraySize(keyVals)> mappedVals; 
```

### Function Arguments
function types can decay into function pointers and everything discussed applied to type deduction for functions

```
    void someFunc(int, double); // someFunc is a function, type is void(int, double)

    template<typename T>
    void f1(T param);   // in f1, param passed by value

    template<typename T>
    void f2(T& param);  // in f2, param passed by ref

    f1(someFunc);       // param deduced as ptr-to-func; type is void(*)(int, double)

    f1(someFunc);       // param deduced as ref-to-func, typed is void(&)(int, double)
```

- During template type deduction, arguments that are references are treated as non-reference
- when deducing types for universal reference parameters, lvalue arguments get special treatment
- when deducing types for by-value parameters, const and volatile arguments are treated as non-const and non-volatile
- during template type deduction, arguments that are array or function names decay to pointers, unless they are used to initilaize ref.



























































































