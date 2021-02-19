#include <memory>

#include "BundleControlPoint.h"
#include "BundleMeasure.h"
#include "BundleSettings.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "IException.h"
#include "SpecialPixel.h"

#include "gmock/gmock.h"

using namespace std;

// To be used for basic setup of a BundleMeasure test object. (Fixture Function)
class BundleMeasure_CreateObj : public ::testing::Test {

  protected:

    // Declare unique pointers to expose them when using the fixture.
    unique_ptr<ControlPoint> controlPoint;
    unique_ptr<ControlMeasure> controlMeasure;
    unique_ptr<BundleMeasure> testBundleMeasurePtr;
    unique_ptr<BundleControlPoint> testBundleControlPointPtr;


    void SetUp() override {

      // Create a ControlPoint object using the default constructor
      // to feed its pointer into the BundleControlPoint constructor.
      controlPoint = unique_ptr<ControlPoint>( new ControlPoint() );

      // Create a ControlMeasure object using the default constructor
      // to feed its pointer into the BundleMeasure constructor.
      controlMeasure = unique_ptr<ControlMeasure>( new ControlMeasure() );

      // Create a BundleSettingsQsp to feed it to
      // the BundleMeasure constructor.
      BundleSettingsQsp bundleSettingsQsp( new BundleSettings );

      // The pointers are then linked to newly-created BundleMeasure and
      // BundleControlPoint objects.
      testBundleControlPointPtr = unique_ptr<BundleControlPoint>( new BundleControlPoint( bundleSettingsQsp,
                                                                                     controlPoint.get() ) );

      testBundleMeasurePtr = unique_ptr<BundleMeasure>( new BundleMeasure( controlMeasure.get(),
                                                                           testBundleControlPointPtr.get() ) );

    }
};

TEST_F( BundleMeasure_CreateObj, Constructor ) {
  EXPECT_FALSE( testBundleMeasurePtr->isRejected() );

  EXPECT_EQ( NULL, testBundleMeasurePtr->camera() );

  // May need to check for Not Null rather than checking the BCP pointer.
  EXPECT_EQ( testBundleControlPointPtr.get(), testBundleMeasurePtr->parentControlPoint() );

  // Without the use of their respective setters, these QSharedPointers will be pointed to NULL
  EXPECT_TRUE( testBundleMeasurePtr->parentBundleImage().isNull() );
  EXPECT_TRUE( testBundleMeasurePtr->parentBundleObservation().isNull() );
  EXPECT_THROW( testBundleMeasurePtr->observationSolveSettings(), IException );

  // Uses ISIS' Null value.
  EXPECT_EQ( Null, testBundleMeasurePtr->sample() );
  EXPECT_EQ( Null, testBundleMeasurePtr->sampleResidual() );
  EXPECT_EQ( Null, testBundleMeasurePtr->line() );
  EXPECT_EQ( Null, testBundleMeasurePtr->lineResidual() );
  EXPECT_EQ( Null, testBundleMeasurePtr->residualMagnitude() );
  EXPECT_TRUE( testBundleMeasurePtr->cubeSerialNumber().isNull() );
  EXPECT_EQ( Null, testBundleMeasurePtr->focalPlaneComputedX() );
  EXPECT_EQ( Null, testBundleMeasurePtr->focalPlaneComputedY() );
  EXPECT_EQ( Null, testBundleMeasurePtr->focalPlaneMeasuredX() );
  EXPECT_EQ( Null, testBundleMeasurePtr->focalPlaneMeasuredY() );
  EXPECT_THROW( testBundleMeasurePtr->observationIndex(), IException );
}

TEST_F( BundleMeasure_CreateObj, CopyConstructor ) {

  // First, check if the lineResidual is being initialized to ISIS Null.
  EXPECT_EQ( testBundleMeasurePtr->lineResidual(), Null );

  // The interior ControlMeasure object's Line Residual will be used to confirm
  // that the BundleMeasure was copied properly.
  controlMeasure->SetResidual( 1.0, 1.0 );

  // Confirm that the Line Residual was set to 1.0
  EXPECT_EQ( testBundleMeasurePtr->lineResidual(), 1.0 );

  // The copy constructor is then used on the object associated with
  // testBundleMeasurePtr.
  BundleMeasure copiedBundleMeasure( *testBundleMeasurePtr );

  // Confirm that this is a copy of the testBundleMeasure by checking the
  // isRejected value.
  EXPECT_EQ( copiedBundleMeasure.lineResidual(), 1.0 );

  // Set the IsRejected boolean to true for further testing.
  // (The default value is false)
  copiedBundleMeasure.setRejected( true );

  // Both are expected to be true, since BundleMeasure's copy constructor
  // is a shallow copy, meaning that it copies the pointers to the internal
  // objects
  EXPECT_TRUE( copiedBundleMeasure.isRejected() );
  EXPECT_TRUE( testBundleMeasurePtr->isRejected() );
}

TEST_F( BundleMeasure_CreateObj, AssignmentOperator ) {

  // First, check if the lineResidual is being initialized to ISIS Null.
  EXPECT_EQ( testBundleMeasurePtr->lineResidual(), Null );

  // The interior ControlMeasure object's Line Residual will be used to confirm
  // that the BundleMeasure was assigned properly.
  controlMeasure->SetResidual( 1.0, 1.0 );

  // Confirm that the Line Residual was set to 1.0
  EXPECT_EQ( testBundleMeasurePtr->lineResidual(), 1.0 );

  // The assignment operator is then used on the testBundleMeasure object.
  BundleMeasure assignedBundleMeasure = *testBundleMeasurePtr;

  // Confirm that the assignment of testBundleMeasure worked by checking the
  // isRejected boolean.
  EXPECT_EQ( assignedBundleMeasure.lineResidual(), 1.0 );

  // Set the IsRejected boolean to true for further testing.
  // (The default value is false)
  assignedBundleMeasure.setRejected( true );

  // Finally, check if the isRejected boolean was changed for both references.
  EXPECT_TRUE( assignedBundleMeasure.isRejected() );
  EXPECT_TRUE( testBundleMeasurePtr->isRejected() );

}

TEST_F( BundleMeasure_CreateObj, IsRejected ) {
  EXPECT_EQ( false, testBundleMeasurePtr->isRejected() );
  testBundleMeasurePtr->setRejected( true );
  EXPECT_EQ( true, testBundleMeasurePtr->isRejected() );
  testBundleMeasurePtr->setRejected( false );
  EXPECT_EQ( false, testBundleMeasurePtr->isRejected() );

}
