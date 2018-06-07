#ifndef AbstractStringFilter_H
#define AbstractStringFilter_H

#include "AbstractFilter.h"

class QLineEdit;
class QString;


namespace Isis {
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  /**
   * @brief Base class for filters that are string-based
   *
   * This class is the base class that all filters that are string-based.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   *   @history 2018-06-01 Jesse Mapel - Changed ControlCubeGraphNode to image serial number.
   *                           References #5434.
   */
  class AbstractStringFilter : public AbstractFilter {
      Q_OBJECT

    public:
      AbstractStringFilter(AbstractFilter::FilterEffectivenessFlag,
                           int minimumForSuccess = -1);
      AbstractStringFilter(const AbstractStringFilter &other);
      virtual ~AbstractStringFilter();


    protected:
      using Isis::AbstractFilter::evaluate;
      bool evaluate(QString) const;
      QString descriptionSuffix() const;


    private slots:
      void updateLineEditText(QString);


    private:
      void createWidget();
      void nullify();


    private:
      QLineEdit *m_lineEdit;
      QString *m_lineEditText;
  };
}

#endif
