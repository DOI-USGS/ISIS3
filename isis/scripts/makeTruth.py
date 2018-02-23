'''
Script to create truthData for unitTests. This might be more work
using the scripts then just documenting what you have to do to save
output as Truth
'''

import sys
import os

if not os.environ['ISISROOT']:
    print("The $ISISROOT variable is not set")


elif "_unit_" in sys.argv[1]:
    unitTestExecutable = sys.argv[1]

    unitTestName = unitTestExecutable.split("_test_")[1] + ".truth"
    # we should probably append the path to the front so it ends up in
    # in the same directory as the test
    unitTestPath = unitTestExecutable.split("/")
    del unitTestPath[-1]
    unitTestPath = "/".join(unitTestPath)

    os.system(unitTestExecutable + ">&" + unitTestPath + "/" + unitTestName)

else:
    builddir = os.environ['ISISROOT']
    apptest = sys.argv[1]
    makefilePath = ""
    with open(builddir + "/objects/CTestTestfile.cmake") as testFile:
        for line in testFile:
            if apptest in line:
                makefilePath = line.split(" ")[1][11:]
                break
    print(makefilePath)
