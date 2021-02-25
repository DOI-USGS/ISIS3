#ifndef AbstractFilter_H
#define AbstractFilter_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QWidget>

class QBoxLayout;
class QButtonGroup;
class QCheckBox;
template< typename U, typename V > struct QPair;
class QString;
template< typename T > class QFlags;


namespace Isis {
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;
  class ControlNet;

  /**
   * @brief Base class for control net filters
   *
   * This class is the base class that all other filters derive from. It
   * encompasses both the widget and the filter functionality itself. See the
   * cneteditor architecture document for further information about the
   * filtering system.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   *   @history 2018-09-28 Kaitlyn Lee - Changed the declaration of QPair from class to struct.
   *                           Fixes build warning on MacOS 10.13. References #5520.
   */
  class AbstractFilter : public QWidget {
      Q_OBJECT

    public:
      enum FilterEffectiveness {
        Images = 1,
        Points = 2,
        Measures = 4
      };
      Q_DECLARE_FLAGS(FilterEffectivenessFlag, FilterEffectiveness)


    public:
      AbstractFilter(FilterEffectivenessFlag, int minimumForSuccess = -1);
      AbstractFilter(const AbstractFilter &other);
      virtual ~AbstractFilter();

      virtual bool canFilterImages() const;
      virtual bool canFilterPoints() const;
      virtual bool canFilterMeasures() const;

      virtual bool evaluate(const QPair<QString, ControlNet *> *) const = 0;
      virtual bool evaluate(const ControlPoint *) const = 0;
      virtual bool evaluate(const ControlMeasure *) const = 0;

      virtual AbstractFilter *clone() const = 0;

      virtual QString getImageDescription() const;
      virtual QString getPointDescription() const;
      virtual QString getMeasureDescription() const;


    signals:
      void filterChanged();


    protected:
      bool inclusive() const;
      int getMinForSuccess() const {
        return m_minForSuccess;
      }
      AbstractFilter::FilterEffectivenessFlag *getEffectivenessFlags() const;
      QBoxLayout *getMainLayout() const;
      QBoxLayout *getInclusiveExclusiveLayout() const;

      bool evaluateImageFromPointFilter(const QPair<QString, ControlNet *> *) const;
      bool evaluateImageFromMeasureFilter(const QPair<QString, ControlNet *> *) const;
      bool evaluatePointFromMeasureFilter(const ControlPoint *) const;

      virtual bool evaluate(const ControlPoint *,
          bool (ControlPoint:: *)() const) const;
      virtual bool evaluate(const ControlMeasure *,
          bool (ControlMeasure:: *)() const) const;


    private:
      void createWidget();
      bool evaluateFromCount(QList< ControlMeasure * >, bool) const;
      void nullify();


    private slots:
      void updateEffectiveness();
      void updateMinForSuccess(int);


    private:
      QCheckBox *createEffectivenessCheckBox(QString);


    private:
      QBoxLayout *m_mainLayout;
      QBoxLayout *m_inclusiveExclusiveLayout;
      QButtonGroup *m_inclusiveExclusiveGroup;
      QButtonGroup *m_effectivenessGroup;
      QWidget *m_minWidget;


    private:
      int m_minForSuccess;
      FilterEffectivenessFlag *m_effectivenessFlags;
      QFont *m_smallFont;
  };

  Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractFilter::FilterEffectivenessFlag)
}

#endif
