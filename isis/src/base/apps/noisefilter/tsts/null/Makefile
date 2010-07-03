APPNAME = noisefilter

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/nsFilTruth2.cub \
	  toldef= stddev \
	  tolmin= 1.25 \
	  tolmax= 1.25 \
	  replace= null \
	  samples= 21 \
	  lines= 21  > /dev/null;
