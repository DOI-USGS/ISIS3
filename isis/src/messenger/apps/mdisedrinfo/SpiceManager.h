#ifndef SpiceManager_h
#define SpiceManager_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include <iostream>

#include "Pvl.h"
#include "Cube.h"
#include "Camera.h"
#include "IException.h"

namespace Isis {


  /**
   * @brief Load/Unload SPICE kernels defined in an ISIS file
   *
   * This class determines all SPICE kernels associated to an ISIS cube file and
   * optionally loads them using the NAIF toolkit.  This creates the kernel pool
   * as it was when spiceinit determined all the kernels and it initialized the
   * file for geometric operations.
   *
   * Note that ISIS caches of some of the voluminous NAIF kernels, extracting
   * only what is required from the SPK and CK (generally) kernels for a given
   * observation.  After this occurs, these kernels are no longer loaded by the
   * ISIS Spice class hierarchy.  This class provides that environment so that
   * further NAIF operations can occur, such as velocity vectors.
   *
   * @ingroup Utility
   * @author 2007-08-19 Kris Becker
   *
   * @internal
   * @history 2015-07-22 Kristin Berry -  Added NaifStatus::CheckErrors() to see if any NAIF errors
   *                       were signaled. References #2248.
   */
  class SpiceManager {
    public:
      /** Default Constructor */
      SpiceManager() : _kernlist(), _furnish(true) { }
      SpiceManager(const QString &filename, bool furnish = true);
      SpiceManager(Cube &cube, bool furnish = true);
      SpiceManager(Pvl &pvl, bool furnish = true);
      /** Destructor always unloads the kernels from the pool */
      virtual ~SpiceManager() {
        Unload();
      }

      /** Returns the number of kernels found and/or loaded */
      int size() const {
        return (_kernlist.size());
      }

      void Load(Pvl &pvl, bool furnish = true);
      void add(const QString &kernel);
      std::vector<QString> getList(bool removePath = false) const;
      void Unload();


    private:
      std::vector<QString> _kernlist;  //!< The list of kernels
      bool _furnish;                       //!< Load the kernels found?

      void loadKernel(PvlKeyword &key);
      void loadKernelFromTable(PvlKeyword &key, const QString &tblname,
                               Pvl &pvl);
      void addKernelName(const QString &kname);

  };

}     // namespace Isis
#endif
