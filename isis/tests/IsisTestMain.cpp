#include <gtest/gtest.h>

#include <cstdlib>

#include <QTemporaryDir>

#include "Preference.h"

using namespace Isis;

int main(int argc, char **argv) {
   Isis::Preference::Preferences(true);

   // Keep SpiceQL from generating a new cache directory per test process
   QTemporaryDir spiceqlCache;
   if (spiceqlCache.isValid() && getenv("SPICEQL_CACHE_DIR") == NULL) {
     setenv("SPICEQL_CACHE_DIR", spiceqlCache.path().toLatin1().data(), 1);
   }

   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
