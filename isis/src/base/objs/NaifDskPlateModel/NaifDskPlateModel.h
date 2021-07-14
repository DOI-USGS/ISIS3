#ifndef NaifDskPlateModel_h
#define NaifDskPlateModel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   *   @history 2017-05-19 Christopher Combs - Modified unitTest.cpp: added ReportError method to
   *                          truncate paths before data directory. Allows test to pass when not
   *                          using the default data area. Fixes #4738.
   *   @history 2017-06-28 Kris Becker - Updated DSK includes for NAIF N0066 release that now
   *                           includes the DSK formally. The includes are now all in SpiceUsr.h.
   *                           Removed SPICE includes from the cpp file as well. Fixes #4947.
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
