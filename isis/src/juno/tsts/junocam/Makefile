IMPORT    = junocam2isis
SPICEINIT = spiceinit
CROP      = crop
PROJECT   = cam2map
MOSAIC    = automos
INPUTFILE = JNCR_2016346_03C00104_V01
MAP       = juno_pp.map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(IMPORT) $(TSTARGS) from=$(INPUT)/$(INPUTFILE).LBL \
		  to=$(OUTPUT)/$(INPUTFILE).cub > /dev/null;
	$(IMPORT) $(TSTARGS) from=$(INPUT)/$(INPUTFILE).LBL \
		  to=$(OUTPUT)/$(INPUTFILE)_FULLCCD.cub fullccd=yes \
		  > /dev/null;
	$(SPICEINIT) $(TSTARGS) from=$(OUTPUT)/$\$$\1 \
		     -batchlist=$(OUTPUT)/$(INPUTFILE).lis > /dev/null;
	$(SPICEINIT) $(TSTARGS) from=$(OUTPUT)/$\$$\1 \
		     -batchlist=$(OUTPUT)/$(INPUTFILE)_FULLCCD.lis > /dev/null;
	$(CROP) $(TSTARGS) from=$\$$\1.cub to=$\$$\1_GREEN.cub \
		line=611 nlines=128 -batchlist=$(INPUT)/FULLCCD.lis \
		> /dev/null;
	$(PROJECT) $(TSTARGS) from=$\$$\1.cub map=$(INPUT)/$(MAP) \
		   to=$\$$\1_pp.cub pixres=mpp resolution=40000 \
		   -batchlist=$(INPUT)/RED.lis > /dev/null;
	$(PROJECT) $(TSTARGS) from=$\$$\1_GREEN.cub map=$(INPUT)/$(MAP) \
		   to=$\$$\1_GREEN_pp.cub pixres=mpp resolution=40000 \
		   -batchlist=$(INPUT)/FULLCCD.lis > /dev/null;
	$(LS) $(OUTPUT)/$(INPUTFILE)_RED*_pp.cub > $(OUTPUT)/RED_PP.lis;
	$(LS) $(OUTPUT)/$(INPUTFILE)_FULLCCD*_GREEN_pp.cub > $(OUTPUT)/FULLCCD_GREEN_PP.lis;
	$(MOSAIC) $(TSTARGS) fromlist=$(OUTPUT)/RED_PP.lis \
		  MOSAIC=$(OUTPUT)/$(INPUTFILE)_RED_PP.cub > /dev/null;
	$(MOSAIC) $(TSTARGS) fromlist=$(OUTPUT)/FULLCCD_GREEN_PP.lis \
		  MOSAIC=$(OUTPUT)/$(INPUTFILE)_FULLCCD_GREEN_PP.cub > /dev/null;
	$(RM) $(OUTPUT)/$(INPUTFILE)*[0-9][0-9][0-9][0-9].cub;
	$(RM) $(OUTPUT)/$(INPUTFILE)*_pp.cub;
	$(RM) $(OUTPUT)/$(INPUTFILE)*_GREEN.cub;
	$(RM) $(OUTPUT)/*.lis;
