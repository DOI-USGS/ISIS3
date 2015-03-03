# test for not providing a sample or line value (defaults to center)
# also tests for not providing a value to the TO parameter
APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	> $(OUTPUT)/noSampleLineTruth.txt;
	cat $(OUTPUT)/noSampleLineTruth.txt | \
	sed 's/\([0-9]*\.[0-9]\{5\}\)[0-9]*/\1/g' \
	>& $(OUTPUT)/noSampleLineTruthtmp.txt;
	$(MV) $(OUTPUT)/noSampleLineTruthtmp.txt \
	$(OUTPUT)/noSampleLineTruth.txt;
