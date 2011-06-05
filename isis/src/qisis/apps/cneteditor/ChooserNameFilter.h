#ifndef ChooserNameFilter_H
#define ChooserNameFilter_H

#include "AbstractFilter.h"


class QLineEdit;
class QString;


namespace Isis
{
  class AbstractFilterSelector;
  class ControlPoint;
  class ControlMeasure;

  class ChooserNameFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      ChooserNameFilter(AbstractFilter::FilterEffectivenessFlag,
          AbstractFilterSelector *, int minimumForImageSuccess = -1);
      virtual ~ChooserNameFilter();
      
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
