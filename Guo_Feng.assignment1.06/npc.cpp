#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "utils.h"
#include "npc.h"
#include "dungeon.h"
#include "character.h"
#include "move.h"
#include "path.h"


npc_characteristics_t getcharacter( npc_t* c){

  return ((npc*) c)->characteristics;
}

void setcharacter(npc_t* c, npc_characteristics_t ch){

  ((npc*) c)->characteristics=ch;
}



uint32_t gethasseen( npc_t* c){

  return ((npc*) c)->have_seen_pc;
}

void sethasseen(npc_t* c, uint32_t seen){

   ((npc*) c)->have_seen_pc=seen;
}

uint8_t getlastknownX( npc_t* c){
  return ((npc*) c)->pc_last_known_position[dim_x];

}

uint8_t getlastknownY( npc_t* c){

return ((npc*) c)->pc_last_known_position[dim_y];

}

void setlastknown(npc_t* c, uint8_t x, uint8_t y){
  ((npc*) c)->pc_last_known_position[dim_x]=x;
  ((npc*) c)->pc_last_known_position[dim_y]=y;

}

void npc_delete(character_t *n)
{
  if (n) {
    free(n);
  }
}

void gen_monsters(dungeon_t *d, uint32_t nummon, uint32_t game_turn)
{
  uint32_t i;
  character_t* m;
  npc* tmp;
  uint32_t room;
  pair_t p;
  const static char symbol[] = "0123456789abcdef";

  d->num_monsters = nummon;

  for (i = 0; i < nummon; i++) {
   // m = malloc(sizeof (*m));
   // memset(m, 0, sizeof (*m));
    //tmp=new npc();
      npc* tmp=new npc();
      m=(npc_t*) tmp;

    do {
      room = rand_range(1, d->num_rooms - 1);
      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                            (d->rooms[room].position[dim_y] +
                             d->rooms[room].size[dim_y] - 1));
      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                            (d->rooms[room].position[dim_x] +
                             d->rooms[room].size[dim_x] - 1));
    } while (d->character[p[dim_y]][p[dim_x]]);
   // m->position[dim_y] = p[dim_y];
    setPosition(m,p[dim_x],p[dim_y]);

  //  m->position[dim_x] = p[dim_x];

   
    

  //  m->speed = rand_range(5, 20);
    setSpeed(m,rand_range(5,20));

    //m->next_turn = game_turn;
    setNextTurn(m,game_turn);

//    m->alive = 1;
    setAlive(m,1);
    //m->sequence_number = ++d->character_sequence_number;
    (d->character_sequence_number)++;
      setSeq(m,d->character_sequence_number);

    //m->pc = NULL;
      
    //m->npc = malloc(sizeof (*m->npc));
    
    //m->npc->characteristics = rand() & 0x0000000f;
    setcharacter(m,rand() & 0x0000000f);

//    m->symbol = symbol[m->npc->characteristics];
    setSymbol(m, symbol[getcharacter(m)]);
   // m->npc->have_seen_pc = 0;
    sethasseen(m,0);

    d->character[p[dim_y]][p[dim_x]] = m;

    heap_insert(&d->next_turn,m);
  }
}

void npc_next_pos_rand_tunnel(dungeon_t *d, character_t *c, pair_t next)
{
  pair_t n;
  union {
    uint32_t i;
    uint8_t a[4];
  } r;

  do {
    n[dim_y] = next[dim_y];
    n[dim_x] = next[dim_x];
    r.i = rand();
    if (r.a[0] > 85 /* 255 / 3 */) {
      if (r.a[0] & 1) {
        n[dim_y]--;
      } else {
        n[dim_y]++;
      }
    }
    if (r.a[1] > 85 /* 255 / 3 */) {
      if (r.a[1] & 1) {
        n[dim_x]--;
      } else {
        n[dim_x]++;
      }
    }
  } while (mappair(n) == ter_wall_immutable);

  if (hardnesspair(n) <= 60) {
    if (hardnesspair(n)) {
      hardnesspair(n) = 0;
      mappair(n) = ter_floor_hall;

      /* Update distance maps because map has changed. */
      dijkstra(d);
      dijkstra_tunnel(d);
    }

    next[dim_x] = n[dim_x];
    next[dim_y] = n[dim_y];
  } else {
    hardnesspair(n) -= 60;
  }
}

void npc_next_pos_rand(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("random");
    refresh();
    sleep(1);
     */
    
  pair_t n;
  union {
    uint32_t i;
    uint8_t a[4];
  } r;

  do {
      /*
      printw("RRR");
      refresh();
      sleep(1);
       */
    n[dim_y] = next[dim_y];
    n[dim_x] = next[dim_x];
    r.i = rand();
    if (r.a[0] > 85 /* 255 / 3 */) {
      if (r.a[0] & 1) {
        n[dim_y]--;
      } else {
        n[dim_y]++;
      }
    }
    if (r.a[1] > 85 /* 255 / 3 */) {
      if (r.a[1] & 1) {
        n[dim_x]--;
      } else {
        n[dim_x]++;
      }
    }
  } while (mappair(n) < ter_floor);

  next[dim_y] = n[dim_y];
  next[dim_x] = n[dim_x];
}

