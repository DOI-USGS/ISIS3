# 2017-01-03 Jeannie Backer - Make sure that proper spice is added to labels.
#
# This test uses a full frame cube to verify that the cube has the proper labels after 
# 1. spiceinit with dsk
# 2. sumspice, updating times
# 3. spiceinit
# 4. sumspice, updating spice
 
include $(ISISROOT)/make/isismake.tsts

commands:
	amica2isis $(TSTARGS) from=$(INPUT)/st_2393680156_v.lbl \
	                      to=$(OUTPUT)/st_2393680156_v.cub \
	                      > /dev/null;
	catlab $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                  to=$(OUTPUT)/st_2393680156_v_label.pvl \
	                  > /dev/null;
	spiceinit $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                     ckpredict=true \
	                     spkpredict=true \
	                     shape=user \
	                     model='$$hayabusa/kernels/dsk/hay_a_amica_5_itokawashape_v1_0_512q.bds' \
	                     > /dev/null;
	catlab $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                  to=$(OUTPUT)/st_2393680156_v_label_init.pvl \
	                  > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                 to=$(OUTPUT)/st_2393680156_v_campt_init.pvl \
	                 > /dev/null;
	sumspice $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                    sumfile=$(INPUT)/N2391934788.SUM \
	                    update=times \
	                    > /dev/null;
	catlab $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                  to=$(OUTPUT)/st_2393680156_v_label_init_timeupdate.pvl \
	                  > /dev/null;
	spiceinit $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                     ckpredict=true \
	                     spkpredict=true \
	                     shape=user \
	                     model='$$hayabusa/kernels/dsk/hay_a_amica_5_itokawashape_v1_0_512q.bds' \
	                     > /dev/null;
	catlab $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                  to=$(OUTPUT)/st_2393680156_v_label_init_timeupdate_init.pvl \
	                  > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                 to=$(OUTPUT)/st_2393680156_v_campt_init_timeupdate_init.pvl \
					 sample=450 \
					 line=512 \
	                 > /dev/null;
	sumspice $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                    sumfile=$(INPUT)/N2391934788.SUM \
	                    update=spice \
	                    > /dev/null;
	catlab $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                  to=$(OUTPUT)/st_2393680156_v_label_init_timeupdate_init_spiceupdate.pvl \
	                  > /dev/null;
	campt $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                 to=$(OUTPUT)/st_2393680156_v_campt_init_timeupdate_init_spiceupdate.pvl \
					 sample=450 \
					 line=512 \
	                 > /dev/null;
	$(ECHO) "Output of getsn..." > $(OUTPUT)/st_2393680156_v--getsnOutput.txt
	getsn $(TSTARGS) from=$(OUTPUT)/st_2393680156_v.cub \
	                 >> $(OUTPUT)/st_2393680156_v--getsnOutput.txt
	                 > /dev/null;
