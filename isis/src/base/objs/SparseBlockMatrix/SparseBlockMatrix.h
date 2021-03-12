#ifndef SparseBlockMatrix_h
#define SparseBlockMatrix_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// std library
#include <iostream>

// Qt library
#include <QMap>
#include <QList>

// boost library
#include <boost/numeric/ublas/fwd.hpp>

// Isis library
#include "LinearAlgebra.h"

// Qt library
class QDebug;

namespace Isis {

  /**
   * @brief SparseBlockColumnMatrix
   *
   * The SparseBlockMatrix class is a QList of SparseBlockColumnMatrix objects. Each
   * SparseBlockColumnMatrix is a QMap of square matrix blocks and represents a column of square
   * matrix blocks in the reduced normal equations matrix. The key into each column map is the
   * block's row index. The value at each key is a square dense matrix (Boost matrix) with a
   * dimension equivalent to the number of exterior orientation parameters used for the image.
   * Zero blocks are not stored.
   *
   * @ingroup Utility
   *
   * @author 2011-07-29 Ken Edmundson
   *
   * @internal
   *   @history 2011-07-29 Ken Edmundson - Original Version
   *   @history 2014-02-25 Ken Edmundson - operators to read/write matrices to binary disk file and
   *                           to write matrices to QDebug stream.
   *   @history 2014-07-23 Jeannie Backer - Modified QDataStream >> and << operators to use qint32,
   *                           as recommended by Qt documentation.
   *   @history 2015-12-18 Ken Edmundson - 1) added more detailed documentation; 2) brought up to
   *                           ISIS coding standards.
   *   @history 2016-08-10 Jeannie Backer - Replaced boost matrix with Isis::LinearAlgebra::Matrix.
   *                           References #4163.
   *   @history 2017-05-09 Ken Edmundson - Added m_startColumn member and mutator/accessor methods
   *                           to SparseBlockColumnMatrix. Done to eliminate lengthy computation of
   *                           leading colums and rows. References #4664.
   */
  class SparseBlockColumnMatrix :
      public QMap< int, LinearAlgebra::Matrix * > {

  public:
    SparseBlockColumnMatrix();  // default constructor
    ~SparseBlockColumnMatrix(); // destructor

    // copy constructor
    SparseBlockColumnMatrix(const SparseBlockColumnMatrix& src);

    SparseBlockColumnMatrix& operator=(const SparseBlockColumnMatrix& src);

    void wipe();
    void copy(const SparseBlockColumnMatrix& src);

    void zeroBlocks();
    bool insertMatrixBlock(int nColumnBlock, int nRows, int nCols);

    void setStartColumn(int nStartColumn);
    int startColumn() const;
    int numberOfElements();
    int numberOfRows();
    int numberOfColumns();
    void print(std::ostream& outstream);
    void printClean(std::ostream& outstream);

  protected:
    int m_startColumn; /**< starting column for this Block Column in full matrix
                            e.g. for Block Column 4, if the preceding Block Columns each have 6
                            columns, then the starting column for Block Column 4 is 24 */
  };

  // operators to read/write SparseBlockColumnMatrix to/from binary disk file
  QDataStream &operator<<(QDataStream &, const SparseBlockColumnMatrix &);
  QDataStream &operator>>(QDataStream &, SparseBlockColumnMatrix &);

  // operator to write SparseBlockColumnMatrix to QDebug stream
  QDebug operator<<(QDebug dbg, const SparseBlockColumnMatrix &sbcm);


  /**
   * @brief SparseBlockRowMatrix
   *
   * A SparseBlockRowMatrix is a QMap of square matrix blocks and represents a row of square
   *  matrix blocks in the reduced normal equations matrix. The key into each row map is the
   *  block’s column index. The value at each key is a square dense matrix (Boost matrix) with a
   *  dimension equivalent to the number of exterior orientation parameters used for the image.
   *  Zero blocks are not stored.
   *
   * Note that this class is not apparently different than the SparseBlockColumnMatrix. It was
   *  implemented for convenience in the BundleAdjustment class for those times when we need to
   *  access rows of matrix blocks as opposed to columns of matrix blocks.
   *
   * TO BE RESOLVED: Do we really need this as a separate class? Can we do everything we need with
   *  the SparseBlockColumnMatrix?
   *
   * @ingroup Utility
   *
   * @author 2011-07-29 Ken Edmundson
   *
   * @internal
   *   @history 2011-07-29 Ken Edmundson Created
   *   @history 2014-02-25 Ken Edmundson - operators to read/write matrices to binary disk file and
   *                           to write matrices to QDebug stream.
   *   @history 2015-12-18 Ken Edmundson - 1) added more detailed documentation; 2) brought closer
   *                           to ISIS coding standards.
   */
  class SparseBlockRowMatrix :
      public QMap< int, LinearAlgebra::Matrix  * > {

  public:
    SparseBlockRowMatrix(){} // default constructor
    ~SparseBlockRowMatrix();

    // copy constructor
    SparseBlockRowMatrix(const SparseBlockRowMatrix& src);

    SparseBlockRowMatrix& operator=(const SparseBlockRowMatrix& src);


    void wipe();
    void copy(const SparseBlockRowMatrix& src);

    void zeroBlocks();
    bool insertMatrixBlock(int nRowBlock, int nRows, int nCols);
    void copyToBoost(boost::numeric::ublas::compressed_matrix<double>& B);
    int getLeadingColumnsForBlock(int nblockColumn);
    int numberOfElements();
    void print(std::ostream& outstream);
    void printClean(std::ostream& outstream);
  };

