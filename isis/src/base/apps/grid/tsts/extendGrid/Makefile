APPNAME = grid

MOSAIC = Mars_Viking_MDIM21_Mosaic_TEST_SIMP_REDUCED_GRID

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/$(MOSAIC).cub to=$(OUTPUT)/$(MOSAIC)_GRID.cub \
	linewidth=3 2> $(OUTPUT)/warning.txt 1> /dev/null;
	$(APPNAME) from=$(INPUT)/$(MOSAIC).cub to=$(OUTPUT)/$(MOSAIC)_EXTENDGRID.cub \
	linewidth=3 extendgrid=true > /dev/null;
