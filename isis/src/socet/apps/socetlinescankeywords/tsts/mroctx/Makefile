APPNAME = socetlinescankeywords

include $(ISISROOT)/make/isismake.tsts

#Use sed to lower output precision to 5 decimal places in output txt files

commands:
	$(APPNAME) from=$(INPUT)/P01_001602_1701_XI_09S091W.8bit.cub \
	  to=$(OUTPUT)/P01_001602_1701_XI_09S091W_keywords.txt > /dev/null;
	cat $(OUTPUT)/P01_001602_1701_XI_09S091W_keywords.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_P01_001602_1701_XI_09S091W_keywords.txt
	$(RM) $(OUTPUT)/P01_001602_1701_XI_09S091W_keywords.txt
