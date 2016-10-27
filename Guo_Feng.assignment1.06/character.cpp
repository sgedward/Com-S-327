#include <stdlib.h>

#include "character.h"
#include "heap.h"
#include "npc.h"
#include "pc.h"
#include "dungeon.h"


uint32_t getNextTurn( character_t* c){
  return  ((character*) c)->next_turn;
}

void setNextTurn(character_t* c,uint32_t next_turn){
  ((character*) c)->next_turn=next_turn;
}
void IncrementTurn(character_t* c){
    uint32_t tmp=((character *) c)->next_turn;
  ((character *) c)->next_turn =tmp+(1000 / ((character*) c)->speed);
}
uint32_t getSeq( character_t* c){
  return  ((character*) c)->sequence_number;
}

void setSeq(character_t* c, uint32_t seq){
    ((character*)c)->sequence_number=seq;
}

char getSymbol( character_t* c){
  return  ((character*) c)->symbol;
}

void setPosition(character_t* c, uint8_t x, uint8_t y){
   ((character*) c)->position[dim_y]=y;
    ((character*) c)->position[dim_x]=x;
}

void setSymbol(character_t* c, char ch){
   ((character*) c)->symbol=ch;
}


int32_t getSpeed( character_t* c){

  return  ((character*) c)->speed;
}


void setSpeed(character_t* c, int32_t speed){
 ((character*) c)->speed=speed;
}

uint32_t getAlive( character_t* c){
  return  ((character*) c)->alive;
}


void  setAlive(character_t* c, uint32_t live){
   ((character*) c)->alive=live;
}

uint8_t getPositionX( character_t* c){

  return  ((character*) c)->position[dim_x];
}

uint8_t getPositionY( character_t* c){

  return  ((character*) c)->position[dim_y];
}







char *print_character(const void *v)
{
   character_t *c = (character_t*) v;

  static char string[80];

  snprintf(string, 80, "%d:%d", getNextTurn((character_t*)c),getSeq((character_t*)c));

  return string;
}

void character_delete(void* v)
{
  /* The PC is never malloc()ed anymore, do don't attempt to free it here. */
  //character *c=new character();

    character* c;
    
    if (v) {
        c = (character*)v;
        
        delete  c;
    }

}

int32_t compare_characters_by_next_turn(const void *character1,
                                        const void *character2)
{
  int32_t diff;

  diff = ( getNextTurn((character_t*) character1)-
          getNextTurn((character_t*) character2));
  return diff ? diff : (getSeq((character_t*) character1) -
                        getSeq((character_t*) character2));
}

uint32_t can_see(dungeon_t *d, character_t *voyeur, character_t *exhibitionist)
{
  /* Application of Bresenham's Line Drawing Algorithm.  If we can draw *
   * a line from v to e without intersecting any walls, then v can see  *
   * e.  Unfortunately, Bresenham isn't symmetric, so line-of-sight     *
   * based on this approach is not reciprocal (Helmholtz Reciprocity).  *
   * This is a very real problem in roguelike games, and one we're      *
   * going to ignore for now.  Algorithms that are symmetrical are far  *
   * more expensive.                                                    */

  pair_t first, second;
  pair_t del, f;
  int16_t a, b, c, i;

  //first[dim_x] = voyeur->position[dim_x];
  first[dim_x] =getPositionX(voyeur);


  //first[dim_y] = voyeur->position[dim_y];
    first[dim_y] =getPositionY(voyeur);

//  second[dim_x] = exhibitionist->position[dim_x];
 second[dim_x] =getPositionX(exhibitionist);
  //second[dim_y] = exhibitionist->position[dim_y];
second[dim_y] =getPositionY(exhibitionist);

  if ((abs(first[dim_x] - second[dim_x]) > VISUAL_RANGE) ||
      (abs(first[dim_y] - second[dim_y]) > VISUAL_RANGE)) {
    return 0;
  }

  /*
  mappair(first) = ter_debug;
  mappair(second) = ter_debug;
  */

  if (second[dim_x] > first[dim_x]) {
    del[dim_x] = second[dim_x] - first[dim_x];
    f[dim_x] = 1;
  } else {
    del[dim_x] = first[dim_x] - second[dim_x];
    f[dim_x] = -1;
  }

  if (second[dim_y] > first[dim_y]) {
    del[dim_y] = second[dim_y] - first[dim_y];
    f[dim_y] = 1;
  } else {
    del[dim_y] = first[dim_y] - second[dim_y];
    f[dim_y] = -1;
  }

  if (del[dim_x] > del[dim_y]) {
    a = del[dim_y] + del[dim_y];
    c = a - del[dim_x];
    b = c - del[dim_x];
    for (i = 0; i <= del[dim_x]; i++) {
      if ((mappair(first) < ter_floor) && i && (i != del[dim_x])) {
        return 0;
      }
      /*      mappair(first) = ter_debug;*/
      first[dim_x] += f[dim_x];
      if (c < 0) {
        c += a;
      } else {
        c += b;
        first[dim_y] += f[dim_y];
      }
    }
    return 1;
  } else {
    a = del[dim_x] + del[dim_x];
    c = a - del[dim_y];
    b = c - del[dim_y];
    for (i = 0; i <= del[dim_y]; i++) {
      if ((mappair(first) < ter_floor) && i && (i != del[dim_y])) {
        return 0;
      }
      /*      mappair(first) = ter_debug;*/
      first[dim_y] += f[dim_y];
      if (c < 0) {
        c += a;
      } else {
        c += b;
        first[dim_x] += f[dim_x];
      }
    }
    return 1;
  }

  return 1;
}
