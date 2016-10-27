#ifndef PC_H
# define PC_H

# include <stdint.h>

# include "dims.h"




typedef struct dungeon dungeon_t;

typedef character_t pc_t;

#ifdef __cplusplus
//#include <iostream>

class pc : public character
{
	
public:
    int p;
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "path.h"
#include "move.h"
#include "character.h"
int  cmp_pc(character_t* a);
void pc_delete(pc_t *pc);
void set_p(pc_t* c);
void pc_delete(pc_t *pc);
uint32_t pc_is_alive(dungeon_t *d);
void config_pc(dungeon_t *d);
uint32_t pc_next_pos(dungeon_t *d, pair_t dir);
void place_pc(dungeon_t *d);

#ifdef __cplusplus
}
#endif



#endif
