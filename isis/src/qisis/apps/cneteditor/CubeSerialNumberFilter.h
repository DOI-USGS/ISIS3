#ifndef CubeSerialNumberFilter_H
#define CubeSerialNumberFilter_H

#include "AbstractFilter.h"


class QLineEdit;
class QString;


namespace Isis
{
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  class CubeSerialNumberFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      CubeSerialNumberFilter(AbstractFilter::FilterEffectivenessFlag,
          AbstractFilterSelector *, int minimumForSuccess = -1);
      virtual ~CubeSerialNumberFilter();
      
      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;


      QString getImageDescription() const;
      QString getPointDescription() const;
      QString getMeasureDescription() const;


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
