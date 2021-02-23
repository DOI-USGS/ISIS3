#ifndef NormModelFactory_h
#define NormModelFactory_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

namespace Isis {
  class Pvl;
  class PhotoModel;
  class AtmosModel;
  class NormModel;

  /**
   * This class is used to create NormModel objects. Typically, applications which
   * perform normalization corrections need to use different methods such as
   * Shade, ShadeAtm, Albedo, etc. If this factory is given a Pvl object which
   * contains a NormModel definition, it will create that specific instance of the
   * class. For example,
   *
   * @code
   * Object = NormalizationModel
   *   Group = Algorithm
   *     Name = Shade
   *     ...
   *   EndGroup
   *   ...
   * EndObject
   * End
   * @endcode
   *
   * Will create a Shade normalization object (which is derived from
   * NormModel). The simplest way to create a NormModel class is to use
   * the static Create method.
   *
   * @code
   * Pvl p("mynormmodel.pvl");
   * NormModel *ar = NormModelFactory::Create(p);
   * @endcode
   *
   * @ingroup PatternMatching
   *
   * @author 2006-01-23 Janet Barrett
   *
   * @internal
   *   @history 2006-01-23 Janet Barrett - Original version
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Errors
   */
  class NormModelFactory {
    public:
      static NormModel *Create(Pvl &pvl, PhotoModel &pmodel);
      static NormModel *Create(Pvl &pvl, PhotoModel &pmodel, AtmosModel &amodel);

    private:
      /**
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      NormModelFactory() {};

      //! Destroys the NormModelFactory
      ~NormModelFactory() {};
  };
};

#endif
