# This test will test cropping cubes using user-entered Jitter analysis file
#
# The first test uses an unaltered jitter file.
#
# The second test uses a jitter file that was created from the first by 
# removing the sample = 0 = line entry and reordering the times. To make 
# sure that the first and last lines are grabbed properly.
APPNAME = hicrop

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped1.cub \
	source=jitterfile \
        jitter=$(INPUT)/PSP_008469_2040.txt > /dev/null;
	catlab from=$(OUTPUT)/cropped1.cub to=$(OUTPUT)/croppedLabel1.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped2.cub \
	source=jitterfile \
        jitter=$(INPUT)/PSP_008469_2040_noZeroOffset.txt > /dev/null;
	catlab from=$(OUTPUT)/cropped2.cub to=$(OUTPUT)/croppedLabel2.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped3.cub \
	source=jitterfile \
        jitter=$(INPUT)/PSP_008469_2040_zeroSampOffset.txt > /dev/null;
	catlab from=$(OUTPUT)/cropped3.cub to=$(OUTPUT)/croppedLabel3.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/PSP_008469_2040_RED6.balance.cub \
	ck=$(INPUT)/mro_sc_psp_080513_080519.bc \
	sclk=$(INPUT)/MRO_SCLKSCET.00049.65536.tsc \
	lsk=$(INPUT)/naif0010.tls \
	to=$(OUTPUT)/cropped4.cub \
	source=jitterfile \
        jitter=$(INPUT)/PSP_008469_2040_zeroLineOffset.txt > /dev/null;
	catlab from=$(OUTPUT)/cropped4.cub to=$(OUTPUT)/croppedLabel4.pvl > /dev/null;
	cubediff from=$(OUTPUT)/cropped1.cub from2=$(OUTPUT)/cropped2.cub to=$(OUTPUT)/cubediff12.txt > /dev/null;
	cubediff from=$(OUTPUT)/cropped2.cub from2=$(OUTPUT)/cropped3.cub to=$(OUTPUT)/cubediff23.txt > /dev/null;
	cubediff from=$(OUTPUT)/cropped3.cub from2=$(OUTPUT)/cropped4.cub to=$(OUTPUT)/cubediff34.txt > /dev/null;
