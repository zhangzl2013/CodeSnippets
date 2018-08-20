#!/usr/bin/env python3

### here are 68 built-in functions by groups.

#
# return Boolean values:
#     all(iterable)
#     any(iterable)
#     callable(object)
#     isinstance(object, classinfo)
#     issubclass(class, classinfo)
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
# culculate:
#     max()
#     min()
#     sum()
#     abs()
#     divmod()
#     pow()
#     round(number[, ndigits])
#
# data type:
#     list()
#     set()
#     frozenset()
#     bool()
#     float()
#     tuple()
#     dict()
#     complex()
#
# attributes:
#     delattr()
#     hasattr()
#     getattr()
#     setattr()
#     
# Unicode convertion:
#     ord()
#     chr()
#
# Iteration:
#     iter()
#     filter()
#     map()
#     reversed()
#     next()
#
# Utils:
#     hash()
#     id()
#     zip()
#
# Others:
#     ascii()
#     bytearray()
#     bytes()
#     chr()
#     compile()
#     dir()
#     enumerate()
#     eval()
#     exec()
#     format()
#     globals()
#     help()
#     input()
#     len()
#     locals()
#     memoryview()
#     object()
#     open()
#     print()
#     range()
#     repr()
#     slice()
#     sorted()
#     str()
#     super()
#     type()
#     vars()
#     __import__()

## abs(x)

print(abs(1))
# 1

print(abs(-1))
# 1

print(abs(-1.1))
# 1.1

print(abs(1+1j))
# 1.4142135623730951

print(abs(complex(1,1)))
# 1.4142135623730951



#### return Boolean values:
####     all(iterable)
####     any(iterable)
####     callable(object)
####     isinstance(object, classinfo)
####     issubclass(class, classinfo)

## all(iterable)

# Equivalent to:
# def all(iterable):
#     for element in iterable:
#         if not element:
#             return False
#     return True

print(all([1,2,3]))
# True

print(all([1,0,2]))
# False


## any(iterable)

# Equivalent to:
# def all(iterable):
#     for element in iterable:
#         if element:
#            return True
#     return False

print(any([1,2,3]))
# True

print(any([1,0,2]))
# True

print(any([0,0,0]))
# False


#### integer convertion
####     bin(x)
####     hex(x)
####     oct(x)

## ascii(object)
print(ascii([1,2,3]))
# '[1, 2, 3]'


print(ascii(False))
# 'False'

class a:
    pass

print(ascii(a))
# "<class '__main__.a'>"


## bin(x)
print(bin(3))
# '0b11'

print(bin(-10))
# '-0b1010'



#### @classmethod vs @staticmethod

### @classmethod

### @staticmethod
