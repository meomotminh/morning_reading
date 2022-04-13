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



# Item 2: Understand auto type deduction

template type deduction involves templates and functions and parameter, auto deals with none of those things

when a var is declared using auto, auto plays the role of T in the template and the type specifier for the variable acts as ParamType

```
    auto x = 27;

    const auto cx = x;

    const auto& rx = x;
```

To deduce types for x, cx and rx, compilers act as if there were a template for each declaration as well as a call to that template with the corresponding initializing expression:

```
    template<typename T>
    void func_for_x(T param);   // conceptual template for deducing x's type

    func_for_x(27); // param type is x's type

    template<typename T>
    void func_for_cx(const T param);    // conceptual template for deducing cx's type

    func_for_cx(x); // param type is cx's type

    template<typename T>
    void func_for_rx(const T& param);   // conceptual template for deducing rx's type

    func_for_rx(x); // param'S deduced type is rx's type

```

- Case 1: The type specifier is a pointer or ref, but not a universal reference
- Case 2: The type specifier is a universal ref
- Case 3: The type specifier if neither a pointer nor a ref

```
    auto x = 27; // case 3

    const auto cx = x;  // case 3

    const auto& rx = x; // case 1

    auto&&  uref1 = x;  // x is int and lvalue, so uref1 type is int&

    auto&& uref2 = cx;  // cx is const int and lvalue, so uref2 type is constint&

    auto&& uref3 = 27;  // 27 is int and rvalue, so uref3 type is int&&

    const char name[] = "R. N. Briggs"; // name type is const char[13]

    auto arr1 = name;   // arr1 type is const char*

    auto& arr2 = name;  // arr2 type is const char(&)[13]

    void someFunc(int, double); // someFunc is a function, type is void(int, double)

    auto func1 = someFunc;  // func1 type is void (*)(int, double)

    auto& func2 = someFunc; // func2 type is void (&)(int, double)
```

Exception if you want to declare an int with an initial value of 27, C++98 gives u 2 syntactic choices:

```
    int x1 = 27;
    int x2(27);

    // C++11 support uniform initialization
    int x3 = {27};
    int x4{27};
```

advantage to declare var using auto instead of fixed types, so it's be nice to replace int with auto 

``` 
    auto x1 = 27;
    auto x2(27);
    auto x3 = {27};
    auto x4{27};
```

the first 2 statement declare a var of type int with value 27, the second 2 however declare a var of type std::initializer_list<int> containging a single element with value 27

This is due to a special type deduction rule for auto. when the initializer for an auto-declared var is enclosed in braces, the deduced type is a std::initializer_list. If such a type can't be deduced, the code whill be rejected


```
    auto x5 = {1,2,3.0}; // error
```

2 kinds of type deduction takiong place:
- 1: stems from the use of auto: x5's type has to be deduced. because x5 initilaizer is in braces, x5 must be deduced to be a std::initializer_list but std::initializer_list is a template. Instantiations are std::initializer_list<T> for some type T-> mean T's must also be deduced
- 2: template type deduction

The treatment of braced initializers is the only way in which auto type deduction and template type deduction differ. wwhen auto declared var is initialized with a braced initializer, the deduced type is an instantiation of std::initializer_list but if the corresponding template is passed the same initializer, type deduction fails

```
    auto x = {11, 23, 9};   // x type is std::initializer_list<int>

    template<typename T>    
    void f(T param);    // template with parameter declaration equivalent to x

    f({11, 23, 9});     // error can't deduce type for T
```

if specify in the template that param is a std::initializer_list<T> for some unknown T, template type deduction will deduce

```
    template<typename T>
    void f(std::initializer_list<T> initList);

    f({11, 23, 9}); // T deduced as int, and initLIst's type is std::initializer_list<int>
```

The only real difference bw auto and template type deduction is that auto assumes that a braced initializer represnet a std::initializer_list but template type deduction doesn't

C++14 permits auto to indicate that a funtion's return type should be deduced and C++14 lambda may use auto in parameter declaration. however, these uses of auto employ template type deduction, not auto type deduction. SO a function with an auto return type that return a braced initializer won't compile