void npc_next_pos_line_of_sight(dungeon_t *d, character_t *c, pair_t next)
{
  pair_t dir;

  dir[dim_y] = getPositionY(d->pc) - getPositionY(c);
  dir[dim_x] = getPositionX(d->pc) - getPositionX(c);
  if (dir[dim_y]) {
    dir[dim_y] /= abs(dir[dim_y]);
  }
  if (dir[dim_x]) {
    dir[dim_x] /= abs(dir[dim_x]);
  }

  if (mapxy(next[dim_x] + dir[dim_x],
            next[dim_y] + dir[dim_y]) >= ter_floor) {
    next[dim_x] += dir[dim_x];
    next[dim_y] += dir[dim_y];
  } else if (mapxy(next[dim_x] + dir[dim_x], next[dim_y]) >= ter_floor) {
    next[dim_x] += dir[dim_x];
  } else if (mapxy(next[dim_x], next[dim_y] + dir[dim_y]) >= ter_floor) {
    next[dim_y] += dir[dim_y];
  }
}

void npc_next_pos_line_of_sight_tunnel(dungeon_t *d,
                                       character_t *c,
                                       pair_t next)
{
  pair_t dir;

  dir[dim_y] = getPositionY(d->pc) - getPositionY(c);
  dir[dim_x] = getPositionX(d->pc) - getPositionX(c);
  if (dir[dim_y]) {
    dir[dim_y] /= abs(dir[dim_y]);
  }
  if (dir[dim_x]) {
    dir[dim_x] /= abs(dir[dim_x]);
  }

  dir[dim_x] += next[dim_x];
  dir[dim_y] += next[dim_y];

  if (hardnesspair(dir) <= 60) {
    if (hardnesspair(dir)) {
      hardnesspair(dir) = 0;
      mappair(dir) = ter_floor_hall;

      /* Update distance maps because map has changed. */
      dijkstra(d);
      dijkstra_tunnel(d);
    }

    next[dim_x] = dir[dim_x];
    next[dim_y] = dir[dim_y];
  } else {
    hardnesspair(dir) -= 60;
  }
}

