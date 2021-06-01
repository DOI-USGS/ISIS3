#ifndef AtmosModelFactory_h
#define AtmosModelFactory_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

namespace Isis {
  class Pvl;
  class PhotoModel;
  class AtmosModel;


  /**
   *  This class is used to create AtmosModel objects. Typically, applications
   *which perform atmospheric corrections need to use different types of
   *atmospheric function such as Isotropic1, Anisotropic1, HapkeAtm1, etc. If this
   *factory is given a Pvl object which contains an AtmosModel definition, it will
   *create that specific instance of the class. For example,
   *
   * @code
   *  Object = AtmosphericModel Group = Algorithm
   *    AstName/Name = Isotropic1 ...
   *  EndGroup ...
   *EndObject End
   * @endcode
   *
   * Will create an Isotropic 1st order object (which is derived from AtmosModel).
   * The simplest way to create an AtmosModel class is to use the static Create
   * method
   *
   * @code
   * Pvl p("myatmosmodel.pvl");
   * AtmosModel *ar = AtmosModelFactory::Create(p);
   * @endcode
   *
   * @ingroup PatternMatching
   *
   * @author 2006-01-23 Janet Barrett
   *
   * @internal
   *  @history 2008-06-18 Christopher Austin - Fixed documentation errors
   *  @history 2011-08-25 Sharmila Prasad - Use 'AstName' for Model name to be able to be
   *             used in combo Gui which requires unique parameter name. Maintaining 'Name'
   *             to support existing apps using this keyword.
   */
  class AtmosModelFactory {
    public:
      static AtmosModel *Create(Pvl &pvl, PhotoModel &pmodel);

    private:
      /**
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      AtmosModelFactory() {};

      //! Destroys the AtmosModelFactory
      ~AtmosModelFactory() {};
  };
};

#endif
