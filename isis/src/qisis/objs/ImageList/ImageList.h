#ifndef ImageList_H
#define ImageList_H

#include <QDebug>
#include <QObject>
#include <QList>
#include <QMetaType>

#include "Image.h"
#include "ImageDisplayProperties.h"
#include "ImageListActionWorkOrder.h"
#include "WorkOrder.h"
#include "XmlStackedHandler.h"

class QStringList;
class QXmlStreamWriter;

namespace Isis {
  class FileName;
  class XmlStackedHandlerReader;

  /**
   * @author 2012-??-?? ???
   *
   * @internal 
   * @history 2014-01-08 Tracie Sucharski - Added layer re-ordering connections to all images 
   *                         in list instead of just the first image.  Fixes #1755. 
   */
  class ImageList : public QObject, public QList<Image *> {
    Q_OBJECT

    public:
      friend class ImageListActionWorkOrder;

      ImageList(QString name, QString path, QObject *parent = NULL);
      explicit ImageList(QObject *parent = NULL);
      explicit ImageList(QList<Image *>, QObject *parent = NULL);
      explicit ImageList(Project *project,
                         XmlStackedHandlerReader *xmlReader, QObject *parent = NULL);
      explicit ImageList(QStringList &);
      ImageList(const ImageList &);
      ~ImageList();

      // These are overridden (-ish) in order to add notifications to the list changing
      void append(Image * const & value);
      void append(const QList<Image *> &value);

      void clear();

      iterator erase(iterator pos);
      iterator erase(iterator begin, iterator end);

      void insert(int i, Image * const & value);
      iterator insert(iterator before, Image * const & value);

      void prepend(Image * const & value);
      void push_back(Image * const & value);
      void push_front(Image * const & value);
      int removeAll(Image * const & value);
      void removeAt(int i);
      void removeFirst();
      void removeLast();
      bool removeOne(Image * const & value);
      void swap(QList<Image *> &other);
      Image *takeAt(int i);
      Image *takeFirst();
      Image *takeLast();

      ImageList &operator+=(const QList<Image *> &other);
      ImageList &operator+=(Image * const &other);
      ImageList &operator<<(const QList<Image *> &other);
      ImageList &operator<<(Image * const &other);
      ImageList &operator=(const QList<Image *> &rhs);

      // This is our own assignment, but it needs to notify just like the operator=(QList)
      ImageList &operator=(const ImageList &rhs);

      // Done overriding (-ish)


      QList<QAction *> supportedActions(Project *project = NULL);
      bool allSupport(ImageDisplayProperties::Property prop);

      void setName(QString newName);
      void setPath(QString newPath);

      QString name() const;
      QString path() const;

      void deleteFromDisk(Project *project);
      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;


    signals:
      void countChanged(int newCount);

    private:
      /**
       * @author 2012-07-01 Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(ImageList *imageList, Project *project);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          ImageList *m_imageList;
          Project *m_project;
      };


      /**
       * This functor is used for copying the images between two projects quickly. This is designed
       *   to work with QtConcurrentMap, though the results are all NULL (QtConcurrentMap is much
       *   faster than many QtConcurrentRun calls).
       *
       * @author 2012-09-?? Steven Lambright
       *
       * @internal
       */
      class CopyImageDataFunctor : public std::unary_function<Image * const &, void *> {
        public:
          CopyImageDataFunctor(const Project *project, FileName newProjectRoot);
          CopyImageDataFunctor(const CopyImageDataFunctor &other);
          ~CopyImageDataFunctor();

          void *operator()(Image * const &imageToCopy);

          CopyImageDataFunctor &operator=(const CopyImageDataFunctor &rhs);

        private:
          const Project *m_project;
          FileName m_newProjectRoot;
      };

    private:
      QAction *createWorkOrder(Project *project, ImageListActionWorkOrder::Action action) {
        QAction *result = NULL;

        if (project) {
          result = new ImageListActionWorkOrder(action, project);
          ((ImageListActionWorkOrder *)result)->setData(this);
        }
        else {
          result = new QAction(
              ImageListActionWorkOrder::qualifyString(ImageListActionWorkOrder::toString(action),
                                                      this),
              this);
        }

        return result;
      }

      void applyAlphas(QStringList alphaValues);
      void applyColors(QStringList colorValues, int column = 0);
      void applyShowLabel(QStringList showLabelValues);
      void applyShowFill(QStringList showFillValues);
      void applyShowDNs(QStringList showDNsValues);
      void applyShowOutline(QStringList showOutlineValues);
      bool askAlpha(int *alphaResult) const;
      bool askNewColor(QColor *colorResult) const;
      QStringList saveAndApplyAlpha(int newAlpha);
      QStringList saveAndApplyColor(QColor newColor);
      QStringList saveAndApplyRandomColor();

    private slots:
      void askAndUpdateAlpha();
      void askAndUpdateColor();
      void showRandomColor();
      QStringList saveAndToggleShowDNs();
      QStringList saveAndToggleShowFill();
      QStringList saveAndToggleShowLabel();
      QStringList saveAndToggleShowOutline();

    private:
      QString m_name;

      /**
       * This stores the directory name that contains the images in this image list.
       *
       * For example:
       *   import1
       * or
       *   import2
       *
       * This path is relative to Project::imageDataRoot()
       */
      QString m_path;
  };
}

Q_DECLARE_METATYPE(Isis::ImageList *);

#endif
