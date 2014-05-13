#ifndef CorrelationMatrix_h
#define CorrelationMatrix_h

/**
 * @file
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "FileName.h"

#include <QString>
#include <QList>
#include <QStringList>
#include <QMap>

// class QMap<QString, QStringList>;
template <typename A, typename B> class QMap;
template <typename A> class QList;

namespace Isis {
  class MosaicSceneWidget;
  class FileName;
  class SparseBlockColumnMatrix;
  /**
   * @brief This is a container for the correlation matrix that comes from a bundle adjust
   *
   * The bundle adjust will output the covariance matrix to a file. This class will read that file
   * in and compute the correlation matrix. The entire correlation matrix will be written to a file
   * and values will be read/displayed on an as-needed basis.
   *
   * @ingroup Visualization Tools
   *
   * @author 2014-05-02 Kimberly Oyama
   *
   * @internal
   *
   *  @history
   */
  class CorrelationMatrix {
    public:
      CorrelationMatrix();
      ~CorrelationMatrix();
      // get values given upper and lower corners of range...
      // display block... when user wants to see one (x,y) we can show whole block for reference.
      void getCorrelationMatrix(QString fileName);
      void addToUpperTriangle(SparseBlockColumnMatrix sbcm);

      void setCovarianceFileName(FileName covarianceFileName);
      double at(int x, int y) ;

    private:
      QMap<QString, QStringList> m_imagesAndParameters;
      FileName m_covarianceFileName;
      FileName m_correlationFileName;
      QList<double> m_diagonals;
  };
};

#endif
