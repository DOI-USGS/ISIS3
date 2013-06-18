#ifndef ImageFileListWidget_H
#define ImageFileListWidget_H

#include <QWidget>

#include <QPointer>
#include <QScopedPointer>

#include "ImageList.h"
#include "ImageTreeWidget.h"
#include "PvlObject.h"

class QProgressBar;
class QSettings;
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
   *    @history 2011-07-29 Steven Lambright - Expansion state is now stored in
   *                            the project file. This change will cause older
   *                            versions of qmos to fail to read newer project
   *                            files. References #275.
   *    @history 2011-08-12 Steven Lambright - Added export options,
   *                            references #342
   *    @history 2011-08-29 Steven Lambright - Reworded save file list export
   *                            action, references #342
   *    @history 2011-09-27 Steven Lambright - Improved user documentation
   *    @history 2012-06-22 Tracie Sucharski - Adapted from MosaicFileListWidget.
   *    @history 2012-09-12 Steven Lambright - Added save/load from XML capabilities.
   *    @history 2012-10-02 Steven Lambright - Added context menu to show/hide the various columns
   *    @history 2013-03-19 Steven Lambright - Added setDefaultFileListCols().
   *    @history 2013-06-17 Jeannie Backer, Tracie Sucharski, and Stuart Sides - replaced
   *                            setDefaultFileListCols() with actions() to call
   *                            setDefaultFileListCols() from ImageTreeWidget.
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

      static QWidget * getLongHelp(QWidget * fileListContainer = NULL);

    public slots:
      void addImages(ImageList *images);

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

          ImageFileListWidget *m_fileList;
          ImageList *m_currentImageList;
          QTreeWidgetItem *m_currentImageListItem;
          QTreeWidgetItem *m_currentGroup;
      };

    private:
      QPointer<ProgressBar> m_progress;
      //! Serialized (file) version of this object
      QScopedPointer<PvlObject> m_serialized;
      ImageTreeWidget *m_tree; //!< Tree item associated with this mosaic item
      Directory *m_directory;
  };
}

#endif

