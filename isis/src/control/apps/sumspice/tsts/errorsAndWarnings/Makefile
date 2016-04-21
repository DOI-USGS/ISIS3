# Tests errors thrown by the sumspice program
# Error: Neither FROM file nor FROMLIST entered
# Error: Neither SUMFILE file nor SUMFILELIST entered
#
APPNAME = sumspice

include $(ISISROOT)/make/isismake.tsts

commands:
	# copy cube to output directory since the input cube is altered by this program
	echo -e "No cubes given" >> $(OUTPUT)/error.txt;
	if [ `$(APPNAME) \
	  sumfile=$(INPUT)/N2395699394.SUM \
	  2>> $(OUTPUT)/error.txt \
	  > /dev/null` ]; \
	then true; \
	fi;
	echo -e "\nNo sumfiles given" >> $(OUTPUT)/error.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/st_2395699394_v.lev0.cub \
	  2>> $(OUTPUT)/error.txt \
	  > /dev/null` ]; \
	then true; \
	fi;
