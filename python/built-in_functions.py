#!/usr/bin/env python3

### here are 68 built-in functions by groups.

#
# return Boolean values:
#     all(iterable)
#     any(iterable)
#
# integer convertion:
#     bin(x)
#     hex(x)
#     oct(x)
#
# decorators:
#     @staticmethod
#     @classmethod
#     @property
#
# math culculation:
#     max()
#     min()
#     sum()
#     abs()
#     divmod()
#     pow()
#     round(number[, ndigits])
#
# Data types:
#     int()       # numbers.Integral (int)
#     bool()      # numbers.Integral (bool)
#     float()     # numbers.Real (float)
#     complex()   # numbers.Complex (complex)
#
#     N/A         # Immutable sequences : Strings
#     tuple()     # Immutable sequences : Tuples
#     bytes()     # Immutable sequences : Bytes
#
#     list()      # Mutable sequences: Lists
#     bytearray() # Mutable sequences: Byte Arrays
#
#     set()       # Set types: Sets
#     frozenset() # Set types: Frozen sets
#
#     dict()      # Mappings: Dictionaries
#
# Unicode convertion: (Unicode code range: U+0 to U+10FFFF)
#     ord()       # Unicode character  ->  Unicode code
#     chr()       # Unicode code       ->  Unicode character
#
# Iteration:
#     iter()
#     reversed()
#     next()
#     enumerate()
#
# High-order functions: (Functions that act on or return other functions)
#     sorted()
#     filter()
#     map()
#
# Utils:
#     hash()
#     zip()
#
# Objects related: (Reflection and Introspection)
#     id()
#     object()
#     type()                        # Reflection-enabling function
#     super()
#
#     str()
#     repr()
#     ascii()
#
#     dir()                         # Reflection-enabling function
#     globals()
#     locals()
#
#     vars()
#     memoryview()
#     __import__()
#
#     callable(object)              # Reflection-enabling function
#     isinstance(object, classinfo) # Reflection-enabling function
#     issubclass(class, classinfo)
#
#  # attributes:
#     delattr()
#     hasattr()
#     getattr()                     # Reflection-enabling function
#     setattr()
#     
#
# Sequence helpers:
#     len()
#     slice()
#     range()
#
# Dynamic execution:
#     compile()
#     eval()
#     exec()
#
# String operation:
#     format()
#
# Interactive:
#     print()
#     input()
#     help()
#
# Others:
#     open()


# Introspection vs Reflection
#
# In computing, type introspection is the ability of a program to *examine*
# the type or properties of an object *at runtime*. Some programming languages
# possess this capability.
#
# Introspection should not be confused with reflection, which goes a step
# further and is the ability for a program to *manipulate* the values, meta-
# data, properties and/or functions of an object at runtime. Some programming
# languages, e.g. Java, also possess that capability.
#                        -- [https://en.wikipedia.org/wiki/Type_introspection]
#
# In computer science, reflection is the ability of a computer program to
# examine, introspect, and modify its own structure and behavior at runtime.
#         -- [https://en.wikipedia.org/wiki/Reflection_(computer_programming)]
#









##################################################################
# return Boolean values:
#     all(iterable)
#     any(iterable)
##################################################################


#

## all(iterable)

# Equivalent to:
# def all(iterable):
#     for element in iterable:
#         if not element:
#             return False
#     return True

all([1,2,3])
# True

all([1,0,2])
# False


## any(iterable)

# Equivalent to:
# def all(iterable):
#     for element in iterable:
#         if element:
#            return True
#     return False

any([1,2,3])
# True

any([1,0,2])
# True

any([0,0,0])
# False


##################################################################
# integer convertion:
#     bin(x)
#     hex(x)
#     oct(x)
##################################################################


#                           bin(x)
bin(3)
# '0b11'

bin(-10)
# '-0b1010'

#                           hex(x)

#                           oct(x)

##################################################################
# decorators:
#     @staticmethod
#     @classmethod
#     @property
##################################################################


#
#### @classmethod vs @staticmethod

### @classmethod

### @staticmethod
##################################################################
# math culculation:
#     max()
#     min()
#     sum()
#     abs()
#     divmod()
#     pow()
#     round(number[, ndigits])
##################################################################


#
## abs(x)

abs(1)
# 1

abs(-1)
# 1

abs(-1.1)
# 1.1

abs(1+1j)
# 1.4142135623730951

abs(complex(1,1))
# 1.4142135623730951

sum([1,2,3])
# 6
sum([1,2,3],1)
# 7

##################################################################
# Data types:
#     int()       # numbers.Integral (int)
#     bool()      # numbers.Integral (bool)
#     float()     # numbers.Real (float)
#     complex()   # numbers.Complex (complex)
#
#     N/A         # Immutable sequences : Strings
#     tuple()     # Immutable sequences : Tuples
#     bytes()     # Immutable sequences : Bytes
#
#     list()      # Mutable sequences: Lists
#     bytearray() # Mutable sequences: Byte Arrays
#
#     set()       # Set types: Sets
#     frozenset() # Set types: Frozen sets
#
#     dict()      # Mappings: Dictionaries
##################################################################


#
##################################################################
# Unicode convertion: (Unicode code range: U+0 to U+10FFFF)
#     ord()       # Unicode character  ->  Unicode code
#     chr()       # Unicode code       ->  Unicode character
##################################################################


##################################################################
# Iteration:
#     iter()
#     reversed()
#     next()
#     enumerate()
##################################################################

# Iterable vs Iterator
#
# Iterable: has __iter__() method, can not be passed to next()
# Iterator: has __next__() method, can be passed to next()

#                      iter()
#                      next()
t = (1,2)
i = iter(t)
type(t)
# output: <class 'tuple'>
type(i)
# output: <class 'tuple_iterator'>
next(t)
# output:
# Traceback (most recent call last):
#   File "<stdin>", line 1, in <module>
# TypeError: 'tuple' object is not an iterator
next(i)
# 1
next(i)
# 2
next(i)
# Traceback (most recent call last):
#   File "<stdin>", line 1, in <module>
# StopIteration


##################################################################
# High-order functions: (Functions that act on or return other functions)
#     sorted()
#     filter()
#     map()
##################################################################


##################################################################
# Utils:
#     hash()
#     zip()
##################################################################


##################################################################
# Objects related: (Reflection and Introspection)
#     id()
#     object()
#     type()                        # Reflection-enabling function
#     super()
#
#     str()
#     repr()
#     ascii()
#
#     dir()                         # Reflection-enabling function
#     globals()
#     locals()
#
#     vars()
#     memoryview()
#     __import__()
#
#     callable(object)              # Reflection-enabling function
#     isinstance(object, classinfo) # Reflection-enabling function
#     issubclass(class, classinfo)
#
#  # attributes:
#     delattr()
#     hasattr()
#     getattr()                     # Reflection-enabling function
#     setattr()
##################################################################


#     
#
## ascii(object)
ascii([1,2,3])
# '[1, 2, 3]'


ascii(False)
# 'False'

class a:
    pass

ascii(a)
# "<class '__main__.a'>"

##################################################################
# Sequence helpers:
#     len()
#     slice()
#     range()
##################################################################


##################################################################
# Dynamic execution:
#     compile()
#     eval()
#     exec()
##################################################################


#     [https://stackoverflow.com/questions/2220699/whats-the-difference-between-eval-exec-and-compile-in-python]

##################################################################
# String operation:
#     format()
##################################################################


##################################################################
# Interactive:
#     print()
#     input()
#     help()
#
##################################################################


##################################################################
# Others:
#     open()
##################################################################
