#include "CorrelationMatrix.h"

#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QString>
#include <QStringList>
#include <QtCore/qmath.h>

#include "IException.h"
#include "Pvl.h"
#include "PvlObject.h"
#include "SparseBlockMatrix.h"

namespace Isis {
  /**
   * Default Constructor
   */
  CorrelationMatrix::CorrelationMatrix() {
    m_imagesAndParameters = NULL;
    m_visibleImagesAndParameters = NULL; // TODO: unused variable ???
    m_covarianceFileName = NULL; // new FileName("/work/users/koyama/testData/covarianceMatrix2.dat");
    m_correlationFileName = NULL; // new FileName("/work/users/koyama/testData/correlationMatrix.dat");
    m_diagonals = NULL;
//     m_visibleElements = new QList<MatrixElement*>;
    m_visibleBlocks = NULL;
    QStringList param;
    param << "X" << "Y" << "Z" << "RA" << "DEC" << "TWI";

    m_imagesAndParameters = new QMap<QString, QStringList>;
    QString img = "Sub4-AS15-M-0583_msk.cub";
    m_imagesAndParameters->insert(img, param);
    img = "Sub4-AS15-M-0584_msk.cub";
    m_imagesAndParameters->insert(img, param);
    img = "Sub4-AS15-M-0585_msk.cub";
    m_imagesAndParameters->insert(img, param);
    img = "Sub4-AS15-M-0586_msk.cub";
    m_imagesAndParameters->insert(img, param);
    img = "Sub4-AS15-M-0587_msk.cub";
    m_imagesAndParameters->insert(img, param);
    img = "Sub4-AS15-M-1423.cub";
    m_imagesAndParameters->insert(img, param);
    img = "Sub4-AS15-M-1537.cub";
    m_imagesAndParameters->insert(img, param);

    m_diagonals = new QList<double>;

/*    if (m_correlationFileName == NULL) {
      computeCorrelationMatrix(); // Just for the first time.. Once it's created we don't need to do this.
    }*/    
  }



