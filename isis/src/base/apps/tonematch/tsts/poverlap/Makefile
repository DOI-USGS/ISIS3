APPNAME = tonematch

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from= $(INPUT)/singleBand_I00824006RDR.lev2.cub \
	  match=$(INPUT)/singleBand_I01523019RDR.lev2.cub \
	  to=$(OUTPUT)/tonematchTruth2.cub \
	  poverlap=true outstats=$(OUTPUT)/outputStats.pvl> /dev/null;
