/*
 * tokens.h - cleri tokens element. (like token but can contain more tokens
 *            in one element)
 *
 * author       : Jeroen van der Heijden
 * email        : jeroen@transceptor.technology
 * copyright    : 2016, Transceptor Technology
 *
 * changes
 *  - initial version, 08-03-2016
 *  - refactoring, 17-06-2017
 */
#ifndef CLERI_TOKENS_H_
#define CLERI_TOKENS_H_

#include <stddef.h>
#include <inttypes.h>
#include <cleri/object.h>

/* typedefs */
typedef struct cleri_object_s cleri_object_t;

typedef struct cleri_tlist_s
{
    const char * token;
    size_t len;
    struct cleri_tlist_s * next;
} cleri_tlist_t;

typedef struct cleri_tokens_s
{
    uint32_t gid;
    char * tokens;
    char * spaced;
    struct cleri_tlist_s * tlist;
} cleri_tokens_t;

cleri_object_t * cleri_tokens(
        uint32_t gid,
        const char * tokens);

#endif /* CLERI_TOKENS_H_ */