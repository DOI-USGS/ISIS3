APPNAME = gradient

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "Error Test A: Invalid GRADTYPE value" > $(OUTPUT)/error.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/error.cub \
	  gradtype=invalidtype \
	  2>> $(OUTPUT)/error.txt > /dev/null` ]; \
	  then true; \
	fi;
	echo -e "Error Test B: Invalid METHOD value" >> $(OUTPUT)/error.txt;
	if [ `$(APPNAME) \
	  from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/error.cub \
	  method=invalidmethod \
	  2>> $(OUTPUT)/error.txt > /dev/null` ]; \
	  then true; \
	fi;
