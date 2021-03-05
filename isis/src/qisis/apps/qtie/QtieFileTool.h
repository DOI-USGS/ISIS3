#ifndef QtieFileTool_h
#define QtieFileTool_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
