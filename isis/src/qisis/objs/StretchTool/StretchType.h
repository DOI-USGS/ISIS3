#ifndef STRETCHTYPE_H
#define STRETCHTYPE_H

#include <QWidget>

namespace Isis {
  class Stretch;
  class Cube;
  class Histogram;
}

class QTableWidget;
class QGridLayout;


namespace Isis {
  class HistogramWidget;

  /**
   * @brief This is the base class for advanced stretches
   *
   * This has the common functionality between the advanced stretch types. It
   *  provides a histogram, the stretch table, and layouts. It also stores the
   *  stretch pairs. Using this should keep all of the advanced stretch
   *  interfaces similar and uniformly accessible from the stretch tool.
   *
   * @ingroup Visualization Tools
   *
   * @author 2010-05-20 Steven Lambright
   *
   * @internal
   *   @history 2011-11-04 Steven Lambright - Split up updateGraph and
   *                           updateTable and moved them to protected. The
   *                           manual stretch type needs to choose explicitly
   *                           when to update the table, but always wants to
   *                           update the graph.
   */
  class StretchType : public QWidget {
      Q_OBJECT

    public:
      StretchType(const Histogram &hist, const Stretch &stretch,
                  const QString &name, const QColor &color);

      virtual ~StretchType();

      virtual Stretch getStretch();

      /**
       * Children must re-implement this to update their stretch pairs and GUI
       *   elements appropriately. This could be called with a
       *   different histogram but not a different stretch.
       */
      virtual void setStretch(Stretch) = 0;

      virtual void setHistogram(const Histogram &);


    protected: // methods
      QTableWidget *createStretchTable();

    protected slots:
      void updateGraph();
      void updateTable();

    protected: // data
      QGridLayout *p_mainLayout; //!< Main layout
      Histogram *p_cubeHist; //!< Visible area histogram
      QTableWidget *p_table; //!< Pairs table
      HistogramWidget *p_graph; //!< Histogram graph
      Stretch *p_stretch; //!< Current stretch pairs stored here


    signals:
      //! Emitted when a new Stretch object is available
      void saveToCube();
      void deleteFromCube();
      void stretchChanged();
      void loadStretch(); 

    private slots:
      void savePairs();

  };
};


#endif
