'''
Script to create truthData for unitTests. This might be more work
using the scripts then just documenting what you have to do to save
output as Truth
'''

unitTestExecutable = sys.argv[1]

unitTestName = unitTestExecutable.split("_test_")[1] + ".truth"

os.system(unitTestExecutable + ">&" + unitTestName)
