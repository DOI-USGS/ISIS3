APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/image_error.lis \
	to=$(OUTPUT)/pointlistErrorTruth.pvl \
	append=false coordtype=IMAGE > /dev/null;
	cat $(OUTPUT)/pointlistErrorTruth.pvl | \
	$(SED) 's/\(.*= \).*\(ab102401.cub\)/\1\2/g' \
	> $(OUTPUT)/pointlistErrorTruthtmp.pvl;
	$(MV) $(OUTPUT)/pointlistErrorTruthtmp.pvl \
	$(OUTPUT)/pointlistErrorTruth.pvl;
