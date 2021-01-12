APPNAME = caminfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/ab102401.foot.cub \
	TO=$(OUTPUT)/caminfo.pvl ISISLABEL=false ORIGINAL=false \
	STATISTICS=false CAMSTATS=false USELABEL=true > /dev/null;
	cat $(OUTPUT)/caminfo.pvl | \
	  sed 's/\(MULTIPOLYGON.*[0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' | \
	  sed 's/\([0-9]*\.[0-9]\{5\}\)\([0-9]*)))\)/\1)))/g' | \
	  sed 's/\([-0-9][0-9]*\.[0-9]\{5\}\)\([0-9]*\)\(, [-0-9]*\.[0-9]\{5\}\)\([0-9]*\)/\1\3/g' \
	  > $(OUTPUT)/caminfo2.pvl;
	$(MV) $(OUTPUT)/caminfo2.pvl $(OUTPUT)/caminfo.pvl;
	cat $(OUTPUT)/caminfo.pvl | \
	  sed 's/\([0-9]*\.[0-9]\{6\}\)\([0-9]*\)/\1/g' \
	  > $(OUTPUT)/caminfo2.pvl;
	$(MV) $(OUTPUT)/caminfo2.pvl $(OUTPUT)/caminfo.pvl;
