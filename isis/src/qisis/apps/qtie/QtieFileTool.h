#ifndef QtieFileTool_h
#define QtieFileTool_h

/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2010/05/12 17:22:27 $
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
#include "FileTool.h"


namespace Isis {
  class ControlNet;
  class Cube;
  class UniversalGroundMap;

  /**
   * @brief Qtie File operations
   *
   * @ingroup Visualization Tools
   *
   * @author 2007-08-09 Tracie Sucharski, adapted from QnetFileTool
   *
   * @internal
   *   @history 2009-06-10 Tracie Sucharski - Added new signal for opening new
   *                          files.
   *   @history 2012-05-10  Tracie Sucharski - The FileTool::closeAll method no longer 
   *                          closes viewports, so re-implemented closing of old
   *                          cube viewports before opening new.
   *   @history 2012-05-15 Tracie Sucharski - Moved much of the error checking out of QtieTool into 
   *                          this class.  Added new method, checkNet to perform error checking
   *                          of input control network.
   *   @history 2012-06-20 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972.
   *   @history 2012-10-11 Debbie A. Cook, Updated to use new Target class.  References Mantis tickets 
   *                          #775 and #1114.
   *   @history 2016-04-22 Jeannie Backer - Modified to use cube labels to set
   *                           ControlNet's target instead of the TargetName.
   *                           References #3892
   *   @history 2016-08-15 Jesse Mapel - Added an output control network to account for
   *                           BundleAdjust creating a new control network.  References #4159.
   *
   */

  class QtieFileTool : public FileTool {
      Q_OBJECT

    signals:
      void cubesOpened(Cube *baseCube, Cube *matchCube, ControlNet *cnet);
      void newFiles();

    public:
      QtieFileTool(QWidget *parent);

    public slots:
      virtual void open();
      //virtual void exit();

    private:
      bool checkNet(Cube *baseCube, UniversalGroundMap *baseGM, 
                    Cube *matchCube, UniversalGroundMap *matchGM, ControlNet *cnet);

  };
};

#endif
