docker run --rm -v "$PWD":/usr/src/myapp -w /usr/src/myapp rikorose/gcc-cmake:gcc-5  sh -c "rm -rf build &&mkdir build&&cd build&&cmake ..&&make -j 4"
