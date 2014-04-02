#include "SparseBlockMatrix.h"

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <iostream>
#include <iomanip>
#include <QDebug>
#include <QMapIterator>
#include <QListIterator>

#include "IString.h"

using namespace boost::numeric::ublas;

namespace Isis {

  /**
   * Destructor. See description of wipe method below.
   */
  SparseBlockColumnMatrix::~SparseBlockColumnMatrix() {
    wipe();
  }


  /**
   * Deletes all pointer elements and removes them from the map.
   * Effectively, a destructor, and in fact, called by the
   * ~SparseBlockColumnMatrix above.
   */
  void SparseBlockColumnMatrix::wipe() {
    qDeleteAll(values());
    clear();
  }


  /**
   * Copy constructor. Calls copy method immediately below.
   */
  SparseBlockColumnMatrix::SparseBlockColumnMatrix(const SparseBlockColumnMatrix& src) {
    copy(src);
  }


  /**
   * Copy method.
   */
  void SparseBlockColumnMatrix::copy(const SparseBlockColumnMatrix& src) {
    // handi-wipe
    wipe();

    // copy matrix blocks from src
    QMapIterator<int, matrix<double>*> it(src);
     while (it.hasNext()) {
         it.next();

         // copy matrix block from src
         matrix<double>* m = new matrix<double>(*(it.value()));

         // insert matrix into map
         this->insert(it.key(),m);
     }
  }


  /**
   * "Equals" operator.
   */
  SparseBlockColumnMatrix&
      SparseBlockColumnMatrix::operator=(const SparseBlockColumnMatrix& src) {
    if ( this == &src )
      return *this;

    copy(src);

    return *this;
  }


  /**
   * Inserts a "newed" boost matrix<double>* of size (nRows, nCols) into the
   * map with the block column number as key. The matrix::clear call initializes
   * the matrix elements to zero.
   * If an entry exists at the key nColumnBlock, no insertion is made.
   *
   * @param nColumnBlock block column number of inserted matrix (key into map)
   * @param nRows number of rows in matrix to be inserted
   * @param nCols number of columns in matrix to be inserted
   */
  bool SparseBlockColumnMatrix::InsertMatrixBlock(int nColumnBlock, int nRows,
                                                  int nCols) {

    // check if matrix already exists at the key "nColumnBlock"
    if ( this->contains(nColumnBlock) )
      return true;

    // allocate matrix block with nRows and nCols
    matrix<double>* m = new matrix<double>(nRows,nCols);

    if ( !m )
      return false;

    // zero matrix elements
    m->clear();

    // insert matrix into map
    this->insert(nColumnBlock,m);

    return true;
  }


  /**
   * Returns total number of matrix elements in map (NOTE: NOT the number of
   * matrix blocks). The sum of all the elements of all the matrix blocks.
   */
  int SparseBlockColumnMatrix::numberOfElements() {
    int nElements = 0;

    QMapIterator<int, matrix<double>*> it(*this);
     while (it.hasNext()) {
         it.next();

         if( !it.value() )
           continue;

         nElements += it.value()->size1()*it.value()->size2();
     }

     return nElements;
  }


  /**
   * Returns total number of columns in map (NOTE: NOT the number of
   * matrix blocks).
   */
  int SparseBlockColumnMatrix::numberOfColumns() {

    int nColumns = 0;

    QMapIterator<int, matrix<double>*> it(*this);
     while (it.hasNext()) {
         it.next();

         if( !it.value() )
           continue;

         nColumns = it.value()->size2();
         break;
     }

     return nColumns;
  }


  /**
   * Returns total number of rows in map (this needs to be clarified and maybe rewritten)
   * its the number of rows in the block on the diagonal (the last one in the column).
   */
  int SparseBlockColumnMatrix::numberOfRows() {

    // iterate to last block (the diagonal one)
    QMapIterator<int, matrix<double>*> it(*this);
     while (it.hasNext()) {
         it.next();

         if( !it.value() )
           continue;
     }

     int nRows = it.value()->size1();

    return nRows;
  }


