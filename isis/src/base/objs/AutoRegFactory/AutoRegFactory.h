#ifndef AutoRegFactory_h
#define AutoRegFactory_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


namespace Isis {
  class Pvl;
  class AutoReg;

  /**
   * This class is used to create AutoReg objects.  Typically applications which
   * need use autoregistration would like to use different techniques such as
   * MaximumCorrelation or MinimumDifference.  If this factory is given a Pvl
   * object which contains a AutoReg definition it will create that specific
   * instance of the class.  For example,
   *
   * @code
   * Object = AutoReg
   *   Group = Algorithm
   *     Name = MinimumDifference
   *     ...
   *   EndGroup
   *   ...
   * EndObject
   * End
   * @endcode
   *
   * Will create a MinimumDifference object (which is derived from AutoReg).
   * The simplest way to create an AutoReg class is to use the static Create
   * method
   *
   * @code
   * Pvl p("myautoreg.pvl");
   * AutoReg *ar = AutoRegFactory::Create(p);
   * @endcode
   *
   * @ingroup PatternMatching
   *
   * @author 2005-05-04 Jeff Anderson
   *
   * @internal
   *   @history 2006-03-27 Jacob Danton Added unitTest
   *   @history 2008-06-18 Christopher Austin Fixed documentation errors
   *   @history 2008-06-19 Steven Lambright Fixed memory leak
   */
  class AutoRegFactory {
    public:
      static AutoReg *Create(Pvl &pvl);

    private:
      /**
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      AutoRegFactory() {};

      //! Destroys the AutoRegFactory
      ~AutoRegFactory() {};
  };
};

#endif
