APPNAME = campt

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/camptTruthTemp.txt \
	  format=FLAT \
	  type=ground \
	  latitude=34.444196777709 \
	  longitude=223.84999971295 > /dev/null;
	cut -d , -f 2- $(OUTPUT)/camptTruthTemp.txt >& $(OUTPUT)/camptTruth2.txt;
	rm $(OUTPUT)/camptTruthTemp.txt;