  /**
   * Prints matrix blocks to std output stream out for debugging.
   */
  void SparseBlockColumnMatrix::print(std::ostream& outstream) {
    if ( size() == 0 ) {
      outstream << "Empty SparseBlockColumnMatrix..." << std::endl;
      return;
    }

    outstream << "Printing SparseBlockColumnMatrix..." << std::endl;
    QMapIterator<int, matrix<double>*> it(*this);
     while (it.hasNext()) {
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
   * Sets all elements of all matrix blocks to zero.
   */
  void SparseBlockColumnMatrix::zeroBlocks() {
    QMapIterator<int, matrix<double>*> it(*this);
    while ( it.hasNext() ) {
      it.next();
      it.value()->clear();
    }
  }


  /**
   * Writes matrix to binary disk file pointed to by QDataStream stream
   */
  QDataStream &operator<<(QDataStream &stream, const SparseBlockColumnMatrix &sbcm) {
    // write number of blocks in this column
    int nBlocks = sbcm.size();
    stream << nBlocks;

    QMapIterator<int, matrix<double>*> it(sbcm);
     while ( it.hasNext() ) {
       it.next();

       if( !it.value() )
         continue;

       int nRows = it.value()->size1();
       int nCols = it.value()->size2();

       // write block number (key); rows (size1); and columns (size2)
       stream << it.key() << nRows << nCols;

       double* data = &it.value()->data()[0];

       // write raw matrix data
       stream.writeRawData((const char*)data, nRows*nCols*sizeof(double));
     }

    return stream;
  }


  /**
   * Reads matrix from binary disk file pointed to by QDataStream stream
   */
  QDataStream &operator>>(QDataStream &stream, SparseBlockColumnMatrix &sbcm) {
    int nBlocks, nBlockNumber, nRows, nCols;
    int i, r, c;

    stream >> nBlocks;

    for ( i = 0; i < nBlocks; i++) {
      // read block number (key); rows (size1); and columns (size2)
      stream >> nBlockNumber >> nRows >> nCols;

      double data[nRows*nCols];

      // read raw matrix data
      stream.readRawData((char*)data, nRows*nCols*sizeof(double));

      // insert matrix at correct key
      sbcm.InsertMatrixBlock(nBlockNumber, nRows, nCols);

      // get matrix
      matrix<double>* matrix = sbcm[nBlockNumber];

      // fill with data
      for (r = 0; r < nRows; r++ ) {
        for (c = 0; c < nCols; c++ ) {
          int nLocation = r*nRows + c;
          (*matrix)(r,c) = data[nLocation];
        }
      }
    }

    return stream;
  }

  /**
   * Writes matrix to QDebug stream (dbg)
   */
  QDebug operator<<(QDebug dbg, const SparseBlockColumnMatrix &sbcm) {
    dbg.space() << "New Block" << endl;

    QMapIterator<int, matrix<double>*> it(sbcm);
     while ( it.hasNext() ) {
       it.next();

       if( !it.value() )
         continue;

       // get matrix
       matrix<double>* matrix = it.value();

       // matrix rows, columns
       int nRows = matrix->size1();
       int nCols = matrix->size2();

       dbg.nospace() << qSetFieldWidth(4);
       dbg.nospace() << qSetRealNumberPrecision(8);

       for (int r = 0; r < nRows; r++ ) {
         for (int c = 0; c < nCols; c++ ) {
             dbg.space() << (*matrix)(r,c);
         }
         dbg.space() << endl;
       }
       dbg.space() << endl;
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
   * Deletes all pointer elements and removes them from the map.
   * Effectively, a destructor, and in fact, called by the
   * ~SparseBlockColumnMatrix above.
   */
  void SparseBlockRowMatrix::wipe() {
    qDeleteAll(values());
    clear();
  }


  /**
   * Copy constructor. Calls method immediately below.
   */
  SparseBlockRowMatrix::SparseBlockRowMatrix(const SparseBlockRowMatrix& src) {
    copy(src);
  }


  /**
   * Copy method.
   */
  void SparseBlockRowMatrix::copy(const SparseBlockRowMatrix& src) {
    // handi-wipe
    wipe();

    // copy matrix blocks from src
    QMapIterator<int, matrix<double>*> it(src);
     while (it.hasNext()) {
         it.next();

         // copy matrix block from src
         matrix<double>* m = new matrix<double>(*(it.value()));

         // insert matrix into map
         this->insert(it.key(),m);
     }
  }


  /**
   * "Equals" operator.
   */
  SparseBlockRowMatrix&
      SparseBlockRowMatrix::operator=(const SparseBlockRowMatrix& src) {
    if ( this == &src )
      return *this;

    copy(src);

    return *this;
  }


  /**
   * Inserts a "newed" boost matrix<double>* of size (nRows, nCols) into the
   * map with the block row number as key. The matrix::clear call initializes
   * the matrix elements to zero.
   * If an entry exists at the key nRowBlock, no insertion is made.
   *
   * @param nRowBlock block row number of inserted matrix (key into map)
   * @param nRows number of rows in matrix to be inserted
   * @param nCols number of columns in matrix to be inserted
   */
  bool SparseBlockRowMatrix::InsertMatrixBlock(int nRowBlock, int nRows,
                                                  int nCols) {
    if ( this->contains(nRowBlock) )
      return false;

    matrix<double>* m = new matrix<double>(nRows,nCols);

    if ( !m )
      return false;

    m->clear();

    this->insert(nRowBlock,m);

    return true;
  }


  /**
   * Returns total number of matrix elements in map (NOTE: NOT the number of
   * matrix blocks). The sum of all the elements of all the matrix blocks.
   */
  int SparseBlockRowMatrix::numberOfElements() {
    int nElements = 0;

    QMapIterator<int, matrix<double>*> it(*this);
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
   */
  void SparseBlockRowMatrix::print(std::ostream& outstream) {
    if ( size() == 0 ) {
      outstream << "Empty SparseBlockRowMatrix..." << std::endl;
      return;
    }

    outstream << "Printing SparseBlockRowMatrix..." << std::endl;
    QMapIterator<int, matrix<double>*> it(*this);
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
   * Sets all elements of all matrix blocks to zero.
   */
  void SparseBlockRowMatrix::zeroBlocks() {
    QMapIterator<int, matrix<double>*> it(*this);
    while ( it.hasNext() ) {
      it.next();
      it.value()->clear();
    }
  }


  /**
   * Copies a SparseBlockRowMatrix to a Boost compressed_matrix
   * This may be a temporary implementation
   */
  void SparseBlockRowMatrix::copyToBoost(compressed_matrix<double>& B) {
    B.clear();

    int ncols, nstart, nend, nrowBlock;
    range rRow = range(0,3);
    range rCol;

    QMapIterator<int, matrix<double>*> it(*this);
    while ( it.hasNext() ) {
      it.next();

      nrowBlock = it.key();
      matrix<double>* m = it.value();

      ncols = m->size2();

      nstart = nrowBlock*ncols;
      nend = nstart + ncols;

      rCol = range(nstart,nend);

      matrix_range<compressed_matrix<double> > m1 (B, rRow, rCol);

      m1 = *m;
    }
  }

  /**
   * Writes matrix to binary disk file pointed to by QDataStream stream
   */
  QDataStream &operator<<(QDataStream &stream, const SparseBlockRowMatrix &sbrm) {
    // write number of blocks in this column
    int nBlocks = sbrm.size();
    stream << nBlocks;

    QMapIterator<int, matrix<double>*> it(sbrm);
     while ( it.hasNext() ) {
       it.next();

       if( !it.value() )
         continue;

       int nRows = it.value()->size1();
       int nCols = it.value()->size2();

       // write block number (key); rows (size1); and columns (size2)
       stream << it.key() << nRows << nCols;

       double* data = &it.value()->data()[0];

       // write raw matrix data
       stream.writeRawData((const char*)data, nRows*nCols*sizeof(double));
     }

    return stream;
  }


  /**
   * Reads matrix from binary disk file pointed to by QDataStream stream
   */
  QDataStream &operator>>(QDataStream &stream, SparseBlockRowMatrix &sbrm) {
    int nBlocks, nBlockNumber, nRows, nCols;
    int i, r, c;

    stream >> nBlocks;

    for ( i = 0; i < nBlocks; i++) {
      // read block number (key); rows (size1); and columns (size2)
      stream >> nBlockNumber >> nRows >> nCols;

      double data[nRows*nCols];

      // read raw matrix data
      stream.readRawData((char*)data, nRows*nCols*sizeof(double));

      // insert matrix at correct key
      sbrm.InsertMatrixBlock(nBlockNumber, nRows, nCols);

      // get matrix
      matrix<double>* matrix = sbrm[nBlockNumber];

      // fill with data
      for (r = 0; r < nRows; r++ ) {
        for (c = 0; c < nCols; c++ ) {
          int nLocation = r*nRows + c;
          (*matrix)(r,c) = data[nLocation];
        }
      }
    }

    return stream;
  }

  /**
   * Writes matrix to QDebug stream (dbg)
   */
  QDebug operator<<(QDebug dbg, const SparseBlockRowMatrix &sbrm) {
    dbg.space() << "New Block" << endl;

    QMapIterator<int, matrix<double>*> it(sbrm);
     while ( it.hasNext() ) {
       it.next();

       if( !it.value() )
         continue;

       // get matrix
       matrix<double>* matrix = it.value();

       // matrix rows, columns
       int nRows = matrix->size1();
       int nCols = matrix->size2();

       dbg.nospace() << qSetFieldWidth(4);
       dbg.nospace() << qSetRealNumberPrecision(8);

       for (int r = 0; r < nRows; r++ ) {
         for (int c = 0; c < nCols; c++ ) {
             dbg.space() << (*matrix)(r,c);
         }
         dbg.space() << endl;
       }
       dbg.space() << endl;
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
   * Deletes all pointer elements and removes them from the list.
   * Effectively, a destructor, and in fact, called by the
   * ~SparseBlockMatrix above.
   */
  void SparseBlockMatrix::wipe() {
    qDeleteAll(*this);
    clear();
  }


  /**
   * Copy constructor. Calls copy method immediately below.
   */
  SparseBlockMatrix::SparseBlockMatrix(const SparseBlockMatrix& src) {
    copy(src);
  }

  /**
   * Copy method.
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
   */
  bool SparseBlockMatrix::setNumberOfColumns( int n ) {

    for( int i = 0; i < n; i++ )
      append( new SparseBlockColumnMatrix() );

    return true;
  }


  /**
   * Inserts a "newed" boost matrix<double>* of size (nRows, nCols) into the
   * matrix at nColumnBlock, nRowBlock. The inserted matrix elements are
   * initialized to zero.
   * If an entry exists at nColumnBlock, RowBlock, no insertion is made.
   *
   * @param nColumnBlock block column number of inserted matrix (QList index)
   * @param nRowBlock block row number of inserted matrix (key into map)
   * @param nRows number of rows in matrix to be inserted
   * @param nCols number of columns in matrix to be inserted
   */
  bool SparseBlockMatrix::InsertMatrixBlock(int nColumnBlock, int nRowBlock,
                                            int nRows, int nCols) {
    return (*this)[nColumnBlock]->InsertMatrixBlock(nRowBlock, nRows, nCols);
  }


  /**
   * Returns total number of blocks in matrix.
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
   * Returns number of diagonal matrix blocks (equivalent to size - there is one
   * per column).
   */
  int SparseBlockMatrix::numberOfDiagonalBlocks() {
    int ndiagBlocks = 0;

    for( int i = 0; i < size(); i++ ) {
      SparseBlockColumnMatrix* column = at(i);

      if ( !column )
        continue;

      QMapIterator<int, matrix<double>*> it(*column);
      while (it.hasNext()) {
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
   */
  int SparseBlockMatrix::numberOfOffDiagonalBlocks() {
    return (numberOfBlocks() - numberOfDiagonalBlocks());
  }


  /**
   * Returns number of matrix elements in matrix.
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
   * Returns pointer to boost matrix at (column, row).
   *
   * @param column block column number
   * @param row block row number
   */
  matrix<double>* SparseBlockMatrix::getBlock(int column, int row) {
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
   * Writes matrix to binary disk file pointed to by QDataStream stream
   */
  QDataStream &operator<<(QDataStream &stream, const SparseBlockMatrix &sparseBlockMatrix) {
    int nBlockColumns = sparseBlockMatrix.size();

    stream << nBlockColumns;

    for (int i =0; i < nBlockColumns; i++)
      stream << *sparseBlockMatrix.at(i);

    return stream;
  }


  /**
   * Reads matrix from binary disk file pointed to by QDataStream stream
   */
  QDataStream &operator>>(QDataStream &stream, SparseBlockMatrix &sparseBlockMatrix) {
    int nBlockColumns;

    // read and set number of block columns
    stream >> nBlockColumns;
    sparseBlockMatrix.setNumberOfColumns(nBlockColumns);

    for (int i =0; i < nBlockColumns; i++)
      stream >> *sparseBlockMatrix.at(i);

    return stream;
  }

  /**
   * Writes matrix to QDebug stream (dbg)
   */
  QDebug operator<<(QDebug dbg, const SparseBlockMatrix &m) {
    int nBlockColumns = m.size();

    for (int i =0; i < nBlockColumns; i++)
      dbg << *m.at(i);

    return dbg;
  }
}
