APPNAME = percent

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub+1 \
	  to=$(OUTPUT)/multiple.pvl \
	  percentage=.1,99.5,50.0 > /dev/null;
