APPNAME = skypt

include $(ISISROOT)/make/isismake.tsts

commands:
	 $(APPNAME) from=$(INPUT)/m0402852.cub \
	  to=$(OUTPUT)/skyptTemp.txt \
	  format=flat \
	  type=sky \
	  ra=322.34523648672 \
	  dec=53.787370722722 > /dev/null;
	$(APPNAME) from=$(INPUT)/lub0428b.253.cub \
	  format=flat SAMP=15 LINE=15 \
	  TO=$(OUTPUT)/skyptTemp2.txt > /dev/null; 
	cut -d , -f 2- $(OUTPUT)/skyptTemp.txt >& $(OUTPUT)/skypt_1.txt;
	cut -d , -f 2- $(OUTPUT)/skyptTemp2.txt >& $(OUTPUT)/skypt_2.txt;
	rm $(OUTPUT)/skyptTemp.txt;
	rm $(OUTPUT)/skyptTemp2.txt;
