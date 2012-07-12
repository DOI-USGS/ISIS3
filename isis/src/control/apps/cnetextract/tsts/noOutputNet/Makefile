# Test cnetextract when no output control net is created.
# No points or measures are extracted.
# No TOLIST file will be created.
# 
# The following reports will be created:
#     outIgnoreMeasures.txt
#     outIgnorePoints.txt (Notice every point in this file is in the given pointlist) 
APPNAME = cnetextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	  cnet=$(INPUT)/cnet.net \
	  prefix=$(OUTPUT)/out \
	  tolist=$(OUTPUT)/emptyOut.lis \
	  onet=$(OUTPUT)/emptyOut.net \
	  networkid=new \
	  description=new \
	  noignore=true \
	  pointlist=$(INPUT)/points.lis \
	  | $(SED) 's+.*% Processed.*#+#+' \
	  | grep -v "Processed" \
	  | grep -v "Working" \
	  > $(OUTPUT)/resultsLog.pvl;
	$(RM) $(OUTPUT)/list.lis;

