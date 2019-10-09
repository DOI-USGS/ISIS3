APPNAME = ckwriter

include $(ISISROOT)/make/isismake.tsts

commands:
	# Steup and create output CK from ckwriter
	$(CP) $(INPUT)/*.cub $(OUTPUT)
	ls -1 $(OUTPUT)/*.cub > $(OUTPUT)/files.lis;

	# It is necessary to spiceinit the input cubes fixing all the kernels except for the sclks.
	# All but the last 10 sclks are cleaned up from the kernels area, and a changing sclk
	# is unlikely to affect the resulting position / pointing (unlike the other kernels, which
	# are fixed for the purposes of this test.)
	spiceinit from=$(OUTPUT)/M103189611LE.cub \
	ls='$$base/kernels/lsk/naif0012.tls' \
	pck='($$base/kernels/pck/pck00009.tpc,$$lro/kernels/pck/moon_080317.tf,$$lro/kernels/pck/moon_assoc_me.tf)' \
	tspk='($$lro/kernels/tspk/moon_pa_de421_1900-2050.bpc,$$lro/kernels/tspk/de421.bsp)' \
	ik='$$lro/kernels/ik/lro_lroc_v18.ti' \
	ck='($$lro/kernels/ck/lrolc_2009181_2009213_v10.bc, $$lro/kernels/ck/moc42r_2009181_2009213_v14.bc)' \
	spk='$$lro/kernels/spk/fdf29r_2009182_2009213_v01.bsp' \
	fk='$$lro/kernels/fk/lro_frames_2014049_v01.tf' \
	iak='$$lro/kernels/iak/lro_instrumentAddendum_v04.ti' \
	shape=user \
	model='$$base/dems/ldem_128ppd_Mar2011_clon180_radius_pad.cub' > /dev/null;

	spiceinit from=$(OUTPUT)/M136241759RE.cub
	ls='$$base/kernels/lsk/naif0012.tls' \
	pck='($$base/kernels/pck/pck00009.tpc,$$lro/kernels/pck/moon_080317.tf,$$lro/kernels/pck/moon_assoc_me.tf)' \
	tspk='($$lro/kernels/tspk/moon_pa_de421_1900-2050.bpc,$$lro/kernels/tspk/de421.bsp)' \
	ik='$$lro/kernels/ik/lro_lroc_v18.ti' \
	ck='($$lro/kernels/ck/lrolc_2010212_2010244_v03.bc,$$lro/kernels/ck/moc42r_2010212_2010244_v04.bc)' \
	spk='$$lro/kernels/spk/fdf29r_2010213_2010244_v50.bsp' \
	fk='$$lro/kernels/fk/lro_frames_2014049_v01.tf' \
	iak='$$lro/kernels/iak/lro_instrumentAddendum_v04.ti' \
	shape=user \
	model='$$base/dems/ldem_128ppd_Mar2011_clon180_radius_pad.cub' > /dev/null;

	$(APPNAME) FROMLIST=$(OUTPUT)/files.lis TO=$(OUTPUT)/lroc.bc > /dev/null;
	$(RM) $(OUTPUT)/files.lis

	#  Only use 12 digits in the output of tabledump due to precision issues
	# do for each cube
	getkey from=$(OUTPUT)/M136241759RE.cub objnam=isiscube grpname=Kernels keyword=InstrumentPointing keyindex=2 > $(OUTPUT)/extra.ck;
	spiceinit from=$(OUTPUT)/M136241759RE.cub ck=$(OUTPUT)/lroc.bc \
          extra=`cat $(OUTPUT)/extra.ck` > /dev/null;
	tabledump from=$(OUTPUT)/M136241759RE.cub to=$(OUTPUT)/M136241759RE.dat \
	  name=InstrumentPointing > /dev/null;
	cat $(OUTPUT)/M136241759RE.dat \
	  | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/M136241759RE.txt;

	getkey from=$(OUTPUT)/M103189611LE.cub objnam=isiscube grpname=Kernels keyword=InstrumentPointing keyindex=2 > $(OUTPUT)/extra.ck;
	spiceinit from=$(OUTPUT)/M103189611LE.cub ck=$(OUTPUT)/lroc.bc \
          extra=`cat $(OUTPUT)/extra.ck` > /dev/null;
	tabledump from=$(OUTPUT)/M103189611LE.cub to=$(OUTPUT)/M103189611LE.dat \
	  name=InstrumentPointing > /dev/null;
	cat $(OUTPUT)/M103189611LE.dat \
	  | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/M103189611LE.txt;

	$(RM) $(OUTPUT)/M103189611LE.dat;
	$(RM) $(OUTPUT)/M103189611LE.cub;
	$(RM) $(OUTPUT)/M136241759RE.dat;
	$(RM) $(OUTPUT)/M136241759RE.cub;
	$(RM) $(OUTPUT)/lroc.bc;
	$(RM) $(OUTPUT)/extra.ck;



