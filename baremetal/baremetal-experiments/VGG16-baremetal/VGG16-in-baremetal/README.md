# Running Vgg16 in keystone environment
These are the steps to build vgg16 in keystone which is non docker but having docker configurations.
# 1. Copy the code
```
cd keystone/sdk/examples/hello/eapp
```
Paste all the files related to vgg16 in the hello/eapp
```
cd ..
```
Change the CMakeLists.txt with CMakeLists.txt provided within the repo into hello directory 
# 2. Making the package
```
cd ../../../build
make hello-package
cp examples/hello/hello.ke ./overlay/root
make image
./scripts/run-qemu.sh
```
Result should show lines:
Welcome to Buildroot
```
buildroot login:
```
Login as `root` with the password `sifive`.
You can exit QEMU by `ctrl-a + x` or using `poweroff` command.
# 5. To run package
```
insmod keystone-driver.ko
./hello.ke
```