```
    auto createInitList(){
        return {1,2,3}; // error: can't deduce type for {1,2,3}
    }

    std::vector<int> v;
    ...

    auto resetV = [&v](const auto& newValue){v = newValue;};

    resetV({1,2,3});    // error, can't deduce type for {1,2,3}
```

- auto type deduction is usually the same as template type deduction, but auto type deduction assumes that a braced initilaizer represent a std::initializer_list and template type deduction doesn't
- auto in a function return type or a lambda parameter implies template type deduction, not auto type deduction

## Item 3; Understand decltype

decltype tell u the name's or the expression type

```
    const int i = 0;    // decltype(i) is const int

    bool f(const Widget& w);    // decltype(w) is const Widget&
                                // decltype(f) is bool(const Widget&)

    struct Point{
        int x,y;        // decltype(Point::x) is int
    }

    Widget w;           // delctype(w) is Widget

    if (f(w))...        // decltype(f(w)) is bool

    template<typename T>    
    class vector {
        public:
            ...
            T& operator[](std:size_t index);
            ...
    };

    vector<int> v;  // decltype(v) is vector<int>

    if (v[0] == 0)...   // decltype(v[0]) is int&
```

IN C++11 primary use of decltype is declaring function templates where the function's return type depends on its parameter types-> suppose weite a function takes container that supports indexing via square brackets and index, then authenticate user brfore returing result of the indexing operation. Ther return type of the function should be the same as the type returned by the indexing operation

operator[] on a container of obj of type T typically returns a T&. std::deque and std::vector
for std::vector<bool> operator[] does not return a bool& instead returns a brand new obj

decltype makes it easy to express that, use of decltype to compute the return type

```
    template<typename Container, typename Index>
    auto authAndAcces(Container& c, Index i)
    -> decltype(c[i])
    {
        authenticateUser();
        return c[i];
    }
```

the use of auto before the function name has nothing to do with type deduction. Rather it indicates that C++11's trailing return type syntax is being used that the function's return type will be declared folloing the paramter list. A trailing return type has the advantage that the function's parameters can be used in the specification of the return type. IN authAndAccess, we specify the return type using c and i. if we were to have the return type precede the function name in the conventional fashion, c and i would be unavailable.

with this declaration, authAndAccess return whatever type operator[] returns when applied to the passed-in container

C++11 permit return type for single-statement lambdas to be deduced, C++14 extends this to both all lambdas and all function, including those with multiple statements

```
    template<typename Container, typename INdex>    // C++14
    auto authAndAccess(Container& c, Index i)
    {
        authenticateUser();
        return c[i];    // return type deduced from c[i]
    }
```

for function with an auto return type spedicification, compiler employ template type deduction. operator[] for most container of T return a T&, but during template type deduction, the ref of an initializing is ignored

```
    std::deque<int> d;
    ...
    authAndAccess(d, 5) = 10;   // authenticate user, return d[5] then assign 10 to it, won't compile
```

d[5] return an int&, but auto return type deduction for authAndAccess will strip off the ref thus yielding a reutrn type of int, is an rvalue

To get authAndAccess work, we need to use decltype type deduction for its return type, to specify that authAndAccess should return exactly the same type that the expression c[i] return.

```
    template<typename Container, typename Index>    // C++14
    decltype(auto)
    authAndAccess(Container& c, Index i){
        authenticateUser();
        return c[i];
    }
```

The use of decltype(auto) is not limited to function return types, it can also be convenient for declaring var when u want to apply the decltype type deduction rules to the initializing expression:

```
    Widget w;

    const Widget& cw = w;

    auto myWidget1 = cw;        // auto type deduction, myWidget1 type is Widget

    decltype(auto) myWidget2 = cw;  // decltype type deduction, myWidget2 type is const Widget&


    template<typename Container, typename Index>
    decltype(auto) authAndAccess(Container& c, Index i);
```

container is passed by lvalue-ref-to-non-const, because returning a ref to an element of the container permits clients to modify that container. but means it's not possible to pass rvalue continer to this function. Rvalues can't bind to lvalue ref (unless they aer lvalue-ref-to-const)

