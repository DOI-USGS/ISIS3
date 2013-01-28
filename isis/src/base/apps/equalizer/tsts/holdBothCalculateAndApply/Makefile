# This test passes in a holdlist, lets process default to both (i.e. calculate 
# statistics and apply them), and lets adjust default to both (contrast and 
# brightness).
# 
# It is run once without creating the output statistics file and a second time
# creating the outstats file for test coverage. 
APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  holdlist=$(INPUT)/HoldList.lst \
	  tolist=$(INPUT)/ToList.lst \
	  solvemethod=qrd \
	  > /dev/null;
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  holdlist=$(INPUT)/HoldList.lst \
	  tolist=$(INPUT)/ToList.lst \
	  outstats=$(OUTPUT)/outstats.pvl \
	  solvemethod=qrd \
	  > /dev/null;
	$(MV) I00824006RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth1.cub;
	$(MV) I01523019RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth2.cub;
	$(MV) I02609002RDR.lev2.equ.cub $(OUTPUT)/equalizerTruth3.cub; 
