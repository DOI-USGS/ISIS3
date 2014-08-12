#include <iostream>

#include <boost/numeric/ublas/symmetric.hpp>

#include <QDebug>
#include <QFile>

#include "CorrelationMatrix.h"
#include "IException.h"
#include "PvlObject.h"
#include "Pvl.h"
#include "SparseBlockMatrix.h"

using namespace std;
using namespace boost::numeric::ublas;
using namespace Isis;

int main() {
  
  qDebug() << "Test Default Constructor";
  
  CorrelationMatrix corrMat;

  qDebug() << "Test Copy Constructor";

  try {
    CorrelationMatrix corrMat2(corrMat);
  }
  catch(IException &e) {
    throw IException(IException::Unknown,
                     "Failed to create object with copy constructor",
                     _FILEINFO_);
  }
  
  qDebug() << "Test Equal Operator ( = )";
  
  try {
    CorrelationMatrix corrMat3 = corrMat;
  }
  catch(IException &e) {
    throw IException(IException::Unknown,
                     "Failed to copy object using equal operator",
                     _FILEINFO_);
  }

  qDebug() << "Does the matrix exist?" << corrMat.isValid();

  qDebug() << "Creating Covariance Matrix.";
  FileName corrFile = "corrMat.dat";
  FileName covFile = "covMat.dat";

  corrMat.setCorrelationFileName(corrFile);
  corrMat.setCovarianceFileName(covFile);

  // Make Dummy Covariance Matrix
  SparseBlockMatrix tmpMat;
  tmpMat.setNumberOfColumns(2);
  tmpMat.InsertMatrixBlock(0, 0, 2, 2);
  tmpMat.InsertMatrixBlock(1, 0, 2, 2);
  tmpMat.InsertMatrixBlock(1, 1, 2, 2);
  int initVal = 1;
  for (int sbcm = 0; sbcm < 2; sbcm++) {
    for (int block = 0; block < tmpMat[sbcm]->size(); block++) {
      for (int row = 0; row < 2; row++) {
        for (int column = 0; column < 2; column++) {
          (*(*tmpMat[sbcm])[block])(row, column) = initVal;
          initVal++;
        }
      }
    }
  }

  qDebug() << "Covariance Matrix:";
  qDebug() << tmpMat;

  QFile covQFile( covFile.expanded() );
  covQFile.open(QIODevice::ReadWrite);
  QDataStream dataStream(&covQFile);

  foreach (SparseBlockColumnMatrix *col, tmpMat) {
    dataStream << *col;
  }
  covQFile.close();

  qDebug() << "Testing Compute Correlation Matrix Method.";
  corrMat.computeCorrelationMatrix();

  qDebug() << "Correlation Matrix:";
  foreach( SparseBlockColumnMatrix sbcm, *corrMat.visibleBlocks() ) {
    qDebug() << sbcm;
  }

  qDebug() << "Does the matrix exist?" << corrMat.isValid();
 
  QMap<QString, QStringList> imgsAndParams;

  QStringList param;
  param << "X" << "Y" ;

  QString img = "Image1";
  imgsAndParams.insert(img, param);
  img = "Image2";
  imgsAndParams.insert(img, param);

  corrMat.setImagesAndParameters(imgsAndParams);

  qDebug();
  qDebug() << "***Correlation Matrix Information***";
  qDebug();
  qDebug() << "Covariance File Name:" << corrMat.covarianceFileName().expanded();
  qDebug() << "Correlation File Name:" << corrMat.correlationFileName().expanded();
  qDebug() << "Images and Parameters:";
  QMapIterator<QString, QStringList> imgIt( *corrMat.imagesAndParameters() );
  while ( imgIt.hasNext() ) {
    imgIt.next();
    qDebug() << "\t" << imgIt.key();
    foreach ( QString param, imgIt.value() ) {
      qDebug() << "\t\t" << param;
    }
  }

//   qDebug() << "Testing pvlObject()";
// 
//   PvlObject corrMatObject( corrMat.pvlObject() );
//   Pvl tester;
//   tester += corrMatObject;
//   cout << tester;
// 
//   qDebug() << "Testing constructor that takes a pvlObject";
// 
//   try {
//     CorrelationMatrix corrMat4(corrMatObject);
// 
//     qDebug();
//     qDebug() << "***Correlation Matrix Information***";
//     qDebug();
//     qDebug() << "Covariance File Name:" << corrMat4.correlationFileName().expanded();
//     qDebug() << "Correlation File Name:" << corrMat4.correlationFileName().expanded();
//     qDebug() << "Images and Parameters:";
//   //   QMapIterator<QString, QStringList> imgIt( *corrMat4.imagesAndParameters() );
//     imgIt.toFront();
//     while ( imgIt.hasNext() ) {
//       imgIt.next();
//       qDebug() << "\t" << imgIt.key();
//       foreach ( QString param, imgIt.value() ) {
//         qDebug() << "\t\t" << param;
//       }
//     }
//   }
//   catch(IException &e) {
//     QString msg = "Failed to create object using CorrelationMatrix(PvlObject).";
//     throw(e, IException::Programmer, msg, _FILEINFO_);
//   }
    
  //delete mat files
  covQFile.remove();
  QFile(corrMat.correlationFileName().expanded()).remove();
  
}
