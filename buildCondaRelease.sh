sed -e 's/-DKAKADU_INCLUDE_DIR=[^[:space:]]*//' recipe/build.sh
conda build recipe/ -c usgs-astrogeology -c conda-forge --no-test

