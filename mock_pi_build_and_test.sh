cmake -DMOCK_RPI=on -B build
cd build
make
chmod +x propulsion_test
./propulsion_test
