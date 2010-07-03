APPNAME = handmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  mosaic=$(OUTPUT)/handmosTruth1.cub \
	  priority=ontop \
	  outsample=1 \
	  outline=2 \
	  outband=1 \
	  create=yes \
	  track=yes \
	  nsamples=10 \
	  nlines=20 \
	  nbands=2 > /dev/null;
