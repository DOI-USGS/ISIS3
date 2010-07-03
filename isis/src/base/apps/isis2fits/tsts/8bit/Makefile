APPNAME = isis2fits

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/isis2fitsTruth4.fits \
	  bittype= 8BIT > /dev/null;
	raw2isis from=$(OUTPUT)/isis2fitsTruth4.fits \
	  to=$(OUTPUT)/isis2fitsTruth4.cub SAMPLES=126 \
	  LINES=126 BANDS=2 SKIP=2880 BITTYPE=UNSIGNEDBYTE \
	  BYTEORDER=LSB > /dev/null;
	dd bs=2880 count=1 if=$(OUTPUT)/isis2fitsTruth4.fits > $(OUTPUT)/labels.txt 2> /dev/null;
	$(RM) $(OUTPUT)/isis2fitsTruth4.fits; > /dev/null;
