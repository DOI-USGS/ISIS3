APPNAME = divfilter

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/divFilterTruth1.cub \
	  samples= 11 \
	  lines= 11 \
	  low= -1e-20 \
	  high= 1e+20 \
	  minimum= 85 \
	  propagate= True > /dev/null;
