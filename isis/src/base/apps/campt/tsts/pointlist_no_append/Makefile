APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	to=$(OUTPUT)/pointlistNoAppendTruth.pvl \
	coordtype=IMAGE append=FALSE > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	to=$(OUTPUT)/pointlistNoAppendTruth.pvl \
	coordtype=IMAGE append=FALSE > /dev/null;
	cat $(OUTPUT)/pointlistNoAppendTruth.pvl | \
	$(SED) 's/\(.*= \).*\(ab102401.cub\)/\1\2/g' \
	> $(OUTPUT)/pointlistNoAppendTruthtmp.pvl;
	$(MV) $(OUTPUT)/pointlistNoAppendTruthtmp.pvl \
	$(OUTPUT)/pointlistNoAppendTruth.pvl;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	format=FLAT coordtype=IMAGE \
	to=$(OUTPUT)/pointlistNoAppendTruth.csv \
	append=FALSE > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	format=FLAT coordtype=IMAGE \
	to=$(OUTPUT)/pointlistNoAppendTruth.csv \
	append=FALSE > /dev/null;
	cat $(OUTPUT)/pointlistNoAppendTruth.csv | \
	$(SED) 's/^.*\(ab102401.cub\)\(.*\)/\1\2/g' \
	> $(OUTPUT)/pointlistNoAppendTruthtmp.csv;
	$(MV) $(OUTPUT)/pointlistNoAppendTruthtmp.csv \
	$(OUTPUT)/pointlistNoAppendTruth.csv;
