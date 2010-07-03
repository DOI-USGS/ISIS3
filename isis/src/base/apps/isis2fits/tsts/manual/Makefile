APPNAME = isis2fits

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	  to=$(OUTPUT)/isis2fitsTruth7.fits \
	  bittype= 16BIT \
	  stretch= manual \
	  minimum = 0 \
	  maximum = 65535 > /dev/null;
	raw2isis from=$(OUTPUT)/isis2fitsTruth7.fits \
	  to=$(OUTPUT)/truth.cub \
	  SAMPLES=126 LINES=126 BANDS=1 SKIP=2880 \
	  BITTYPE=UNSIGNEDWORD BYTEORDER=MSB > /dev/null;
	dd bs=2880 count=1 if=$(OUTPUT)/isis2fitsTruth7.fits > $(OUTPUT)/labels.txt 2> /dev/null;
	$(RM) $(OUTPUT)/isis2fitsTruth7.fits; > /dev/null;
