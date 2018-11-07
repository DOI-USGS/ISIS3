#include "gtest/gtest.h"

#include "Matrix.h"
#include "IException.h"


TEST (MatrixTests, ConstructorWithBadArgs) {
  try {
    Isis::Matrix A(0, 2);
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Invalid matrix dimensions");
  }
  catch(...) {
    FAIL() << "Expected error message: **PROGRAMMER ERROR** Invalid matrix dimensions"
  }
  
  try {
    Isis::Matrix A(-1, 2);
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Invalid matrix dimensions");
  }
  catch(...) {
    FAIL() << "Expected error message: **PROGRAMMER ERROR** Invalid matrix dimensions"
  }
}


TEST (MatrixTests, IdentityConstructorWithBadArgs) {
  try {
    Isis::Matrix::Identity(0);
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Invalid matrix dimensions");
  }
  catch(...) {
    FAIL() << "Expected error message: **PROGRAMMER ERROR** Invalid matrix dimensions"
  }
  
  try {
    Isis::Matrix::Identity(-1);
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Invalid matrix dimensions");
  }
  catch(...) {
    FAIL() << "Expected error message: **PROGRAMMER ERROR** Invalid matrix dimensions"
  }
}


TEST (MatrixTests, DeterminantWithBadArgs) {
  Isis::Matrix A(2, 1);
  
  try {
    A.determinant();
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Unable to calculate the determinant, the matrix is not square.");
  }
  catch(...) {
    FAIL() << "Expected error message: **PROGRAMMER ERROR** Unable to calculate the determinant, the matrix is not square."
  }
}


TEST (MatrixTests, TraceWithBadArgs) {
  Isis::Matrix A(2, 1);
  
  try {
    A.Trace();
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Unable to calculate the trace, the matrix is not square.");
  }
  catch(...) {
    FAIL() << "Expected error message: **PROGRAMMER ERROR** Unable to calculate the trace, the matrix is not square."
  }
}


TEST (MatrixTests, AddMatricesWithBadArgs) {
  Isis::Matrix A(2, 1);
  Isis::Matrix B(1, 2);
  
  try {
    A + B;
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Incompatible matrix dimensions, cannot add the matrices.");
  }
  catch(...) {
    FAIL() << "Expected error message: **PROGRAMMER ERROR** Incompatible matrix dimensions, cannot add the matrices."
  }
}


TEST (MatrixTests, SubtractMatricesWithBadArgs) {
  Isis::Matrix A(2, 1);
  Isis::Matrix B(1, 2);
  
  try {
    A - B;
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Incompatible matrix dimensions, cannot subtract the matrices.");
  }
  catch(...) {
    FAIL() << "Expected error message: **PROGRAMMER ERROR** Incompatible matrix dimensions, cannot subtract the matrices."
  }
}


TEST (MatrixTests, MultiplyMatrixWithBadArgs) {
  Isis::Matrix A(2, 1);
  Isis::Matrix B(1, 2);
  
  try {
    A * B;
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Incompatible matrix dimensions, cannot multiply the matrices.");
  }
  catch(...) {
    FAIL() << "Expected error message: **PROGRAMMER ERROR** Incompatible matrix dimensions, cannot multiply the matrices."
  }
}


TEST (MatrixTests, MultiplyMatrixElementwiseWithBadArgs) {
  Isis::Matrix A(2, 1);
  Isis::Matrix B(1, 2);
  
  try {
    A.MultiplyElementWise(B);
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Incompatible matrix dimensions, cannot multiply the matrices.");
  }
  catch(...) {
    FAIL() << "Expected error message: **PROGRAMMER ERROR** Incompatible matrix dimensions, cannot multiply the matrices."
  }
}







TEST (MatrixTests, Constructor) {
  Isis::Matrix A(2, 2, 2);
  ASSERT_EQ(2, A[0][0]);
  ASSERT_EQ(2, A[0][1]);
  ASSERT_EQ(2, A[1][0]);
  ASSERT_EQ(2, A[1][1]);
}

TEST (MatrixTests, SetValues) {
  Isis::Matrix A(2, 2);
  
  A[0][0] = 1;
  A[0][1] = 2;
  A[1][0] = 3;
  A[1][1] = 4;
  
  ASSERT_EQ(1, A[0][0]);
  ASSERT_EQ(2, A[0][1]);
  ASSERT_EQ(3, A[1][0]);
  ASSERT_EQ(4, A[1][1]);
}

TEST (MatrixTests, IdentityMatrix) {
  Isis::Matrix I = Isis::Matrix::Identity(2);
  
  ASSERT_EQ(1, I[0][0]);
  ASSERT_EQ(0, I[0][1]);
  ASSERT_EQ(0, I[1][0]);
  ASSERT_EQ(1, I[1][1]);
}

TEST (MatrixTests, PlusOperator) {
  Isis::Matrix I = Isis::Matrix::Identity(2);
  Isis::Matrix A(2, 2, 2);
  Isis::Matrix API = A + I;
  
  ASSERT_EQ(3, API[0][0]);
  ASSERT_EQ(2, API[0][1]);
  ASSERT_EQ(2, API[1][0]);
  ASSERT_EQ(3, API[1][1]);  
}

TEST (MatrixTests, MinusOperator) {
  Isis::Matrix I = Isis::Matrix::Identity(2);
  Isis::Matrix A(2, 2, 2);
  Isis::Matrix AMI = A - I;
  
  ASSERT_EQ(1, AMI[0][0]);
  ASSERT_EQ(2, AMI[0][1]);
  ASSERT_EQ(2, AMI[1][0]);
  ASSERT_EQ(1, AMI[1][1]);  
}

TEST (MatrixTests, MultiplyByScalarOperator) {
  Isis::Matrix A(2, 2, 2);
  Isis::Matrix AMSI = A * 2;
  
  ASSERT_EQ(4, AMSI[0][0]);
  ASSERT_EQ(4, AMSI[0][1]);
  ASSERT_EQ(4, AMSI[1][0]);
  ASSERT_EQ(4, AMSI[1][1]);  
}

TEST (MatrixTests, MultiplyByMatrixOperator) {
  Isis::Matrix I = Isis::Matrix::Identity(2);
  Isis::Matrix A(2, 2, 2);
  Isis::Matrix AMMI = A * I;
  
  EXPECT_EQ(2, AMMI[0][0]);
  EXPECT_EQ(0, AMMI[0][1]);
  EXPECT_EQ(0, AMMI[1][0]);
  EXPECT_EQ(2, AMMI[1][1]);  
}
