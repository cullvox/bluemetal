# Create the build folder if it doesn't exist
if [ ! -d "build" ]; then
  mkdir build
fi

cmake -S . -B build

make -C build