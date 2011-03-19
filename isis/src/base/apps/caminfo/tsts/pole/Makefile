APPNAME = caminfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/f167s71.cub \
	TO=$(OUTPUT)/caminfo.pvl ISISLABEL=true ORIGINAL=true \
	STATISTICS=true CAMSTATS=true linc=25 sinc=25 \
	POLYGON=true flatinc=false polysinc=100 polylinc=100 > /dev/null;
	cat $(OUTPUT)/caminfo.pvl | \
	  sed 's/\(MULTIPOLYGON.*[0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' | \
	  sed 's/\([0-9]*\.[0-9]\{5\}\)\([0-9]*)))\)/\1)))/g' | \
	  sed 's/\([-0-9][0-9]*\.[0-9]\{5\}\)\([0-9]*\)\(, [-0-9]*\.[0-9]\{5\}\)\([0-9]*\)/\1\3/g' \
	  > $(OUTPUT)/caminfo2.pvl;
	$(MV) $(OUTPUT)/caminfo2.pvl $(OUTPUT)/caminfo.pvl;
