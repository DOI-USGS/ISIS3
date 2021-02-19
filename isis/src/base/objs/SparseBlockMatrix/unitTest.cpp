/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

// std lib
#include <iostream>

// boost lib
#include <boost/numeric/ublas/matrix.hpp>

// Isis lib
#include "IException.h"
#include "Preference.h"
#include "SparseBlockMatrix.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cerr.precision(6);

  cerr << "----- Testing SparseBlockColumnMatrix -----" << endl << endl;

  try {
    cerr << "----- default constructor" << endl;
    SparseBlockColumnMatrix spcm;
    cerr << "  # matrix blocks: " << spcm.size() << endl;
    cerr << "# matrix elements: " << spcm.numberOfElements() << endl;
    spcm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- Insert boost block matrix of zeros" << endl;
    SparseBlockColumnMatrix spcm;
    spcm.insertMatrixBlock(0, 3, 3);
    cerr << "  # of matrix blocks: " << spcm.size() << endl;
    cerr << "# of matrix elements: " << spcm.numberOfElements() << endl;
    spcm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- Insert boost block matrix of zeros and set values" << endl;
    SparseBlockColumnMatrix spcm;
    spcm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(spcm[0]))(i,j) = 3*i+j;
      }
    }
    cerr << "  # of matrix blocks = " << spcm.size() << endl;
    cerr << "# of matrix elements = " << spcm.numberOfElements() << endl;
    spcm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- copy method" << endl;
    SparseBlockColumnMatrix spcm;
    spcm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(spcm[0]))(i,j) = 3*i+j;
      }
    }
    SparseBlockColumnMatrix clone;
    clone.copy(spcm);
    cerr << "  # of matrix blocks = " << clone.size() << endl;
    cerr << "# of matrix elements = " << clone.numberOfElements() << endl;
    clone.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- copy constructor" << endl;
    SparseBlockColumnMatrix spcm;
    spcm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(spcm[0]))(i,j) = 3*i+j;
      }
    }
    SparseBlockColumnMatrix clone(spcm);
    cerr << "  # of matrix blocks = " << clone.size() << endl;
    cerr << "# of matrix elements = " << clone.numberOfElements() << endl;
    clone.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- = operator" << endl;
    SparseBlockColumnMatrix spcm;
    spcm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(spcm[0]))(i,j) = 3*i+j;
      }
    }
    SparseBlockColumnMatrix clone = spcm;
    cerr << "  # of matrix blocks = " << clone.size() << endl;
    cerr << "# of matrix elements = " << clone.numberOfElements() << endl;
    clone.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }
  try {
    cerr << "----- zero blocks" << endl;
    SparseBlockColumnMatrix spcm;
    spcm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(spcm[0]))(i,j) = 3*i+j;
      }
    }
    spcm.zeroBlocks();
    cerr << "  # of matrix blocks = " << spcm.size() << endl;
    cerr << "# of matrix elements = " << spcm.numberOfElements() << endl;
    spcm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- wipe" << endl;
    SparseBlockColumnMatrix spcm;
    spcm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(spcm[0]))(i,j) = 3*i+j;
      }
    }
    spcm.wipe();
    cerr << "  # of matrix blocks = " << spcm.size() << endl;
    cerr << "# of matrix elements = " << spcm.numberOfElements() << endl;
    spcm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  cerr << endl << "----- Testing SparseBlockRowMatrix -----" << endl << endl;

  try {
    cerr << "----- default constructor" << endl;
    SparseBlockRowMatrix sprm;
    cerr << "  # matrix blocks: " << sprm.size() << endl;
    cerr << "# matrix elements: " << sprm.numberOfElements() << endl;
    sprm.print(std::cerr);    
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- Insert boost block matrix of zeros" << endl;
    SparseBlockRowMatrix sprm;
    sprm.insertMatrixBlock(0, 3, 3);
    cerr << "  # of matrix blocks: " << sprm.size() << endl;
    cerr << "# of matrix elements: " << sprm.numberOfElements() << endl;
    sprm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- Insert boost block matrix of zeros and set values" << endl;
    SparseBlockRowMatrix sprm;
    sprm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(sprm[0]))(i,j) = 3*i+j;
      }
    }
    cerr << "  # of matrix blocks = " << sprm.size() << endl;
    cerr << "# of matrix elements = " << sprm.numberOfElements() << endl;
    sprm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- copy method" << endl;
    SparseBlockRowMatrix sprm;
    sprm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(sprm[0]))(i,j) = 3*i+j;
      }
    }
    SparseBlockRowMatrix clone;
    clone.copy(sprm);
    cerr << "  # of matrix blocks = " << clone.size() << endl;
    cerr << "# of matrix elements = " << clone.numberOfElements() << endl;
    clone.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- copy constructor" << endl;
    SparseBlockRowMatrix sprm;
    sprm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(sprm[0]))(i,j) = 3*i+j;
      }
    }
    SparseBlockRowMatrix clone(sprm);
    cerr << "  # of matrix blocks = " << clone.size() << endl;
    cerr << "# of matrix elements = " << clone.numberOfElements() << endl;
    clone.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- = operator" << endl;
    SparseBlockRowMatrix spcm;
    spcm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(spcm[0]))(i,j) = 3*i+j;
      }
    }
    SparseBlockRowMatrix clone = spcm;
    cerr << "  # of matrix blocks = " << clone.size() << endl;
    cerr << "# of matrix elements = " << clone.numberOfElements() << endl;
    clone.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }
  try {
    cerr << "----- zero blocks" << endl;
    SparseBlockRowMatrix spcm;
    spcm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(spcm[0]))(i,j) = 3*i+j;
      }
    }
    spcm.zeroBlocks();
    cerr << "  # of matrix blocks = " << spcm.size() << endl;
    cerr << "# of matrix elements = " << spcm.numberOfElements() << endl;
    spcm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "----- wipe" << endl;
    SparseBlockRowMatrix spcm;
    spcm.insertMatrixBlock(0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(spcm[0]))(i,j) = 3*i+j;
      }
    }
    spcm.wipe();
    cerr << "  # of matrix blocks = " << spcm.size() << endl;
    cerr << "# of matrix elements = " << spcm.numberOfElements() << endl;
    spcm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  cerr << endl << "----- Testing SparseBlockMatrix -----" << endl << endl;

  try {
    cerr << "----- default constructor" << endl;
    SparseBlockMatrix sbm;
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "           # matrix elements: " << sbm.numberOfElements() << endl;
    sbm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << endl << "----- set number of columns" << endl;
    SparseBlockMatrix sbm;
    sbm.setNumberOfColumns(3);
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "           # matrix elements: " << sbm.numberOfElements() << endl;
    sbm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }
  
  try {
    cerr << endl << "----- Insert boost block matrix of zeros" << endl;
    SparseBlockMatrix sbm;
    sbm.setNumberOfColumns(3);
    sbm.insertMatrixBlock(1, 1, 3, 3);
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "           # matrix elements: " << sbm.numberOfElements() << endl;
    sbm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }  

  try {
    cerr << endl << "----- Insert boost block matrix of zeros and set values" << endl;
    SparseBlockMatrix sbm;
    sbm.setNumberOfColumns(3);
    sbm.insertMatrixBlock(1, 1, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[1])[1])(i,j) = 3*i+j;
      }
    }
    
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "           # matrix elements: " << sbm.numberOfElements() << endl;
    sbm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }
  
