#ifndef NaifDskShape_h
#define NaifDskShape_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "ShapeModel.h"

#include <vector>

#include <QVector>

#include "Intercept.h"
#include "NaifDskPlateModel.h"

namespace Isis {
  class Intercept;

  /**
   * @brief Provides support for NAIF's Digital Shape Kernel (DSK)
   *
   * @author 2014-02-07 Kris Becker
   *
   * @internal 
   *   @history 20104-02-07 - Kris Becker - Original Version
   *   @history 2015-03-08 Jeannie Backer - Changed name from  NaifDskShapeModel to match other
   *                           classes derived from ShapeModel. Added documentation and test.
   *                           Added class to ISIS trunk. References #2035
   *  
   *   @todo Remove Model from name to match other derived classes
   */
  class NaifDskShape : public ShapeModel {
    public:
      // Constructors
      NaifDskShape();
      NaifDskShape(Target *target, Pvl &pvl);
      NaifDskShape(const NaifDskPlateModel &model);

      // Destructor
      ~NaifDskShape();

      // Intersect the shape model
      bool intersectSurface(std::vector<double> observerPos,
                            std::vector<double> lookDirection);

      // Calculate the default normal of the current intersection point
      void calculateDefaultNormal(); 

      // Calculate the surface normal of the current intersection point
      void setLocalNormalFromIntercept();
      void calculateLocalNormal(QVector<double *> cornerNeighborPoints); 
      void calculateSurfaceNormal(); 

      Distance localRadius(const Latitude &lat, const Longitude &lon);

      QVector<double> ellipsoidNormal();

      const NaifDskPlateModel &model() const;
      const Intercept *intercept() const;

    private:
      // Disallow copying because ShapeModel is not copyable
      NaifDskShape(const NaifDskShape &model);
      NaifDskShape &operator=(const NaifDskShape &model);

      NaifDskPlateModel         m_model;     //!< Plate model to intersect.
      QScopedPointer<Intercept> m_intercept; //!< Pointer to the shape's intercept.


  };
}

#endif

