APPNAME = blend

I00824006RDR.lev2.blend.cub.TOLERANCE = 0.001
I01523019RDR.lev2.blend.cub.TOLERANCE = 0.001
I02609002RDR.lev2.blend.cub.TOLERANCE = 0.001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > fromlist.lis;
	$(APPNAME) fromlist=fromlist.lis \
	  tolist=$(INPUT)/tolist.lis \
	  stop=200 \
	  > /dev/null;
	$(MV) I00824006RDR.lev2.blend.cub $(OUTPUT)/I00824006RDR.lev2.blend.cub;
	$(MV) I01523019RDR.lev2.blend.cub $(OUTPUT)/I01523019RDR.lev2.blend.cub;
	$(MV) I02609002RDR.lev2.blend.cub $(OUTPUT)/I02609002RDR.lev2.blend.cub;
	$(RM) fromlist.lis;
