
 Shows how to implement a fast "pair manager", i.e. a data structure holding pairs of objects.
 A pair is identified by a couple of shorts (id0, id1) but the code is easy to modify to match
 your needs. This version has been designed for a sweep-and-prune implementation.
 The class is very small and fast, providing:

 - O(1) insertion
 - O(1) removal
 - O(1) find

 In addition, at any time the current set of pairs is available in a contiguous memory array.

 ---------------------------

 This is public domain code. Feel free to use it anywhere.



 - Pierre Terdiman
 pierre@codercorner.com
