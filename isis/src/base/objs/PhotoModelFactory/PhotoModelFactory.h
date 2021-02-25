#ifndef PhotoModelFactory_h
#define PhotoModelFactory_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

namespace Isis {
  class Pvl;
  class PhotoModel;

  /**
   * This class is used to create PhotoModel objects. Typically, applications which
   * perform photometric corrections need to use different types of photometric
   * function such as Lambert, Minnaert, HapkeLegendre, etc. If this factory is
   * given a Pvl object which contains a PhotoModel definition, it will create that
   * specific instance of the class. For example,
   *
   * @code
   * Object = PhotometricModel
   *   Group = Algorithm
   *     Name = Minnaert
   *     ...
   *   EndGroup
   *   ...
   * EndObject
   * End
   * @endcode
   *
   * Will create a Minnaert object (which is derived from PhotoModel). The
   * simplest way to create a PhotoModel class is to use the static Create
   * method.
   *
   * @code
   * Pvl p("myphotmodel.pvl");
   * PhotoModel *ar = PhotoModelFactory::Create(p);
   * @endcode
   *
   * @ingroup PatternMatching
   *
   * @author 2006-01-23 Janet Barrett
   *
   * @internal
   *   @history 2006-01-23 Janet Barrett - Original version
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   *
   */
  class PhotoModelFactory {
    public:
      static PhotoModel *Create(Pvl &pvl);

    private:
      /**
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      PhotoModelFactory() {};

      //! Destroys the PhotoModelFactory
      ~PhotoModelFactory() {};
  };
};

#endif
