Convex SAT optimization test
============================

Install:

1) Download the Bullet Library.
2) Update the include paths in the "Property Pages / Additional Include Directories".
3) Compile and run. Keys are printed to the console.

The timings are like this:

(Speed ratio) | Speed for Ref Algo | Speed for Optimized Algo | PD for Ref Algo | PD for Optimized Algo

PD = Penetration Depth. If everything works fine, both values should be the same.

------------------------------------------

This is a small proof-of-concept test, and only the tip of the iceberg as far as
convex SAT optimizations are concerned. Other optimizations that come to mind are:

- optimizing the hull projection using cubemaps + hill-climbing, Dobkin-Kirkpatrick
  hierarchies, SIMD code, etc
- moving computations to local space instead of world space
- precomputing projection values for face normals (results are already known for one
  object at least)
- culling features more aggressively
- not computing the penetration distance all the time. The PD is only a means to an
  end, not what we really want. What we really want are two polygons to clip, to
  generate contacts.....
- etc, etc

The good thing about this "vanilla" version is that is doesn't use precomputed
structures, it provides nice speedups, it's simple and easy to code, robust, and it
still gives exact results.

The bad thing is that it's not as fast as alternatives like GJK/EPA - or more advanced
convex SAT optimizations.

- Pierre Terdiman
February, 22, 2008