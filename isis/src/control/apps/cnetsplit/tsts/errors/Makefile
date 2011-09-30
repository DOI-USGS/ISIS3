# Test for output errors in cnetsplit.  No control nets will be output, instead
# we will have a text file containing the errors.
.IGNORE:

APPNAME = cnetsplit

include $(ISISROOT)/make/isismake.tsts

commands:
    # There are 8 control points in the input, ask for 11 output files.
	$(APPNAME) \
	cnet=$(INPUT)/controlNet.net \
	onet_prefix="outputSplit??" \
	num_output_files=11 2> $(OUTPUT)/error1.txt > /dev/null;
    # replace the filename path, /.../input/, with the directory name, input/,
    # so test is succesful whether run in testData area
    # or in your own work directory
	$(SED) s+/.*input/+input/+g $(OUTPUT)/error1.txt > $(OUTPUT)/error.txt;
	rm -f $(OUTPUT)/error1.txt;
