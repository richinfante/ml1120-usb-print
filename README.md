# USB ML1120

This is a simple user-space driver that allows printing to an Microline 1120 dot matrix printer. The following is tested on macOS, and with my ML1120, but it likely works on pretty much every system and for lots of printers.

To compile this for another printer, change the vendor / product IDs in usb_print.c. This simply reads the file and sends it in chunks over USB.

Since I'm on macOS, I used `brew install libusb`.

Then, run `make`

Now, you should be able to use it as described in the next section.

## Usage
```bash
# Printing a text file
usb_print hello_world.txt


# printing stdin
echo "Hello World\r\n" | usb_print /dev/stdin
```