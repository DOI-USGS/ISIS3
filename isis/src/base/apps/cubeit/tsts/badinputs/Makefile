#This test will print errors thrown by cubeit when given 
#input cubes of different sizes or nothing but TRAKCING bands
APPNAME = cubeit

include $(ISISROOT)/make/isismake.tsts

commands:
#    TEST A: pass in a list of cubes with different sizes
	echo -e "Different sized cubes:" > $(OUTPUT)/error_temp.txt;
	if [ `$(APPNAME) \
	  fromlist=$(INPUT)/input.lis \
	  to=$(OUTPUT)/error.cub \
	  2>> $(OUTPUT)/error_temp.txt > /dev/null` ]; \
	  then true; \
	fi;

#    TEST B: pass in a list of cubes with only TRACKING bands
	echo -e "Only TRACKING bands:" >> $(OUTPUT)/error_temp.txt;
	ls $(INPUT)/*TRACKING.cub >> $(OUTPUT)/tracking.lis;
	if [ `$(APPNAME) \
	  fromlist=$(OUTPUT)/tracking.lis \
	  to=$(OUTPUT)/error2.cub \
	  2>> $(OUTPUT)/error_temp.txt > /dev/null` ]; \
	  then true; \
	fi;

#    TEST C: pass in a cube with a TRACKING band but ill-formed BandBin Group
	echo -e "TRACKING and broken BandBin:" >> $(OUTPUT)/error_temp.txt;
	ls $(INPUT)/*.cub >> $(OUTPUT)/broken.lis;
	if [ `$(APPNAME) \
	  fromlist=$(OUTPUT)/broken.lis \
	  to=$(OUTPUT)/error3.cub \
	  2>> $(OUTPUT)/error_temp.txt > /dev/null` ]; \
	  then true; \
	fi;

#    Remove Paths
	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error_temp2.txt;
	$(SED) 's+\[/.*/output/+\[output/+' $(OUTPUT)/error_temp2.txt > $(OUTPUT)/error.txt;

#    Cleanup
	$(RM) $(OUTPUT)/error_temp.txt; 
	$(RM) $(OUTPUT)/error_temp2.txt; 
	$(RM) $(OUTPUT)/tracking.lis; 
	$(RM) $(OUTPUT)/broken.lis; 


