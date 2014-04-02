#ifndef SparseBlockMatrix_h
#define SparseBlockMatrix_h

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


#include <QMap>
#include <QList>
#include <iostream>

#include <boost/numeric/ublas/fwd.hpp>

class QDebug;

namespace Isis {

  /**
   * @brief SparseBlockColumnMatrix
   *
   * This class is bla bla bla.
   *
   * @ingroup Utility
   *
   * @author 2011-07-29 Ken Edmundson
   *
   * @internal
   *   @history 2011-07-29 Ken Edmundson Created
   *   @history 2014-02-25 Ken Edmundson - operators to read/write matrices to binary disk file and
   *                       to write matrices to QDebug stream.
   */
  class SparseBlockColumnMatrix :
      public QMap< int, boost::numeric::ublas::matrix<double>* > {

  public:
    SparseBlockColumnMatrix(){} // default constructor
    ~SparseBlockColumnMatrix(); // destructor

    // copy constructor
    SparseBlockColumnMatrix(const SparseBlockColumnMatrix& src);

    SparseBlockColumnMatrix& operator=(const SparseBlockColumnMatrix& src);

    void wipe();
    void copy(const SparseBlockColumnMatrix& src);

    void zeroBlocks();
    bool InsertMatrixBlock(int nColumnBlock, int nRows, int nCols);
    int numberOfElements();
    int numberOfRows();
    int numberOfColumns();
    void print(std::ostream& outstream);
  };

  // operators to read/write SparseBlockColumnMatrix to/from binary disk file
  QDataStream &operator<<(QDataStream &, const SparseBlockColumnMatrix &);
  QDataStream &operator>>(QDataStream &, SparseBlockColumnMatrix &);

  // operator to write SparseBlockColumnMatrix to QDebug stream
  QDebug operator<<(QDebug dbg, const SparseBlockColumnMatrix &sbcm);


  /**
   * @brief SparseBlockRowMatrix
   *
   * This class is bla bla bla.
   *
   * @ingroup Utility
   *
   * @author 2011-07-29 Ken Edmundson
   *
   * @internal
   *   @history 2011-07-29 Ken Edmundson Created
   *   @history 2014-02-25 Ken Edmundson - operators to read/write matrices to binary disk file and
   *                       to write matrices to QDebug stream.
   */
  class SparseBlockRowMatrix :
      public QMap< int, boost::numeric::ublas::matrix<double>* > {

  public:
    SparseBlockRowMatrix(){} // default constructor
    ~SparseBlockRowMatrix();

    // copy constructor
    SparseBlockRowMatrix(const SparseBlockRowMatrix& src);

    SparseBlockRowMatrix& operator=(const SparseBlockRowMatrix& src);


    void wipe();
    void copy(const SparseBlockRowMatrix& src);

    void zeroBlocks();
    bool InsertMatrixBlock(int nRowBlock, int nRows, int nCols);
    void copyToBoost(boost::numeric::ublas::compressed_matrix<double>& B);
    int numberOfElements();
    void print(std::ostream& outstream);
  };

  // operators to read/write SparseBlockRowMatrix to/from binary disk file
  QDataStream &operator<<(QDataStream &, const SparseBlockRowMatrix &);
  QDataStream &operator>>(QDataStream &, SparseBlockRowMatrix &);

  // operator to write SparseBlockRowMatrix to QDebug stream
  QDebug operator<<(QDebug dbg, const SparseBlockRowMatrix &sbcm);

  /**
   * @brief SparseBlockMatrix
   *
   * This class is bla bla bla.
   *
   * @ingroup Utility
   *
   * @author 2011-07-29 Ken Edmundson
   *
   * @internal
   *   @history 2011-07-29 Ken Edmundson Created
   *   @history 2014-02-25 Ken Edmundson - operators to read/write matrices to binary disk file and
   *                       to write matrices to QDebug stream.
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
    bool InsertMatrixBlock(int nColumnBlock, int nRowBlock, int nRows, int nCols);
    boost::numeric::ublas::matrix<double>* getBlock(int column, int row);
    int numberOfBlocks();
    int numberOfDiagonalBlocks();
    int numberOfOffDiagonalBlocks();
    int numberOfElements();
    void print(std::ostream& outstream);
    bool write(std::ofstream &fp_out, bool binary=true);
  };

  // operators to read/write SparseBlockMatrix to/from binary disk file
  QDataStream &operator<<(QDataStream &, const SparseBlockMatrix &);
  QDataStream &operator>>(QDataStream &, SparseBlockMatrix &);

  // operator to write SparseBlockMatrix to QDebug stream
  QDebug operator<<(QDebug dbg, const SparseBlockMatrix &m);
}

#endif
