APPNAME = caminfo

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/20190328T182543S3900_pol_iofL2pan.cub \
	TO=$(OUTPUT)/stats_from_table.pvl ISISLABEL=false ORIGINAL=false geometry=false \
	STATISTICS=false CAMSTATS=false USELABEL=false USECAMSTATSTBL=true > /dev/null;
	$(APPNAME) FROM=$(INPUT)/20190328T182543S3900_pol_iofL2pan.cub \
	TO=$(OUTPUT)/stats_from_camstats.pvl ISISLABEL=false ORIGINAL=false geometry=false \
	STATISTICS=false CAMSTATS=true LINC=10 SINC=10 USELABEL=false USECAMSTATSTBL=false > /dev/null;
#	cat $(OUTPUT)/stats_from_table.pvl | \
#	  sed 's/\(MULTIPOLYGON.*[0-9]\.[0-9]\{5\}\)\([0-9]*\)/\1/g' | \
#	  sed 's/\([0-9]*\.[0-9]\{5\}\)\([0-9]*)))\)/\1)))/g' | \
#	  sed 's/\([-0-9][0-9]*\.[0-9]\{5\}\)\([0-9]*\)\(, [-0-9]*\.[0-9]\{5\}\)\([0-9]*\)/\1\3/g' \
#	  > $(OUTPUT)/caminfo2.pvl;
#	$(MV) $(OUTPUT)/caminfo2.pvl $(OUTPUT)/caminfo.pvl;
#	cat $(OUTPUT)/caminfo.pvl | \
#	  sed 's/\([0-9]*\.[0-9]\{6\}\)\([0-9]*\)/\1/g' \
#	  > $(OUTPUT)/caminfo2.pvl;
#	$(MV) $(OUTPUT)/caminfo2.pvl $(OUTPUT)/caminfo.pvl;
