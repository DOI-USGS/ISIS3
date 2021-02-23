#ifndef InterestOperatorFactory_h
#define InterestOperatorFactory_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

namespace Isis {
  class Pvl;
  class InterestOperator;

  /**
   * This class is used to create InterestOperator objects.  Typically applications which
   * need use InterestOperators would like to use different techniques such as
   * Standard Deviation or Gradient.  If this factory is given a Pvl
   * object which contains a InterestOperator definition it will create that specific
   * instance of the class.
   *
   * @author 2006-02-04 Jacob Danton
   * @internal
   *   @history 2009-01-16 Steven Koechle - Fixed memory leak
   */

  class InterestOperatorFactory {
    public:
      static InterestOperator *Create(Pvl &pPvl);

    private:
      /**
       * Constructor (its private so you can't use it).  Use the Create Method
       * instead.
       */
      InterestOperatorFactory() {};

      //! Destroys the InterestOperatorFactory
      ~InterestOperatorFactory() {};
  };
};

#endif
