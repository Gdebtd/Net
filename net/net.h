#ifndef NET_H
#define NET_H

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"
#include "net_connection.h"
#include "net_client.h"
#include "net_server.h"

enum class CustomMsgTypes : uint32_t {
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage
};

/* Every uint32_t is a typename
uint8_t -> 0
uint16_t -> 1
uint32_t -> 2
uint64_t -> 3
float -> 4
double -> 5
*/

#endif /* NET_H */