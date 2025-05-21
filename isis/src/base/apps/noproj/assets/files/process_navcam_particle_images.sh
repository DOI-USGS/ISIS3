# filename: process_navcam_particle_images.sh
#!/bin/sh


# Short names refer to Figure S3 in the Supplemental Material accompanying Lauretta et al. 2019 Science v.366. 
# i.e., Figure S3 panels A and B, l=long, s=short
# https://www.science.org/doi/suppl/10.1126/science.aay3544/suppl_file/aay3544-lauretta-sm.pdf
 fig_s3_A_l="20190106T205618S792_ncm_L0"
 fig_s3_B_s="20190106T205611S433_ncm_L0"
 
 
# Download the image data.
# If needed, install the Unix program curl by running: conda install curl
 curl https://sbnarchive.psi.edu/pds4/orex/orex.tagcams/data_raw/orbit_a/20190106T205618S792_ncm_L0.fits -o "${fig_s3_A_l}.fits"
 curl https://sbnarchive.psi.edu/pds4/orex/orex.tagcams/data_raw/orbit_a/20190106T205611S433_ncm_L0.fits -o "${fig_s3_B_s}.fits"


# Download the high-resolution global shapemodel.
 curl "https://naif.jpl.nasa.gov/pub/naif/pds/pds4/orex/orex_spice/spice_kernels/dsk/bennu_g_00880mm_alt_obj_0000n00000_v020.{bds,xml}" -O


# # Move the shapemodel to the ISIS Data, OSIRIS-REx kernel directory.
# mv bennu_g_03170mm_spc_obj_0000n00000_v020.bds $ISISDATA/osirisrex/kernels/dsk/
 dem="$ISISDATA/osirisrex/kernels/dsk/bennu_g_00880mm_alt_obj_0000n00000_v020.bds"
 mv bennu_g_00880mm_alt_obj_0000n00000_v020.bds $ISISDATA/osirisrex/kernels/dsk/

# Import the NAVCAM images
 tagcams2isis from="${fig_s3_A_l}.fits"  to="${fig_s3_A_l}.cub" 
 tagcams2isis from="${fig_s3_B_s}.fits"  to="${fig_s3_B_s}.cub" 


# Apply SPICE 
# Make sure the BulletEngineSelect.pref file is in your working directory, or modify the path for the -pref argument accordingly.
 spiceinit from="${fig_s3_A_l}.cub" shape=user model="${dem}" -pref=$ISISROOT/BulletEngineSelect.pref 
 spiceinit from="${fig_s3_B_s}.cub" shape=user model="${dem}" -pref=$ISISROOT/BulletEngineSelect.pref 


# Correct camera distortion for the long exposure image, preserving off-body pixels.
 noproj from="${fig_s3_A_l}.cub" match="${fig_s3_A_l}.cub" to="${fig_s3_A_l}_noproj.cub" offbody=true offbodytrim=false interp=bilinear 


# Correct camera distortion for the short exposure image, using the noproj'ed long exposure
#   cube as the match. Do not preserve off-body pixels.
#   The specifications file is required because the MATCH cube has been noproj'ed and is
#   now defined as an "Ideal" camera.
 noproj from="${fig_s3_B_s}.cub" match="${fig_s3_A_l}_noproj.cub" to="${fig_s3_B_s}_noproj.cub" specs=specs.pvl offbody=false offbodytrim=true 


# Perform a rudimentary contrast stretch on the images. (This will not reproduce the figures in the article.)
 stretch from="${fig_s3_A_l}_noproj.cub" to="${fig_s3_A_l}_noproj_stretch.cub" pairs="0.0:0 254.0:0 1500.0:0.2 3800:0.5 4096:1.0"
 stretch from="${fig_s3_B_s}_noproj.cub" to="${fig_s3_B_s}_noproj_stretch.cub" pairs="0.0:0 156.0:0 884:1.0"


# Mosaic the short exposure image on top of the long exposure image to replace the 
#   saturated pixels with the visible surface of Bennu.
 cp "${fig_s3_A_l}_noproj_stretch.cub" particle_mosaic.cub
 handmos from="${fig_s3_B_s}_noproj_stretch.cub" mosaic=particle_mosaic.cub 

## View the resulting mosaic.
#  qview particle_mosaic.cub &