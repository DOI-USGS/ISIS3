#ifndef CorrelationMatrix_h
#define CorrelationMatrix_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "FileName.h"

#include <QDebug>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

#include <boost/numeric/ublas/matrix_sparse.hpp>

template <typename A, typename B> class QMap;
template <typename A> class QList;

namespace Isis {
  class FileName;
  class MosaicSceneWidget;
  class PvlObject;
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
   *   @history 2014-05-02 Kimberly Oyama - Original version.
   *   @history 2014-07-23 Jeannie Backer - Added QDataStream >> and << operators and read/write
   *                           methods. Created unitTest. Added new operators to assignments in
   *                           copy constructor and operator= methods.
   *   @history 2015-10-14 Jeffrey Covington - Declared CorrelationMatrix as a
   *                           Qt metatype for use with QVariant.
   *   @history 2016-06-06 Tyler Wilson - Fixed a problem with a PvlKeywordIterator not
   *                           being incremented in the constructor which accepts a PvlObject.
   *                           There was also an issue with a QMap data structure not being
   *                           initialized, resulting in a segmentation fault.  Also added
   *                           testing for exceptions being thrown in this constructor,
   *                           as well as the function computeCorrelationMatrix. Fixes #3997,3999.
   *   @history 2016-07-11 Jesse Mapel - Updated method names to meet coding standards.
   *                           Fixes #4112.
   *   @history 2016-08-10 Jeannie Backer - Replaced boost matrix with Isis::LinearAlgebra::Matrix.
   *                           References #4163.
   */
  class CorrelationMatrix {
    public:
      CorrelationMatrix();
      CorrelationMatrix(PvlObject storedMatrixData);
      CorrelationMatrix(const CorrelationMatrix &other);
      ~CorrelationMatrix();

      CorrelationMatrix &operator=(const CorrelationMatrix &other);

      void computeCorrelationMatrix();
      void retrieveVisibleElements(int x, int y);

      bool isValid();
      bool hasCovMat();
     //const bool hasCovMat() const;

      void setCorrelationFileName(FileName correlationFileName);
      void setCovarianceFileName(FileName covarianceFileName);
      void setImagesAndParameters(QMap<QString, QStringList> imagesAndParameters);

      SparseBlockColumnMatrix correlationMatrixFromFile(QDataStream inStream);
      //might need something called deleteLater(), called from MatrixTreeWidgetItem constructor.

      //if cov filename is null we need to ask the user to find it.

      FileName correlationFileName();
      FileName covarianceFileName();
      QMap<QString, QStringList> *imagesAndParameters();

      void retrieveWholeMatrix();
      void retrieveThreeVisibleBlocks();

      // Need these for range used to pick colors....
      QList<SparseBlockColumnMatrix> *visibleBlocks();

      PvlObject pvlObject();


    private:
      //! This map holds the images used to create this matrix and their associated parameters.
      QMap<QString, QStringList> *m_imagesAndParameters;

      //! FileName of the covariance matrix calculated when the bundle was run.
      FileName *m_covarianceFileName;

      //! FileName of the correlation matrix
      FileName *m_correlationFileName;

      /**
       * List of the parameter values. Stored so we don't need to store all the SBCMs when
       * calculating the correlation values.
       */
      QList<double> *m_diagonals;

      /**
       * This will be the three blocks (or whole matrix depending on size) that apply to
       * the given area.
       */
      QList<SparseBlockColumnMatrix> *m_visibleBlocks;
  };

};

Q_DECLARE_METATYPE(Isis::CorrelationMatrix);

#endif
