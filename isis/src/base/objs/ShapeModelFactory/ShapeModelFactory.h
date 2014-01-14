#ifndef ShapeModelFactory_h
#define ShapeModelFactory_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/06/19 23:35:38 $
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

#include "ShapeModel.h"

namespace Isis {
  class Pvl;

  /**
   * This class is used to create ShapeModel objects.  It determines the type
   * of shape model in the input Pvl object and creates the appropriate type of 
   * shape model.
   *
   * @author 2010-07-29 Debbie A. Cook
   *
   * @internal
   *   @history 2012-10-31 - Ken Edmundson - Implemented plane shape
   *   @history 2014-01-14 - Jeannie Backer - Improved error message. Fixes
   *            #1957.
   */
  class ShapeModelFactory {
    public:
    static ShapeModel *create(Target *target, Pvl &pvl);

    private:
      /**
       * Constructor is private to avoid instantiating the class.  Use the Create method.
       * 
       * @author dcook (7/29/2010)
       */
      ShapeModelFactory() {};

      //! Destructor
      ~ShapeModelFactory() {};

      // Supported shape models
      enum {
        Ellipsoid,
        Isis3EquatorialCylindrical,
        Isis3Dem,
        Plane,
        Stack,
        NaifDSK};
  };
}

#endif
