#ifndef AbstractMultipleChoiceFilter_H
#define AbstractMultipleChoiceFilter_H


// parent
#include "AbstractFilter.h"


class QComboBox;
class QString;


namespace Isis {
  class AbstractFilterSelector;

  /**
   * @brief Base class for combo box type filters
   *
   * This class is the base class that all filters that use combo boxes.
   *
   * @author ????-??-?? Eric Hyer
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class AbstractMultipleChoiceFilter : public AbstractFilter {
      Q_OBJECT

    public:
      AbstractMultipleChoiceFilter(AbstractFilter::FilterEffectivenessFlag,
          int minimumForSuccess = -1);
      AbstractMultipleChoiceFilter(const AbstractMultipleChoiceFilter &other);
      virtual ~AbstractMultipleChoiceFilter();


    protected:
      void createWidget(QStringList options);
      QString const &getCurrentChoice() const;
      using Isis::AbstractFilter::evaluate;

    private:
      void nullify();


    private slots:
      void updateCurChoice(QString);


    private:
      QComboBox *m_combo;
      QString *m_curChoice;
  };
}

#endif
