'''
Script to create truthData for tests.

It expects the command in the form of:
    python makeTruth.py testName
where testname is the cmake name for the unit or app test

The unit tests are pretty trivial because
the executable outputs the results of the unitTest to stdout and stderr so we
just redirect the streams to the file named object.truth where object is the
ISIS object being tested
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
                makefilePath = line.split("\" \"")[1][11:]
                break
    print(makefilePath)

    makefilePath = makefilePath.split("/")
    del makefilePath[-1]
    makefilePath = "/".join(makefilePath)
    print(makefilePath)

    isissrc = makefilePath.split("src")[0]
    print(isissrc)
    os.chdir(makefilePath)
    os.system("make checkout")
    os.system("make output")
    os.system("make truthdata")
    os.system("rm -rf " + builddir + "/testOutputDir/truth")
    os.system("cp -r truth " + builddir + "/testOutputDir")

    if len(sys.argv) == 3:
        if sys.argv[2] == "truth":
            os.system("make checkin")
            print("Checked In Truth Data")

    # doing this instead of make release because make release
    # can give feedback to the user that we would rather avoid
    os.system("rm -rf input output truth print.prt")
