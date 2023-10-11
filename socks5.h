#ifndef _MZ_SOCKS5_H
#define _MZ_SOCKS5_H
// https://www.ietf.org/rfc/rfc1928.txt
#include <stdint.h>
#include "utils/app_def.h"

typedef enum {
  kMethodNoAuthentication = 0x00,
  kMethodGssapi = 0x01,
  kMethodUsernamePassword = 0x02,
  kMethodNoAcceptable = 0xFF
} Socks5Method;

typedef enum {
  kCmdConnect = 0x01,
  kCmdBind = 0x02,
  kCmdUdpAssociate = 0x03
} Socks5Command;

typedef enum {
  kAddrTypeIPv4 = 0x01,
  kAddrTypeDomain = 0x03,
  kAddrTypeIPv6 = 0x04
} Socks5AddressType;

typedef union {
  // 0x01
  uint32_t ipv4;
  // 0x03
  char domain[256];
  // 0x04
  uint8_t ipv6[16];
} AddrVariable;

// typedef struct {
// Socks5Header header;
// } Socks5Request;

// 1. client -> server ask auth method
//+----+----------+----------+
//|VER | NMETHODS | METHODS  |
//+----+----------+----------+
//| 1  |    1     | 1 to 255 |
//+----+----------+----------+
typedef struct Socks5MethodSelect {
  uint8_t version;
  uint8_t nmethods;
  uint8_t methods[0];
} Socks5MethodSelect;

// 1. server -> client reply auth method
// +----+--------+
// |VER | METHOD |
// +----+--------+
// | 1  |   1    |
// +----+--------+
struct Socks5MethodSelectResponse {
  uint8_t version;
  uint8_t method;
};

//  +----+-----+-------+------+----------+----------+
//  |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
//  +----+-----+-------+------+----------+----------+
//  | 1  |  1  | X'00' |  1   | Variable |    2     |
//  +----+-----+-------+------+----------+----------+
typedef struct Socks5Request {
  uint8_t version;
  uint8_t command;
  uint8_t rsv;
  Socks5AddressType atyp;
  AddrVariable dst_addr;
  uint16_t dst_port;
} Socks5Request;
// o  VER    protocol version: X'05'
// o  CMD
//    o  CONNECT X'01'
//    o  BIND X'02'
//    o  UDP ASSOCIATE X'03'
// o  RSV    RESERVED
// o  ATYP   address type of following address
//    o  IP V4 address: X'01'
//    o  DOMAINNAME: X'03'
//    o  IP V6 address: X'04'
// o  DST.ADDR       desired destination address
// o  DST.PORT desired destination port in network octet
//    order

// +----+-----+-------+------+----------+----------+
// |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
// +----+-----+-------+------+----------+----------+
// | 1  |  1  | X'00' |  1   | Variable |    2     |
// +----+-----+-------+------+----------+----------+
typedef enum Socks5ReplyStatus {
  kReplySuccess = 0x00,
  kReplyGeneralFailure = 0x01,
  kReplyConnectionNotAllowedByRuleset = 0x02,
  kReplyNetworkUnreachable = 0x03,
  kReplyHostUnreachable = 0x04,
  kReplyConnectionRefused = 0x05,
  kReplyTTLExpired = 0x06,
  kReplyCommandNotSupported = 0x07,
  kReplyAddressTypeNotSupported = 0x08
} Socks5ReplyStatus;

// o  VER    protocol version: X'05'
// o  REP    Reply field:
//    o  X'00' succeeded
//    o  X'01' general SOCKS server failure
//    o  X'02' connection not allowed by ruleset
//    o  X'03' Network unreachable
//    o  X'04' Host unreachable
//    o  X'05' Connection refused
//    o  X'06' TTL expired
//    o  X'07' Command not supported
//    o  X'08' Address type not supported
//    o  X'09' to X'FF' unassigned
// o  RSV    RESERVED  must be :0x0
// o  ATYP   address type of following address
//       o  IP V4 address: X'01'
//       o  DOMAINNAME: X'03'
//       o  IP V6 address: X'04'
// o  BND.ADDR       server bound address
// o  BND.PORT       server bound port in network octet order
typedef struct Socks5Response {
  uint8_t version;
  Socks5ReplyStatus reply;
  uint8_t rsv;
  Socks5AddressType atyp;
  AddrVariable dst_addr;
  uint16_t dst_port;
} Socks5Response;


// CONNECT
// BIND
// UDP ASSOCIATE
void DebugRequest(Socks5Request *request);
void DebugResposne(Socks5Response *response);

void ipv4_to_string(uint32_t ipv4, char *str, int len) ;
void run_socks5(MzOpts *opts);
#endif
