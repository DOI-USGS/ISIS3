APPNAME = raw2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.dat \
	  to=$(OUTPUT)/raw2isisTruth2.cub \
	  samples=126 \
	  lines=126 \
	  bands=2 \
	  skip=0 \
	  bittype=real \
	  byteorder=lsb \
	  SETHRSRANGE=yes \
	  HRSMIN=20 \
	  HRSMAX=200 \
	  SETLRSRANGE=yes \
	  LRSMIN=-1e20 \
	  LRSMAX=-1e18 \
	  SETNULLRANGE=yes \
	  NULLMIN=-1e-20 \
	  NULLMAX=-1e-22 > /dev/null;
