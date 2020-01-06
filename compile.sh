echo -e "\e[32m* Create build folder *\e[0m"
mkdir -p build
echo -e "\e[32m* Create bin folder *\e[0m"
mkdir -p bin
cd build
echo -e "\e[32m* Run CMake *\e[0m"
cmake ..
echo -e "\e[32m* Compile the app *\e[0m"
make
echo -e "\e[32m* Copy executable *\e[0m"
cp bin/MainProject ../bin/image_thread
cd ..
echo -e "\e[32m* Clean build *\e[0m"
rm -rf build
