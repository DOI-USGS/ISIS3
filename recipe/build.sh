CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"  # To enable conda builds targeting macos 13 from macos 15
mkdir build
cd build
export ISISROOT=$PWD
cmake -GNinja -DbuildDocs=OFF -Dpybindings=ON -DbuildTests=OFF -DBUILD_CORE_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PREFIX ../isis
ninja install

# Set ISISROOT on environment activation. ISISROOT is always the environment
# prefix, and every ISIS program fails without it (it cannot find
# IsisPreferences), so it is set here on activation the same way the gdal and
# proj packages set GDAL_DATA and PROJ_DATA. This lets ISIS run right after
# "conda activate" with no manual step. The data-location variables (ISISDATA,
# ISISTESTDATA, ALESPICEROOT) are left to isisVarInit.py, since those are user
# choices. isisVarInit.py still works and sets ISISROOT to the same value.
mkdir -p "$PREFIX/etc/conda/activate.d" "$PREFIX/etc/conda/deactivate.d"

cat > "$PREFIX/etc/conda/activate.d/isisroot.sh" <<'EOF'
#!/usr/bin/env sh
export ISISROOT=$CONDA_PREFIX
EOF
cat > "$PREFIX/etc/conda/deactivate.d/isisroot.sh" <<'EOF'
#!/usr/bin/env sh
unset ISISROOT
EOF

cat > "$PREFIX/etc/conda/activate.d/isisroot.csh" <<'EOF'
#!/usr/bin/env csh
setenv ISISROOT $CONDA_PREFIX
EOF
cat > "$PREFIX/etc/conda/deactivate.d/isisroot.csh" <<'EOF'
#!/usr/bin/env csh
unsetenv ISISROOT
EOF

cat > "$PREFIX/etc/conda/activate.d/isisroot.fish" <<'EOF'
#!/usr/bin/env fish
set -gx ISISROOT $CONDA_PREFIX
EOF
cat > "$PREFIX/etc/conda/deactivate.d/isisroot.fish" <<'EOF'
#!/usr/bin/env fish
set -e ISISROOT
EOF
