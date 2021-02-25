#ifndef HiBlob_h
#define HiBlob_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include "Blobber.h"
#include "HiCalTypes.h"

namespace Isis {

  /**
   * @brief BLOB extraction class
   *
   * This class provides access and processing of HiRISE data as
   * stored in an ISIS BLOB (Table).
   *
   * Note that the file provided must be a HiRISE ISIS cube file
   * that is freshly converted from a PDS compatable EDR
   * (hi2isis).  It must contain an ISIS Table named "HiRISE
   * Calibration Image".  From that table, data is extracted from
   * the "Calibration" field.
   *
   * @ingroup MarsReconnaissanceOrbiter
   *
   * @author 2007-10-09 Kris Becker
   * @see Blobber
   *
   * @internal
   *  @history 2008-05-12 Steven Lambright - Removed references to CubeInfo
   */
  class HiBlob : public Blobber {
    public:
      /**
       * @brief Default, mostly useless constructor
       */
      HiBlob(): Blobber() { }

      /**
       * @brief Constructor providing interface to an ISIS Cube object
       */
      HiBlob(Cube &cube, const QString &tblname,
             const QString &field,
             const QString &name = "HiBlob") :
        Blobber(cube, tblname, field, name)  { }

      /** Destructor */
      virtual ~HiBlob() { }

      /** Return a reference to the data */
      const HiMatrix &buffer() const {
        return (ref());
      }

  };
};

#endif
