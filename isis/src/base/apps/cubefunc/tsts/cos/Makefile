APPNAME = cubefunc

include $(ISISROOT)/make/isismake.tsts

commands:
	crop from=$(INPUT)/isisTruth.cub \
	to=$(OUTPUT)/tempCrop.cub \
	sample=1 \
	nsamples=13 \
	sinc=1 \
	nlines=13  \
	line=1 \
	linc=1  > /dev/null;
	$(APPNAME) from=$(OUTPUT)/tempCrop.cub \
	  to=$(OUTPUT)/cubefuncTruth1.cub \
	  function=cos > /dev/null;
