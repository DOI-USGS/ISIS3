APPNAME = ckwriter 
APPNAME2 = spkwriter

LEISA = lsb_0034933739_0x53c_sci_1.cub
MVIC = mc3_0034948318_0x536_sci_1.cub
LORRI = lor_0034732622_0x630_sci_1.fit.cub

include $(ISISROOT)/make/isismake.tsts

# Test New Horizons camera models by spiciniting using default kernels, running campt on a few 
# points then generating new kernels using ckwriter and spkwriter. Re-spiceiniting with those 
# kernels, running campt on the same (s,l) and comparing the output values. 

# This test uses images of Jupiter. 

# The three camera models tested and associated images are: 
# MVIC = mc3_0034948318_0x536_sci_1.cub
# LEISA = lsb_0034933739_0x53c_sci_1.cub
# LORRI = lor_0034732622_0x630_sci_1.fit.cub

commands:
	#cp images from input to output for modification
	cp $(INPUT)/$(LEISA) $(OUTPUT) > /dev/null;
	cp $(INPUT)/$(MVIC) $(OUTPUT) > /dev/null;
	cp $(INPUT)/$(LORRI) $(OUTPUT) > /dev/null;

	#initial spiceinit 
	spiceinit $(TSTARGS) from=$(OUTPUT)/$(MVIC) > /dev/null;
	spiceinit $(TSTARGS) from=$(OUTPUT)/$(LEISA) > /dev/null;
	spiceinit $(TSTARGS) from=$(OUTPUT)/$(LORRI) > /dev/null;

	#initial campt

	#9 points from MVIC image
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh.pvl sample=2570 line=1000 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh.pvl sample=1144 line=688 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh.pvl sample=2542 line=618 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh.pvl sample=3798 line=557 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh.pvl sample=3727 line=1033 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh.pvl sample=2664 line=1023 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh.pvl sample=1398 line=1083 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh.pvl sample=1843 line=1479 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh.pvl sample=2380 line=1894 > /dev/null; 

	# 9 points from LEISA image
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh.pvl sample=150 line=480 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh.pvl sample=40 line=210 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh.pvl sample=221 line=123 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh.pvl sample=169 line=409 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh.pvl sample=221 line=568 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh.pvl sample=24 line=668 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh.pvl sample=106 line=710 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh.pvl sample=207 line=849 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh.pvl sample=242 line=933 > /dev/null;

	# 9 points from LORRI image
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh.pvl sample=345 line=580 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh.pvl sample=305 line=545 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh.pvl sample=336 line=594 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh.pvl sample=390 line=574 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh.pvl sample=370 line=533 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh.pvl sample=348 line=631 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh.pvl sample=299 line=588 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh.pvl sample=347 line=572 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh.pvl sample=372 line=611 > /dev/null; 

	#ckwriter from spiceinit'd image to ckernel
	$(APPNAME) from=$(OUTPUT)/$(MVIC) TO=$(OUTPUT)/ck_mvic.bc > /dev/null; 
	$(APPNAME) from=$(OUTPUT)/$(LEISA) TO=$(OUTPUT)/ck_leisa.bc > /dev/null; 
	$(APPNAME) from=$(OUTPUT)/$(LORRI) TO=$(OUTPUT)/ck_lorri.bc > /dev/null; 

	#spkwriter from spiceinit'd image to spkernel 
	$(APPNAME2) $(TSTARGS) from=$(OUTPUT)/$(MVIC) TO=$(OUTPUT)/spk_mvic.bsp > /dev/null;
	$(APPNAME2) $(TSTARGS) from=$(OUTPUT)/$(LEISA) TO=$(OUTPUT)/spk_leisa.bsp > /dev/null;
	$(APPNAME2) $(TSTARGS) from=$(OUTPUT)/$(LORRI) TO=$(OUTPUT)/spk_lorri.bsp > /dev/null;

	#re-spiceinit inputs to generated kernels
	spiceinit $(TSTARGS) from=$(OUTPUT)/$(MVIC)  CK=$(OUTPUT)/ck_mvic.bc SPK=$(OUTPUT)/spk_mvic.bsp > /dev/null;   
	spiceinit $(TSTARGS) from=$(OUTPUT)/$(LEISA) CK=$(OUTPUT)/ck_leisa.bc SPK=$(OUTPUT)/spk_leisa.bsp > /dev/null; 
	spiceinit $(TSTARGS) from=$(OUTPUT)/$(LORRI) CK=$(OUTPUT)/ck_lorri.bc SPK=$(OUTPUT)/spk_lorri.bsp > /dev/null;
 
	#final campt

# 9 points from MVIC image
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh2.pvl sample=2570 line=1000 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh2.pvl sample=1144 line=688 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh2.pvl sample=2542 line=618 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh2.pvl sample=3798 line=557 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh2.pvl sample=3727 line=1033 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh2.pvl sample=2664 line=1023 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh2.pvl sample=1398 line=1083 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh2.pvl sample=1843 line=1479 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(MVIC) to=$(OUTPUT)/nh2.pvl sample=2380 line=1894 > /dev/null; 

# 9 points from LEISA image
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh2.pvl sample=150 line=480 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh2.pvl sample=40 line=210 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh2.pvl sample=221 line=123 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh2.pvl sample=169 line=409 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh2.pvl sample=221 line=568 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh2.pvl sample=24 line=668 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh2.pvl sample=106 line=710 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh2.pvl sample=207 line=849 > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/$(LEISA) to=$(OUTPUT)/nh2.pvl sample=242 line=933 > /dev/null;

# 9 points form LORRI image
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh2.pvl sample=345 line=580 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh2.pvl sample=305 line=545 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh2.pvl sample=336 line=594 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh2.pvl sample=390 line=574 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh2.pvl sample=370 line=533 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh2.pvl sample=348 line=631 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh2.pvl sample=299 line=588 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh2.pvl sample=347 line=572 > /dev/null; 
	campt $(TSTARGS) from=$(OUTPUT)/$(LORRI) to=$(OUTPUT)/nh2.pvl sample=372 line=611 > /dev/null; 

	#compare initial and final pvl
	pvldiff $(TSTARGS) from=$(OUTPUT)/nh.pvl from2=$(OUTPUT)/nh2.pvl diff=$(INPUT)/tolerances.pvl \
	to=$(OUTPUT)/diff.pvl > /dev/null;

	#cleanup
	$(RM) $(OUTPUT)/$(LEISA) > /dev/null;
	$(RM) $(OUTPUT)/$(MVIC) > /dev/null;
	$(RM) $(OUTPUT)/$(LORRI) > /dev/null; 
	$(RM) $(OUTPUT)/nh.pvl > /dev/null;
	$(RM) $(OUTPUT)/nh2.pvl > /dev/null;
	$(RM) $(OUTPUT)/ck_mvic.bc > /dev/null;
	$(RM) $(OUTPUT)/ck_leisa.bc > /dev/null;
	$(RM) $(OUTPUT)/ck_lorri.bc > /dev/null;
	$(RM) $(OUTPUT)/spk_mvic.bsp > /dev/null; 
	$(RM) $(OUTPUT)/spk_leisa.bsp > /dev/null; 
	$(RM) $(OUTPUT)/spk_lorri.bsp > /dev/null; 

