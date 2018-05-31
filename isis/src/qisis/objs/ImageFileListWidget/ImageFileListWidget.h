#ifndef ImageFileListWidget_H
#define ImageFileListWidget_H

#include <QWidget>

#include <QPointer>
#include <QScopedPointer>

#include "ImageList.h"
#include "ImageTreeWidget.h"
#include "PvlObject.h"

class QLabel;
class QLineEdit;
class QProgressBar;
class QSettings;
class QToolBar;
class QTreeWidgetItem;

namespace Isis {
  class Directory;
  class ImageTreeWidgetItem;
  class ProgressBar;

  /**
   * @brief A colored, grouped cube list
   *
   * @author 2011-07-29 Steven Lambright
   *
   * @internal
   *   @history 2011-07-29 Steven Lambright - Expansion state is now stored in
   *                          the project file. This change will cause older
   *                          versions of qmos to fail to read newer project
   *                          files. References #275.
   *   @history 2011-08-12 Steven Lambright - Added export options,
   *                          references #342
   *   @history 2011-08-29 Steven Lambright - Reworded save file list export
   *                          action, references #342
   *   @history 2011-09-27 Steven Lambright - Improved user documentation
   *   @history 2012-06-22 Tracie Sucharski - Adapted from MosaicFileListWidget.
   *   @history 2012-09-12 Steven Lambright - Added save/load from XML capabilities.
   *   @history 2012-10-02 Steven Lambright - Added context menu to show/hide the various columns
   *   @history 2013-03-19 Steven Lambright - Added setDefaultFileListCols().
   *   @history 2013-06-17 Jeannie Backer, Tracie Sucharski, and Stuart Sides - replaced
   *                          setDefaultFileListCols() with actions() to call
   *                          setDefaultFileListCols() from ImageTreeWidget.
   *   @history 2014-09-05 Kim Oyama - Modified XmlHandler::startElement and save().
   *   @history 2016-04-28 Tracie Sucharski - Modified to use Qt5 and v006 libraries.
   *   @history 2016-06-07 Makayla Shepherd - Updated documentation. Fixes #3962.
   *   @history 2016-09-14 Ian Humphrey - Modified getLongHelp() - replaced deprecated static
   *                           QPixmap::grabWidget with QWidget::grab (Qt5). Fixes #4304.
   *   @history 2017-07-18 Cole Neubauer - Added removeImages slot to be able to remove from the
   *                           ImageFileList in IPCE Fixes #4996
   *   @history 2017-08-22 Cole Neuabuer - Added ability to search ImageFileListWidget. Fixes #1556
   *   @history 2018-05-15 Tracie Sucharski - Fixed xml serialization for Ipce project saves.  Fixes
   *                            #5422.
   */
  class ImageFileListWidget : public QWidget {
      Q_OBJECT
    public:
      ImageFileListWidget(Directory *directory = 0, QWidget *parent = 0);
      virtual ~ImageFileListWidget();

      QProgressBar *getProgress();
      void fromPvl(PvlObject &pvl);
      PvlObject toPvl() const;
      void load(XmlStackedHandlerReader *xmlReader);
      void save(QXmlStreamWriter &stream, Project *project, FileName newProjectRoot) const;

      QList<QAction *> actions();
      QList<QAction *> getViewActions();
      QList<QAction *> getExportActions();

      static QWidget *getLongHelp(QWidget *fileListContainer = NULL);

    public slots:
      void addImages(ImageList *images);
      void removeImages(ImageList *images);
      void clear();
      void filterFileList();

    protected:
      void contextMenuEvent(QContextMenuEvent *event);

    private slots:
      void saveList();

    private:
      void save(QXmlStreamWriter &stream, QTreeWidgetItem *itemToWrite) const;

      ImageTreeWidget::ImagePosition find(const Image *image) const;
      void restoreExpandedStates(QVariant expandedStates, QTreeWidgetItem *item);
      QVariant saveExpandedStates(QTreeWidgetItem *item);

    private:
      /**
       * @author 2012-09-?? Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(ImageFileListWidget *fileList);
          ~XmlHandler();

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          ImageFileListWidget *m_fileList; //!< The widget we are working with
          ImageList *m_currentImageList; //!< The list of images being worked on
          QTreeWidgetItem *m_currentImageListItem; //!< The image being worked on
          QTreeWidgetItem *m_currentGroup; //!< The group of cubes being worked on
      };

    private:
      QPointer<ProgressBar> m_progress; //!< The ProgressBar of the ImageFileListWidget
      //! Serialized (file) version of this object
      QScopedPointer<PvlObject> m_serialized;

      QToolBar *m_searchToolbar; //!< Tool bar for the FileList widget to search
      QLineEdit *m_searchLineEdit;
      QLabel *m_fileCount;


      ImageTreeWidget *m_tree; //!< Tree item associated with this mosaic item
      Directory *m_directory; //!< The directory of the project
  };
}

#endif
