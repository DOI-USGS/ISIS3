APPNAME = camrange

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/camrangeTruth1.txt > /dev/null;
	$(MV) $(OUTPUT)/camrangeTruth1.txt \
	$(OUTPUT)/camrangeTruth1.pvl > /dev/null;
