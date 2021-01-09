AMF3 extension for PHP [![Build Status](https://travis-ci.org/rraptorr/php-amf3.svg?branch=php7)](https://travis-ci.org/rraptorr/php-amf3)
======================

What is PHP AMF3 extension?
---------------------------

PHP AMF3 extension offers two functions to use in PHP scripts:

string amf3_encode(mixed value)

- Encodes "value" into an AMF3 byte-stream
- On success, returns a byte-stream representation of "value"
- On error, returns FALSE and issues a warning message
  (the only error case is a wrong argument count)

mixed amf3_decode(string data [, int &count])

- Decodes "data" (AMF3 byte-stream) into a PHP value
- On success, returns a resulting PHP value
- On error, returns NULL and issues a warning message
- If parameter "count" is provided, the number of bytes read from "data" is stored
  in it (value of -1 indicates an error)


Required PHP version
--------------------

This module works with PHP 7.3, 7.4 and 8.0.

Installation
------------

To install the PHP-AMF3 extension, type the following in the source directory:

phpize
./configure --enable-amf3
make
make install

This should install the extension to your default PHP extension directory.
If it doesn't work as expected, manually put the target amf3.so library to
what the "extension_dir" variable in your php.ini file points to.
Add the following line to the corresponding extension section in your php.ini:

extension=amf3.so


Usage constraints
-----------------

- NULL, boolean, integer, float (double), string and array values are
  fully convertible back and forth to their corresponding types

- Objects are encoded with their corresponding class information
  (class name and members list). Objects of stdClass are encoded as
  anonymous classes with dynamic members.
- Dates are encoded from/to DateTime objects.
- If XML extension is enabled, XML datatypes are encoded from/to
  SimpleXMLElement objects.
- In a special case, PHP integers are converted into AMF3 doubles according
  to the specification (see the link below)
- Encoder's string reference table is maintained comparing strings by value
- Encoder's object reference table is maintained comparing objects by
  identity (as if using === operator)

Official Adobe AMF3 specification is available here:
http://download.macromedia.com/pub/labs/amf/amf3_spec_121207.pdf

NOTE: A PHP array is encoded as a sequence (dense array) when it has integer keys
that start with zero and have no gaps. In all other cases, an array is considered
as an associative array to avoid ambiguity.

NOTE: All objects are encoded as having additional dynamic properties
(possibly empty). This is caused by the fact that objects in PHP work
exactly like that, one can assign any dynamic properties to any
object.