  /**
   * This constructor will create a CorrelationMatrix object given a pvl.
   *
   * Object = CorrelationMatrix
   *
   *    CovarianceFileName = fileName.dat
   *    CorrelationFileName = fileName.dat
   *
   *    Group = ImagesAndParameters
   *       Image1 = "Parameter1", "Parameter2", "..."
   *       Image2 = "Parameter1", "Parameter2", "..."
   *       Image3 = "Parameter1", "Parameter2", "..."
   *    End_Group
   *
   * End_Object
   *
   */
  CorrelationMatrix::CorrelationMatrix(PvlObject storedMatrixData) {
    m_imagesAndParameters = NULL;
    m_visibleImagesAndParameters = NULL; // TODO: unused variable ???
    m_covarianceFileName = NULL;
    m_correlationFileName = NULL;
    m_diagonals = NULL;
    m_visibleBlocks = NULL;

    if (storedMatrixData.name() != "CorrelationMatrixData") {
      QString msg = "This Pvl Object does not have the correct correlation information. The Object "
                    "you are looking for is called CorrelationMatrixData";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    try {
      m_covarianceFileName = new FileName(storedMatrixData.
                                              findKeyword("CovarianceMatrixFileName")[0]);
    }
    catch (IException &e) {
      QString msg = "Could not find the Covariance Matrix .dat file name.";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
    
    try {
      QString corrFileName = storedMatrixData.findKeyword("CorrelationMatrixFileName")[0];
      if(corrFileName == "NULL") {
        m_correlationFileName = NULL;
      }
      else {
        m_correlationFileName= new FileName(corrFileName);
      }
    }
    catch (IException &e) {
      QString msg = "Could not find the Correlation Matrix .dat file name.";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }

    try {
      PvlObject::PvlKeywordIterator imgsIt = storedMatrixData.findGroup("ImagesAndParameters").begin();
      while ( imgsIt != storedMatrixData.findGroup("ImagesAndParameters").end() ) {
        QStringList params = (*imgsIt)[0].split(",");
        m_imagesAndParameters->insert(imgsIt->name(), params);
      }
    }
    catch (IException &e) {
      QString msg = "Could not get Images and Parameters from ImagesAndParameters group.";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
  }



  /**
   * Copy Constructor
   *
   * @param other The CorrelationMatrix to copy
   */
  CorrelationMatrix::CorrelationMatrix(const CorrelationMatrix &other) {
    m_imagesAndParameters = new QMap<QString, QStringList>(*other.m_imagesAndParameters);
    m_visibleImagesAndParameters
        = new QMap<QString, QStringList>(*other.m_visibleImagesAndParameters); // TODO: unused variable ???
    m_covarianceFileName = new FileName(*other.m_covarianceFileName);
    m_correlationFileName = new FileName(*other.m_correlationFileName);
    m_diagonals = new QList<double>(*other.m_diagonals);
    m_visibleBlocks = new QList<SparseBlockColumnMatrix>(*other.m_visibleBlocks);
  }



  /**
   * Destructor
   */
  CorrelationMatrix::~CorrelationMatrix() {

    // TODO: Why does this cause seg fault + abort of unitTest??? 
    // valgrind:
    // ==13332== Invalid read of size 4
    // ==13332==    at 0x5265CE8: Isis::CorrelationMatrix::~CorrelationMatrix() (in /scratch/ipce/isis/src/control/objs/CorrelationMatrix/libisis3.4.7.so)
    // ==13332==    by 0x409300: main (unitTest.cpp:157)
    // ==13332==  Address 0x101e8458 is 104 bytes inside a block of size 128 free'd
    // ==13332==    at 0x4A073CC: operator delete(void*) (vg_replace_malloc.c:480)
    // ==13332==    by 0x40B4F3: QMap<QString, QStringList>::freeData(QMapData*) (qmap.h:655)
    // ==13332==    by 0x40A74D: QMap<QString, QStringList>::~QMap() (in /scratch/ipce/isis/src/control/objs/CorrelationMatrix/unitTest)
    // ==13332==    by 0x4092A6: main (unitTest.cpp:157)
    // ==13332==
    // ==13332== Invalid free() / delete / delete[] / realloc()
    // ==13332==    at 0x4A073CC: operator delete(void*) (vg_replace_malloc.c:480)
    // ==13332==    by 0x5265D06: Isis::CorrelationMatrix::~CorrelationMatrix() (in /scratch/ipce/isis/src/control/objs/CorrelationMatrix/libisis3.4.7.so)
    // ==13332==    by 0x409300: main (unitTest.cpp:157)
    // ==13332==  Address 0x7feffef00 is on thread 1's stack
    // 
//   delete m_imagesAndParameters;
//   m_imagesAndParameters = NULL;

    delete m_visibleImagesAndParameters; // TODO: unused variable ???
    m_visibleImagesAndParameters = NULL; // TODO: unused variable ???

    delete m_covarianceFileName;
    m_covarianceFileName = NULL;

    delete m_correlationFileName;
    m_correlationFileName = NULL;

    delete m_diagonals;
    m_diagonals = NULL;

    delete m_visibleBlocks;
    m_visibleBlocks = NULL;

  }



  /**
   * Equal Operator
   *
   * Should this call the copy constructor???
   *
   * @param other The matrix to assign to this matrix.
   * @return CorrelationMatrix This new matrix.
   */
  CorrelationMatrix &CorrelationMatrix::operator=(const CorrelationMatrix &other) {

    if (&other != this) {

      delete m_imagesAndParameters;
      m_imagesAndParameters = new QMap<QString, QStringList>(*other.m_imagesAndParameters);
  
      delete m_visibleImagesAndParameters; // TODO: unused variable ???
      m_visibleImagesAndParameters
          = new QMap<QString, QStringList>(*other.m_visibleImagesAndParameters); // TODO: unused variable ???
  
      delete m_covarianceFileName;
      m_covarianceFileName = new FileName(*other.m_covarianceFileName);
  
      delete m_correlationFileName;
      m_correlationFileName = new FileName(*other.m_correlationFileName);
  
      delete m_diagonals;
      m_diagonals = new QList<double>(*other.m_diagonals);
  
      delete m_visibleBlocks;
      m_visibleBlocks = new QList<SparseBlockColumnMatrix>(*other.m_visibleBlocks);

    }

    return *this;
  }



  /**
  * This mehtod reads the covariance matrix in from a file, one SparseBlockColumnMatrix at a time.
  * It then stores the diagonal values from that column and computes the correlation values. The
  * resulting SparseBlockMatrix is written to a new file, one SparseBlockColumnMatrix at a time.
  */
  void CorrelationMatrix::computeCorrelationMatrix() {

    if ( !isValid() ) {
      QString msg = "Cannot compute correlation matrix without a specified file name. Use "
                    "setCorrelationFileName(FileName) before calling computeCorrelationMatrix().";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    m_visibleBlocks = new QList<SparseBlockColumnMatrix>;

    // Create file handle
    QFile matrixInput( m_covarianceFileName->expanded() );
    QFile matrixOutput( m_correlationFileName->expanded() );

    // Open file to write to
    matrixInput.open(QIODevice::ReadOnly);
    matrixOutput.open(QIODevice::WriteOnly);

    // Open Stream
    QDataStream inStream(&matrixInput);
    QDataStream outStream(&matrixOutput);

    double firstParam1 = 0, //starting param for each iteration
            firstParam2 = 0;
    double param1 = 0, // current param for each iteration
            param2 = 0;
    // Read one column at a time
    SparseBlockColumnMatrix sbcm;
    while( !inStream.atEnd() ) {
      inStream >> sbcm;

      // Store diagonal
      int numOfBlocks = sbcm.size();
      int lastBlock = numOfBlocks - 1;
      int numDiagonals = sbcm[lastBlock]->size1();

      // Get Diagonals
      for (int i = 0; i < numDiagonals; i++) {
        double val = ( *(sbcm[lastBlock]) )(i, i);
        m_diagonals->append(val);
      }

      // compute correlations
      QMapIterator<int, boost::numeric::ublas::matrix<double>*> block(sbcm);

      while ( block.hasNext() ) { // each block in the column
        block.next();
        for (int row = 0; row < (int)block.value()->size1(); row++) { // each row in the block
          for (int column = 0; column < (int)block.value()->size2(); column++) { // each column 
            // correlation = covariance / (variance1 * variance2)
            ( *block.value() )(row, column) = ( *block.value() )(row, column) /
                                              sqrt( (*m_diagonals)[param1] *
                                                    (*m_diagonals)[param2] );
            param2++;
          }
          param1++;
          param2 = firstParam2;
        }
        firstParam1 += block.value()->size1();
        param1 = firstParam1;
      }
      firstParam1 = 0;  // start each column at first element of diagonal list
      param1 = firstParam1;
      firstParam2 += block.value()->size2();
      param2 = firstParam2;

      outStream << sbcm;
      m_visibleBlocks->append(sbcm);
    }
    
    // close file
    matrixInput.close();
    matrixOutput.close();
  }



  /**
   * This method will open the correlation matrix file and read in the blocks that apply to
   *   the requested area. It will populate m_visibleElements.
   *
   * @param x fisrt coordinate of the location in the matrix that the user wants to see.
   * @param y second coordinate of the location in the matrix that the user wants to see.
   */
  void CorrelationMatrix::retrieveVisibleElements(int x, int y) {

//     if ( !correlationMatrixExists() ) {
      // call computeCorrelationMatrix
//     }
    // read the values we want from the correlation matrix file.

    // store values by column?
    // return list of values in m_visibleElements
  }



  /**
   * This is the public accessor for the list of elements that should be displayed in the current
   *   view.
   *
   * @return QList<MatrixElement*> The list of currently visible elements.
   *
   */
//   QList<MatrixElement*> CorrelationMatrix::visibleElements() {
//     return *m_visibleElements;
//   }



  /**
   * See if the correlation matrix has already been calculated by checking to see if
   *   the correlation matrix file has been created.
   *
   * @return true if the correlation matrix has already been set.
   */
  bool CorrelationMatrix::isValid() {
    return !(m_correlationFileName == NULL || m_covarianceFileName == NULL); 
  }



  // Set Methods
  /**
   *Set the qmap of images and parameters.
   *
   * @param
   */
  void CorrelationMatrix::setCorrelationFileName(FileName correlationFileName) {
    if (m_correlationFileName == NULL) {
      m_correlationFileName = new FileName(correlationFileName);
    }
    else {
      m_correlationFileName = &correlationFileName;
    }
  }



  /**
   *Set the qmap of images and parameters.
   *
   * @param
   */
  void CorrelationMatrix::setCovarianceFileName(FileName covarianceFileName) {
    if (m_covarianceFileName == NULL) {
      m_covarianceFileName = new FileName(covarianceFileName);
    }
    else {
      m_covarianceFileName = &covarianceFileName;
    }
  }



  /**
   * Set the qmap of images and parameters.
   *
   * @param
   */
  void CorrelationMatrix::setImagesAndParameters(QMap<QString, QStringList> imagesAndParameters) {
    m_imagesAndParameters = &imagesAndParameters;
  }



  /**
   * Public access for the correlation matrix file name.
   *
   * @return
   */
  FileName CorrelationMatrix::correlationFileName() {
    return *m_correlationFileName;
  }



  /**
   * Public access for the covariance matrix file name.
   *
   * @return
   */
  FileName CorrelationMatrix::covarianceFileName() {
    return *m_covarianceFileName;
  }



  /**
   * Public access for the qmap of images and parameters.
   *
   * @return
   */
  QMap<QString, QStringList> *CorrelationMatrix::imagesAndParameters() {
    return m_imagesAndParameters;
  }



  /**
   * This method will read the matrix in from the file and hold on to the whole thing.
   * This will only be used when the matrix is small enough that this will be useful.
   *
   */
  void CorrelationMatrix::getWholeMatrix() {
//     SparseBlockColumnMatrix sbcm;
//     QFile matrixInput( m_correlationFileName->expanded() );
//     matrixInput.open(QIODevice::ReadOnly);
//     QDataStream inStream(&matrixInput);
//
//     while( !inStream.atEnd() ) {
//       inStream >> sbcm;
//       m_visibleBlocks->append(&sbcm);
//     }
  }



  /**
   * This method will be used when the matrix is too big to display the whole thing.
   * It will read in the block we want to see and the two blocks for the diagonals that belong to
   * the right images.
   */
  void CorrelationMatrix::getThreeVisibleBlocks() {
  }



  /**
   * Get the visible part of the matrix.
   */
  QList<SparseBlockColumnMatrix> *CorrelationMatrix::visibleBlocks() {
    return m_visibleBlocks;
  }



  /**
   * This method creates a pvl group with the information necessary to recreate this correlation
   * matrix.
   *
   * Object = CorrelationMatrixData
   *   CovarianceMatrixFileName = /location/covarianceTmpFileName.dat
   *   CorrelationMatrixFileName = /location/correlationTmpFileName.dat
   * 
   *   Group = ImagesAndParameters
   *     Image1Name = "Param1, Param2, ..., ParamN"
   *     ...
   *     ImageNName = "..."
   *   End_Group
   * End_Object
   * 
   * @return PvlGroup The info needed to recreate this correlation matrix.
   */
  PvlObject CorrelationMatrix::pvlObject() {
    PvlObject corrMatInfo("CorrelationMatrixData");
    
    corrMatInfo += PvlKeyword( "CovarianceMatrixFileName", m_covarianceFileName->expanded() );
    corrMatInfo += PvlKeyword( "CorrelationMatrixFileName", m_correlationFileName->expanded() );

    PvlGroup imgsAndParams("ImagesAndParameters");
    QMapIterator<QString, QStringList> imgParamIt(*m_imagesAndParameters);
    while ( imgParamIt.hasNext() ) {
      imgParamIt.next();
      imgsAndParams += PvlKeyword( imgParamIt.key(), imgParamIt.value().join(",") );
    }
    corrMatInfo += imgsAndParams;
    
    return corrMatInfo;
  }



  QDataStream &CorrelationMatrix::write(QDataStream &stream) const {
    // QMaps
    stream << *m_imagesAndParameters << *m_visibleImagesAndParameters;
    // FileNames
    stream << m_covarianceFileName->expanded() << m_correlationFileName->expanded();
    // QLists
    stream << *m_diagonals << *m_visibleBlocks;
    return stream;
  }



  QDataStream &CorrelationMatrix::read(QDataStream &stream) {
    // QMaps
    QMap<QString, QStringList> imagesAndParameters;
    stream >> imagesAndParameters;
    delete m_imagesAndParameters;
    m_imagesAndParameters = new QMap<QString, QStringList>(imagesAndParameters);

    QMap<QString, QStringList> visibleImagesAndParameters;
    stream >> visibleImagesAndParameters;
    delete m_visibleImagesAndParameters;
    m_visibleImagesAndParameters = new QMap<QString, QStringList>(visibleImagesAndParameters);

    // FileNames
    QString covarianceFileName;
    stream >> covarianceFileName;
    delete m_covarianceFileName;
    m_covarianceFileName  = new FileName(covarianceFileName);

    QString correlationFileName;
    stream >>correlationFileName;
    delete m_correlationFileName;
    m_correlationFileName = new FileName(correlationFileName);

    // QLists
    QList<double> diagonals;
    stream >> diagonals;
    delete m_diagonals;
    m_diagonals = new QList<double>(diagonals);

    QList<SparseBlockColumnMatrix> visibleBlocks;
    stream << visibleBlocks;
    delete m_visibleBlocks;
    m_visibleBlocks = new QList<SparseBlockColumnMatrix>(visibleBlocks);

    return stream;
  }



  QDataStream &operator<<(QDataStream &stream, const CorrelationMatrix &matrix) {
    return matrix.write(stream);
  }



  QDataStream &operator>>(QDataStream &stream, CorrelationMatrix &matrix) {
    return matrix.read(stream);
  }
}
