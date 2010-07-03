APPNAME = gradient

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/gradTruth1.cub \
	  gradType=Sobel > /dev/null;
