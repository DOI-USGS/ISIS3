APPNAME = isis2fits

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/isis2fitsTruth1.fits > /dev/null;
	raw2isis from=$(OUTPUT)/isis2fitsTruth1.fits \
	  to=$(OUTPUT)/isis2fitsTruth1.cub BITTYPE=REAL SAMPLES=126 \
	  LINES=126 BANDS=1 SKIP=2880 BYTEORDER=MSB > /dev/null;
	dd bs=2880 count=1 if=$(OUTPUT)/isis2fitsTruth1.fits > $(OUTPUT)/labels.txt 2> /dev/null;
	$(RM) $(OUTPUT)/isis2fitsR\Truth1.fits; > /dev/null;
