##### Sur votre machine ######
patch /tmp/linux-4.2.3/mm/swapfile.c < projet-linux-4.2.3.patch
cd /tmp/linux-4.2.3/
make -j 16

cd -
make
./qemu-run-externKernel.sh
