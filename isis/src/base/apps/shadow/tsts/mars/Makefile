APPNAME = shadow

include $(ISISROOT)/make/isismake.tsts

ab102401.default.shadowed.subtracted.cub.TOLERANCE = 0.00001
ab102401.suncenter.subtracted.cub.TOLERANCE = 0.00001
ab102401.noshadow.subtracted.cub.TOLERANCE = 0.00001
ab102401.nolight.subtracted.cub.TOLERANCE = 0.00001

# For the sunbelowhorizon cube, the sun's center is below the horizon - making the hillshade
#   algorithm compute an initial sun direction that's below the surface.
commands: default nosunedge noshadow nolight

default:
	$(APPNAME) from=$(INPUT)/ab102401.sunbelowhorizon.dem.cub match=$(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/ab102401.default.shadowed.cub PRESET=custom BASESHADOWCACHESIZE=100 \
	  BASELIGHTCACHESIZE=100 | grep -v 'shadow:' | grep -v Processed \
	  > $(OUTPUT)/ab102401.default.shadowed.log.pvl;
	fx f1=$(OUTPUT)/ab102401.default.shadowed.cub f2=$(INPUT)/ab102401.default.shadowed.cub \
	  equation='f2-f1' to=$(OUTPUT)/ab102401.default.shadowed.subtracted.cub > /dev/null;
	$(RM) $(OUTPUT)/ab102401.default.shadowed.cub;

nosunedge:
	$(APPNAME) from=$(INPUT)/ab102401.sunbelowhorizon.dem.cub match=$(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/ab102401.suncenter.cub SUNEDGE=false PRESET=custom BASESHADOWCACHESIZE=0 \
	  BASELIGHTCACHESIZE=0 | grep -v 'shadow:' | grep -v Processed \
	  > $(OUTPUT)/ab102401.suncenter.log.pvl;
	fx f1=$(OUTPUT)/ab102401.suncenter.cub f2=$(INPUT)/ab102401.suncenter.cub \
	  equation='f2-f1' to=$(OUTPUT)/ab102401.suncenter.subtracted.cub > /dev/null;
	$(RM) $(OUTPUT)/ab102401.suncenter.cub;

noshadow:
	$(APPNAME) from=$(INPUT)/ab102401.sunbelowhorizon.dem.cub match=$(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/ab102401.noshadow.cub SHADOWMAP=false PRESET=custom \
	  BASELIGHTCACHESIZE=10000 | grep -v 'shadow:' | grep -v Processed \
	  > $(OUTPUT)/ab102401.noshadow.log.pvl;
	fx f1=$(OUTPUT)/ab102401.noshadow.cub f2=$(INPUT)/ab102401.noshadow.cub \
	  equation='f2-f1' to=$(OUTPUT)/ab102401.noshadow.subtracted.cub > /dev/null;
	$(RM) $(OUTPUT)/ab102401.noshadow.cub;

nolight:
	$(APPNAME) from=$(INPUT)/ab102401.sunbelowhorizon.dem.cub match=$(INPUT)/ab102401.cub \
	  to=$(OUTPUT)/ab102401.nolight.cub PRESET=custom LIGHTCURTAIN=false \
	  BASESHADOWCACHESIZE=1 | grep -v 'shadow:' | grep -v Processed \
	  > $(OUTPUT)/ab102401.nolight.log.pvl;
	fx f1=$(OUTPUT)/ab102401.nolight.cub f2=$(INPUT)/ab102401.nolight.cub \
	  equation='f2-f1' to=$(OUTPUT)/ab102401.nolight.subtracted.cub > /dev/null;
	$(RM) $(OUTPUT)/ab102401.nolight.cub;
