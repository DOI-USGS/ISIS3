'''
Script to create truthData for tests.

For output it expects the command in the form of:
    python3 makeOutput.py test

where test is the cmake name for the unit or app test

To check in truth data the command should be in the form of:
    python makeOutput.py -t test

The -t option checks in truth data

The unit tests are pretty trivial because the executable outputs the results of
the unitTest to stdout and stderr so we just redirect the streams to the file
named object.truth where object is the ISIS object being tested.

The app tests output has to rely on the old make system because the app test
infrastructure relies on the old make system. Otherwise all the logic for the old
makesystem would need to be reimplemented here. Because we wish to replace the testing
system with something that allows unit and app test to live in the same space the effort
to recreate the logic is not worth the outcome.
'''

import argparse
import sys
import os

try:
    builddir = os.environ['ISISROOT']
except KeyError:
    print("The $ISISROOT environment variable is not set")

parser = argparse.ArgumentParser()
parser.add_argument('test', action='store', help='Provide the name of the Test to create output of')
parser.add_argument('-t', action='store_true', default=False, dest='truth', help='Flag whether output is sent to truth data')
userInput = parser.parse_args()
testInput = userInput.test

if "_unit_" in testInput:
    unitTestName = testInput.split("_test_")[1] + ".truth"
    # we should probably append the path to the front so it ends up in
    # in the same directory as the test
    unitTestPath = builddir + "/unitTest/"

    os.system(unitTestPath + testInput + ">&" + builddir + "/testOutputDir/" + unitTestName)
    print("Unit Test Output In " + builddir + "/testOutputDir/ As " + unitTestName)

    if userInput.truth:
        with open(builddir + "/objects/CTestTestfile.cmake") as testFile:
            for line in testFile:
                if unitTestName in line:
                    unitTestSrcPath = line.split("\" \"")[2][13:]
                    os.system("cp -f " + builddir + "/testOutputDir/" + unitTestName + " " + unitTestSrcPath)
                    break

        print("Checked In Truth Data To " + unitTestSrcPath)


else:
    apptest = testInput
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
    os.system("cp -rf truth " + builddir + "/testOutputDir")
    print("App Test Output In " + builddir + "/testOutputDir/truth")

    # check if the user wants data checked in
    if userInput.truth:
        os.system("make checkin")
        print("Checked In Truth Data")

    # doing this instead of make release because make release
    # can give feedback to the user that we would rather avoid
    os.system("rm -rf input output truth print.prt")
