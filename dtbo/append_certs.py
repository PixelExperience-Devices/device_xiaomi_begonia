#!/usr/bin/env python

import sys
import argparse
import os
from shutil import copyfile


def padding_file(input_file, align_num):
    """
    Fill 0 to make input_file's size a multiple of align_num.
    """
    filesize = os.stat(input_file).st_size
    file1 = open(input_file, 'ab+')
    padding = filesize % align_num
    if padding != 0:
        padding = align_num - padding
        for _ in range(padding):
            file1.write("\x00")
    file1.close()

def append_file(img_file, cert_file):
    """
    Append provided cert_file to the end of target img_file.
    """
    padding_file(img_file, 16)
    file1 = open(img_file, 'ab+')
    file2 = open(cert_file, 'rb')
    file1.write(file2.read())
    file1.close()
    file2.close()

def main():
    parser = argparse.ArgumentParser(description='Sign an image with provided der certs')
    parser.add_argument('--image', type=str, help='Target image')
    parser.add_argument('--cert1', type=str, help='Certificate - cert1.der')
    parser.add_argument('--cert2', type=str, help='Certificate - cert2.der')
    parser.add_argument('--output', type=str, help='Output image')
    args = parser.parse_args()

    copyfile(args.image, args.output)
    append_file(args.output, args.cert1)
    append_file(args.output, args.cert2)

if __name__ == '__main__':
    main()
