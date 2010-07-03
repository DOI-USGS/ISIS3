APPNAME = isis2fits

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isis2fitsTruthMultiBand.cub \
	  to=$(OUTPUT)/isis2fitsTruth6.fits \
	  bittype= 16BIT > /dev/null;
	raw2isis from=$(OUTPUT)/isis2fitsTruth6.fits \
	  to=$(OUTPUT)/truth.cub \
	  SAMPLES=640 LINES=768 BANDS=3 SKIP=2880 \
	  BITTYPE=SIGNEDWORD BYTEORDER=MSB > /dev/null;
	dd bs=2880 count=1 if=$(OUTPUT)/isis2fitsTruth6.fits > \
	  $(OUTPUT)/labels.txt 2> /dev/null;
	$(RM) $(OUTPUT)/isis2fitsTruth6.fits; > /dev/null;
