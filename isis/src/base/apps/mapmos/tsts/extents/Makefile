APPNAME  = mapmos

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/ab102401.lev2.cub \
	  mosaic=$(OUTPUT)/mapmosTruthLats.cub \
	  create=true \
	  minlat=10.765 \
	  maxlat=34.445 \
	  null=true > /dev/null;
	$(APPNAME) from=$(INPUT)/ab102401.lev2.cub \
	  mosaic=$(OUTPUT)/mapmosTruthLons.cub \
	  create=true \
	  minlon=219.720 \
	  maxlon=236.190 \
	  null=true > /dev/null;
	$(APPNAME) from=$(INPUT)/copy_ab102401.lev2.cub \
	  mosaic=$(OUTPUT)/mapmosTruthLons.cub \
	  create=true \
	  minlon=219.720 \
	  maxlon=236.190 \
	  null=true >& $(OUTPUT)/errors.txt || true;
	$(APPNAME) from=$(INPUT)/ab102401.lev2.cub \
	  mosaic=$(OUTPUT)/mapmosTruthLons.cub \
	  create=true \
	  minlon=219.720 \
	  null=true >& $(OUTPUT)/errors_tmp.txt || true;
	  cat $(OUTPUT)/errors_tmp.txt >> $(OUTPUT)/errors.txt;
	  $(RM) $(OUTPUT)/errors_tmp.txt;
