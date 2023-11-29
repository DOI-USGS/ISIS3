#ifndef GuiCameraList_H
#define GuiCameraList_H

#include <QDebug>
#include <QList>
#include <QMetaType>
#include <QObject>

#include "GuiCamera.h"
#include "GuiCameraDisplayProperties.h"
//#include "GuiCameraListActionWorkOrder.h"  TODO - will we need this?
#include "WorkOrder.h"

class QStringList;
class QXmlStreamWriter;

namespace Isis {
  class FileName;

  /**
   * List of GuiCameras saved as QSharedPointers.  Overrides many QList methods in order to
   * emit signals about that status of the list.
   * 
   * @brief List of GuiCameras.
   * 
   * @author 2015-06-11 Ken Edmundson
   *
   * @internal 
   *   @history 2015-06-11 Ken Edmundson - original version
   *   @history 2016-06-15 Jesse Mapel - Added documentation in preparation for merge from IPCE
   *                           to ISIS.  Fixes #4005.
   */
  class GuiCameraList : public QObject, public QList<GuiCameraQsp> {
    Q_OBJECT

    public:
//      friend class GuiCameraListActionWorkOrder;

      GuiCameraList(QString name, QString path, QObject *parent = NULL);
      explicit GuiCameraList(QObject *parent = NULL);
      explicit GuiCameraList(QList<GuiCameraQsp>, QObject *parent = NULL);
//    explicit GuiCameraList(QStringList &);
      GuiCameraList(const GuiCameraList &);
      ~GuiCameraList();

      // These are overridden (-ish) in order to add notifications to the list changing
      void append(GuiCameraQsp const & value);
      void append(const QList<GuiCameraQsp> &value);

      void clear();

      iterator erase(iterator pos);
      iterator erase(iterator begin, iterator end);

      void insert(int i, GuiCameraQsp const & value);
      iterator insert(iterator before, GuiCameraQsp const & value);

      void prepend(GuiCameraQsp const & value);
      void push_back(GuiCameraQsp const & value);
      void push_front(GuiCameraQsp const & value);
      int removeAll(GuiCameraQsp const & value);
      void removeAt(int i);
      void removeFirst();
      void removeLast();
      bool removeOne(GuiCameraQsp const & value);
      void swap(QList<GuiCameraQsp> &other);
      GuiCameraQsp takeAt(int i);
      GuiCameraQsp takeFirst();
      GuiCameraQsp takeLast();

      GuiCameraList &operator+=(const QList<GuiCameraQsp> &other);
      GuiCameraList &operator+=(GuiCameraQsp const &other);
      GuiCameraList &operator<<(const QList<GuiCameraQsp> &other);
      GuiCameraList &operator<<(GuiCameraQsp const &other);
      GuiCameraList &operator=(const QList<GuiCameraQsp> &rhs);

      // This is our own assignment, but it needs to notify just like the operator=(QList)
      GuiCameraList &operator=(const GuiCameraList &rhs);

      // Done overriding (-ish)


      QList<QAction *> supportedActions(Project *project = NULL);
      bool allSupport(GuiCameraDisplayProperties::Property prop);

      void setName(QString newName);
      void setPath(QString newPath);

      QString name() const;
      QString path() const;

//      void deleteFromDisk(Project *project);
      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;


    signals:
      /**
       * Emitted when the number of GuiCameras in the list changes.
       * Not currently connected to anything.
       */
      void countChanged(int newCount);


      /**
       * This functor is used for copying the GuiCamera objects between two projects quickly. This is designed
       *   to work with QtConcurrentMap, though the results are all NULL (QtConcurrentMap is much
       *   faster than many QtConcurrentRun calls).
       * 
       * JAM - This appears to have been copied from TargetBodyList.  Is this functionality needed?
       *           Is this going to be implemented at a later date?
       *
       * @author 2015-06-11 Ken Edmundson (after Steven Lambright)
       *
       * @internal
       */
//      class CopyTargetBodyDataFunctor : public std::unary_function<TargetBodyQsp const &, void *> {
//        public:
//          CopyTargetBodyDataFunctor(const Project *project, FileName newProjectRoot);
//          CopyTargetBodyDataFunctor(const CopyTargetBodyDataFunctor &other);
//          ~CopyTargetBodyDataFunctor();

//          void *operator()(TargetBodyQsp const &imageToCopy);

//          CopyTargetBodyDataFunctor &operator=(const CopyTargetBodyDataFunctor &rhs);

//        private:
//          const Project *m_project;
//          FileName m_newProjectRoot;
//      };

    private:
//      QAction *createWorkOrder(Project *project, GuiCameraListActionWorkOrder::Action action) {
//        QAction *result = NULL;

//        if (project) {
//          result = new GuiCameraListActionWorkOrder(action, project);
//          ((GuiCameraListActionWorkOrder *)result)->setData(this);
//        }
//        else {
//          result = new QAction(
//              GuiCameraListActionWorkOrder::qualifyString(GuiCameraListActionWorkOrder::toString(action),
//                                                      this),
//              this);
//        }

//        return result;
//      }

//      void applyAlphas(QStringList alphaValues);
//      void applyColors(QStringList colorValues, int column = 0);
//      void applyShowLabel(QStringList showLabelValues);
//      void applyShowFill(QStringList showFillValues);
//      void applyShowDNs(QStringList showDNsValues);
//      void applyShowOutline(QStringList showOutlineValues);
//      bool askAlpha(int *alphaResult) const;
//      bool askNewColor(QColor *colorResult) const;
//      QStringList saveAndApplyAlpha(int newAlpha);
//      QStringList saveAndApplyColor(QColor newColor);
//      QStringList saveAndApplyRandomColor();

    private slots:
//      void askAndUpdateAlpha();
//      void askAndUpdateColor();
//      void showRandomColor();
//      QStringList saveAndToggleShowDNs();
//      QStringList saveAndToggleShowFill();
//      QStringList saveAndToggleShowLabel();
//      QStringList saveAndToggleShowOutline();

    private:
      QString m_name; //!< The disply name of the GuiCameraList.  Not used by anonymous lists.

      /**
       * This stores the directory name that contains the GuiCamera objects in this list.
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
  // TODO: add QDataStream >> and << ???
}

Q_DECLARE_METATYPE(Isis::GuiCameraList *);

#endif
