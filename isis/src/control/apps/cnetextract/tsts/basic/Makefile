# Basic cnetextract test using noignore, nosinglemeasure, and nomeasureless filters
#
# Since a prefix is given, with the given filters, this test will make the 
# following reports:
#		preIgnoredPoints.txt 
#		preIgnoredMeasures.txt 
#		preSingleMeasurePoints.txt 
#		preMeasurelessPoints.txt 
#
# The following will be extracted
#		--points that are not ignored, with at least two measures
#		for each of these points the following measures will be extracted:
#			--reference measures (ignored or not)
# 			--non-ignored measures
APPNAME = cnetextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/list.lis;
	$(APPNAME) fromlist=$(OUTPUT)/list.lis \
	  cnet=$(INPUT)/cnet.net \
	  prefix=$(OUTPUT)/pre \
	  tolist=$(OUTPUT)/newList.lis \
	  onet=$(OUTPUT)/newNet.net \
	  networkid=new \
	  description=new \
	  noignore=true \
	  nosinglemeasure=true \
	  nomeasureless=true \
	  > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/newList.lis > $(OUTPUT)/newList.txt;
	$(RM) $(OUTPUT)/newList.lis;
	$(RM) $(OUTPUT)/list.lis;
