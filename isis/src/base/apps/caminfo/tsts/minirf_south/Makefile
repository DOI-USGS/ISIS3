APPNAME = caminfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/LSZ_04790_1CD_XKU_89S148_V1_lev1.cub+1 \
	TO=$(OUTPUT)/caminfo.pvl ISISLABEL=true ORIGINAL=true \
	STATISTICS=true CAMSTATS=true linc=25 sinc=25 \
	POLYGON=true polysinc=100 polylinc=100 > /dev/null;
	cat $(OUTPUT)/caminfo.pvl | \
	  sed 's/\(MULTIPOLYGON.*[0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' | \
	  sed 's/\([0-9]*\.[0-9]\{5\}\)\([0-9]*)))\)/\1)))/g' | \
	  sed 's/\([-0-9][0-9]*\.[0-9]\{5\}\)\([0-9]*\)\(, [-0-9]*\.[0-9]\{5\}\)\([0-9]*\)/\1\3/g' | \
	  sed 's/\([-0-9][0-9]*\.[0-9]\{5\}\)\([0-9]*\)\()), (([-0-9]*\.[0-9]\{5\}\)\([0-9]*\)/\1\3/g' \
	  > $(OUTPUT)/caminfo2.pvl;
	$(MV) $(OUTPUT)/caminfo2.pvl $(OUTPUT)/caminfo.pvl;
