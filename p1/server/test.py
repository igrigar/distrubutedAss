#!/usr/bin/env python

import sys
import socket
import time

TCP_IP = '127.0.0.1'
TCP_PORT = 5005
BUFFER_SIZE = 1024

def send(service, usr):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((TCP_IP, TCP_PORT))
    if service != 'debug':
        s.send(service +'\n')
    s.send(str(usr) + '\n')
    print 'RET: ' + s.recv(BUFFER_SIZE)
    s.close()

def main():
    while True:
        service, usr = raw_input('$> ').strip().split(' ')
        send(service, usr)


if __name__ == '__main__':
    main()

