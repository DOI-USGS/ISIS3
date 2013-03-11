#ifndef ToolList_h
#define ToolList_h

/**
 * @file
 * $Date: 2010/07/01 22:52:23 $
 * $Revision: 1.21 $
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

#include <QList>

#include <QPointer>

class QStackedWidget;
class QToolBar;

namespace Isis {
  class RubberBandTool;
  class Tool;

  /**
  * @brief Allows tools to share data between each other
  *
  * @ingroup Visualization Tools
  *
  * @author 2012-09-18 Steven Lambright
  *
  * @internal
  */
  class ToolList {
    public:
      ToolList();
      ~ToolList();

      void append(Tool *tool);
      int count() const;
      RubberBandTool *rubberBandTool();
      QStackedWidget *toolBarStack();
      QStackedWidget *toolBarStack(QToolBar *toolBar);

      Tool *operator[](int index);

    private:
      Q_DISABLE_COPY(ToolList);

      QList<Tool *> m_tools;

      QPointer<QStackedWidget> m_activeToolBarStack;
      QPointer<RubberBandTool> m_rubberBandTool;
  };
}

#endif
