APPNAME = vimscal
#vimscal from=/work/projects/isis/latest/m04370a/isis/src/cassini/apps/vimscal/rawcubes/ir/V1824733761_1.ir.cub to=/work/projects/isis/latest/m04370a/isis/src/cassini/apps/vimscal/rawcubes/ir/C1824733761_1.ir.2013Flat.cub flatfield=2013flat

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/V1824733761_1.ir.cub \
	  TO=$(OUTPUT)/C1824733761_1.ir.2006Flat.cub \
	  flatfield=2006FLAT > /dev/null;
	catlab from=$(OUTPUT)/C1824733761_1.ir.2006Flat.cub \
	  TO=$(OUTPUT)/C1824733761_1.ir.2006Flat.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/V1824733761_1.ir.cub \
	  TO=$(OUTPUT)/C1824733761_1.ir.2006SSFlat.cub \
	  flatfield=2006SSFLAT > /dev/null;
	catlab from=$(OUTPUT)/C1824733761_1.ir.2006SSFlat.cub \
	  TO=$(OUTPUT)/C1824733761_1.ir.2006SSFlat.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/V1824733761_1.ir.cub \
	  TO=$(OUTPUT)/C1824733761_1.ir.2013Flat.cub \
	  flatfield=2013FLAT > /dev/null;
	catlab from=$(OUTPUT)/C1824733761_1.ir.2013Flat.cub \
	  TO=$(OUTPUT)/C1824733761_1.ir.2013Flat.pvl > /dev/null;

	$(RM) $(OUTPUT)/*.cub
