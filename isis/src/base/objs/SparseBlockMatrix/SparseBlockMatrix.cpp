/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "SparseBlockMatrix.h"

// std lib
#include <iostream>
#include <iomanip>

// qt lib
#include <QDataStream>
#include <QDebug>
#include <QMapIterator>
#include <QListIterator>

// boost lib
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>

// Isis lib
#include "IString.h"
#include "LinearAlgebra.h"

using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * Default constructor.
   */
  SparseBlockColumnMatrix::SparseBlockColumnMatrix() {
    m_startColumn = 0;
  }


  /**
   * Destructor. See description of wipe method below.
   */
  SparseBlockColumnMatrix::~SparseBlockColumnMatrix() {
    wipe();
  }


  /**
   * Deletes all pointer elements and removes them from the map. Effectively, a destructor, and in
   *  fact, called by the ~SparseBlockColumnMatrix above.
   */
  void SparseBlockColumnMatrix::wipe() {
    qDeleteAll(values());
    clear();
  }


  /**
   * Copy constructor. Calls copy method immediately below.
   *
   * @param src SparseBlockColumnMatrix to copy
   */
  SparseBlockColumnMatrix::SparseBlockColumnMatrix(const SparseBlockColumnMatrix& src) {
    copy(src);
  }


  /**
   * Copy method.
   *
   * @param src SparseBlockColumnMatrix to copy
   */
  void SparseBlockColumnMatrix::copy(const SparseBlockColumnMatrix& src) {
    // handi-wipe
    wipe();

    // copy matrix blocks from src
    QMapIterator<int, LinearAlgebra::Matrix *> it(src);
    while ( it.hasNext() ) {
      it.next();

      // copy matrix block from src
      LinearAlgebra::Matrix *m = new LinearAlgebra::Matrix(*(it.value()));

      // insert matrix into map
      this->insert(it.key(),m);
    }

    m_startColumn = src.startColumn();
  }


  /**
   * "Equals" operator.
   *
   * @param src SparseBlockColumnMatrix to check against
   */
  SparseBlockColumnMatrix&
      SparseBlockColumnMatrix::operator=(const SparseBlockColumnMatrix& src) {
    if ( this == &src )
      return *this;

    copy(src);

    return *this;
  }


  /**
   * Inserts a "newed" LinearAlgebra::Matrix pointer of size (nRows, nCols) into the map with the
   *  block column number as key. The clear call initializes the matrix elements to zero. If an
   *  entry exists at the key nColumnBlock, no insertion is made.
   *
   * @param nColumnBlock block column number of inserted matrix (key into map)
   * @param nRows number of rows in matrix to be inserted
   * @param nCols number of columns in matrix to be inserted
   *
   * @return bool Returns true if insertion is successful or if block already exists at nColumnBlock
   *              Returns false if attempt to allocate new block fails
   */
  bool SparseBlockColumnMatrix::insertMatrixBlock(int nColumnBlock, int nRows, int nCols) {
    // check if matrix already exists at the key "nColumnBlock"
    if ( this->contains(nColumnBlock) )
      return true;

    // allocate matrix block with nRows and nCols
    LinearAlgebra::Matrix *m = new LinearAlgebra::Matrix(nRows,nCols);

    if ( !m )
      return false;

    // zero matrix elements
    m->clear();

    // insert matrix into map   
    this->insert(nColumnBlock,m);

    return true;
  }


  /**
   * Sets starting column for block in full matrix.
   *
   * @param nStartColumn value for starting column in full matrix for this block columns
   */
  void SparseBlockColumnMatrix::setStartColumn(int nStartColumn) {
    m_startColumn = nStartColumn;
  }


  /**
   * Sets starting column for block in full matrix.
   *
   * @return int returns the starting column in the full matrix
   */
  int SparseBlockColumnMatrix::startColumn() const {
    return m_startColumn;
  }


  /**
   * Returns total number of matrix elements in map (NOTE: NOT the number of matrix blocks). The sum
   *  of all the elements in all of the matrix blocks.
   *
   * @return int Total number of matrix elements in SparseBlockColumnMatrix
   */
  int SparseBlockColumnMatrix::numberOfElements() {
    int nElements = 0;

    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();

      if( !it.value() )
        continue;

      nElements += it.value()->size1()*it.value()->size2();
    }

    return nElements;
  }


  /**
   * Returns total number of columns in map (NOTE: NOT the number of matrix blocks).
   *
   * @return int Total number of columns in SparseBlockColumnMatrix
   */
  int SparseBlockColumnMatrix::numberOfColumns() {

    int nColumns = 0;

    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();

      if( !it.value() )
        continue;

      nColumns = it.value()->size2();
      break;
    }

    return nColumns;
  }


  /**
   * Returns total number of rows in map (this needs to be clarified and maybe rewritten). It's the
   *  number of rows in the block on the diagonal (the last one in the column).
   *
   * @return int Total number of rows in SparseBlockColumnMatrix
   */
  int SparseBlockColumnMatrix::numberOfRows() {

    // iterate to last block (the diagonal one)
    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();

      if( !it.value() )
        continue;
    }

    int nRows = it.value()->size1();

    return nRows;
  }


  /**
   * Prints matrix blocks to std output stream out for debugging. This version makes use of the
   *  Boost matrix library output of matrices as opposed to the printClean method below which
   *  explicitly prints the matrix elements for more control over the format.
   *
   * @param outstream output stream
   */
  void SparseBlockColumnMatrix::print(std::ostream& outstream) {
    if ( size() == 0 ) {
      outstream << "Empty SparseBlockColumnMatrix..." << std::endl;
      return;
    }

    outstream << "Printing SparseBlockColumnMatrix..." << std::endl;
    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();

      if( it.value() )
        outstream << it.key() << std::endl << *(it.value()) << std::endl
                  << std::endl;
      else
        outstream << "NULL block pointer at row[" << IString(it.key())
                  << "]!" << std::endl;
    }
  }


  /**
   * Prints matrix blocks to std output stream out for debugging. Explicitly prints the matrix
   *  elements for more control over the format as opposed to the print method above.
   *
   * @param outstream output stream
   */
  void SparseBlockColumnMatrix::printClean(std::ostream& outstream) {
    if ( size() == 0 ) {
      outstream << "Empty SparseBlockColumnMatrix..." << std::endl;
      return;
    }

    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();

      LinearAlgebra::Matrix * m = it.value();

      int rows = m->size1();
      int cols = m->size2();

      for ( int i = 0; i < rows; i++ ) {
        for ( int j = 0; j < cols; j++ ) {
          double d = m->at_element(i,j);
          if ( j == cols-1 )
            outstream << std::setprecision(12) << d << std::endl;
          else
            outstream << std::setprecision(12) << d << ",";
        }
      }

    }
    outstream << std::endl;
  }


  /**
   * Sets all elements of all matrix blocks to zero.
   */
  void SparseBlockColumnMatrix::zeroBlocks() {
    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();
      it.value()->clear();
    }
  }


  /**
   * Writes matrix to binary disk file pointed to by QDataStream stream
   *
   * @param stream stream pointing to binary disk file
   * @param sbcm SparseBlockColumnMatrix to write
   */
  QDataStream &operator<<(QDataStream &stream, const SparseBlockColumnMatrix &sbcm) {
    // write number of blocks in this column
    int nBlocks = sbcm.size();
    stream << (qint32)nBlocks;

    QMapIterator<int, LinearAlgebra::Matrix *> it(sbcm);
    while ( it.hasNext() ) {
      it.next();

      if( !it.value() )
        continue;

      int nRows = it.value()->size1();
      int nCols = it.value()->size2();

      // write block number (key); rows (size1); and columns (size2)
      stream << it.key() << (qint32)nRows << (qint32)nCols;

      double* data = &it.value()->data()[0];

      // write raw matrix data
      stream.writeRawData((const char*)data, nRows*nCols*sizeof(double));
    }

    return stream;
  }


  /**
   * Reads matrix from binary disk file pointed to by QDataStream stream
   *
   * @param stream stream pointing to binary disk file
   * @param sbcm SparseBlockColumnMatrix to read
   */
  QDataStream &operator>>(QDataStream &stream, SparseBlockColumnMatrix &sbcm) {
    qint32 nBlocks, nBlockNumber, nRows, nCols;
    int i, r, c;

    stream >> nBlocks;

    for ( i = 0; i < nBlocks; i++ ) {
      // read block number (key); rows (size1); and columns (size2)
      stream >> nBlockNumber >> nRows >> nCols;

      double data[nRows*nCols];

      // read raw matrix data
      stream.readRawData((char*)data, nRows*nCols*sizeof(double));

      // insert matrix at correct key
      sbcm.insertMatrixBlock(nBlockNumber, nRows, nCols);

      // get matrix
      LinearAlgebra::Matrix *matrix = sbcm[nBlockNumber];

      // fill with data
      for ( r = 0; r < nRows; r++ ) {
        for ( c = 0; c < nCols; c++ ) {
          int nLocation = r*nRows + c;
          (*matrix)(r,c) = data[nLocation];
        }
      }
    }

    return stream;
  }


  /**
   * Writes matrix to QDebug stream
   *
   * @param dbg debug stream
   * @param sbcm SparseBlockColumnMatrix to write to debug stream
   */
  QDebug operator<<(QDebug dbg, const SparseBlockColumnMatrix &sbcm) {
    dbg.space() << "New Block" << Qt::endl;

    QMapIterator<int, LinearAlgebra::Matrix *> it(sbcm);
    while ( it.hasNext() ) {
      it.next();

      if( !it.value() )
        continue;

      // get matrix
      LinearAlgebra::Matrix *matrix = it.value();

      // matrix rows, columns
      int nRows = matrix->size1();
      int nCols = matrix->size2();

      dbg.nospace() << qSetFieldWidth(4);
      dbg.nospace() << qSetRealNumberPrecision(8);

      for ( int r = 0; r < nRows; r++ ) {
        for ( int c = 0; c < nCols; c++ ) {
          dbg.space() << (*matrix)(r,c);
        }
        dbg.space() << Qt::endl;
      }

      dbg.space() << Qt::endl;
    }

    return dbg;
  }


  //////////////////////////////////////////////////////////////////////////////
  // SparseBlockRowMatrix methods

  /**
   * Destructor. See description of wipe method below.
   */
  SparseBlockRowMatrix::~SparseBlockRowMatrix() {
    wipe();
  }


  /**
   * Deletes all pointer elements and removes them from the map. Effectively, a destructor, and in
   * fact, called by the ~SparseBlockColumnMatrix above.
   */
  void SparseBlockRowMatrix::wipe() {
    qDeleteAll(values());
    clear();
  }


  /**
   * Copy constructor. Calls copy method immediately below.
   *
   * @param src SparseBlockRowMatrix to copy
   */
  SparseBlockRowMatrix::SparseBlockRowMatrix(const SparseBlockRowMatrix& src) {
    copy(src);
  }


  /**
   * Copy method.
   *
   * @param src SparseBlockRowMatrix to copy
   */
  void SparseBlockRowMatrix::copy(const SparseBlockRowMatrix& src) {
    // handi-wipe
    wipe();

    // copy matrix blocks from src
    QMapIterator<int, LinearAlgebra::Matrix *> it(src);
    while ( it.hasNext() ) {
      it.next();

      // copy matrix block from src
      LinearAlgebra::Matrix *m = new LinearAlgebra::Matrix(*(it.value()));

      // insert matrix into map
      this->insert(it.key(),m);
    }
  }


  /**
   * "Equals" operator.
   *
   * @param src SparseBlockRowMatrix to check against
   */
  SparseBlockRowMatrix&
      SparseBlockRowMatrix::operator=(const SparseBlockRowMatrix& src) {
    if ( this == &src )
      return *this;

    copy(src);

    return *this;
  }


  /**
   * Inserts a "newed" LinearAlgebra::Matrix pointer of size (nRows, nCols) into the map with the 
   * block row number as key. The matrix::clear call initializes the matrix elements to zero. If an 
   * entry exists at the key nRowBlock, no insertion is made. 
   *
   * @param nRowBlock block row number of inserted matrix (key into map)
   * @param nRows number of rows in matrix to be inserted
   * @param nCols number of columns in matrix to be inserted
   *
   * @return bool Returns true if insertion successful
   *              Returns false if block already exists at nRowBlock or if allocation of new block
   *              fails
   *              TODO: we return true in the SparseBlockColumnMatrix if the block already exists,
   *                    why is it different here?
   */
  bool SparseBlockRowMatrix::insertMatrixBlock(int nRowBlock, int nRows, int nCols) {
    if ( this->contains(nRowBlock) )
      return false;

    LinearAlgebra::Matrix *m = new LinearAlgebra::Matrix(nRows,nCols);

    if ( !m )
      return false;

    m->clear();

    this->insert(nRowBlock,m);

    return true;
  }


  /**
   * Returns total number of matrix elements in map (NOTE: NOT the number of
   * matrix blocks). The sum of all the elements of all the matrix blocks.
   *
   * @return int Total number of matrix elements in SparseBlockRowMatrix
   */
  int SparseBlockRowMatrix::numberOfElements() {
    int nElements = 0;

    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();

      if( !it.value() )
        continue;

      nElements += it.value()->size1()*it.value()->size2();
    }

    return nElements;
  }


  /**
   * Prints matrix blocks to std output stream out for debugging.
   *
   * @param outstream output stream
   */
  void SparseBlockRowMatrix::print(std::ostream& outstream) {
    if ( size() == 0 ) {
      outstream << "Empty SparseBlockRowMatrix..." << std::endl;
      return;
    }

    outstream << "Printing SparseBlockRowMatrix..." << std::endl;
    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();

      if( it.value() )
        outstream << it.key() << std::endl << *(it.value()) << std::endl
                  << std::endl;
      else
        outstream << "NULL block pointer at column[" << IString(it.key())
                  << "]!" << std::endl;
    }
  }


  /**
   * Prints matrix blocks to std output stream out for debugging.
   *
   * @param outstream output stream
   */
  void SparseBlockRowMatrix::printClean(std::ostream& outstream) {
    if ( size() == 0 ) {
      outstream << "Empty SparseBlockRowMatrix..." << std::endl;
      return;
    }

    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();

      LinearAlgebra::Matrix *m = it.value();

      int rows = m->size1();
      int cols = m->size2();

      for ( int i = 0; i < rows; i++ ) {
        for ( int j = 0; j < cols; j++ ) {
          double d = m->at_element(i,j);
          if ( j == cols-1 )
            outstream << std::setprecision(9) << d << std::endl;
          else
            outstream << std::setprecision(9) << d << ",";
        }
      }

    }
    outstream << std::endl;
  }


  /**
   * Sets all elements of all matrix blocks to zero.
   */
  void SparseBlockRowMatrix::zeroBlocks() {
    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();
      it.value()->clear();
    }
  }


  /**
   * Copies a SparseBlockRowMatrix to a Boost compressed_matrix. This may be a temporary
   *  implementation
   *
   * @param B Boost matrix to copy this SparseBlockRowMatrix to
   */
  void SparseBlockRowMatrix::copyToBoost(compressed_matrix<double>& B) {
    B.clear();

    int ncols, nstart, nend, nrowBlock;
    range rRow = range(0,3);
    range rCol;

    QMapIterator<int, LinearAlgebra::Matrix *> it(*this);
    while ( it.hasNext() ) {
      it.next();

      nrowBlock = it.key();
      LinearAlgebra::Matrix *m = it.value();

      ncols = m->size2();

      nstart = nrowBlock*ncols;
      nend = nstart + ncols;

      rCol = range(nstart,nend);

      matrix_range<compressed_matrix<double> > m1 (B, rRow, rCol);

      m1 = *m;
    }
  }


  /**
   * Sums and returns the number of columns in each matrix block prior to nblockColumn
   *
   * @param nblockColumn
   *
   * @return int Number of leading columns for block at nblockColumn
   */
    int SparseBlockRowMatrix::getLeadingColumnsForBlock(int nblockColumn) {

      if ( nblockColumn == 0 )
        return 0;

      int nLeadingColumnsElements = 0;

      int nCol = 0;

      while ( nCol < nblockColumn ) {
        if ( !(*this)[nCol] ) {
          nCol++;
          continue;
        }

        int ncolumns = (*this)[nCol]->size2();

        if ( ncolumns == -1 )
          continue;

        nLeadingColumnsElements += ncolumns;

        nCol++;
      }

      return nLeadingColumnsElements;
    }


  /**
   * Writes matrix to binary disk file pointed to by QDataStream stream
   *
   * @param stream stream pointing to binary disk file
   * @param sbrm SparseBlockRowMatrix to write
   */
  QDataStream &operator<<(QDataStream &stream, const SparseBlockRowMatrix &sbrm) {
    // write number of blocks in this column
    int nBlocks = sbrm.size();
    stream << (qint32)nBlocks;

    QMapIterator<int, LinearAlgebra::Matrix *> it(sbrm);
    while ( it.hasNext() ) {
      it.next();

      if( !it.value() )
        continue;

      int nRows = it.value()->size1();
      int nCols = it.value()->size2();

      // write block number (key); rows (size1); and columns (size2)
      stream << it.key() << (qint32)nRows << (qint32)nCols;

      double* data = &it.value()->data()[0];

      // write raw matrix data
      stream.writeRawData((const char*)data, nRows*nCols*sizeof(double));
    }

    return stream;
  }


  /**
   * Reads matrix from binary disk file pointed to by QDataStream stream
   *
   * @param stream stream pointing to binary disk file
   * @param sbcm SparseBlockColumnMatrix to read
   */
  QDataStream &operator>>(QDataStream &stream, SparseBlockRowMatrix &sbrm) {
    qint32 nBlocks, nBlockNumber, nRows, nCols;
    int i, r, c;

    stream >> nBlocks;

    for ( i = 0; i < nBlocks; i++ ) {
      // read block number (key); rows (size1); and columns (size2)
      stream >> nBlockNumber >> nRows >> nCols;

      double data[nRows*nCols];

      // read raw matrix data
      stream.readRawData((char*)data, nRows*nCols*sizeof(double));

      // insert matrix at correct key
      sbrm.insertMatrixBlock(nBlockNumber, nRows, nCols);

      // get matrix
      LinearAlgebra::Matrix *matrix = sbrm[nBlockNumber];

      // fill with data
      for ( r = 0; r < nRows; r++ ) {
        for ( c = 0; c < nCols; c++ ) {
          int nLocation = r*nRows + c;
          (*matrix)(r,c) = data[nLocation];
        }
      }
    }

    return stream;
  }


  /**
   * Writes matrix to QDebug stream
   *
   * @param dbt debug stream
   * @param sbcm SparseBlockRowMatrix to write to debug stream
   */
  QDebug operator<<(QDebug dbg, const SparseBlockRowMatrix &sbrm) {
    dbg.space() << "New Block" << Qt::endl;

    QMapIterator<int, LinearAlgebra::Matrix *> it(sbrm);
    while ( it.hasNext() ) {
      it.next();

      if( !it.value() )
        continue;

      // get matrix
      LinearAlgebra::Matrix *matrix = it.value();

      // matrix rows, columns
      int nRows = matrix->size1();
      int nCols = matrix->size2();

      dbg.nospace() << qSetFieldWidth(4);
      dbg.nospace() << qSetRealNumberPrecision(8);

      for ( int r = 0; r < nRows; r++ ) {
        for ( int c = 0; c < nCols; c++ ) {
          dbg.space() << (*matrix)(r,c);
        }
        dbg.space() << Qt::endl;
      }
      dbg.space() << Qt::endl;
    }

    return dbg;
  }


  //////////////////////////////////////////////////////////////////////////////
  // SparseBlockMatrix methods

  /**
   * Destructor.
   */
  SparseBlockMatrix::~SparseBlockMatrix() {
    wipe();
  }


  /**
   * Deletes all pointer elements and removes them from the list. Effectively, a destructor, and in
   *  fact, called by the ~SparseBlockMatrix above.
   */
  void SparseBlockMatrix::wipe() {
    qDeleteAll(*this);
    clear();
  }


  /**
   * Copy constructor. Calls copy method immediately below.
   *
   * @param src SparseBlockMatrix to copy
   */
  SparseBlockMatrix::SparseBlockMatrix(const SparseBlockMatrix& src) {
    copy(src);
  }


  /**
   * Copy method.
   *
   * @param src SparseBlockMatrix to copy
   */
  void SparseBlockMatrix::copy(const SparseBlockMatrix& src) {
    // handi-wipe
    wipe();

    // copy all SparseBlockColumnMatrix objects from src
    for( int i = 0; i < src.size(); i++ ) {
      append( new SparseBlockColumnMatrix(*(src.at(i))));
    }
  }


  /**
   * "Equals" operator.
   *
   * @param src SparseBlockMatrix to check against
   */
  SparseBlockMatrix& SparseBlockMatrix::operator=(const SparseBlockMatrix& src) {
    if ( this == &src )
      return *this;

    copy(src);

    return *this;
  }


  /**
   * Initializes number of columns (SparseBlockColumnMatrix).
   *
   * @param n number of columns to insert
   *
   * @return bool Always returns true
   *              TODO: why bother returning bool? should there be a false condition?
   */
  bool SparseBlockMatrix::setNumberOfColumns( int n ) {

    for( int i = 0; i < n; i++ )
      append( new SparseBlockColumnMatrix() );

    return true;
  }


  /**
   * Inserts a "newed" boost LinearAlgebra::Matrix pointer of size (nRows, 
   * nCols) into the matrix at nColumnBlock, nRowBlock. The inserted matrix 
   * elements are initialized to zero. 
   * If an entry exists at nColumnBlock, RowBlock, no insertion is made.
   *
   * @param nColumnBlock block column number of inserted matrix (QList index)
   * @param nRowBlock block row number of inserted matrix (key into map)
   * @param nRows number of rows in matrix to be inserted
   * @param nCols number of columns in matrix to be inserted
   *
   * @return bool Returns result of SparseBlockColumnMatrix::insertMatrixBlock()
   */
  bool SparseBlockMatrix::insertMatrixBlock(int nColumnBlock, int nRowBlock, int nRows, int nCols) {
    return (*this)[nColumnBlock]->insertMatrixBlock(nRowBlock, nRows, nCols);
  }


  /**
   * Returns total number of blocks in matrix.
   *
   * @return int Total number of blocks in matrix
   */
  int SparseBlockMatrix::numberOfBlocks() {
    int nBlocks = 0;

    for( int i = 0; i < size(); i++ ) {
      if ( !(*this)[i] )
        continue;

      nBlocks += (*this)[i]->size();
    }

    return nBlocks;
  }


  /**
   * Returns number of diagonal matrix blocks (equivalent to size - there is one per column).
   *
   * @return int Number of diagnonal blocks in matrix
   */
  int SparseBlockMatrix::numberOfDiagonalBlocks() {
    int ndiagBlocks = 0;

    for( int i = 0; i < size(); i++ ) {
      SparseBlockColumnMatrix* column = at(i);

      if ( !column )
        continue;

      QMapIterator<int, LinearAlgebra::Matrix *> it(*column);
      while ( it.hasNext() ) {
        it.next();

        if( it.key() == i ) {
          ndiagBlocks++;
          break;
        }
      }
    }

    return ndiagBlocks;
  }


  /**
   * Returns number of off-diagonal matrix blocks.
   *
   * @return int Number of off-diagonal blocks in matrix
   */
  int SparseBlockMatrix::numberOfOffDiagonalBlocks() {
    return (numberOfBlocks() - numberOfDiagonalBlocks());
  }


  /**
   * Returns number of matrix elements in matrix.
   *
   * @return int Total number of matrix elements
   */
  int SparseBlockMatrix::numberOfElements() {
    int nElements = 0;

    for( int i = 0; i < size(); i++ ) {
      if ( !(*this)[i] )
        continue;

      nElements += (*this)[i]->numberOfElements();
    }

    return nElements;
  }


  /**
   * Returns pointer to boost matrix at position (column, row).
   *
   * @param column block column number
   * @param row block row number
   *
   * @return LinearAlgebra::Matrix  Pointer to Boost matrix at position (column,
   *         row)
   */
  LinearAlgebra::Matrix *SparseBlockMatrix::getBlock(int column, int row) {
    return (*(*this)[column])[row];
  }


  /**
   * Sets all elements of all matrix blocks to zero.
   */
  void SparseBlockMatrix::zeroBlocks() {
    for ( int i = 0; i < size(); i++ )
      (*this)[i]->zeroBlocks();
  }


  /**
   * Prints matrix blocks to std output stream out for debugging.
   *
   * @param outstream output stream
   */
  void SparseBlockMatrix::print(std::ostream& outstream) {
    if ( size() == 0 ) {
      outstream << "Empty SparseBlockMatrix..." << std::endl;
      return;
    }

    outstream << "Printing SparseBlockMatrix..." << std::endl;
    for( int i = 0; i < size(); i++ ) {
      SparseBlockColumnMatrix* column = at(i);

      if ( column )
        column->print(outstream);
      else
        outstream << "NULL column pointer at column[" << IString(i)
                  << "]!" << std::endl;
    }
  }


  /**
   * Prints matrix blocks to std output stream out for debugging.
   *
   * @param outstream output stream
   */
  void SparseBlockMatrix::printClean(std::ostream& outstream) {
    if ( size() == 0 ) {
      outstream << "Empty SparseBlockMatrix..." << std::endl;
      return;
    }

    for( int i = 0; i < size(); i++ ) {
      SparseBlockColumnMatrix* column = at(i);

      if ( column )
        column->printClean(outstream);
      else
        outstream << "NULL column pointer at column[" << IString(i)
                  << "]!" << std::endl;
    }
  }


  /**
   * Sums and returns the number of columns in each matrix block prior to nblockColumn
   *
   * @param nblockColumn
   *
   * @return int Number of leading column elements for block at nblockColumn
   */
  int SparseBlockMatrix::getLeadingColumnsForBlock(int nblockColumn) {

    if ( nblockColumn == 0 )
      return 0;

    int nLeadingColumnsElements = 0;

    int nCol = 0;

    while ( nCol < nblockColumn ) {
      if ( !(*this)[nCol] )
        continue;

      int ncolumns = (*this)[nCol]->numberOfColumns();

      if ( ncolumns == -1 )
        continue;

      nLeadingColumnsElements += ncolumns;

      nCol++;
    }

    return nLeadingColumnsElements;
  }


  /**
   * Sums and returns the number of rows in each matrix block prior to nblockRow
   *
   * @param nblockRow
   *
   * @return int Number of leading row elements for block at nblockRow
   */
  int SparseBlockMatrix::getLeadingRowsForBlock(int nblockRow) {

    if ( nblockRow == 0 )
      return 0;

    int i = 0;
    int nLeadingRows = 0;

    while ( i < nblockRow ) {
      SparseBlockColumnMatrix* column = at(i);

      if ( !column )
        continue;

      QMapIterator<int, LinearAlgebra::Matrix *> it(*column);
      // iterate to last element in column
      while ( it.hasNext() ) {
        it.next();

        if( it.key() == i )
          nLeadingRows += it.value()->size1();
      }
      i++;
    }

    return nLeadingRows;
  }


  /**
   * Writes matrix to binary disk file pointed to by QDataStream stream
   *
   * @param stream stream pointing to binary disk file
   * @param sparseBlockMatrix SparseBlockMatrix to write
   */
  QDataStream &operator<<(QDataStream &stream, const SparseBlockMatrix &sparseBlockMatrix) {
    int nBlockColumns = sparseBlockMatrix.size();

    stream << (qint32)nBlockColumns;

    for ( int i =0; i < nBlockColumns; i++ )
      stream << *sparseBlockMatrix.at(i);

    return stream;
  }


  /**
   * Reads matrix from binary disk file pointed to by QDataStream stream
   *
   * @param stream stream pointing to binary disk file
   * @param sparseBlockMatrix SparseBlockMatrix to read
   */
  QDataStream &operator>>(QDataStream &stream, SparseBlockMatrix &sparseBlockMatrix) {
    qint32 nBlockColumns;

    // read and set number of block columns
    stream >> nBlockColumns;
    sparseBlockMatrix.setNumberOfColumns(nBlockColumns);

    for ( int i =0; i < nBlockColumns; i++ )
      stream >> *sparseBlockMatrix.at(i);

    return stream;
  }

  /**
   * Writes matrix to QDebug stream
   *
   * @param dbg debug stream
   * @param m SparseBlockMatrix to write to debug stream
   */
  QDebug operator<<(QDebug dbg, const SparseBlockMatrix &m) {
    int nBlockColumns = m.size();

    for ( int i =0; i < nBlockColumns; i++ )
      dbg << *m.at(i);

    return dbg;
  }
}
