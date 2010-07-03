APPNAME = camrange

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ab102402.cub \
	  to=$(OUTPUT)/camrangeTruth2.txt > /dev/null;
	$(MV) $(OUTPUT)/camrangeTruth2.txt \
	  $(OUTPUT)/camrangeTruth2.pvl > /dev/null;
