#include "BundleControlPoint.h"
#include "BundleMeasure.h"
#include "BundleSettings.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "IException.h"
#include "SpecialPixel.h"

#include "gmock/gmock.h"

// To be used for basic setup of a BundleMeasure test object. (Fixture Function)
class BundleMeasure_CreateObj : public ::testing::Test
{

  protected:
    // Create a ControlPoint object using the default constructor
    // to feed its pointer into the BundleControlPoint constructor.
    ControlPoint controlPoint;

    // Create a ControlMeasure object using the default constructor
    // to feed its pointer into the BundleMeasure constructor.
    ControlMeasure controlMeasure;

    // Since BundleMeasure and BundleControlPoint do not have
    // default constructors, and C++ does not allow for object declarations
    // pointers to each respective object are used.
    BundleMeasure *testBundleMeasurePtr;
    BundleControlPoint *testBundleControlPointPtr;


    void SetUp() override
    {
      // BundleControlPoint testBundleControlPoint;
      // Create a BundleSettingsQsp to feed it to
      // the BundleMeasure constructor.
      BundleSettingsQsp bundleSettingsQsp( new BundleSettings );

      // Create a BundleControlPoint object to feed its pointer into
      // the BundleMeasure constructor.
      BundleControlPoint bundleControlPoint( bundleSettingsQsp, &controlPoint );

      // Finally, the SetUp function creates the BundleMeasure object
      // from all of the objects made previously.
      // std::cout << &controlMeasure << '\n';
      BundleMeasure bundleMeasure( &controlMeasure, &bundleControlPoint );

      // The pointers are then linked to the newly-created BundleMeasure and
      // BundleControlPoint objects.
      testBundleMeasurePtr = &bundleMeasure;
      testBundleControlPointPtr = &bundleControlPoint;

    }

};

TEST_F( BundleMeasure_CreateObj, test1 ) {
  testBundleMeasurePtr->setRejected(true);
  EXPECT_TRUE(controlPoint.IsRejected());
}

// TEST_F( BundleMeasure_CreateObj, Constructor )
// {
//   EXPECT_FALSE( testBundleMeasurePtr->isRejected() );
//
//   EXPECT_EQ( NULL, testBundleMeasurePtr->camera() );
//
//   // May need to check for Not Null rather than checking the BCP pointer.
//   EXPECT_EQ( testBundleControlPointPtr, testBundleMeasurePtr->parentControlPoint() );
//
//   // Without the use of their respective setters, these QSharedPointers will be pointed to NULL
//   EXPECT_TRUE( testBundleMeasurePtr->parentBundleImage().isNull() );
//   EXPECT_TRUE( testBundleMeasurePtr->parentBundleObservation().isNull() );
//   EXPECT_THROW( testBundleMeasurePtr->observationSolveSettings(), IException );
//
//   // Uses ISIS' Null value.
//   EXPECT_EQ( Null, testBundleMeasurePtr->sample() );
//   EXPECT_EQ( Null, testBundleMeasurePtr->sampleResidual() );
//   EXPECT_EQ( Null, testBundleMeasurePtr->line() );
//   EXPECT_EQ( Null, testBundleMeasurePtr->lineResidual() );
//   EXPECT_EQ( Null, testBundleMeasurePtr->residualMagnitude() );
//   EXPECT_TRUE( testBundleMeasurePtr->cubeSerialNumber().isNull() );
//   EXPECT_EQ( Null, testBundleMeasurePtr->focalPlaneComputedX() );
//   EXPECT_EQ( Null, testBundleMeasurePtr->focalPlaneComputedY() );
//   EXPECT_EQ( Null, testBundleMeasurePtr->focalPlaneMeasuredX() );
//   EXPECT_EQ( Null, testBundleMeasurePtr->focalPlaneMeasuredY() );
//   EXPECT_THROW( testBundleMeasurePtr->observationIndex(), IException );
// }
//
//
// TEST_F( BundleMeasure_CreateObj, CopyConstructor )
// {
//   BundleMeasure testBundleMeasure = *testBundleMeasurePtr;
//
//   // Using the isRejected Boolean to confirm that the testBundleMeasure
//   // was copied correctly.
//   // Note: The default value of isRejected is false.
//   testBundleMeasure.setRejected( true );
//
//   // The copy constructor is then used on the testBundleMeasure object.
//   BundleMeasure copiedBundleMeasure = BundleMeasure( testBundleMeasure );
//
//   // Confirm that this is a copy of the testBundleMeasure by checking the
//   // isRejected value.
//   EXPECT_TRUE( copiedBundleMeasure.isRejected() );
//
//   // Confirm that the copied BundleMeasure is not simply pointing to
//   // the testBundleMeasure using the isRejected value again.
//
//   copiedBundleMeasure.setRejected( false );
//
//   EXPECT_FALSE( copiedBundleMeasure.isRejected() );
//   EXPECT_TRUE( testBundleMeasure.isRejected() );
// }
//
// TEST_F( BundleMeasure_CreateObj, CopyConstructor2 )
// {
//   // Using the isRejected Boolean to confirm that the testBundleMeasure
//   // was copied correctly.
//   // Note: The default value of isRejected is false.
//   std::cout << "Setting Reject" << '\n';
//   testBundleMeasurePtr->setRejected( true );
//   std::cout << "Reject Set" << '\n';
//
//   // The copy constructor is then used on the testBundleMeasure object.
//   BundleMeasure copiedBundleMeasure( *testBundleMeasurePtr );
//
//   // Confirm that this is a copy of the testBundleMeasure by checking the
//   // isRejected value.
//   EXPECT_TRUE( copiedBundleMeasure.isRejected() );
//
//   // Confirm that the copied BundleMeasure is not simply pointing to
//   // the testBundleMeasure using the isRejected value again.
//
//   copiedBundleMeasure.setRejected( false );
//
//   EXPECT_FALSE( copiedBundleMeasure.isRejected() );
//   EXPECT_TRUE( testBundleMeasurePtr->isRejected() );
// }
//
// TEST_F( BundleMeasure_CreateObj, AssignmentOperator )
// {
//   // Using the isRejected Boolean to confirm that the testBundleMeasure
//   // was assigned correctly.
//   // Note: The default value of isRejected is false.
//   //testBundleMeasurePtr->setRejected( true );
//   BundleMeasure assignedBundleMeasure = *testBundleMeasurePtr;
//
//   // Using the isRejected Boolean to confirm that the testBundleMeasure
//   // was assigned correctly.
//   // Note: The default value of isRejected is false.
//   //assignedBundleMeasure.setRejected( true );
//
//
//
//
// }
//
// TEST_F( BundleMeasure_CreateObj, IsRejected )
// {
//   EXPECT_EQ( false, testBundleMeasurePtr->isRejected() );
//   testBundleMeasurePtr->setRejected( true );
//   EXPECT_EQ( true, testBundleMeasurePtr->isRejected() );
//   testBundleMeasurePtr->setRejected( false );
//   EXPECT_EQ( false, testBundleMeasurePtr->isRejected() );
//
// }
