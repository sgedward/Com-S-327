#include <stdlib.h>

#include "string.h"
#include "dungeon.h"
#include "pc.h"
#include "utils.h"
#include "move.h"
#include "path.h"

void pc_delete(pc_t *pc)
{
  
  if (pc) {
   delete (character*) pc;
  }
  
  
}

int  cmp_pc(character_t* a){
    return ((pc*)a)->p;
}

void set_p(character_t* a){
    ((pc*)a)->p=1;
}



uint32_t pc_is_alive(dungeon_t *d)
{
  //return d->pc.alive;
    return getAlive(d->pc);

}

void place_pc(dungeon_t *d)
{

    setPosition(d->pc,rand_range(d->rooms->position[dim_x],
                                 (d->rooms->position[dim_x] +
                                  d->rooms->size[dim_x] - 1)),rand_range(d->rooms->position[dim_y],
                                                                         (d->rooms->position[dim_y] +
                                                                          d->rooms->size[dim_y] - 1)));
/*

  d->pc.position[dim_y] = rand_range(d->rooms->position[dim_y],
                                     (d->rooms->position[dim_y] +
                                     d->rooms->size[dim_y] - 1));

  d->pc.position[dim_x] = rand_range(d->rooms->position[dim_x],
                                     (d->rooms->position[dim_x] +
                                     d->rooms->size[dim_x] - 1));
 */
}

void config_pc(dungeon_t *d)
{
  memset(&d->pc, 0, sizeof (d->pc));
    pc* tmp=new pc();
  d->pc=(pc_t*) tmp;
  //d->pc.symbol = '@';
  setSymbol(d->pc,'@');
    set_p(d->pc);
  place_pc(d);

  //d->pc.speed = PC_SPEED;
  setSpeed(d->pc,PC_SPEED);
  //d->pc.next_turn = 0;
    setNextTurn(d->pc,0);

  //d->pc.alive = 1;
  setAlive(d->pc, 1);
  //d->pc.sequence_number = 0;
  setSeq(d->pc,0);

  //d->pc.pc = calloc(1, sizeof (*d->pc.pc));
  
  //d->pc.npc = NULL;

  d->character[getPositionY(d->pc)][getPositionX(d->pc)] = d->pc;

  
  int i,j;
    for(j=getPositionY(d->pc)-3; j<= getPositionY(d->pc)+3; j++){
      for(i=getPositionX(d->pc)-3; i<= getPositionX(d->pc)+3; i++){
        if(j > 0 && j < 19 && i > 0 && i < 79)
        d->dark[j][i]=d->map[j][i];
      }
    }
    

  dijkstra(d);
  dijkstra_tunnel(d);
}

uint32_t pc_next_pos(dungeon_t *d, pair_t dir)
{
  dir[dim_y] = dir[dim_x] = 0;

  /* Tunnel to the nearest dungeon corner, then move around in hopes *
   * of killing a couple of monsters before we die ourself.          */

  if (in_corner(d, d->pc)) {
    /*
    dir[dim_x] = (mapxy(d->pc.position[dim_x] - 1,
                        d->pc.position[dim_y]) ==
                  ter_wall_immutable) ? 1 : -1;
    */
    dir[dim_y] = (mapxy(getPositionX(d->pc),
                        getPositionY(d->pc) - 1) ==
                  ter_wall_immutable) ? 1 : -1;
  } else {
    dir_nearest_wall(d, d->pc, dir);
  }

  return 0;
}
