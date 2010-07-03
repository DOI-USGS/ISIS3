APPNAME = raw2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.dat \
	  to=$(OUTPUT)/raw2isisTruth1.cub \
	  samples=126 \
	  lines=126 \
	  bands=2 \
	  skip=0 \
	  bittype=real \
	  byteorder=lsb  > /dev/null;
