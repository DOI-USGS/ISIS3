APPNAME = socetlinescankeywords

include $(ISISROOT)/make/isismake.tsts

#Use sed to lower output precision to 5 decimal places in output txt files

commands:
#Test HiRISE noproj'ed (Ideal Camera) cube
	$(APPNAME) from=$(INPUT)/ESP_028449_1700_REDmos_hijitreged.balance.noproj.8bit.cub \
	  to=$(OUTPUT)/ESP_028449_1700_REDmos_hijitreged_keywords.txt > /dev/null;
	cat $(OUTPUT)/ESP_028449_1700_REDmos_hijitreged_keywords.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_ESP_028449_1700_REDmos_hijitreged_keywords.txt
	$(RM) $(OUTPUT)/ESP_028449_1700_REDmos_hijitreged_keywords.txt

#Test HiRISE hijitter'ed (Ideal Camera) cube
	$(APPNAME) from=$(INPUT)/ESP_026339_1700_RED_DEJITTERED.8bit.cub \
	  to=$(OUTPUT)/ESP_026339_1700_RED_DEJITTERED_keywords.txt > /dev/null;
	cat $(OUTPUT)/ESP_026339_1700_RED_DEJITTERED_keywords.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_ESP_026339_1700_RED_DEJITTERED_keywords.txt
	$(RM) $(OUTPUT)/ESP_026339_1700_RED_DEJITTERED_keywords.txt

#Test HiRISE CCD (HiRISE Camera) cube
	$(APPNAME) from=$(INPUT)/ESP_028449_1700_RED5.balance.cub \
	  to=$(OUTPUT)/ESP_028449_1700_RED5_keywords.txt > /dev/null;
	cat $(OUTPUT)/ESP_028449_1700_RED5_keywords.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_ESP_028449_1700_RED5_keywords.txt
	$(RM) $(OUTPUT)/ESP_028449_1700_RED5_keywords.txt
