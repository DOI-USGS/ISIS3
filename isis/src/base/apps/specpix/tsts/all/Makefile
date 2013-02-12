APPNAME = specpix

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/specpixTruth6.cub \
	  nullmin=0 \
	  nullmax=20 \
	  lrsmin=39 \
	  lrsmax=59 \
	  hrsmin=79 \
	  hrsmax=98 \
	  lismin=118 \
	  lismax=138 \
	  hismin=158 \
	  hismax=177 > /dev/null;

	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/outputSingleDN.cub \
	  nullmin=20 \
	  nullmax=20 \
	  lrsmin=39 \
	  lrsmax=39 \
	  hrsmin=59 \
	  hrsmax=59 \
	  lismin=79 \
	  lismax=79 \
	  hismin=98 \
	  hismax=98 > /dev/null;
