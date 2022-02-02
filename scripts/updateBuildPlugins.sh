CMAKE_BINARY_DIR=../build/
> $CMAKE_BINARY_DIR/lib/Camera.plugin
for i in find ../isis/src/*/objs/*/Camera.plugin
  do
    if test -f "$i"; then
        cat "$i" >> $CMAKE_BINARY_DIR/lib/Camera.plugin
    fi
done