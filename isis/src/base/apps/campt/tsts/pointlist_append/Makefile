APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	to=$(OUTPUT)/pointlistAppendTruth.pvl \
	coordtype=IMAGE > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	to=$(OUTPUT)/pointlistAppendTruth.pvl \
	coordtype=IMAGE > /dev/null;
	cat $(OUTPUT)/pointlistAppendTruth.pvl | \
	$(SED) 's/\(.*= \).*\(ab102401.cub\)/\1\2/g' \
	> $(OUTPUT)/pointlistAppendTruthtmp.pvl;
	$(MV) $(OUTPUT)/pointlistAppendTruthtmp.pvl \
	$(OUTPUT)/pointlistAppendTruth.pvl;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	format=FLAT coordtype=IMAGE \
	to=$(OUTPUT)/pointlistAppendTruth.csv > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	format=FLAT coordtype=IMAGE \
	to=$(OUTPUT)/pointlistAppendTruth.csv > /dev/null;
	cat $(OUTPUT)/pointlistAppendTruth.csv | \
	$(SED) 's/^.*\(ab102401.cub\)\(.*\)/\1\2/g' \
	> $(OUTPUT)/pointlistAppendTruthtmp.csv;
	$(MV) $(OUTPUT)/pointlistAppendTruthtmp.csv \
	$(OUTPUT)/pointlistAppendTruth.csv;
