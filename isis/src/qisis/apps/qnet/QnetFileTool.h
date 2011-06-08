#ifndef QnetFileTool_h
#define QnetFileTool_h

/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2010/07/01 19:04:52 $
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

#include "FileTool.h" // parent

// forward declarations
class QString;
class QWidget;

namespace Isis {
  class ControlNet;
  class ControlPoint;
}

namespace Qisis {

  /**
   * @brief Qnet File operations
   *
   * @ingroup Visualization Tools
   *
   * @author 2006-02-01 Jeff Anderson
   *
   * @internal
   *  @history 2006-08-02 Tracie Sucharski - Initialize cameras for every image
   *                         in cube list.
   *  @history 2008-11-24 Jeannie Walldren - Replace references to PointEdit class
   *                         with ControlPointEdit
   *  @history 2008-11-26 Jeannie Walldren - Added cNetName parameter to
   *                         controlNetworkUpdated() so that QnetTool can read the
   *                         name of the control net file.
   * @history  2008-11-26 Tracie Sucharski - Remove all polygon/overlap
   *                         polygon/overlap references, this functionality will
   *                         be in qmos.
   *  @history 2008-12-10 Jeannie Walldren - Reworded "What's this?" description
   *                         for saveAs action. Changed "Save As" action text to
   *                         match QnetTool's "Save As" action 2010-06-03 Jeannie
   *                         Walldren - Removed "std::" in .cpp file since "using
   *                         namespace std"
   *  @history 2010-07-01 Jeannie Walldren - Added file extension filters for
   *                         input control network. Replaced #includes with
   *                            forward class declarations and moved #include to
   *                            .cpp file.
   *  @history 2010-10-28 Tracie Sucharski - Fixed some include problems caused
   *                          by changes made to the ControlNet,ControlPoint,
   *                          ControlMeasure header files.
   *  @history 2010-11-17 Eric Hyer - Added newControlNetwork SIGNAL
   *  @history 2010-12-10 Tracie Sucharski - Renamed slot loadPoint to
   *                          loadPointImages.
   *   @history 2011-06-03 Tracie Sucharski - Add Open Ground & Open Dem
   *                          signals.
   *
   */

  class QnetFileTool : public FileTool {
      Q_OBJECT

    signals:
      void serialNumberListUpdated();
      void controlNetworkUpdated(QString cNetName);
      void newControlNetwork(Isis::ControlNet *);
      void newGroundFile();
      void newDemFile();

    public:
      QnetFileTool(QWidget *parent);
      QString cNetFilename;

    public slots:
      virtual void open();
      virtual void exit();
      virtual void saveAs();
      void loadPointImages(Isis::ControlPoint *point);
      void loadImage(const QString &serialNumber);
      void setSaveNet();

    private:
      bool p_saveNet;
  };
};

#endif
