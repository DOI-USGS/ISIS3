APPNAME = tonematch

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from= $(INPUT)/I00824006RDR.lev2.cub+1 \
	  match=$(INPUT)/I01523019RDR.lev2.cub+1 \
	  to=$(OUTPUT)/tonematchTruth2.cub \
	  poverlap=true > /dev/null;
