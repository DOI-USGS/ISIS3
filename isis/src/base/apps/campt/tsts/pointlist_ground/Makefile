APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.cub \
	coordlist=$(INPUT)/ground.lis \
	to=$(OUTPUT)/pointlistGroundTruth.pvl \
	append=false coordtype=GROUND > /dev/null;
	cat $(OUTPUT)/pointlistGroundTruth.pvl | \
	$(SED) 's/\(.*= \).*\(ab102401.cub\)/\1\2/g' \
	> $(OUTPUT)/pointlistGroundTruthtmp.pvl; 
	$(MV) $(OUTPUT)/pointlistGroundTruthtmp.pvl \
	$(OUTPUT)/pointlistGroundTruth.pvl;
