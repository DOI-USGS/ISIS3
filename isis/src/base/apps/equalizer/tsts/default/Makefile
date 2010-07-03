APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  holdlist=$(INPUT)/HoldList.lst \
	  tolist=$(INPUT)/ToList.lst \
	  > /dev/null;
	$(MV) I00824006RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth1.cub;
	$(MV) I01523019RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth2.cub;
	$(MV) I02609002RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth3.cub; 
