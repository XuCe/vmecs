#include <string.h>
#include <stdlib.h>

#include "pub/err.h"

#include "common.h"

target_id_t *_target_id_new(byte_t type, uint16_t port)
{
    target_id_t *id = malloc(sizeof(*id));
    ASSERT(id, "out of mem");

    id->port = port;
    id->addr_type = type;

    return id;
}

target_id_t *target_id_new_ipv4(uint8_t addr[4], uint16_t port)
{
    target_id_t *id = _target_id_new(ADDR_TYPE_IPV4, port);
    memcpy(id->addr.ipv4, addr, sizeof(id->addr.ipv4));
    return id;
}

target_id_t *target_id_new_ipv6(uint8_t addr[16], uint16_t port)
{
    target_id_t *id = _target_id_new(ADDR_TYPE_IPV6, port);
    memcpy(id->addr.ipv6, addr, sizeof(id->addr.ipv6));
    return id;
}

target_id_t *target_id_new_domain(const char *domain, uint64_t port)
{
    target_id_t *id = _target_id_new(ADDR_TYPE_DOMAIN, port);
    id->addr.domain = strdup(domain);
    return id;
}

target_id_t *target_id_parse(const char *node, const char *service)
{
    target_id_t *id = _target_id_new(ADDR_TYPE_DOMAIN, atoi(service));
    id->addr.domain = strdup(node);
    return id;
}

struct addrinfo *target_id_resolve(target_id_t *target)
{
    char buf[256 + 1];
    char port[8];
    struct addrinfo hints, *res;

    switch (target->addr_type) {
        case ADDR_TYPE_DOMAIN:
            sprintf(buf, "%s", target->addr.domain);
            break;

        case ADDR_TYPE_IPV4:
            sprintf(buf, "%d.%d.%d.%d",
                    target->addr.ipv4[0], target->addr.ipv4[1],
                    target->addr.ipv4[2], target->addr.ipv4[3]);
            break;

        case ADDR_TYPE_IPV6:
            ASSERT(0, "not implemented");
            break;
    }

    sprintf(port, "%d", target->port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;

    if (getaddrinfo(buf, port, &hints, &res)) {
        perror("getaddrinfo");
        return NULL;
    }

    return res;
}

void target_id_free(target_id_t *target)
{
    if (target) {
        if (target->addr_type == ADDR_TYPE_DOMAIN) {
            free(target->addr.domain);
        }

        free(target);
    }
}