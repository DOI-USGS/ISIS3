APPNAME = lowpass

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/lowTruth3.cub \
	  lines=7 \
	  samples=9 \
	  filter=inside \
	  null=false \
	  lis=false \
	  lrs=false \
	  his=false \
	  hrs=false \
	  low=0 \
	  high=255 > /dev/null;
