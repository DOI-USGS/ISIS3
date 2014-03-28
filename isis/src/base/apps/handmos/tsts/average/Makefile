APPNAME = handmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  mosaic=$(OUTPUT)/handmosTruth1.cub \
	  priority=average \
	  outsample=1 \
	  outline=2 \
	  outband=1 \
	  create=yes \
	  propagate=false \
	  nsamples=10 \
	  nlines=20 \
	  nbands=1 > /dev/null; \
	$(CP) $(INPUT)/average.cub $(OUTPUT)/average.cub; \
	$(APPNAME) from=$(INPUT)/ten.cub \
	  mosaic=$(OUTPUT)/average.cub \
	  priority=average \
	  outsample=5 \
	  outline=5 \
	  outband=1 \
	  matchbandbin=false 2>> $(OUTPUT)/errors.txt > /dev/null;\
	$(RM) $(OUTPUT)/average.cub;
