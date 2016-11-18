# This tests the fromlist capabilities of pixel2map.
# The first test ensures that the proper output mosaic is created.
# The send test ensures that band selection works within a fromlist, and that it creates the
#   expected output (output identical to band 2 of the first test's output).
#
# @history 2016-11-18 Ian Humphrey - Added documentation to Makefile. Added TEST 2.
#                         References #4520.
APPNAME = pixel2map

include $(ISISROOT)/make/isismake.tsts

commands:
	# TEST 1
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/files.lis;
	$(APPNAME) fromtype=fromlist \
	           fromlist=$(OUTPUT)/files.lis \
	           to=$(OUTPUT)/combinedCubes.sinu.cub \
	           > /dev/null;
	$(RM) $(OUTPUT)/files.lis;
	# TEST 2
	# Test the fromlist with band selection (band 2).
	# Note the command below is adding +2 to each of the input cube listings, then redirecting. 
	$(LS) $(INPUT)/*.cub | xargs -i echo "{}+2" > $(OUTPUT)/files.lis;
	$(APPNAME) fromtype=fromlist \
	           fromlist=$(OUTPUT)/files.lis \
	           to=$(OUTPUT)/combinedCubesBand2.sinu.cub \
	           > /dev/null;
	$(RM) $(OUTPUT)/files.lis;
