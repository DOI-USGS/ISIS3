#ifndef CnetEditorView_h
#define CnetEditorView_h
/**
 * @file
 * $Date$
 * $Revision$
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
#include <QList>
#include <QMap>
#include <QPointer>
#include <QSize>

#include "AbstractProjectItemView.h"
#include "FileName.h"
#include "XmlStackedHandler.h"

class QAction;
class QToolBar;
class QWidgetAction;
class QXmlStreamWriter;

namespace Isis {
  class Control;
  class CnetEditorWidget;
  class Directory;
  class FileName;
  class Project;
  class ToolPad;
  class XmlStackedHandlerReader;

  /**
   * Ipce view containing the CnetEditorWidget
   *
   * @author 2018-04-04 Tracie Sucharski
   *    
   * @internal 
   */

class CnetEditorView : public AbstractProjectItemView {

  Q_OBJECT

  public:
    CnetEditorView(Directory *directory, Control *control, FileName configFile, 
                   QWidget *parent = 0);
    ~CnetEditorView();

    virtual QList<QAction *> permToolBarActions();
    virtual QList<QAction *> activeToolBarActions();
    virtual QList<QAction *> toolPadActions();

    CnetEditorWidget *cnetEditorWidget();
    Control *control();

    QSize sizeHint() const;

    void load(XmlStackedHandlerReader *xmlReader);
    void save(QXmlStreamWriter &stream, Project *project, FileName newProjectRoot) const;


    private:
      /**
       * @author 2012-09-?? Steven Lambright
       *
       * @internal
       *   @history 2018-04-04 Tracie Sucharski - Implemented for CnetEditorView
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(CnetEditorView *cnetEditorView);
          ~XmlHandler();

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          CnetEditorView *m_cnetEditorView; //!< The view we are working with
      };

  private:
    QPointer<CnetEditorWidget> m_cnetEditorWidget;
    QPointer<Control> m_control;

    QToolBar *m_permToolBar; //!< The permanent tool bar
    QToolBar *m_activeToolBar; //!< The active tool bar
    ToolPad *m_toolPad; //!< The tool pad

    QWidgetAction *m_activeToolBarAction; //!< Stores the active tool bar
  };
}

#endif // CNETEDITORVIEW_H
