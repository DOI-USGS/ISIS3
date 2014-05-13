#include "CorrelationMatrix.h"

// #include <QDataStream>
// #include <QFile>
// #include <QList>
// #include <QString>
// #include <QStringList>
//
// #include "boost/numeric/ublas/matrix_sparse.hpp"
// #include "boost/numeric/ublas/vector_proxy.hpp"
// #include "boost/lexical_cast.hpp"
//
// #include "boost/numeric/ublas/io.hpp"
//
// #include "SparseBlockMatrix.h"
//
// namespace Isis {
//   /**
//   * This mehtod reads the covariance matrix in from a file, one SparseBlockColumnMatrix at a time.
//   * It then stores the diagonal values from that column and computes the correlation values. The
//   * values are stored in a SparseBlockMatrix which is then converted to a QList< QList<double> > so
//   * only the upper triangle is stored.
//   *
//   * @param fileName the name of the covariance matrix file.
//   */
//   void CorrelationMatrix::getCorrelationMatrix(QString fileName) {
//     // if the correlation file exists, read it
//     // else, start with covariance file.
//
//     SparseBlockMatrix sbm;
//     QList<double> diagonals;
//     // read in from file
//     // Create file handle
//     QFile matrixInput("covarianceMatrix.dat");
//     // Open file to write to
//     matrixInput.open(QIODevice::ReadOnly);
//     QDataStream inStream(&matrixInput);
//     // Read one column at a time
//     SparseBlockColumnMatrix sbcm;
//     while( !inStream.atEnd() ) {
//       inStream >> sbcm;
//
//       // Store diagonal
//       int numOfBlocks = sbcm.size();
//       int lastBlock = numOfBlocks - 1;
//       int numDiagonals = sbcm[lastBlock].size1();
//
//       // Get Diagonals
//       for (int i = 0; i < numDiagonals; i++) {
//         diagonals.append(sbcm[lastBlock][i][i]);
//       }
//
//       // compute correlations
//       QMapIterator<int, matrix<double>*> it(sbcm);
//
//       while ( it.hasNext() ) { // each block in the column
//         it.next();
//         // row = (key - 1) * rowsInBlock + current row
//         for (int j = 0; j < it.value().size1(); j++) { // each row in the block
//           for (int k = 0; k < it.value().size2(); k++) { // each column in the block
//             if (j != k) {
//               // correlation = covariance / (variance1 * variance2)
//               it.value()[j][k]= it.value()[j][k] / (diagonals[j] * diagonals[k]);
//             }
//           }
//         }
//       }
//       sbm.append(sbcm);
//       // call addToUpperTriangle
//   //     addToUpperTriangle(sbcm);
//     }
//     // close stream
//     // close file
//   }
//
//
//   /**
//   * This method should belong to SBM...
//   *
//   * This method will return the value of the correlation matrix at a position (x, y).
//   *
//   * @param x
//   * @param y
//   *
//   * @return double
//   */
// //   double CorrelationMatrix::at(int x, int y) {
// //     // determine which column in sbm
// //     int column =
// //     // determine which block in column
// //     int block =
// //     // determine where in block
// //
// //     return ( (*(*this)[column])[block] )[m][n];
// //   }
//
//   /**
//   *
//   *
//   * @param sbcm The column to add to the upper triangle
//   */
//   // void addToUpperTriangle(SparseBlockColumnMatrix sbcm) {
//   //   QList<double> row;
//   //   (*(*this)[column])[row];
//   // }
// }