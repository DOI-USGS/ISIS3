'''
Script to create truthData for tests.

For output it expects the command in the form of:
    python makeOutput.py testName
where testname is the cmake name for the unit or app test

To check in truth data the command should be in the form of:
    python makeOutput.py testName truth

The unit tests are pretty trivial because the executable outputs the results of
the unitTest to stdout and stderr so we just redirect the streams to the file \
named object.truth where object is the ISIS object being tested.

The app tests output has to rely on the old make system because the app test do
'''

import sys
import os


if not os.environ['ISISROOT']:
    print("The $ISISROOT variable is not set")
    sys.exit()

builddir = os.environ['ISISROOT']

if "_unit_" in sys.argv[1]:
    unitTestExecutable = sys.argv[1]

    unitTestName = unitTestExecutable.split("_test_")[1] + ".truth"
    # we should probably append the path to the front so it ends up in
    # in the same directory as the test
    unitTestPath = unitTestExecutable.split("/")
    del unitTestPath[-1]
    unitTestPath = "/".join(unitTestPath)

    os.system(unitTestExecutable + ">&" + unitTestPath + "/" + unitTestName)
    print("Unit Test Output In " + unitTestPath + " As " + unitTestName)

    if len(sys.argv) == 3:
        if sys.argv[2] == "truth":
            with open(builddir + "/objects/CTestTestfile.cmake") as testFile:
                for line in testFile:
                    if unitTestName in line:
                        unitTestSrcPath = line.split("\" \"")[2][13:]
                        os.system("cp -f " + unitTestPath + "/" + unitTestName + " " + unitTestSrcPath)
                        break

            print("Checked In Truth Data To " + unitTestSrcPath)


else:
    apptest = sys.argv[1]
    makefilePath = ""
    with open(builddir + "/objects/CTestTestfile.cmake") as testFile:
        for line in testFile:
            if apptest in line:
                makefilePath = line.split("\" \"")[1][11:]
                break

    makefilePath = makefilePath.split("/")
    del makefilePath[-1]
    makefilePath = "/".join(makefilePath)

    # change dir to test dir and run make commands
    os.chdir(makefilePath)
    os.system("make checkout")
    os.system("make output")
    os.system("make truthdata")
    os.system("rm -rf " + builddir + "/testOutputDir/truth")
    os.system("cp -r truth " + builddir + "/testOutputDir")
    print("App Test Output In " + builddir + "/testOutputDir/truth")

    # check if the user wants data checked in
    if len(sys.argv) == 3:
        if sys.argv[2] == "truth":
            os.system("make checkin")
            print("Checked In Truth Data")

    # doing this instead of make release because make release
    # can give feedback to the user that we would rather avoid
    os.system("rm -rf input output truth print.prt")
