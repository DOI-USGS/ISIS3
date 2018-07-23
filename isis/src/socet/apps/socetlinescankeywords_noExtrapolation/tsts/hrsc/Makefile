APPNAME = socetlinescankeywords_noExtrapolation

include $(ISISROOT)/make/isismake.tsts

#Use sed to lower output precision to 5 decimal places in output txt files

commands:
#Test HRSC image with constant exposure rate. H1235 nd2, Vallis Marineris.
	$(APPNAME) from=$(INPUT)/h1235_0001_nd2_crop.8bit.cub \
	  to=$(OUTPUT)/h1235_0001_nd2_crop_keywords.txt \
	  hrsc_nadircentertime=158030857.34364 > /dev/null;
	cat $(OUTPUT)/h1235_0001_nd2_crop_keywords.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_h1235_0001_nd2_crop_keywords.txt
	$(RM) $(OUTPUT)/h1235_0001_nd2_crop_keywords.txt

#Test HRSC image with variable exposure rate. H0894 nd2, Nanedi Vallis.
	$(APPNAME) from=$(INPUT)/h0894_0000_nd2_crop.8bit.cub \
	  to=$(OUTPUT)/h0894_0000_nd2_crop_keywords.txt \
	  hrsc_nadircentertime=149781543.44398 > /dev/null;
	cat $(OUTPUT)/h0894_0000_nd2_crop_keywords.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_h0894_0000_nd2_crop_keywords.txt
	$(RM) $(OUTPUT)/h0894_0000_nd2_crop_keywords.txt

#Test HRSC image with exactly two exposure rates throughout image. H4235 s22, Gale Crater.
	$(APPNAME) from=$(INPUT)/h4235_0001_s22_crop.8bit.cub \
	  to=$(OUTPUT)/h4235_0001_s22_crop_keywords.txt \
	  hrsc_nadircentertime=230605329.82915 > /dev/null;
	cat $(OUTPUT)/h4235_0001_s22_crop_keywords.txt | \
	 sed 's/\([0-9]\.[0-9][0-9][0-9][0-9][0-9]\)\([0-9]*\)/\1/g' > \
	 $(OUTPUT)/sed_h4235_0001_s22_crop_keywords.txt
	$(RM) $(OUTPUT)/h4235_0001_s22_crop_keywords.txt
