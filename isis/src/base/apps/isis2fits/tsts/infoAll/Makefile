APPNAME = isis2fits

# This test is for when the isis2fits paramter INFO is set to "ALL"

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/skymapTruth.cub \
	  TO=$(OUTPUT)/truth.fits \
	  BITTYPE=32BIT \
	  INFO=ALL;
	raw2isis FROM=$(OUTPUT)/truth.fits \
	  TO=$(OUTPUT)/truth.cub \
	  SAMPLES=90 LINES=14 BANDS=1 \
	  SKIP=2880 BITTYPE=REAL BYTEORDER=LSB;
	dd bs=2880 count=1 if=$(OUTPUT)/truth.fits > $(OUTPUT)/labels.txt 2> /dev/null;
	$(RM) $(OUTPUT)/truth.fits; > /dev/null;
