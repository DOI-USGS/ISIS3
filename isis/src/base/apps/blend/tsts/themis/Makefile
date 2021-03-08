APPNAME = blend

I01086005.blend.cub.TOLERANCE = .00001
I23851018.blend.cub.TOLERANCE = .00001

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -r $(INPUT)/*.cub > fromlist.lis;
	$(APPNAME) fromlist=fromlist.lis \
	  tolist=$(INPUT)/tolist.lis \
	  stop=200 \
	  > /dev/null;
	$(MV) I01086005.blend.cub $(OUTPUT)/I01086005.blend.cub;
	$(MV) I23851018.blend.cub $(OUTPUT)/I23851018.blend.cub;
	$(RM) fromlist.lis;
