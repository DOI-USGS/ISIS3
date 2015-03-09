#ifndef NaifDskPlateModel_h
#define NaifDskPlateModel_h
/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2009/08/25 01:37:55 $
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

#include <QMutex>
#include <QSharedPointer>
#include <QString>

#include "NaifDskApi.h"

namespace Isis {

  class Intercept;
  class Latitude;
  class Longitude;
  class SurfacePoint;
  
  /**
   * @brief Implementation interface API for NAIF's DSK plate model 
   *  
   * This class implements a thread safe version of the NAIF DSK plate model API. 
   * This version is coded using the "alpha" version of the DSK library toolkit as 
   * release in 2010. 
   *  
   * Part of the design of this implementation is the abilty to efficiently clone 
   * the object so that it is suitable for use in a threaded environment.  Note 
   * that files is only closed when the last reference is released. 
   *  
   * It is recommended that a shared or scoped pointer that provides the necessary 
   * elements for efficient safe thread computing. 
   * 
   * @author 2013-12-05 Kris Becker 
   * @internal 
   *   @history 2013-12-05 Kris Becker  Original Version 
   *   @history 2015-03-08 Jeannie Backer - Added documentation and test. Added class to ISIS trunk.
   *                           References #2035
   */
  class NaifDskPlateModel {
  
    public:
      NaifDskPlateModel();
      NaifDskPlateModel(const QString &dskfile);
      virtual ~NaifDskPlateModel();
  
      bool isValid() const;
      QString filename() const;
  
      int size() const;
      SpiceInt numberPlates() const;
      SpiceInt numberVertices() const;
  
      SurfacePoint *point(const Latitude &lat, const Longitude &lon) const;
      Intercept *intercept(const NaifVertex &vertex, const NaifVector &raydir) const;
  //    Intercept *intercept(const SurfacePoint &pnt) const;
  
      // Lower level I/O
      bool     isPlateIdValid(const SpiceInt plateid) const;
      SpiceInt plateIdOfIntercept(const NaifVertex &vertex, 
                                     const NaifVector &raydir,
                                     NaifVertex &xpoint) const;
      NaifTriangle plate(SpiceInt plateid) const;
  
      NaifDskPlateModel *clone() const;
  
    private:
      /** 
       * Enumeration to indicate whether to throw an exception if an error 
       * occurs. 
       */ 
      enum ErrAction { Throw,  //!< Throw an exception if an error occurs.
                       NoThrow //!< Do not throw an exception if an error occurs.
      };
  
    /**
     * @brief NAIF DSK file descriptor 
     *  
     * This local class is designed to make the plate model object copyable, thread 
     * safe and inherently extensible.  The file remains open as long as the 
     * original NaifDskDescriptor has a reference. 
     * 
     * @author 2013-12-05 Kris Becker 
     * @internal 
     *   @history 2013-12-05 Kris Becker  Original Version 
     */
      class NaifDskDescriptor {
        public:
          NaifDskDescriptor();
          ~NaifDskDescriptor();
  
          QString       m_dskfile;  //!< The NAIF DSK file representing this plate's shape model.
          SpiceInt      m_handle;   //!< The DAS file handle of the DSK file.
          SpiceDLADescr m_dladsc;   /**< The DLA descriptor of the DSK segment representing the 
                                         target surface.*/
          SpiceDSKDescr m_dskdsc;   //!< The DSK descriptor.
          SpiceInt      m_plates;   //!< Number of Plates in the model.
          SpiceInt      m_vertices; //!< Number of vertices defining the plate.
          QMutex        m_mutex;    //!< Mutex for thread saftey
      };
      
      // Shared file descriptor supports copying of object
      typedef QSharedPointer<NaifDskDescriptor>  SharedNaifDskDescriptor;
      SharedNaifDskDescriptor  m_dsk; //!< Shared pointer to the NaifDskDescriptor for this plate. 
  
      NaifDskDescriptor *openDSK(const QString &dskfile);
      bool verify(const bool &test, const QString &errmsg, 
                  const ErrAction &action = Throw) const;
      SurfacePoint *makePoint(const NaifVertex &v) const;
  };

} // namespace Isis

#endif