  // operators to read/write SparseBlockRowMatrix to/from binary disk file
  QDataStream &operator<<(QDataStream &, const SparseBlockRowMatrix &);
  QDataStream &operator>>(QDataStream &, SparseBlockRowMatrix &);

  // operator to write SparseBlockRowMatrix to QDebug stream
  QDebug operator<<(QDebug dbg, const SparseBlockRowMatrix &sbcm);

  /**
   * @brief SparseBlockMatrix
   *
   * The CHOLMOD (Cholesky decomposition) package uses the compressed column storage (CCS) matrix
   *  format which is efficient in memory storage but inefficient to fill in an arbitrary manner
   *  because the insertion of every non-zero entry requires that all succeeding entries be shifted.
   *
   *  To build the reduced normal equations matrix, an interim sparse matrix structure is required
   *  that can be efficiently populated in a random fashion and can be traversed by column in row
   *  order to subsequently construct the CCS matrix required by CHOLMOD. We use a type of Block
   *  Compressed Column Storage (BCCS) which consists of an array of map containers (QList of
   *  SparseBlockColumnMatrices), each representing a column of square matrix blocks in the reduced
   *  normal equations. The key into each column map is the block’s row index.The value at each key
   *  is a square dense matrix (Boost matrix) with a dimension equivalent to the number of exterior
   *  orientation parameters used for the image. Zero blocks are not stored. The BCCS matrix is
   *  created only in the first iteration of the bundle adjustment; in subsequent iterations it need
   *  only be repopulated. As the normal equations matrix is symmetric, only the upper triangular
   *  portion is stored in memory.
   *
   * @ingroup Utility
   *
   * @author 2011-07-29 Ken Edmundson
   *
   * @internal
   *   @history 2011-07-29 Ken Edmundson Created
   *   @history 2014-02-25 Ken Edmundson - operators to read/write matrices to binary disk file and
   *                           to write matrices to QDebug stream.
   *   @history 2015-12-18 Ken Edmundson - 1) added more detailed documentation; 2) brought closer
   *                           to ISIS coding standards.
   */
  class SparseBlockMatrix : public QList< SparseBlockColumnMatrix* > {

  public:
    SparseBlockMatrix() {} // default constructor
    ~SparseBlockMatrix();

    // copy constructor
    SparseBlockMatrix(const SparseBlockMatrix& src);

    SparseBlockMatrix& operator=(const SparseBlockMatrix& src);

    void wipe();
    void copy(const SparseBlockMatrix& src);

    bool setNumberOfColumns( int n );
    void zeroBlocks();
    bool insertMatrixBlock(int nColumnBlock, int nRowBlock, int nRows, int nCols);
    LinearAlgebra::Matrix *getBlock(int column, int row);
    int numberOfBlocks();
    int numberOfDiagonalBlocks();
    int numberOfOffDiagonalBlocks();
    int numberOfElements();
    void print(std::ostream& outstream);
    void printClean(std::ostream& outstream);
    bool write(std::ofstream &fp_out, bool binary=true);
    int getLeadingColumnsForBlock(int nblockColumn);
    int getLeadingRowsForBlock(int nblockRow);
  };

  // operators to read/write SparseBlockMatrix to/from binary disk file
  QDataStream &operator<<(QDataStream &, const SparseBlockMatrix &);
  QDataStream &operator>>(QDataStream &, SparseBlockMatrix &);

  // operator to write SparseBlockMatrix to QDebug stream
  QDebug operator<<(QDebug dbg, const SparseBlockMatrix &m);
}

#endif
