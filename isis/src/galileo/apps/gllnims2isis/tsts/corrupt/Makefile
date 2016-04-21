APPNAME = gllnims2isis

include $(ISISROOT)/make/isismake.tsts

commands:
	if [[ `$(APPNAME) \
	         from=$(INPUT)/32i001cr.qub \
	         core=$(OUTPUT)/32i001cr_core.cub \
	         suffix=$(OUTPUT)/32i001cr_suffix.cub  \
	         2> $(OUTPUT)/error_temp.txt \
	         > /dev/null` ]]; \
	then \
	  true; \
	fi;

	$(SED) 's+\[/.*/input/+\[input/+' $(OUTPUT)/error_temp.txt > $(OUTPUT)/error.txt;
	$(RM) $(OUTPUT)/error_temp.txt;
