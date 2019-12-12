# Standard use case test (default) for cnet2dem
# Tests CNET, TO, MAP, ALGORITHM

truthDemNet.cub.TOLERANCE  = 0.00000001

APPNAME = cnet2dem

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) cnet=$(INPUT)/truthNet.net \
	           to=$(OUTPUT)/truthDemNet \
	           map=$(INPUT)/truthMap.map \
	           search=both \
	           algorithm="FrankeNelsonRadius,ShepardsRadius,AverageRadius,MedianRadius,MinimumRadius,MaximumRadius,StandardDeviationRadius,TypeCount,AverageDistance,MinimumDistance,MaximumDistance,StandardDeviationDistance,MedianDistance,NaturalNeighborRadius" \
	           > /dev/null;

	$(RM) $(OUTPUT)/networks.lis;
