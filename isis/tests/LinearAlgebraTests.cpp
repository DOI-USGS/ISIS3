#include "LinearAlgebra.h"
#include "IException.h"

#include <gtest/gtest.h>

using namespace Isis;

TEST(LinearAlgebraTest, PsuedoinverseInvertible) {
  LinearAlgebra::Matrix mat(2,2);
  mat(0,0) = 1.0;
  mat(0,1) = 2.0;
  mat(1,0) = 3.0;
  mat(1,1) = 4.0;

  LinearAlgebra::Matrix inverse = LinearAlgebra::psuedoinverse(mat);

  EXPECT_TRUE(LinearAlgebra::isIdentity(LinearAlgebra::multiply(inverse, mat)));
  EXPECT_TRUE(LinearAlgebra::isIdentity(LinearAlgebra::multiply(mat, inverse)));
}

TEST(LinearAlgebraTest, PsuedoinverseRightInverse) {
  LinearAlgebra::Matrix mat(2,3);
  mat(0,0) = 1.0;
  mat(0,1) = 2.0;
  mat(0,2) = 3.0;
  mat(1,0) = 4.0;
  mat(1,1) = 5.0;
  mat(1,2) = 6.0;

  LinearAlgebra::Matrix inverse = LinearAlgebra::psuedoinverse(mat);

  EXPECT_TRUE(LinearAlgebra::isIdentity(LinearAlgebra::multiply(mat, inverse)));
}

TEST(LinearAlgebraTest, PsuedoinverseLeftInverse) {
  LinearAlgebra::Matrix mat(3,2);
  mat(0,0) = 1.0;
  mat(0,1) = 2.0;
  mat(1,0) = 3.0;
  mat(1,1) = 4.0;
  mat(2,0) = 5.0;
  mat(2,1) = 6.0;

  LinearAlgebra::Matrix inverse = LinearAlgebra::psuedoinverse(mat);

  EXPECT_TRUE(LinearAlgebra::isIdentity(LinearAlgebra::multiply(inverse, mat)));
}
