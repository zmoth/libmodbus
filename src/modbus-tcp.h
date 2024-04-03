/*
 * Copyright © Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MODBUS_TCP_H
#define MODBUS_TCP_H

#include "modbus.h"

MODBUS_BEGIN_DECLS

#if defined(_WIN32) && !defined(__CYGWIN__)
/* Win32 with MinGW, supplement to <errno.h> */
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#if !defined(ECONNRESET)
#define ECONNRESET WSAECONNRESET
#endif
#if !defined(ECONNREFUSED)
#define ECONNREFUSED WSAECONNREFUSED
#endif
#if !defined(ETIMEDOUT)
#define ETIMEDOUT WSAETIMEDOUT
#endif
#if !defined(ENOPROTOOPT)
#define ENOPROTOOPT WSAENOPROTOOPT
#endif
#if !defined(EINPROGRESS)
#define EINPROGRESS WSAEINPROGRESS
#endif
#endif

#define MODBUS_TCP_DEFAULT_PORT 502
#define MODBUS_TCP_SLAVE        0xFF

/* Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
 * TCP MODBUS ADU = 253 bytes + MBAP (7 bytes) = 260 bytes
 */
#define MODBUS_TCP_MAX_ADU_LENGTH 260

MODBUS_EXPORT modbus_t *modbus_new_tcp(const char *ip_address, int port);
MODBUS_EXPORT int modbus_tcp_listen(modbus_t *ctx, int nb_connection);
MODBUS_EXPORT int modbus_tcp_accept(modbus_t *ctx, int *s);

MODBUS_EXPORT modbus_t *modbus_new_tcp_pi(const char *node, const char *service);
MODBUS_EXPORT int modbus_tcp_pi_listen(modbus_t *ctx, int nb_connection);
MODBUS_EXPORT int modbus_tcp_pi_accept(modbus_t *ctx, int *s);

MODBUS_END_DECLS

#endif /* MODBUS_TCP_H */
