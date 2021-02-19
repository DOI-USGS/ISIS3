APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/FromList.lst;
	$(ECHO) $(INPUT)/I01523019RDR.lev2.cub > $(OUTPUT)/HoldList.lst;
	$(APPNAME) fromlist=$(OUTPUT)/FromList.lst \
	  holdlist=$(OUTPUT)/HoldList.lst \
	  tolist=$(INPUT)/ToList.lst \
	  solvemethod=qrd \
	  > /dev/null;
	$(MV) I00824006RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth1.cub;
	$(MV) I01523019RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth2.cub;
	$(MV) I02609002RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth3.cub;
	$(RM) $(OUTPUT)/FromList.lst;
	$(RM) $(OUTPUT)/HoldList.lst;

