APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image.lis \
	to=$(OUTPUT)/pointlistFlatTruth.csv \
	format=FLAT coordtype=IMAGE \
	append=false > /dev/null;
	cat $(OUTPUT)/pointlistFlatTruth.csv | \
	$(SED) 's/^.*\(ab102401.cub\)\(.*\)/\1\2/g' \
	> $(OUTPUT)/pointlistFlatTruthtmp.csv;
	$(MV) $(OUTPUT)/pointlistFlatTruthtmp.csv \
	$(OUTPUT)/pointlistFlatTruth.csv;
