APPNAME = isis2fits

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+2 \
	  to=$(OUTPUT)/isis2fitsTruth5.fits \
	  bittype= 8BIT \
	  stretch= none > /dev/null;
	 raw2isis from=$(OUTPUT)/isis2fitsTruth5.fits \
          to=$(OUTPUT)/truth.cub \
          SAMPLES=126 LINES=126 BANDS=1 SKIP=2880 \
          BITTYPE=UNSIGNEDBYTE BYTEORDER=MSB > /dev/null;
	 dd bs=2880 count=1 if=$(OUTPUT)/isis2fitsTruth5.fits > $(OUTPUT)/labels.txt 2> /dev/null;
	 $(RM) $(OUTPUT)/ifif2fitsTruth5.fits; > /dev/null;
