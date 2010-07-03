#if !defined(InterestOperatorFactory_h)
#define InterestOperatorFactory_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/06/10 23:42:31 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

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
