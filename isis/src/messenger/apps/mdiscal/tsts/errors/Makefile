APPNAME = mdiscal

include $(ISISROOT)/make/isismake.tsts

commands:
	# multiband input cube
	if [[ `$(APPNAME) \
	  from=$(INPUT)/multiband.cub \
	  to=$(OUTPUT)/multiband.cub 2>> $(OUTPUT)/errors_temp.txt` ]]; \
	then true; \
	fi;
	# samples < 3
	if [[ `$(APPNAME) \
	  from=$(INPUT)/samps.cub \
	  to=$(OUTPUT)/samps.cub &>> $(OUTPUT)/errors_temp.txt` ]]; \
	then true; \
	fi;
	# Unlutted = false
	if [[ `$(APPNAME) \
	  from=$(INPUT)/lutted.cub \
	  to=$(OUTPUT)/lutted.cub &>> $(OUTPUT)/errors_temp.txt` ]]; \
	then true; \
	fi;
	$(SED) 's+\[.*/input/+\[input/+' $(OUTPUT)/errors_temp.txt > $(OUTPUT)/errors.txt;
	$(RM) $(OUTPUT)/errors_temp.txt;