passing an rvalue container to authAndAccess is an edge case. An rvalue container, being a temporary obj, would typically be destroyed at the end of the statement containing the call to authAdnAccess, that meas that a ref to an element in that container would dangle at the end of the statement that created it.

```
    std::deque<std::string> makeStringDeque();  // factory function

    // make copy of 5th element of deque returned from makeStringDeque
    auto s = authAndAccess(makeStringDeque(),5);
```

Overloading would work (one overload would declare an lvalue ref parameter, the other an rvalue ref parameter), but the we'd have 2 functions to maintain. A way to avoid that is to have authAndAccess employ a ref parameter that can bind to lvalues and rvalues

```
    template<typename Container, typename Index> // c is now a universal ref
    decltype(auto) authAndAccess(Container&& c, Index i);
```

In this template, we don't know what type of continer we're operating on, and means we're equally ignorant of the type of index objects it uses. Employing pass-by-value for obj of an unknown type risks the performance hit of unnecessary copying, the behavioral problems of object slicing and the sting of derision. But in the case of container indices, following the example of the Standard LIbrary for index value (std::string, stdd:vector, std::deque) seems reasonable, so stick with pass-by-value

apply std::forward to universal ref:

```
    template<typename Container, typename Index>    // C++14
    decltype(auto)
    authAndAccess(Container&& c, Index i){
        authenticateUser();
        return std::forward<Container>(c)[i]
    }    

    template<typename Container, typename Index>    // C++11
    auto
    authAndAccess(Container&& c, Index i)
    -> decltype(std::forward<Container>(c)[i])
    {
        authenticateUser();
        return std::forward<Container>(c)[i]
    }
```


decltype almost always produces the type u expect, rarely surprise. To fully understnad decltype's behavior, u will have to familiarize urself with a few special cases. Applying decltype to a name yields the declared type for that name. Names are lvalue expression, but doesn't affect decltype's behavior. For lvalue expressions more complicated than names, decltype ensures that the type reported is always an lvalue ref. if an lvalue expression other than a name has type T, decltype reports that type as T&. 

int x = 0;

x is name of var, so decltype(x) is int. But wrapping the name x in parenthese yields an expression more complicated than a name. x is an lvalue, and C++ defines the expression(x) to be an lvalue too. decltype((x)) is therefore int&


``` 
    decltype(auto) f1()
    {
        int x = 0;

        return x;   // decltype(x) is int, so f1 return int
    }

    decltype(auto) f2()
    {
        int x = 0;

        return (x); // decltype((x)) is int&, so f2 return int&
    }
```

not only does f2 have a different return type from f1, it's also returning a ref to a local var-> undefined behavior.


- decltype almost always yield the type of a var or expression without any modifications
- for lvalue expressions of type T other than name, decltype always reports a type of T&
- C++14 support decltype(auto) like auto deduces a type from its initializer, but it performs the type deduction using the decltype rule


# Item 4: Know how to view deduced types

- IDE Editors
    C++ compiler running inside the IDE

- Compiler Diagnosis
    cause compilation problem

```
    template<typename T>    // declaration only
    class TD;               // Type Displayer

    TD<decltype(x)> xType;
    TD<decltype(x)> yType;
```

- Runtime Output
```
    std::cout << typeid(x).name() << '\n';
    std::cout << typeid(y).name() << '\n';
```

invoking typeid on an obj such as x and y wields a std::type_info obj, and std::type_info has a member function name that produces a C-stype string representation of the name of the type

```
    template<typename T>
    void f(const T& param);

    std::vector<Widget> createVec();    // factory function

    const auto vw = createVec();    // init vw w factory return

    if (!vw.empty()){
        f(&vw[0]);  // call f
    }
```

this code involve a user-defined type Widget, an STL container (std::vector) and an auto variable (vw)


Boost TypeIndex isn't part of Standard C++, but neither are IDEs or templates like TD


```
    #include <boost/type_index.hpp>

    template<typename T>
    void f(const T& param)
    {
        using std::cout;

        using boost::typeindex::type_id_with_cvr;

        // show T
        cout << "T = "
             << type_id_with_cvr<T>().pretty_name()
             << '\n'

        // show param type
        cout << "param = "
             << type_id_with_cvr<decltype(param)>().pretty_name()
    }
```










































































