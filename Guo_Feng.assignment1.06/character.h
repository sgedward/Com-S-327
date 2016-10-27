#ifndef CHARACTER_H
# define CHARACTER_H

# include <stdint.h>

# include "dims.h"

typedef struct dungeon dungeon_t;
//typedef struct{} npc_t;
//typedef struct{} pc_t;
typedef struct dice_t dice_t;


typedef struct
{ 
}character_t;
/*
typedef struct character {
  char symbol;
  pair_t position;
  int32_t speed;
  uint32_t next_turn;
  uint32_t alive;
  */
  /* The priority queue is not stable.  It's nice to have a record of *
   * how many monsters have been created, and this sequence number    *
   * serves that purpose, but more importantly, prioritizing lower    *
   * sequence numbers ahead of higher ones guarantees that turn order *
   * is fair.  PC gets sequence number zero, and a global sequence,   *
   * stored in the dungeon, is incremented each time a NPC is         *
   * created and copied here then.                                    */
   /*
  uint32_t sequence_number;
  npc_t *npc;
  pc_t *pc;
} character_t;
*/


#ifdef __cplusplus
//#include <iostream>

class character
{
 
public:
  char symbol;
  pair_t position;
  int32_t speed;
  uint32_t next_turn;
  uint32_t alive;
  uint32_t sequence_number;
};
#endif

#ifdef __cplusplus
extern "C" {
#endif

uint32_t getNextTurn( character_t* c);
void  setNextTurn(character_t *c, uint32_t next_turn);
void IncrementTurn(character_t* c);
uint32_t getSeq( character_t *c);
void setSeq(character_t* c, uint32_t seq);
uint8_t getPositionX( character_t* c);
uint8_t getPositionY( character_t* c);
void setPosition(character_t* c, uint8_t x,uint8_t y);
uint32_t getAlive( character_t* c);
void    setAlive(character_t* c, uint32_t live);
char getSymbol( character_t* c);
void setSymbol(character_t* c, char ch);
int32_t getSpeed( character_t* c);
void setSpeed(character_t* c, int32_t speed);
    
    int32_t compare_characters_by_next_turn(const void *character1,
                                            const void *character2);
    uint32_t can_see(dungeon_t *d, character_t *voyeur, character_t *exhibitionist);
    void character_delete(void* v);

uint32_t can_see(dungeon_t *d, character_t *voyeur, character_t *exhibitionist);
//void character_delete(character_t* v);

#ifdef __cplusplus
}
#endif


#endif
