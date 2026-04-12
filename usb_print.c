#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string.h>

// vendor ID for ML1120 Dot Matrix Printer
#define VENDOR_ID  0x06bc
#define PRODUCT_ID 0x0148

/**
* This program reads a file and sends its contents to the ML1120 printer over USB.
* Requires libusb-1.0 library to be installed.
* Usage: ./usb_print [--print-line-numbers] <file>
*/
int main(int argc, char *argv[]) {
    int print_line_numbers = 0;
    char *filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--print-line-numbers") == 0) {
            print_line_numbers = 1;
        } else if (!filename) {
            filename = argv[i];
        } else {
            fprintf(stderr, "Usage: %s [--print-line-numbers] <file>\n", argv[0]);
            return 1;
        }
    }

    if (!filename) {
        fprintf(stderr, "Usage: %s [--print-line-numbers] <file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        return 1;
    }

    libusb_context *ctx = NULL;
    libusb_device_handle *handle = NULL;

    int r = libusb_init(&ctx);
    if (r < 0) {
        fprintf(stderr, "libusb init failed\n");
        return 1;
    }

    handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);
    if (!handle) {
        fprintf(stderr, "Cannot find device\n");
        return 1;
    }

    libusb_detach_kernel_driver(handle, 0);

    if (libusb_claim_interface(handle, 0) != 0) {
        fprintf(stderr, "Cannot claim interface\n");
        return 1;
    }

    unsigned char endpoint_out = 0x01; // output endpoint

    // send file line by line, optionally prefixing line numbers
    int transferred;
    char line[4096];
    unsigned char buffer[4128];
    int line_num = 1;
    while (fgets(line, sizeof(line), file)) {
        int len;
        if (print_line_numbers) {
            len = snprintf((char *)buffer, sizeof(buffer), "%d: %s", line_num, line);
        } else {
            len = snprintf((char *)buffer, sizeof(buffer), "%s", line);
        }
        if (len > 0) {
            r = libusb_bulk_transfer(handle, endpoint_out, buffer, len, &transferred, 1000);
        }
        line_num++;
    }

    // print status of last transfer
    if (r == 0) {
        printf("Sent %d bytes\n", transferred);
    } else {
        fprintf(stderr, "Transfer failed: %d\n", r);
    }

    // cleanup
    libusb_release_interface(handle, 0);
    libusb_close(handle);
    libusb_exit(ctx);

    return 0;
}