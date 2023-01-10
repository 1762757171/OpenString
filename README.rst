**OpenString** is an open-source C++17 string library for game developing.

.. image:: https://github.com/1762757171/OpenString/actions/workflows/windows.yml/badge.svg
   :target: https://github.com/1762757171/OpenString/actions/workflows/windows.yml

This is based on utf-8 encoding, providing easy-using operating functions
and {fmt}-like formatting method.

=====
Usage
=====

Instead of being named string, the core concepts are `codeunit sequence` and `text`, 
which are more clearly for understanding and more unambiguous than ``string``.

All classes provide user-friendly manipulation functions.

For ordinary usage, class ``text`` is more commonly used as string with readability.

class ``text``
--------------

Class ``text`` is designed as a human-readable character sequence, 
which encapsulates the encoding-related functionalities. 
This means users do not need to learn anything about encoding. 
You just need to use it like common sense, 
treating ASCII characters, Chinese characters, 
or emoji as inseparable code point elements.

class ``text_view``
-------------------

A readonly view for human-readable character sequence.

class ``codeunit_sequence``
---------------------------

As the name suggests, class ``codeunit_sequence`` is a raw container of type `char`.

class ``codeunit_sequence_view``
--------------------------------

A readonly view for a raw code unit sequence.

========
Examples
========

**Construct text_view or text**

.. code:: c++

    #include "text.h"
    
    {
        text_view tv = "Hello world!"_txtv;
        text t = "😀";  // t.size() == 1
    }

==========
Benchmarks
==========

todo

===
Q&A
===

Why both Visual Studio solution file and xmake.lua exist?
---------------------------------------------------------

This project provides two ways to build, 
both Visual Studio and XMake are supported. 
CMake support may be added later.

=======
License
=======

OpenString is distributed under the MIT license.