void npc_next_pos_gradient(dungeon_t *d, character_t *c, pair_t next)
{
  /* Handles both tunneling and non-tunneling versions */
  pair_t min_next;
  uint16_t min_cost;
  if (getcharacter((npc_t*)c) & NPC_TUNNEL) {
    min_cost = (d->pc_tunnel[next[dim_y] - 1][next[dim_x]] +
                (d->hardness[next[dim_y] - 1][next[dim_x]] / 60));
    min_next[dim_x] = next[dim_x];
    min_next[dim_y] = next[dim_y] - 1;
    if ((d->pc_tunnel[next[dim_y] + 1][next[dim_x]    ] +
         (d->hardness[next[dim_y] + 1][next[dim_x]] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] + 1][next[dim_x]] +
                  (d->hardness[next[dim_y] + 1][next[dim_x]] / 60));
      min_next[dim_x] = next[dim_x];
      min_next[dim_y] = next[dim_y] + 1;
    }
    if ((d->pc_tunnel[next[dim_y]    ][next[dim_x] + 1] +
         (d->hardness[next[dim_y]    ][next[dim_x] + 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y]][next[dim_x] + 1] +
                  (d->hardness[next[dim_y]][next[dim_x] + 1] / 60));
      min_next[dim_x] = next[dim_x] + 1;
      min_next[dim_y] = next[dim_y];
    }
    if ((d->pc_tunnel[next[dim_y]    ][next[dim_x] - 1] +
         (d->hardness[next[dim_y]    ][next[dim_x] - 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y]][next[dim_x] - 1] +
                  (d->hardness[next[dim_y]][next[dim_x] - 1] / 60));
      min_next[dim_x] = next[dim_x] - 1;
      min_next[dim_y] = next[dim_y];
    }
    if ((d->pc_tunnel[next[dim_y] - 1][next[dim_x] + 1] +
         (d->hardness[next[dim_y] - 1][next[dim_x] + 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] - 1][next[dim_x] + 1] +
                  (d->hardness[next[dim_y] - 1][next[dim_x] + 1] / 60));
      min_next[dim_x] = next[dim_x] + 1;
      min_next[dim_y] = next[dim_y] - 1;
    }
    if ((d->pc_tunnel[next[dim_y] + 1][next[dim_x] + 1] +
         (d->hardness[next[dim_y] + 1][next[dim_x] + 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] + 1][next[dim_x] + 1] +
                  (d->hardness[next[dim_y] + 1][next[dim_x] + 1] / 60));
      min_next[dim_x] = next[dim_x] + 1;
      min_next[dim_y] = next[dim_y] + 1;
    }
    if ((d->pc_tunnel[next[dim_y] - 1][next[dim_x] - 1] +
         (d->hardness[next[dim_y] - 1][next[dim_x] - 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] - 1][next[dim_x] - 1] +
                  (d->hardness[next[dim_y] - 1][next[dim_x] - 1] / 60));
      min_next[dim_x] = next[dim_x] - 1;
      min_next[dim_y] = next[dim_y] - 1;
    }
    if ((d->pc_tunnel[next[dim_y] + 1][next[dim_x] - 1] +
         (d->hardness[next[dim_y] + 1][next[dim_x] - 1] / 60)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] + 1][next[dim_x] - 1] +
                  (d->hardness[next[dim_y] + 1][next[dim_x] - 1] / 60));
      min_next[dim_x] = next[dim_x] - 1;
      min_next[dim_y] = next[dim_y] + 1;
    }
    if (hardnesspair(min_next) <= 60) {
      if (hardnesspair(min_next)) {
        hardnesspair(min_next) = 0;
        mappair(min_next) = ter_floor_hall;

        /* Update distance maps because map has changed. */
        dijkstra(d);
        dijkstra_tunnel(d);
      }

      next[dim_x] = min_next[dim_x];
      next[dim_y] = min_next[dim_y];
    } else {
      hardnesspair(min_next) -= 60;
    }
  } else {
    /* Make monsters prefer cardinal directions */
    if (d->pc_distance[next[dim_y] - 1][next[dim_x]    ] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]--;
      return;
    }
    if (d->pc_distance[next[dim_y] + 1][next[dim_x]    ] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]++;
      return;
    }
    if (d->pc_distance[next[dim_y]    ][next[dim_x] + 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_x]++;
      return;
    }
    if (d->pc_distance[next[dim_y]    ][next[dim_x] - 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_x]--;
      return;
    }
    if (d->pc_distance[next[dim_y] - 1][next[dim_x] + 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]--;
      next[dim_x]++;
      return;
    }
    if (d->pc_distance[next[dim_y] + 1][next[dim_x] + 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]++;
      next[dim_x]++;
      return;
    }
    if (d->pc_distance[next[dim_y] - 1][next[dim_x] - 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]--;
      next[dim_x]--;
      return;
    }
    if (d->pc_distance[next[dim_y] + 1][next[dim_x] - 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]++;
      next[dim_x]--;
      return;
    }
  }
}

static void npc_next_pos_00(dungeon_t *d, character_t *c, pair_t next)
{
  /* not smart; not telepathic; not tunneling; not erratic */
  if (can_see(d, c, d->pc)) {
 //  setlastknown(c,getPositionX(&d->pc),getlastknownY(&d->pc));
    setlastknown(c,getPositionX(d->pc),getPositionY(d->pc));
 //   //c->npc->pc_last_known_position[dim_x] = d->pc.position[dim_x];
    npc_next_pos_line_of_sight(d, c, next);
  } else {
    npc_next_pos_rand(d, c, next);
  }
}

static void npc_next_pos_01(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("1");
    refresh();
    sleep(1);
     */
    
  /*     smart; not telepathic; not tunneling; not erratic */
  if (can_see(d, c, d->pc)) {
   setlastknown(c,getPositionX(d->pc),getPositionY(d->pc));
    //c->npc->pc_last_known_position[dim_x] = d->pc.position[dim_x];
    //c->npc->have_seen_pc = 1;
    sethasseen(c,1);
    npc_next_pos_line_of_sight(d, c, next);
  } else if (gethasseen(c)) {
    npc_next_pos_line_of_sight(d, c, next);
  }

  if ((next[dim_x] == getlastknownX(c)) &&
      (next[dim_y] == getlastknownY(c))) {
    //c->npc->have_seen_pc = 0;
         sethasseen(c,0);
  }
}

static void npc_next_pos_02(dungeon_t *d, character_t *c, pair_t next)
{   /*
    printw("2");
    refresh();
    sleep(1);
     */
  /* not smart;     telepathic; not tunneling; not erratic */
 setlastknown(c,getPositionX(d->pc),getPositionY(d->pc));
  //c->npc->pc_last_known_position[dim_x] = d->pc.position[dim_x];
  npc_next_pos_line_of_sight(d, c, next);
}

static void npc_next_pos_03(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("3");
    refresh();
    sleep(1);
     */
  /*     smart;     telepathic; not tunneling; not erratic */
  npc_next_pos_gradient(d, c, next);
}

