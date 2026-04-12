all:
	clang usb_print.c -o usb_print -I/opt/homebrew/include \
  -L/opt/homebrew/lib \
	-lusb-1.0