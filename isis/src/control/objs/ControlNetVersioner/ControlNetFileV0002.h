#ifndef ControlNetFileV0002_h
#define ControlNetFileV0002_h
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

#include "ControlNetFile.h"

template <typename A> class QList;

namespace Isis {
  class ControlNetFileHeaderV0002;
  class ControlPointFileEntryV0002;
  class FileName;

  /**
   * @brief Handle Binary Control Network Files version 2
   *
   * We went to binary v2 in order to split up the protocol buffer messages
   *   by ControlPoint in order to completely avoid the maximum file size
   *   limitation (512MB before protocol buffers might fail).
   *
   * This version takes the separate 'log' and 'network' sections and combines
   *   them while simuntaneously splitting up the control points and network
   *   header. Please keep in mind you can play with optional keywords all
   *   day long without requiring a new binary control network version - this
   *   should be done only when necessary. Upgrading the Pvl version does NOT
   *   require having a new "ControlNetFile" child - simply handle that directly
   *   in the ControlNetVersioner.
   *
   * @author 2011-04-07 Steven Lambright
   *
   * @internal
   *   @history 2011-06-21 Steven Lambright - Files can have a larger size now
   *   @history 2013-05-22 Kimberly Oyama and Tracie Sucharski - Added the JIGSAWREJECTED
   *                           keyword to the toPvl() method. Fixes #661.
   */
  class ControlNetFileV0002 : public ControlNetFile {
    public:
      ControlNetFileV0002();
      virtual ~ControlNetFileV0002();

      virtual void Read(const Pvl &header, const FileName &file);
      virtual void Write(const FileName &file) const;
      virtual Pvl toPvl() const;

      /**
       * Get the control network level information - things like NetworkID,
       *   TargetName, etc...
       *
       * "ControlNetFileHeaderV0002::pointmessagesizes" is only used for IO
       *   and you cannot assume it is populated.
       */
      ControlNetFileHeaderV0002 &GetNetworkHeader() {
        return *p_networkHeader;
      }

      /**
       * Get the control point data along with the log data.
       */
      QList<ControlPointFileEntryV0002> &GetNetworkPoints() {
        return *p_controlPoints;
      }

    private:
      //! This contains global cnet information...
      ControlNetFileHeaderV0002 *p_networkHeader;

      //! All of the control points
      QList<ControlPointFileEntryV0002> *p_controlPoints;
  };
}

#endif
