# This tests the exceptions thrown by findfeatures
#   A: "Matching ALGORITHM/ALGOSPECFILE must be provided!"
#   B: "Matching ALGORITHM/ALGOSPECFILE must be provided!"
#   C: "Must provide both a FROM/FROMLIST and MATCH cube or image filename"
#   D: "No control points found!!"
#   E: "NO MATCHES WERE FOUND!!!" (untested)
#   F: "Shucks! Insufficient matches were found" (untested)
APPNAME = findfeatures

include $(ISISROOT)/make/isismake.tsts

commands:
	# TEST A: Not providing an algorithm/algospec and getting info
	#         i.e. listspec=yes
	echo -e "Error Test A: " > $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  listspec=yes \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	# TEST B: Not providing an algorithm/algospec and trying to find features
	echo -e "Error Test B: " >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/Sub4-AS15-M-0583_msk_crop.cub \
	  match=$(INPUT)/EW0211981114G.lev1.cub \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	# TEST C: Not providing input image
	echo -e "Error Test C: " >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  algorithm=brisk/brisk \
	  match=$(INPUT)/EW0211981114G.lev1.cub \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;
	# TEST D: No control points found during matching 
	echo -e "Error Test D: " >> $(OUTPUT)/error_temp.txt;
	if [[ `$(APPNAME) \
	  from=$(INPUT)/Sub4-AS15-M-0583_msk_crop.cub \
	  match=$(INPUT)/EW0211981114G.lev1.cub \
	  algorithm=brisk/brisk \
	  onet=$(OUTPUT)/net_temp.net \
	  2>> $(OUTPUT)/error_temp.txt \
	  > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;

	$(RM) $(OUTPUT)/error_temp.txt;
	$(RM) $(OUTPUT)/output.cub;
