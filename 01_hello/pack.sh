mkdir -p iso/boot/grub
cp makeos.bin iso/boot/
cp grub.cfg iso/boot/grub/
grub-mkrescue -o makeos.iso iso
