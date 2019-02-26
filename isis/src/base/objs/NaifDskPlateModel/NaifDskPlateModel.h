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

#include <QSharedPointer>
#include <QString>
#include <QVector>

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
   *   @history 2017-05-19 Christopher Combs - Modified unitTest.cpp: added ReportError method to
   *                          truncate paths before data directory. Allows test to pass when not
   *                          using the default data area. Fixes #4738.
   *   @history 2017-06-28 Kris Becker - Updated DSK includes for NAIF N0066 release that now
   *                           includes the DSK formally. The includes are now all in SpiceUsr.h.
   *                           Removed SPICE includes from the cpp file as well. Fixes #4947.
   *   @history 2019-02-26 Jesse Mapel - Changed API and logic to account for multi-segment
   *                           DSK files. Fixes #2632.
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
       * @brief NAIF DSK segment descriptor
       *
       * This class represents a segment from a NAIF DSK file.
       *
       * @author 2013-12-05 Kris Becker
       * @internal
       *   @history 2013-12-05 Kris Becker - Original Version
       *   @history 2019-02-26 Jesse Mapel - Removed mutex
       */
      class NaifDskDescriptor {
        public:
          NaifDskDescriptor();
          ~NaifDskDescriptor();

          QString       m_dskfile;  //!< The NAIF DSK file the segment belongs to.
          SpiceInt      m_handle;   //!< The DAS file handle of the DSK file.
          SpiceDLADescr m_dladsc;   //!< The DLA descriptor of the segment.
          SpiceDSKDescr m_dskdsc;   //!< The DSK descriptor of the segment.
          SpiceInt      m_plates;   //!< Number of Plates in the segment.
          SpiceInt      m_vertices; //!< Number of vertices in the segment.
      };

      // Shared file descriptor supports copying of object
      typedef QSharedPointer<NaifDskDescriptor>  SharedNaifDskDescriptor;

      QVector<SharedNaifDskDescriptor>  m_dsk; //!< Shared pointer to the NaifDskDescriptor for this plate.

      QVector<SharedNaifDskDescriptor> openDSK(const QString &dskfile);
      NaifTriangle plate(SpiceInt plateid, SharedNaifDskDescriptor segment) const;
      bool verify(const bool &test, const QString &errmsg,
                  const ErrAction &action = Throw) const;
      SurfacePoint *makePoint(const NaifVertex &v) const;
  };

} // namespace Isis

#endif
