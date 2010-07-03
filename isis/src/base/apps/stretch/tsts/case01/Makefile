APPNAME = stretch

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/stretchTruth1.cub \
	  pairs="0:255 255:0" \
	  null=500 \
	  lis=700 \
	  lrs=800 \
	  his=900 \
	  hrs=1000 > /dev/null;
