# Installing the RISCV toolchain and QEMU
From https://github.com/RamyaBoggaram/Baremetal-RISCV-and-QEMU-setup-using-ubuntu-server
## 1. Install QEMU dependencies
```
sudo apt-get install ninja-build
mkdir riscv-ubuntu/
```
## 2. Install QEMU
```
git clone https://github.com/qemu/qemu
cd qemu
git checkout 0021c4765a6b83e5b09409b75d50c6caaa6971b9
./configure --target-list=riscv64-softmmu
make -j $(nproc)
make install
cd ..
```
## 3. RISCV toolchain
```
git clone https://github.com/riscv-collab/riscv-gnu-toolchain --recursive
cd riscv-gnu-toolchain
git checkout 1a36b5dc44d71ab6a583db5f4f0062c2a4ad963b
```
## 4. Prefix parameter specifying the installation location
```
./configure --prefix=/opt/riscv
sudo make linux -j $(nproc)

cd ..
```
# Installing Server
Follow either of the following sets of instructions to install a virtual machine.
## Booting a preinstalled server image with QEMU
```
sudo apt-get update
sudo apt-get install opensbi qemu-system-misc u-boot-qemu
```
A preinstalled server image can be downloaded from https://cdimage.ubuntu.com/releases/24.04/release/.

First unpack the image
```
xz -dk ubuntu-24.04-preinstalled-server-riscv64.img.xz
```
(Up to Ubuntu 22.04. use the image for the SiFive HiFive Unmatched board, e.g. 'ubuntu-22.04.3-preinstalled-server-riscv64+unmatched.img'.)
Optionally, if you want a larger disk, you can expand the disk (the filesystem will be automatically resized too).
```
qemu-img resize -f raw ubuntu-24.04-preinstalled-server-riscv64.img +5G
```
```
qemu-system-riscv64 \
-machine virt -nographic -m 2048 -smp 4 \
-bios /usr/lib/riscv64-linux-gnu/opensbi/generic/fw_jump.bin \
-kernel /usr/lib/u-boot/qemu-riscv64_smode/uboot.elf \
-device virtio-net-device,netdev=eth0 -netdev user,id=eth0 \
-device virtio-rng-pci \
-drive file=ubuntu-24.04-preinstalled-server-riscv64.img,format=raw,if=virtio
```
Then login using ubuntu:ubuntu.

## Ubuntu installation of a RISC-V virtual machine using a server install image and QEMU
The Jammy release image is available at https://cdimage.ubuntu.com/releases/22.04.3/release/.

Download the image either using your web browser or with
```
wget https://cdimage.ubuntu.com/releases/22.04.3/release/ubuntu-22.04.4-live-server-riscv64.img.gz 
```
Installation:
Extract the image.
```
gzip -d ubuntu-22.04.4-live-server-riscv64.img.gz 
```
Create the disk image on which you will install Ubuntu. 16 GiB should be enough.
```
dd if=/dev/zero bs=1M of=disk count=1 seek=16383
```
Start the installer with:
```
/usr/bin/qemu-system-riscv64 -machine virt -m 4G -smp cpus=2 -nographic \
    -bios /usr/lib/riscv64-linux-gnu/opensbi/generic/fw_jump.bin \
    -kernel /usr/lib/u-boot/qemu-riscv64_smode/u-boot.bin \
    -netdev user,id=net0 \
    -device virtio-net-device,netdev=net0 \
    -drive file=noble-live-server-riscv64.img,format=raw,if=virtio \
    -drive file=disk,format=raw,if=virtio \
    -device virtio-rng-pci
```
When rebooting we have to remove the installer image. Otherwise the installer will restart.

Run Ubuntu:
To run your installed Ubuntu image use
```
/usr/bin/qemu-system-riscv64 -machine virt -m 4G -smp cpus=2 -nographic \
    -bios /usr/lib/riscv64-linux-gnu/opensbi/generic/fw_jump.bin \
    -kernel /usr/lib/u-boot/qemu-riscv64_smode/u-boot.bin \
    -netdev user,id=net0 \
    -device virtio-net-device,netdev=net0 \
    -drive file=disk,format=raw,if=virtio \
    -device virtio-rng-pci
```
