#ifndef KernelDb_h
#define KernelDb_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include <iostream>
#include <queue>
#include <gtest/gtest_prod.h>

#include <QList>
#include <QString>
#include <QStringList>

#include "iTime.h"//???
#include "Kernel.h"
#include "Pvl.h"

class TestKernelDb_TestKernelsSmithOffset_Test;

namespace Isis {
  class FileName;
  class iTime;
  /**
   * @brief KernelDb class
   *
   * This class handles kernel database files.  Once the database files have been
   * loaded into a Pvl, the highest version of each kernel can be found by
   * calling methods with corresponding kernel names, as shown below.
   *
   * <code>
   * KernelDb baseKernels(0);
   *
   * baseKernels.loadSystemDb(mission, lab);
   *
   * Kernel lk, pck, targetSpk, fk, ik, sclk, spk, iak, dem, exk;
   * priority_queue< Kernel > ck;
   * lk        = baseKernels.leapSecond(lab);
   * pck       = baseKernels.targetAttitudeShape(lab);
   * targetSpk = baseKernels.targetPosition(lab);
   * ik        = baseKernels.instrument(lab);
   * sclk      = baseKernels.spacecraftClock(lab);
   * iak       = baseKernels.instrumentAddendum(lab);
   * fk        = ckKernels.frame(lab);
   * ck        = ckKernels.spacecraftPointing(lab);
   * spk       = spkKernels.spacecraftPosition(lab);
   *
   * </code>
   * @ingroup System
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2005-12-27 Jacob Danton - Added support for multiple
   *                           files and fixed a bug in SearchMatch.
   *   @history 2007-07-09 Steven Lambright - Removed inheritance from PVL
   *   @history 2007-10-25 Steven Koechle - Corrected Smithed Enum
   *                           fixed search methods.
   *   @history 2007-07-09 Steven Lambright - Added Kernel class and multiple-ck
   *                           return
   *   @history 2009-05-12 Steven Lambright - Added Camera Version Checking
   *   @history 2010-07-19 Steven Lambright - Added kernel selection merging
   *   @history 2011-10-05 Jacob Danton - The placement of int cameraVersion =
   *                           CameraFactory::CameraVersion(lab); was the reason
   *                           that spiceinit ran so slowly. Its placement in the
   *                           "Matches" function resulted in CameraFactory
   *                           reading in the "Camera.plugin" file for every
   *                           single kernel in both the CK and SPK
   *                           kernels.????.db files. That means, for LRO, it was
   *                           read in around 2100 times. By moving this line of
   *                           code out of the "Matches" function and into the
   *                           "FindAll" function (the only one that calls
   *                           "Matches") and then passing it as a parameter, I
   *                           was able to reduce the time to about 5% (from
   *                           almost 4 minutes to under 15 seconds).
   *   @history 2013-02-26 Jeannie Backer - Modified loadSystemDb() to check
   *                           kernel directories first for config files that are
   *                           mission specific and/or point to multiple db
   *                           files. If the config files do not exist, then the
   *                           newest db file from that kernel directory is read.
   *                           Added m_kernelDbFiles, kernelDbFiles(), and
   *                           readKernelDbFiles(). Moved Kernel class from the
   *                           KernelDb header file into it's own directory.
   *                           Moved the KernelType enumeration from a global
   *                           "spiceInit" namespace in this header file into the
   *                           Kernel class and renamed it Type. Changed method
   *                           names to lower camel case and changed member
   *                           variable prefix to m_ to comply with Isis3
   *                           standards. Documented methods, member variables.
   *                           Added cases to unitTest.Fixes #924.
   *   @history 2013-02-27 Steven Lambright - This class no longer requires the Instrument group (or
   *                           camera version information) to be present in order to enumerate
   *                           kernels. This was done so that the 'shadow' program could find a
   *                           PCK and SPK to load despite not having a cube with camera
   *                           information. References #1232.
   */
  class KernelDb {

    public:
      // constructor
      KernelDb(const unsigned int allowedKernelTypes);
      KernelDb(const QString &dbName, const unsigned int allowedKernelTypes);
      KernelDb(std::istream &dbStream, const unsigned int allowedKernelTypes);

      // destructor
      ~KernelDb();

      // Members for getting kernels
      Kernel leapSecond(Pvl &lab);
      Kernel targetAttitudeShape(Pvl &lab);
      Kernel targetPosition(Pvl &lab);
      QList< std::priority_queue<Kernel> > spacecraftPointing(Pvl &lab);
      Kernel spacecraftClock(Pvl &lab);
      Kernel spacecraftPosition(Pvl &lab);
      Kernel instrument(Pvl &lab);
      Kernel frame(Pvl &lab);
      Kernel instrumentAddendum(Pvl &lab);
      Kernel dem(Pvl &lab);

      Kernel findLast(const QString &entry, Pvl &lab);
      QList< std::priority_queue<Kernel> > findAll(const QString &entry,
                                                   Pvl &lab);

      void loadSystemDb(const QString &mission, const Pvl &lab);
      QList<FileName> kernelDbFiles();

      static bool matches(const Pvl &lab, PvlGroup &kernelDbGrp,
                          iTime timeToMatch, int cameraVersion);
    private:
      FRIEND_TEST(::TestKernelDb, TestKernelsSmithOffset);

      void loadKernelDbFiles(PvlGroup &dataDir,
                             QString directory,
                             const Pvl &lab);
      void readKernelDbFiles();

      QStringList files(PvlGroup &grp);
      QString m_filename; /**< The name of the kernel database file. This
                               may be set to "None" or "internal stream".*/
      QList<FileName> m_kernelDbFiles; /**< List of the kernel database file
                                            names that were read in when the
                                            loadSystemDb() method is called.*/
      unsigned int m_allowedKernelTypes; /**< This integer value represents
                                              which Kernel::Types are allowed. It
                                              is the sum of the enumeration
                                              values of the allowed Kernel::Types.
                                              When this integer value and the
                                              enumeration types are  expressed
                                              binary numbers, it is clear which
                                              types are allowed.*/
      Pvl m_kernelData; /**< Pvl containing the information in the kernel
                             database(s) that is read in from the constructor
                             and whenever the loadSystemDb() method is called.*/
  };
};

#endif
