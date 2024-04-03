#ifndef ImageList_H
#define ImageList_H

#include <QDebug>
#include <QObject>
#include <QList>
#include <QMetaType>
#include <QSharedPointer>

#include "Image.h"
#include "ImageDisplayProperties.h"
#include "ImageListActionWorkOrder.h"
#include "SerialNumberList.h"
#include "WorkOrder.h"

class QStringList;
class QXmlStreamWriter;

namespace Isis {
  class FileName;

  /**
   * @brief Internalizes a list of images and allows for operations on the entire list
   *
   * This class reads a list of images from an images.xml file and internalizes them
   * as aQList of images.  It also allows for modifications to the entire list of
   * images and storing the image list as an images.xml file.
   *
   * @author 2012-??-?? ???
   *
   * @internal
   * @history 2014-01-08 Tracie Sucharski - Added layer re-ordering connections to all images
   *                         in list instead of just the first image.  Fixes #1755.
   * @history 2014-06-13 Tracie Sucharski - Added serialNumberList method.
   * @history 2016-06-08 Jesse Mapel - Updated documentation and merged from IPCE to ISIS branch.
   *                         Fixes #3961.
   * @history 2016-09-19 Tracie Sucharski - Changed serialNumberList method to return a shared
   *                         pointer. TODO:  Currently, serialNumberList created the list on the
   *                         fly.  For speed, this needs to change so that when the ImageList
   *                         changes, update the serial number list.
   * @history 2017-06-08 Makayla Shepherd - Modified ImageList(QStringList &) to close the image
   *                         cubes after adding them to the list. Fixes #4908.
   * @history 2017-11-01 Ian Humphrey, Tracie Sucharski - Add dataRoot parameter to the 
   *                         XmlHandler, during serialization check if images are in results/bundle
   *                         area and set the dataRoot appropriately. Changed project name to new
   *                         project root in serialization method because the project name is not
   *                         changed to new project until it is re-opened. Fixes #4849.
   * @history 2017-12-08 Tracie Sucharski - Changed save to only copy images if project is saved 
   *                         to a new location.
   * @history 2018-01-04 Tracie Sucharski - Changed all serialization to save relative paths so that 
   *                         projects can be moved to new locations.  Fixes #5276. 
   */
  class ImageList : public QObject, public QList<Image *> {
    Q_OBJECT

    public:
      friend class ImageListActionWorkOrder;

      ImageList(QString name, QString path, QObject *parent = NULL);
      explicit ImageList(QObject *parent = NULL);
      explicit ImageList(QList<Image *>, QObject *parent = NULL);
      explicit ImageList(QStringList &);
      ImageList(const ImageList &);
      ~ImageList();

//    QSharedPointer<SerialNumberList> serialNumberList();
      SerialNumberList *serialNumberList();

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
       * This functor is used for copying the images between two projects quickly. This is designed
       *   to work with QtConcurrentMap, though the results are all NULL (QtConcurrentMap is much
       *   faster than many QtConcurrentRun calls).
       *
       * @author 2012-09-?? Steven Lambright
       *
       * @internal
       */
      class CopyImageDataFunctor : public std::function<void *(Image * const &)> {
        public:
          CopyImageDataFunctor(const Project *project, FileName newProjectRoot);
          CopyImageDataFunctor(const CopyImageDataFunctor &other);
          ~CopyImageDataFunctor();

          void *operator()(Image * const &imageToCopy);

          CopyImageDataFunctor &operator=(const CopyImageDataFunctor &rhs);

        private:
          /**
           * This stores the name of the project that is going to be copied to.
           */
          const Project *m_project;
          /**
           * This stores the path to the root of the project that is going to be copied to.
           */
          FileName m_newProjectRoot;
      };

    private:
      /**
       * Creates an ImageListActionWorkOrder and sets the image list as the data for the work order.
       *
       * @param project The project the work order is for
       * @param action The action the work order performs
       *
       * @return @b QAction * The created work order
       */
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
      /**
       * This stores the image list's name
       */
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