try {
    cerr << endl << "----- Insert boost block matrix of zeros in each column, set values, and startColumn" << endl;
    SparseBlockMatrix sbm;
    sbm.setNumberOfColumns(3);
    sbm.insertMatrixBlock(0, 0, 3, 3);
    sbm.insertMatrixBlock(1, 2, 3, 3);
    sbm.insertMatrixBlock(2, 3, 3, 3);
    
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[0])[0])(i,j) = 3*i+j;
      }
    }
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[1])[2])(i,j) = 3*i+j;
      }
    }
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[2])[3])(i,j) = 3*i+j;
      }
    }
    
    sbm.at(0)->setStartColumn(0);
    sbm.at(1)->setStartColumn(3);
    sbm.at(2)->setStartColumn(6);
    
    cerr << "             # block columns: " << sbm.size() << endl;
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "         block 0 startColumn: " << sbm.at(0)->startColumn() << endl;
    cerr << "         block 1 startColumn: " << sbm.at(1)->startColumn() << endl;
    cerr << "         block 2 startColumn: " << sbm.at(2)->startColumn() << endl;
    sbm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }  

  try {
    cerr << endl << "----- copy method" << endl;
    SparseBlockMatrix sbm;
    sbm.setNumberOfColumns(3);
    sbm.insertMatrixBlock(1, 1, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[1])[1])(i,j) = 3*i+j;
      }
    }
    SparseBlockMatrix clone;
    clone.copy(sbm);
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "           # matrix elements: " << sbm.numberOfElements() << endl;
    clone.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << endl << "----- copy constructor" << endl;
    SparseBlockMatrix sbm;
    sbm.setNumberOfColumns(3);
    sbm.insertMatrixBlock(1, 1, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[1])[1])(i,j) = 3*i+j;
      }
    }
    SparseBlockMatrix clone(sbm);
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "           # matrix elements: " << sbm.numberOfElements() << endl;
    clone.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << endl << "----- = operator" << endl;
    SparseBlockMatrix sbm;
    sbm.setNumberOfColumns(3);
    sbm.insertMatrixBlock(1, 1, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[1])[1])(i,j) = 3*i+j;
      }
    }
    SparseBlockMatrix clone = sbm;
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "           # matrix elements: " << sbm.numberOfElements() << endl;
    clone.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << endl << "----- zero blocks" << endl;
    SparseBlockMatrix sbm;
    sbm.setNumberOfColumns(3);
    sbm.insertMatrixBlock(1, 1, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[1])[1])(i,j) = 3*i+j;
      }
    }
    sbm.zeroBlocks();
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "           # matrix elements: " << sbm.numberOfElements() << endl;
    sbm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << endl << "----- wipe" << endl;
    SparseBlockMatrix sbm;
    sbm.setNumberOfColumns(3);
    sbm.insertMatrixBlock(1, 1, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[1])[1])(i,j) = 3*i+j;
      }
    }
    sbm.wipe();
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "           # matrix elements: " << sbm.numberOfElements() << endl;
    sbm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << endl << "----- diagonal and off-diagonal blocks" << endl;
    SparseBlockMatrix sbm;
    sbm.setNumberOfColumns(3);
    sbm.insertMatrixBlock(1, 1, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[1])[1])(i,j) = 3*i+j;
      }
    }
    sbm.insertMatrixBlock(2, 0, 3, 3);
    for ( int i = 0; i < 3; i++ ) {
      for ( int j = 0; j < 3; j++ ) {
        (*(*sbm[2])[0])(i,j) = 3*i+j+9;
      }
    }
    cerr << "             # matrix blocks: " << sbm.numberOfBlocks() << endl;
    cerr << "    # diagonal matrix blocks: " << sbm.numberOfDiagonalBlocks() << endl;
    cerr << "# off-diagonal matrix blocks: " << sbm.numberOfOffDiagonalBlocks() << endl;
    cerr << "           # matrix elements: " << sbm.numberOfElements() << endl;
    cerr << "# leading columns for block2: " << sbm.getLeadingColumnsForBlock(2) << endl;
    cerr << "   # leading rows for block1: " << sbm.getLeadingRowsForBlock(1) << endl;
    sbm.print(std::cerr);
  }
  catch(IException &e) {
    e.print();
  }

/*
  cerr << endl << "----- Testing Operators -----" << endl << endl;
  cerr << endl << "----- Testing Error Checking -----" << endl << endl;
*/
}
