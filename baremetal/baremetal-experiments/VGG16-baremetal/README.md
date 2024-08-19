# VGG16-baremetal-RISCV-AND-QEMU
From https://github.com/RamyaBoggaram/VGG16-baremetal-RISCV-AND-QEMU

These are the steps to build vgg16 in baremetal RISCV and QEMU configurations. To build the toolchains in local machines follow [these instructions](../../baremetal-setup.md)
# 1. Build baremetal binary
Build the VGG16 binary using the files in [the VGG16-in-baremetal folder](VGG16-in-baremetal)
```
cd directory_name
make
```
# 2. Start riscv-ubuntu live server
```
/usr/bin/qemu-system-riscv64 -machine virt -m 4G -smp cpus=2 -nographic     -bios /usr/lib/riscv64-linux-gnu/opensbi/generic/fw_jump.bin     -kernel /usr/lib/u-boot/qemu-riscv64_smode/u-boot.bin     -netdev user,id=net0,hostfwd=tcp::5555-:22     -device virtio-net-device,netdev=net0     -drive file=disk,format=raw,if=virtio     -device virtio-rng-pci
```
or
# 2. Start sifive server:
```
qemu-system-riscv64 -machine virt -nographic -m 2048 -smp 4 -bios /usr/lib/riscv64-linux-gnu/opensbi/generic/fw_jump.bin -kernel /usr/lib/u-boot/qemu-riscv64_smode/uboot.elf -netdev user,id=net0,hostfwd=tcp::5555-:22 -device virtio-net-device,netdev=net0 -device virtio-rng-pci -drive file=ubuntu-22.04.4-preinstalled-server-riscv64.img,format=raw,if=virtio
```
# 3. Upload binary
```
scp  -P 5555 ./vgg16 ubuntu@localhost:~/
```
# 4. Inside the server after uploading the binary run the binary using
```
./vgg16
```