static void npc_next_pos_04(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("4");
    refresh();
    sleep(1);
     */
  /* not smart; not telepathic;     tunneling; not erratic */
  if (can_see(d, c, d->pc)) {
   setlastknown(c,getPositionX(d->pc),getPositionY(d->pc));
    //c->npc->pc_last_known_position[dim_x] = d->pc.position[dim_x];
    npc_next_pos_line_of_sight(d, c, next);
  } else {
    npc_next_pos_rand_tunnel(d, c, next);
  }
}

static void npc_next_pos_05(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("5");
    refresh();
    sleep(1);
     */
  /*     smart; not telepathic;     tunneling; not erratic */
  if (can_see(d, c, d->pc)) {
   setlastknown(c,getPositionX(d->pc),getPositionY(d->pc));
    //c->npc->pc_last_known_position[dim_x] = d->pc.position[dim_x];
    //c->npc->have_seen_pc = 1;
   sethasseen(c,1);
    npc_next_pos_line_of_sight(d, c, next);
  } else if (gethasseen(c)) {
    npc_next_pos_line_of_sight_tunnel(d, c, next);
  }

  if ((next[dim_x] ==getlastknownX(c) ) &&
      (next[dim_y] == getlastknownY(c))) {
    //c->npc->have_seen_pc = 0;
    sethasseen(c,0);
  }
}

static void npc_next_pos_06(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("6");
    refresh();
    sleep(1);
     */
  /* not smart;     telepathic;     tunneling; not erratic */
 setlastknown(c,getPositionX(d->pc),getPositionY(d->pc));
  //c->npc->pc_last_known_position[dim_x] = d->pc.position[dim_x];
  npc_next_pos_line_of_sight_tunnel(d, c, next);
}

static void npc_next_pos_07(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("7");
    refresh();
    sleep(1);
     */
  /*     smart;     telepathic;     tunneling; not erratic */
  npc_next_pos_gradient(d, c, next);
}

static void npc_next_pos_08(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("8");
    refresh();
    sleep(1);
     */
  /* not smart; not telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_00(d, c, next);
  }
}

static void npc_next_pos_09(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("9");
    refresh();
    sleep(1);
     */
  /*     smart; not telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_01(d, c, next);
  }
}

static void npc_next_pos_0a(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("10");
    refresh();
    sleep(1);
     */
  /* not smart;     telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_02(d, c, next);
  }
}

static void npc_next_pos_0b(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("11");
    refresh();
    sleep(1);
     */
  /*     smart;     telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_03(d, c, next);
  }
}

static void npc_next_pos_0c(dungeon_t *d, character_t *c, pair_t next)
{   /*
    printw("12");
    refresh();
    sleep(1);
     */
  /* not smart; not telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_04(d, c, next);
  }
}

static void npc_next_pos_0d(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("13");
    refresh();
    sleep(1);
     */
  /*     smart; not telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_05(d, c, next);
  }
}

static void npc_next_pos_0e(dungeon_t *d, character_t *c, pair_t next)
{   /*
    printw("14");
    refresh();
    sleep(1);
     */
  /* not smart;     telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_06(d, c, next);
  }
}

static void npc_next_pos_0f(dungeon_t *d, character_t *c, pair_t next)
{
    /*
    printw("15");
    refresh();
    sleep(1);
     */
  /*     smart;     telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_07(d, c, next);
  }
}

void (*npc_move_func[])(dungeon_t *d, character_t *c, pair_t next) = {
  /* We'll have one function for each combination of bits, so the *
   * order is based on binary counting through the NPC_* bits.    *
   * It could be very easy to mess this up, so be careful.  We'll *
   * name them according to their hex value.                      */
  npc_next_pos_00,
  npc_next_pos_01,
  npc_next_pos_02,
  npc_next_pos_03,
  npc_next_pos_04,
  npc_next_pos_05,
  npc_next_pos_06,
  npc_next_pos_07,
  npc_next_pos_08,
  npc_next_pos_09,
  npc_next_pos_0a,
  npc_next_pos_0b,
  npc_next_pos_0c,
  npc_next_pos_0d,
  npc_next_pos_0e,
  npc_next_pos_0f,
};

void npc_next_pos(dungeon_t *d, character_t *c, pair_t next)
{
 // next[dim_y] = c->position[dim_y];
 // next[dim_x] = c->position[dim_x];
   /*
    printw("NPC");
    refresh();
    sleep(1);
    */
next[dim_y] =getPositionY(c);
next[dim_x] =getPositionX(c);

  npc_move_func[getcharacter(c) & 0x0000000f](d, c, next);
}

uint32_t dungeon_has_npcs(dungeon_t *d)
{
  return d->num_monsters;
}
