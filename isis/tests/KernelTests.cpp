#include <QString>
#include <QStringList>

#include "Kernel.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

TEST(KernelTests, DefaultConstructor) {
  Kernel defaultKernel;
  EXPECT_EQ(defaultKernel.kernels(), QStringList());
  EXPECT_EQ(defaultKernel.type(), Kernel::None);
}

TEST(KernelTests, Constructor) {
  QStringList kernelList = {"test1", "test2", "test3"};
  Kernel Kernels(Kernel::Predicted, kernelList);
  EXPECT_EQ(Kernels.kernels(), kernelList);
  EXPECT_EQ(Kernels.type(), Kernel::Predicted);
}

TEST(KernelTests, Mutators) {
  QStringList kernelList = {"test1", "test2", "test3"};
  Kernel Kernels;
  Kernels.setKernels(kernelList);
  Kernels.setType(Kernel::Nadir);
  EXPECT_EQ(Kernels.kernels(), kernelList);
  EXPECT_EQ(Kernels.type(), Kernel::Nadir);
}

TEST(KernelTests, Append) {
  QStringList kernelList = {"test1", "test2", "test3"};
  Kernel Kernels;
  for (QString kernel : kernelList) {
    Kernels.push_back(kernel);
  }
  EXPECT_EQ(Kernels.kernels(), kernelList);
}

TEST(KernelTests, Indices) {
  QStringList kernelList = {"test1", "test2", "test3"};
  Kernel Kernels(Kernel::Predicted, kernelList);
  ASSERT_EQ(Kernels.size(), kernelList.size());
  for (int i = 0; i < kernelList.size(); i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, Kernels[i], kernelList[i]);
  }
}

TEST(KernelTests, Comparison) {
  QStringList kernelList;
  Kernel none(Kernel::None, kernelList);
  Kernel predict(Kernel::Predicted, kernelList);
  Kernel nadir(Kernel::Nadir, kernelList);
  Kernel reconstructed(Kernel::Reconstructed, kernelList);
  Kernel smithed(Kernel::Smithed, kernelList);

  EXPECT_FALSE(none < none);
  EXPECT_TRUE( none < predict);
  EXPECT_TRUE( none < nadir);
  EXPECT_TRUE( none < reconstructed);
  EXPECT_TRUE( none < smithed);

  EXPECT_FALSE(predict < none);
  EXPECT_FALSE(predict < predict);
  EXPECT_TRUE( predict < nadir);
  EXPECT_TRUE( predict < reconstructed);
  EXPECT_TRUE( predict < smithed);

  EXPECT_FALSE(nadir < none);
  EXPECT_FALSE(nadir < predict);
  EXPECT_FALSE(nadir < nadir);
  EXPECT_TRUE( nadir < reconstructed);
  EXPECT_TRUE( nadir < smithed);

  EXPECT_FALSE(reconstructed < none);
  EXPECT_FALSE(reconstructed < predict);
  EXPECT_FALSE(reconstructed < nadir);
  EXPECT_FALSE(reconstructed < reconstructed);
  EXPECT_TRUE( reconstructed < smithed);

  EXPECT_FALSE(smithed < none);
  EXPECT_FALSE(smithed < predict);
  EXPECT_FALSE(smithed < nadir);
  EXPECT_FALSE(smithed < reconstructed);
  EXPECT_FALSE(smithed < smithed);
}

TEST(KernelTests, TypeStrings) {
  EXPECT_EQ(Kernel::typeEnum(Kernel::typeEnum(Kernel::None)), Kernel::None);
  EXPECT_EQ(Kernel::typeEnum(Kernel::typeEnum(Kernel::Predicted)), Kernel::Predicted);
  EXPECT_EQ(Kernel::typeEnum(Kernel::typeEnum(Kernel::Nadir)), Kernel::Nadir);
  EXPECT_EQ(Kernel::typeEnum(Kernel::typeEnum(Kernel::Reconstructed)), Kernel::Reconstructed);
  EXPECT_EQ(Kernel::typeEnum(Kernel::typeEnum(Kernel::Smithed)), Kernel::Smithed);
}

TEST(KernelTests, TypeComposition) {
  Kernel::Type compositeType = Kernel::Predicted | Kernel::Nadir;
  EXPECT_EQ(static_cast<int>(compositeType),
            static_cast<int>(Kernel::Predicted) | static_cast<int>(Kernel::Nadir));
}
