APPNAME = hiequal

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/*.cub > fromList.lst;
	$(LS) $(INPUT)/RED2.cub > holdList.lst;
	$(APPNAME) fromlist=fromList.lst \
	  holdlist=holdList.lst \
	  tolist=$(INPUT)/toList.lst \
	  outstats=$(OUTPUT)/stat.pvl \
	  > /dev/null;

	$(MV) RED0.equ.cub $(OUTPUT)/hiequalTruth1.cub;
	$(MV) RED1.equ.cub $(OUTPUT)/hiequalTruth2.cub;
	$(MV) RED2.equ.cub $(OUTPUT)/hiequalTruth3.cub;

	$(RM) fromList.lst holdList.lst \
	  > /dev/null;
