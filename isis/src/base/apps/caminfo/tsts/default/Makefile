APPNAME = caminfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/input2.cub \
	TO=$(OUTPUT)/caminfo.pvl ISISLABEL=true ORIGINAL=true \
	STATISTICS=true CAMSTATS=true POLYGON=true polysinc=100 polylinc=100 > /dev/null;
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
	
	$(APPNAME) FROM=$(INPUT)/input2.cub \
	TO=$(OUTPUT)/caminfo2.pvl ISISLABEL=true ORIGINAL=true \
	STATISTICS=true CAMSTATS=true POLYGON=true > /dev/null;
	cat $(OUTPUT)/caminfo2.pvl | \
	  sed 's/\(MULTIPOLYGON.*[0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' | \
	  sed 's/\([0-9]*\.[0-9]\{5\}\)\([0-9]*)))\)/\1)))/g' | \
	  sed 's/\([-0-9][0-9]*\.[0-9]\{5\}\)\([0-9]*\)\(, [-0-9]*\.[0-9]\{5\}\)\([0-9]*\)/\1\3/g' \
	  > $(OUTPUT)/caminfo3.pvl;
	$(MV) $(OUTPUT)/caminfo3.pvl $(OUTPUT)/caminfo2.pvl;
	cat $(OUTPUT)/caminfo2.pvl | \
	  sed 's/\([0-9]*\.[0-9]\{6\}\)\([0-9]*\)/\1/g' \
	  > $(OUTPUT)/caminfo3.pvl;
	$(MV) $(OUTPUT)/caminfo3.pvl $(OUTPUT)/caminfo2.pvl;
	
	$(APPNAME) FROM=$(INPUT)/input2.cub \
	TO=$(OUTPUT)/caminfo3.pvl ISISLABEL=true ORIGINAL=true \
	STATISTICS=true CAMSTATS=true POLYGON=true INCTYPE=VERTICES > /dev/null;
	cat $(OUTPUT)/caminfo3.pvl | \
	  sed 's/\(MULTIPOLYGON.*[0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' | \
	  sed 's/\([0-9]*\.[0-9]\{5\}\)\([0-9]*)))\)/\1)))/g' | \
	  sed 's/\([-0-9][0-9]*\.[0-9]\{5\}\)\([0-9]*\)\(, [-0-9]*\.[0-9]\{5\}\)\([0-9]*\)/\1\3/g' \
	  > $(OUTPUT)/caminfo4.pvl;
	$(MV) $(OUTPUT)/caminfo4.pvl $(OUTPUT)/caminfo3.pvl;
	cat $(OUTPUT)/caminfo3.pvl | \
	  sed 's/\([0-9]*\.[0-9]\{6\}\)\([0-9]*\)/\1/g' \
	  > $(OUTPUT)/caminfo4.pvl;
	$(MV) $(OUTPUT)/caminfo4.pvl $(OUTPUT)/caminfo3.pvl;
