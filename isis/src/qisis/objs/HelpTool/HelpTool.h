#ifndef HelpTool_h
#define HelpTool_h

/**
 * @file
 * $Date: 2007/06/14 23:57:15 $ $Revision: 1.2 $
 *
 *  Unless noted otherwise, the portions of Isis written by the USGS are public domain. See
 *  individual third-party library and package descriptions for intellectual property information,
 *  user agreements, and related information.
 *
 *  Although Isis has been used by the USGS, no warranty, expressed or implied, is made by the
 *  USGS as to the accuracy and functioning of such software and related material nor shall the
 *  fact of distribution constitute any such warranty, and no responsibility is assumed by the
 *  USGS in connection therewith.
 *
 *  For additional information, launch $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *  in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *  http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *  http://www.usgs.gov/privacy.html.
 */

#include <QAction>
#include "Tool.h"

namespace Qisis {
   /**
   * @brief Qisis Help Tool
   *
   * @ingroup Visualization Tools
   *
   * @author Jeff Anderson
   *
   * @internal @history 2007-06-12 Tracie Sucharski - Added aboutProgram method
   * 
   */
 class HelpTool : public Tool {
    Q_OBJECT

    public:
      HelpTool (QWidget *parent);
      void addTo (QMenu *menu);
      void addToPermanent (QToolBar *perm);

    public slots:
      void whatsThis();

    protected:
      QString menuName() const { return "&Help"; };

    private:
      QAction *p_whatsThis;
      QAction *p_aboutProgram;

    private slots:
      void aboutProgram();
  };
};

#endif
