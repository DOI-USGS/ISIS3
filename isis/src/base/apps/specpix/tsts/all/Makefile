APPNAME = specpix

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/specpixTruth6.cub \
	  nullmin=1 \
	  nullmax=2 \
	  lrsmin=3 \
	  lrsmax=4 \
	  hrsmin=5 \
	  hrsmax=6 \
	  lismin=7 \
	  lismax=8 \
	  hismin=9 \
	  hismax=10 > /dev/null;
