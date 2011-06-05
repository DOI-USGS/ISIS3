#ifndef PointIdFilter_H
#define PointIdFilter_H

#include "AbstractFilter.h"


class QLineEdit;
class QString;


namespace Isis
{
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  class PointIdFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      PointIdFilter(AbstractFilter::FilterEffectivenessFlag,
          AbstractFilterSelector *, int minimumForSuccess = -1);
      virtual ~PointIdFilter();
      
      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const { return true; }

      QString getImageDescription() const;
      QString getPointDescription() const;


    protected:
      void nullify();
      void createWidget();
      
      
    private slots:
      void updateLineEditText(QString);


    private:
      QLineEdit * lineEdit;
      QString * lineEditText;
  };
}

#endif
