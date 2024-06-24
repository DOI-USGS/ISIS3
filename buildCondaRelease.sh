sed -i'.backup' "s/-DKAKADU_INCLUDE_DIR=[^[:space:]]*[[:space:]]//" recipe/build.sh
conda build recipe/ -c conda-forge -c usgs-astrogeology
mv recipe/build.sh.backup recipe/build.sh


