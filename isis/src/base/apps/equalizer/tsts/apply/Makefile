APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  tolist=$(INPUT)/ToList.lst \
	  instats=$(INPUT)/instats.pvl \
	  process=apply \
	  > /dev/null;
	$(MV) I00824006RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth1.cub;
	$(MV) I01523019RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth2.cub;
