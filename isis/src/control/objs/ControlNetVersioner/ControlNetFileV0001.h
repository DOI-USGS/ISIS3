#ifndef ControlNetFileV0001_h
#define ControlNetFileV0001_h
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
  class ControlNetFileProtoV0001;
  class ControlNetLogDataProtoV0001;

  /**
   * @brief Handle Binary Control Network Files version 1
   *
   * This functionality and history was extracted from Isis::ControlNet
   *
   * @author 2011-04-08 Steven Lambright
   *
   * @internal
   *   @history 2010-01-12 Tracie Sucharski Added support for binary networks,
   *                added new parameters, renamed ComputeErrors to
   *                ComputeResiduals, renamed MaximumError to MaximumResidual,
   *                renamed AverageError to AverageResidual.
   *   @history 2010-08-05 Steven Lambright New label format much closer to a
   *                cube so that we can expand upon it easily later. Also added
   *                support for more than just the protocol buffer in the file,
   *                at the current cost of reading the protocol buffer's binary
   *                data into memory. This might need to be changed later.
   *   @history 2011-04-04 Steven Lambright - Reading is more likely to work...
   *                not sure why my changes fixed it for very large networks.
   *                Binary reads now do the same progress as Pvl for console
   *                output consistency (and because it can take time).
   *   @history 2011-04-08 Steven Lambright - Extracted functionality to
   *                ControlNetFileV0001 class
   */
  class ControlNetFileV0001 : public ControlNetFile {
    public:
      ControlNetFileV0001();
      virtual ~ControlNetFileV0001();

      virtual void Read(const Pvl &header, const Filename &file);

      // We don't need old Write methods... since this wasn't already
      //   implemented I'm not going to bother to implement it now.
      virtual void Write(const Filename &file) const {};
      virtual Pvl ToPvl() const;

      /**
       * Retrieve the protocol buffer that encapsulates the entire control
       *   network.
       */
      ControlNetFileProtoV0001 &GetNetwork() {
        return *p_network;
      }


      /**
       * Retrieve the protocol buffer that encapsulates all of the log data.
       */
      ControlNetLogDataProtoV0001 &GetLogData() {
        return *p_logData;
      }

    private:
      //! This contains the entire cnet
      ControlNetFileProtoV0001 *p_network;

      //! This contains all of the log data in the cnet
      ControlNetLogDataProtoV0001 *p_logData;
  };
}

#endif
