#ifndef ControlNetVersioner_h
#define ControlNetVersioner_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2009/07/15 17:33:52 $
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

#include <string>

#include "ControlNetFile.h"
#include "ControlNetFileV0002.pb.h"

class QString;

namespace Isis {
  class ControlNetFileV0001;
  class ControlNetFileV0002;
  class FileName;
  class Progress;
  class Pvl;
  class PvlContainer;
  class PvlObject;

  /**
   * @brief Handle Various Control Network Versions
   *
   * This class is used to read any and all control networks.
   *
   * All publicly released versions of binary control networks should be
   *   supported if possible.
   *
   *                         ControlNetVersioner::Read
   *                        /                         \
   *                    [If Pvl]                    [If Binary]
   *                      /                             \
   *                  Pvl::Read                         |
   *                     |          [If not latest]     |
   *              Update To Latest <-- ToPvl --- ControlNetFileV????::Read
   *                     |                              |  [If latest]
   *                     |                              |
   *                 Latest Pvl  ------------> Latest ControlNetFile
   *                                                    |
   *                                                    |
   *                                             Isis::ControlNet
   *
   *
   * We used to have the 4 conversions:
   *   Pvl    -> Isis::ControlNet
   *   Binary -> Isis::ControlNet
   *   Pvl    <- Isis::ControlNet
   *   Binary <- Isis::ControlNet
   *
   * But maintaining these causes us to need the old ControlNet code around.
   *   These conversions are used instead:
   *   Pvl    -> Binary           *Latest version only
   *   Binary -> Pvl              *All versions
   *   Binary -> Isis::ControlNet *Latest version only
   *   Binary <- Isis::ControlNet *Latest version only
   *
   * The log data classes are still used to understand what log data is what,
   *   so these classes must remain backwards compatible. Otherwise all of the
   *   versioning code is here. I encourage the use of log data to avoid needing
   *   to make any changes in this code.
   *
   * The reason the update cycle is only in Pvl form is because of how much
   *   simpler and less error-prone the code is to convert between versions
   *   in a generic file format. You don't need to do things like
   *   new.setNetworkId(old.getNetworkId()) in Pvl. Hopefully the speed cost
   *   is not significant enough to need an update cycle in binary form. It is
   *   a one-time cost per network, timed at about 5 minutes for our currently
   *   largest network (120MB protocol buffer file).
   *
   * This class is the reason Isis::ControlNet only need to work with the latest
   *   version. Also, we only need 1-way conversions for old file formats
   *   (ControlNetFile::ToPvl).
   *
   * If you want to change the Pvl format, you must update the following:
   *     Update LATEST_PVL_VERSION
   *     Write ConvertVersionAToVersionB
   *     Update ReadPvlNetwork
   *     Update LatestPvlToBinary
   *
   * If you want to change the Binary format, you must update the following:
   *     Update LATEST_BINARY_VERSION
   *     Write ControlNetFileV????
   *     Update ControlNetFile.h
   *     Update ReadBinaryNetwork
   *     Update LatestPvlToBinary
   *
   * @ingroup ControlNetwork
   *
   * @author 2011-04-05 Steven Lambright
   *
   * @internal
   *   @history 2011-04-14 Steven Lambright - Improved handling of V1 networks
   *   @history 2011-05-02 Tracie Sucharski - If Held=True exists in input net,
   *                           set PointType=Ground.
   *   @history 2011-05-02 Debbie A. Cook - Created pvl version 3 which added
   *                           point type of constrained.
   *   @history 2012-08-14 Steven Lambright - Simplified error handling of invalid
   *                           target names in V1 networks (V1->V2 code).
   *   @history 2013-03-13 Steven Lambright and Stuart Sides - Added support for more V1 Pvl
   *                           networks (specifically, isis3.2.1 hijitreg output networks with
   *                           measures that lack Sample/Line and are set to unmeasured). Fixes
   *                           #1554.
   */
  class ControlNetVersioner {
    public:
      static LatestControlNetFile *Read(const FileName &file);
      static void Write(const FileName &file, const LatestControlNetFile &,
                        bool pvl = false);

    private:
      // read Pvl and bring it up to the latest version, then convert to binary
      static LatestControlNetFile *ReadPvlNetwork(Pvl pvl);
      static LatestControlNetFile *LatestPvlToBinary(PvlObject &network);

      // read Binary, convert to Pvl, call ReadPvlNetwork
      static LatestControlNetFile *ReadBinaryNetwork(const Pvl &header,
                                                     const FileName &file);

      static void ConvertVersion1ToVersion2(PvlObject &network);
      static void ConvertVersion2ToVersion3(PvlObject &network);
      static void ConvertVersion3ToVersion4(PvlObject &network);

      // We only need the latest Pvl version because it has our update cycle
      //! The latest version of the Pvl formatted control networks
      static const int LATEST_PVL_VERSION = 4;
      //! The latest version of the Binary formatted control networks
      static const int LATEST_BINARY_VERSION = 2;

    private:
      // helper methods for LatestPvlToBinary
      static void Copy(PvlContainer &container, QString keyName,
          ControlPointFileEntryV0002 &point,
          void (ControlPointFileEntryV0002::*setter)(bool));
      static void Copy(PvlContainer &container,
          QString keyName, ControlPointFileEntryV0002 &point,
          void (ControlPointFileEntryV0002::*setter)(double));
      static void Copy(PvlContainer &container,
          QString keyName, ControlPointFileEntryV0002 &point,
          void (ControlPointFileEntryV0002::*setter)(const std::string&));

      static void Copy(PvlContainer &container, QString keyName,
          ControlPointFileEntryV0002::Measure &measure,
          void (ControlPointFileEntryV0002::Measure::*setter)(bool));
      static void Copy(PvlContainer &container, QString keyName,
          ControlPointFileEntryV0002::Measure &measure,
          void (ControlPointFileEntryV0002::Measure::*setter)(double));
      static void Copy(PvlContainer &container, QString keyName,
          ControlPointFileEntryV0002::Measure &measure,
          void (ControlPointFileEntryV0002::Measure::*setter)
            (const std::string &));

      // This class is static, no instantiation allowed
      /**
       * The constructor is not implemented.
       */
      ControlNetVersioner();
      /**
       * The constructor is not implemented so the copy constructor is
       *   impossible.
       *
       * @param other The versioner to copy from
       */
      ControlNetVersioner(const ControlNetVersioner &other);
      /**
       * The constructor is not implemented so assignment is impossible.
       *
       * @param other The versioner to copy from
       */
      ControlNetVersioner &operator=(const ControlNetVersioner &other);
  };
}

#endif

