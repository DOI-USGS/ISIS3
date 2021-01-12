# Test cnetextract when particular reports are not created.
# Run 4 tests:
#
#	TEST RESULTS:									REPORT NOT CREATED:
#
# (1) All points or measures are extracted.			None created
#
# (2) No ignored points exist, 						IgnoredPoints.txt
#     No non-reference ignore measures exist		IgnoredMeasures.txt
#     No single measure points exist				SingleMeasurePoints.txt
#     No measureless points exist					MeasurelessPoints.txt
#     All points fit tolerance						TolerancePoints.txt
#     All points are in lat/lon range				LatLonOutOfRange.txt
#     All points have determinable lat and lon		NoLatLonPoints.txt
#											   (no output net or toList created)
#
# (3) All measures are ref							NonReferenceMeasures.txt
#     All points are fixed							NonFixedPoints.txt
#     All points are in pointlist 					NonListedPoints.txt
#
# (4) All points have measures in cubelist			NonCubePoints.txt
#     All measures are in cubelist					NonCubeMeasures.txt
#     All measures are in cubelist or references 	NonCubeMeasures.txt	
APPNAME = cnetextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	  cnet=$(INPUT)/1_cnet.net \
	  prefix=$(OUTPUT)/1_ \
	  tolist=$(OUTPUT)/outlist.lis \
	  onet=$(OUTPUT)/1_sameAsInput.net \
	  networkid=test \
	  description=test \
	  pointlist=$(INPUT)/allPoints.lis \
	  | $(SED) 's+.*% Processed.*Group+Group+' \
	  | grep -v "Processed" \
	  | grep -v "cnetextract:" \
	  > $(OUTPUT)/1_resultsLog.pvl;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/outlist.lis > $(OUTPUT)/1_toList.txt;
	$(RM) $(OUTPUT)/outlist.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	  cnet=$(INPUT)/2_cnet.net \
	  prefix=$(OUTPUT)/2_ \
	  tolist=$(OUTPUT)/outlist.lis \
	  onet=$(OUTPUT)/2_outputNet.net \
	  networkid=test \
	  description=test \
	  noignore=true \
	  nosingle=true \
	  nomeasureless=true \
	  fixed=true \
	  tolerance=true \
	  pixel=5 \
	  latlon=true \
	  minlat=-90.0 \
	  maxlat=90.0 \
	  minlon=0.0 \
	  maxlon=360 \
	  | $(SED) 's+.*% Processed.*#+#+' \
	  | grep -v "Processed" \
	  | grep -v "cnetextract:" \
	  > $(OUTPUT)/2_resultsLog.pvl;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	  cnet=$(INPUT)/3_cnet.net \
	  prefix=$(OUTPUT)/3_ \
	  tolist=$(OUTPUT)/outlist.lis \
	  onet=$(OUTPUT)/3_outputNet.net \
	  networkid=test \
	  description=test \
	  reference=true \
	  fixed=true \
	  pointlist=$(INPUT)/allPoints.lis \
	  | $(SED) 's+.*% Processed.*#+#+' \
	  | grep -v "Processed" \
	  | grep -v "cnetextract:" \
	  > $(OUTPUT)/3_resultsLog.pvl;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	  cnet=$(INPUT)/4_cnet.net \
	  prefix=$(OUTPUT)/4_ \
	  tolist=$(OUTPUT)/outlist.lis \
	  onet=$(OUTPUT)/4_outputNet.net \
	  networkid=test \
	  description=test \
	  reference=true \
	  cubes=true \
	  cubelist=$(OUTPUT)/list.lis \
	  cubemeasure=true \
	  retain=true \
	  | $(SED) 's+.*% Processed.*Group+Group+' \
	  | grep -v "Processed" \
	  | grep -v "cnetextract:" \
	  > $(OUTPUT)/4_resultsLog.pvl;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/outlist.lis > $(OUTPUT)/4_toList.txt;
	$(RM) $(OUTPUT)/outlist.lis;
	$(RM) $(OUTPUT)/list.lis;
