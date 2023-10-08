#! /usr/bin/env python3

import argparse
import socket
import os
import ssl
import threading
import time
import sys

LISTEN = 5

CHUNK = 1024

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

    client_done_sending = False

    while True:
        
        if not client_done_sending:
            try:
                data = client.recv(CHUNK)
            except BlockingIOError:
                pass
            except ssl.SSLWantReadError:
                client_done_sending = True
            else:
                server.setblocking(True)
                server.sendall(data)
                server.setblocking(False)

        try:
            data = server.recv(CHUNK)
        except BlockingIOError:
            pass
        else:
            if len(data) == 0:
                break
            client.setblocking(True)
            client.sendall(data)
            client.setblocking(False)
        
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
