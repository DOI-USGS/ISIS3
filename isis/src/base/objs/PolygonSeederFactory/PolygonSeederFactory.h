#ifndef PolygonSeederFactory_h
#define PolygonSeederFactory_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

namespace Isis {
  class Pvl;
  class PolygonSeeder;

  /**
   * This class is used to create PolygonSeeder objects. Applications which
   * use autoseeding of points in polygons can use different techniques such as
   * Grid or ????.  If this factory is given a Pvl
   * object which contains a PolygonSeeder definition it will create that specific
   * instance of the class.  For example,
   *
   * @code
   * Object = PolygonSeeder
   *   Group = Algorithm
   *     Name = Grid
   *     ...
   *   EndGroup
   *   ...
   * EndObject
   * End
   * @endcode
   *
   * Will create a GridPolygonSeeder object (which is derived from PolygonSeeder).
   * The simplest way to create an PolygonSeeder class is to use the static Create
   * method
   *
   * @code
   * Pvl p("myPolygonSeeder.pvl");
   * PolygonSeeder *ps = PolygonSeederFactory::Create(p);
   * @endcode
   *
   * @ingroup PatternMatching
   *
   * @author 2006-01-20 Stuart Sides
   *
   * @internal
   *   @history 2008-12-17 Christopher Austin - Fixed memory leak
   */
  class PolygonSeederFactory {
    public:
      static PolygonSeeder *Create(Pvl &pvl);

    private:
      /**
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      PolygonSeederFactory() {};

      //! Destroys the PolygonSeederFactory
      ~PolygonSeederFactory() {};
  };
};

#endif
