APPNAME = stretch

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/peaks.cub \
	  to=$(OUTPUT)/stretchTruthFile.cub \
	  r=true \
	  i=$(INPUT)/pairsFile \
	  null=500 \
	  lis=700 \
	  lrs=800 \
	  his=900 \
	  hrs=1000 > /dev/null;
