#include "move.h"

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "dungeon.h"
#include "heap.h"
#include "move.h"
#include "npc.h"
#include "pc.h"
#include "character.h"
#include "utils.h"
#include "path.h"

void do_combat(dungeon_t *d, character_t *atk, character_t *def)
{
  setAlive(def,0);
  if (cmp_pc(def) != 1) {
    d->num_monsters--;
  }
}

void move_character(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("MOVE");
    refresh();
    sleep(1);
     */
  if (charpair(next) &&
      ((next[dim_y] != getPositionY(c)) ||
       (next[dim_x] != getPositionX(c)))) {
    do_combat(d, c, charpair(next));
  } else {
    /* No character in new position. */

    d->character[getPositionY(c)][getPositionX(c)] = NULL;
      setPosition(c,next[dim_x],next[dim_y]);
    d->character[getPositionY(c)][getPositionX(c)] = c;
  }
}

//// not modified yet!!
void do_moves(dungeon_t *d,char *order)
{
  pair_t next;
  character_t *c;

  /* Remove the PC when it is PC turn.  Replace on next call.  This allows *
   * use to completely uninit the heap when generating a new level without *
   * worrying about deleting the PC.                                       */

  if (pc_is_alive(d)) {
    heap_insert(&d->next_turn, d->pc);
  }

  while (pc_is_alive(d) && (cmp_pc((c = heap_remove_min(&d->next_turn))))!=1) {
      
      /*
      printw("npc_mov");
      refresh();
      sleep(2);
       */
    if (!(getAlive(c))) {
       /*
        printw("INSIDE");
        refresh();
        sleep(2);
        */
      if (d->character[getPositionY(c)][getPositionX(c)] == c) {
        d->character[getPositionY(c)][getPositionX(c)] = NULL;
      }
      if (cmp_pc(c) !=1) {
        character_delete(c);
      }
      continue;
    }

 //   c->next_turn += (1000 / c->speed);
      IncrementTurn(c);
      /*
      printw("Next");
      refresh();
      sleep(2);
      */
      
      
    npc_next_pos(d, c, next);
   
    move_character(d, c, next);
    
    
    heap_insert(&d->next_turn, c);
  }

  if (pc_is_alive(d) && cmp_pc(c)== 1) {
      /*
      printw("pc_mov");
      refresh();
      sleep(2);
       */
    if(*order =='a'){
    pc_next_pos(d, next);
    next[dim_x] += getPositionX(c);
    next[dim_y] += getPositionY(c);
    
  }
  else{
    switch(*order){
      case '7':
      if(getPositionX(c)-1 > 0 && getPositionY(c)-1 > 0){
      if(d->hardness[getPositionY(c)-1][getPositionX(c)-1] == 0){
          next[dim_x]=getPositionX(c)-1;
          next[dim_y]=getPositionY(c)-1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;

      case 'y':
      if(getPositionX(c)-1 > 0 && getPositionY(c)-1 > 0){
      if(d->hardness[getPositionY(c)-1][getPositionX(c)-1] == 0){
          next[dim_x]=getPositionX(c)-1;
          next[dim_y]=getPositionY(c)-1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;

      case '8':
      if(getPositionY(c)-1 > 0){
      if(d->hardness[getPositionY(c)-1][getPositionX(c)] == 0){
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c)-1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;

           case 'k':
      if(getPositionY(c)-1 > 0){
      if(d->hardness[getPositionY(c)-1][getPositionX(c)] == 0){
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c)-1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;

       case '9':
      if(getPositionY(c)-1 > 0 && getPositionX(c)+1 < 80){
      if(d->hardness[getPositionY(c)-1][getPositionX(c)+1] == 0){
          next[dim_x]=getPositionX(c)+1;
          next[dim_y]=getPositionY(c)-1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;

          case 'u':
      if(getPositionY(c)-1 > 0 && getPositionX(c)+1 < 80){
      if(d->hardness[getPositionY(c)-1][getPositionX(c)+1] == 0){
          next[dim_x]=getPositionX(c)+1;
          next[dim_y]=getPositionY(c)-1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;

      case '6':
      if(getPositionX(c)+1 < 80){
      if(d->hardness[getPositionY(c)][getPositionX(c)+1] == 0){
          next[dim_x]=getPositionX(c)+1;
          next[dim_y]=getPositionY(c);
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;

      case 'l':
      if(getPositionX(c)+1 < 80){
      if(d->hardness[getPositionY(c)][getPositionX(c)+1] == 0){
          next[dim_x]=getPositionX(c)+1;
          next[dim_y]=getPositionY(c);
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;

      case '3':
      if(getPositionX(c)+1 < 80 && getPositionY(c)+1 <20){
      if(d->hardness[getPositionY(c)+1][getPositionX(c)+1] == 0){
          next[dim_x]=getPositionX(c)+1;
          next[dim_y]=getPositionY(c)+1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;


      case 'n':
      if(getPositionX(c)+1 < 80 && getPositionY(c)+1 <20){
      if(d->hardness[getPositionY(c)+1][getPositionX(c)+1] == 0){
          next[dim_x]=getPositionX(c)+1;
          next[dim_y]=getPositionY(c)+1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;

      case '2':
      if(getPositionY(c)+1 <20){
      if(d->hardness[getPositionY(c)+1][getPositionX(c)] == 0){
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c)+1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;


      case 'j':
      if( getPositionY(c)+1 <20){
      if(d->hardness[getPositionY(c)+1][getPositionX(c)] == 0){
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c)+1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;



      case '1':
      if(getPositionY(c)+1 <20 && getPositionY(c)-1 > 0){
      if(d->hardness[getPositionY(c)+1][getPositionX(c)-1] == 0){
          next[dim_x]=getPositionX(c)-1;
          next[dim_y]=getPositionY(c)+1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;

           case 'b':
      if(getPositionY(c)+1 <20 && getPositionY(c)-1 > 0){
      if(d->hardness[getPositionY(c)+1][getPositionX(c)-1] == 0){
          next[dim_x]=getPositionX(c)-1;
          next[dim_y]=getPositionY(c)+1;
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;


            case '4':
      if(getPositionX(c)-1 > 0){
      if(d->hardness[getPositionY(c)][getPositionX(c)-1] == 0){
          next[dim_x]=getPositionX(c)-1;
          next[dim_y]=getPositionY(c);
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;


            case 'h':
      if(getPositionX(c)-1 > 0){
      if(d->hardness[getPositionY(c)][getPositionX(c)-1] == 0){
          next[dim_x]=getPositionX(c)-1;
          next[dim_y]=getPositionY(c);
        }
        else{
          next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
        }
      }
      else{
        next[dim_x]=getPositionX(c);
          next[dim_y]=getPositionY(c);
      }
          break;
            
            break;
            
        case 32:
            next[dim_x]=getPositionX(c);
            next[dim_y]=getPositionY(c);
            break;

    }
   }
  }

 int i,j;
    for(j=getPositionY(d->pc)-3; j<= getPositionY(d->pc)+3; j++){
      for(i=getPositionX(d->pc)-3; i<= getPositionX(d->pc)+3; i++){
        if(j > 0 && j < 19 && i > 0 && i < 79)
        d->dark[j][i]=d->map[j][i];
      }
    }
    

     if (mappair(next) <= ter_floor) {
      mappair(next) = ter_floor_hall;
    }
    move_character(d, c, next);

    dijkstra(d);
    dijkstra_tunnel(d);

    /*
    printw("OUT");
    refresh();
    sleep(2);
     */
  //  c->next_turn += (1000 / c->speed);
    IncrementTurn(c);
  }


void dir_nearest_wall(dungeon_t *d, character_t *c, pair_t dir)
{
  dir[dim_x] = dir[dim_y] = 0;

  if (getPositionX(c) != 1 && getPositionX(c) != DUNGEON_X - 2) {
    dir[dim_x] = (getPositionX(c) > DUNGEON_X - getPositionX(c) ? 1 : -1);
  }
  if (getPositionY(c) != 1 && getPositionY(c) != DUNGEON_Y - 2) {
    dir[dim_y] = (getPositionY(c) > DUNGEON_Y - getPositionY(c) ? 1 : -1);
  }
}

uint32_t in_corner(dungeon_t *d, character_t *c)
{
  uint32_t num_immutable;

  num_immutable = 0;

  num_immutable += (mapxy(getPositionX(c) - 1,
                          getPositionY(c)    ) == ter_wall_immutable);
  num_immutable += (mapxy(getPositionX(c) + 1,
                          getPositionY(c)    ) == ter_wall_immutable);
  num_immutable += (mapxy(getPositionX(c)    ,
                          getPositionY(c) - 1) == ter_wall_immutable);
  num_immutable += (mapxy(getPositionX(c)    ,
                          getPositionY(c) + 1) == ter_wall_immutable);

  return num_immutable > 1;
}

void interface(){

}

uint32_t move_pc(dungeon_t *d, uint32_t dir)
{
  return 1;
}
