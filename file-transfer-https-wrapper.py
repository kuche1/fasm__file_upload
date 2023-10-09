#! /usr/bin/env python3

import argparse
import socket
import os
import ssl
import threading
import time
import sys

LISTEN = 5

# speed limiter

DOWNLOAD_SPEED = 1024 * 1024 * 5 # 5 MiB/s
DOWNLOAD_SLEEP = 0.05 # seconds
DOWNLOAD_CHUNK = int(DOWNLOAD_SPEED * DOWNLOAD_SLEEP)

UPLOAD_SPEED = 1024 * 1024 * 5 # 5 MiB/s
UPLOAD_SLEEP = 0.05 # seconds
UPLOAD_CHUNK = int(UPLOAD_SPEED * UPLOAD_SLEEP)

HERE = os.path.dirname(os.path.realpath(__file__))
SSL_KEYFILE  = os.path.join(HERE, 'certs', 'private.key')
SSL_CERTFILE = os.path.join(HERE, 'certs', 'certificate.crt')

def redirect_traffic(con, addr, server_port):
    try:
        redirect_traffic2(con, addr, server_port)
    finally:
        con.shutdown(socket.SHUT_RDWR)
        con.close()

def redirect_traffic2(client, client_addr, server_port):
    client.setblocking(False)

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.connect(('localhost', server_port))
    server.setblocking(False)

    next_upload = 0
    next_download = 0

    while True:
        
        if time.time() >= next_download:
            try:
                data = client.recv(DOWNLOAD_CHUNK)
            except BlockingIOError:
                pass
            except ssl.SSLWantReadError:
                pass
            else:
                next_download = time.time() + DOWNLOAD_SLEEP * (len(data) / DOWNLOAD_CHUNK)

                server.setblocking(True)
                server.sendall(data)
                server.setblocking(False)

        if time.time() >= next_upload:
            try:
                data = server.recv(UPLOAD_CHUNK)
            except BlockingIOError:
                pass
            else:
                if len(data) == 0:
                    break

                next_upload = time.time() + UPLOAD_SLEEP * (len(data) / UPLOAD_CHUNK)

                client.setblocking(True)
                client.sendall(data)
                client.setblocking(False)
                time.sleep(UPLOAD_SLEEP)
        
        # TODO ideally there would be a sleep here

    print('done')

def main_connection_accepter(sock, server_port):
    while True:
        try:
            main_connection_accepter2(sock, server_port)
        except:
            pass

def main_connection_accepter2(sock, server_port):
    while True:
        conn, addr = sock.accept()
        threading.Thread(target=redirect_traffic, args=[conn, addr, server_port]).start()

def main(server_port, wrapper_port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock = ssl.wrap_socket(
        sock, 
        keyfile=SSL_KEYFILE,
        certfile=SSL_CERTFILE,
        server_side=True,
    )

    sock.bind(('', wrapper_port))
    sock.listen(LISTEN)

    threading.Thread(target=main_connection_accepter, args=[sock, server_port]).start()

    try:
        while True:
            time.sleep(12345)
    except KeyboardInterrupt:
        sys.exit(69)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='ssl wrapper for non-ssl servers')
    parser.add_argument('server_port', type=int, help='port of non-ssl server')
    parser.add_argument('wrapper_port', type=int, help='port for the ssl wrapper')
    args = parser.parse_args()

    main(args.server_port, args.wrapper_port)
