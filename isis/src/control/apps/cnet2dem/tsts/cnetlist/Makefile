# Standard use case test (default) for cnet2dem
# Tests CNETLIST, TO, MAP, ALGORITHM

Equi065S355ED10_dem.cub.TOLERANCE = 0.00000001

APPNAME = cnet2dem

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) -1 $(INPUT)/EN*combined\.net > $(OUTPUT)/networks.lis;
	$(APPNAME) cnetlist=$(OUTPUT)/networks.lis \
	           to=$(OUTPUT)/Equi065S355ED10_dem \
	           map=$(INPUT)/Equi065S355ED10.map \
	           algorithm="FrankeNelsonRadius,ShepardsRadius,AverageRadius,MedianRadius,MinimumRadius,MaximumRadius,StandardDeviationRadius,TypeCount,AverageDistance,MinimumDistance,MaximumDistance,StandardDeviationDistance,MedianDistance,NaturalNeighborRadius" \
	           > /dev/null;
	$(RM) $(OUTPUT)/networks.lis;
