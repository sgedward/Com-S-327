#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#include "dungeon.h"
#include "path.h"
#include "pc.h"
#include "npc.h"
#include "move.h"
#include <ncurses.h>
#include <fcntl.h>
#include "utils.h"


const char *victory =
  "\n                                       o\n"
  "                                      $\"\"$o\n"
  "                                     $\"  $$\n"
  "                                      $$$$\n"
  "                                      o \"$o\n"
  "                                     o\"  \"$\n"
  "                oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o\n"
  "   o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  "
  "\"oo   o o o o\n"
  "   \"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $"
  "   \"o$$\"    o$$\n"
  "     \"\"o       o  $          $\"       $$$$$       o          $  ooo"
  "     o\"\"\n"
  "        \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$"
  "   o\"\n"
  "         \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  "
  "\"$$$  $\n"
  "           \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \""
  "\"\"      \"\"\" \"\n"
  "            \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$\n"
  "             \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\""
  "\"$$$$\n"
  "              $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"\n"
  "              $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""
  "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$\n"
  "              $\"                                                 \"$\n"
  "              $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\""
  "$\"$\"$\"$\"$\"$\"$\"$\n"
  "                                   You win!\n\n";

const char *tombstone =
  "\n\n\n\n                /\"\"\"\"\"/\"\"\"\"\"\"\".\n"
  "               /     /         \\             __\n"
  "              /     /           \\            ||\n"
  "             /____ /  In Loving  \\           ||\n"
  "            |     |    Memory     |          ||\n"
  "            |     |               |          ||\n"
  "            |     |   A. Luser    |          ||\n"
  "            |     |               |          ||\n"
  "            |     |     * *   * * |         _||_\n"
  "            |     |     *\\/* *\\/* |        | TT |\n"
  "            |     |     *_\\_  /   ...\"\"\"\"\"\"| |"
  "| |.\"\"....\"\"\"\"\"\"\"\".\"\"\n"
  "            |     |         \\/..\"\"\"\"\"...\"\"\""
  "\\ || /.\"\"\".......\"\"\"\"...\n"
  "            |     |....\"\"\"\"\"\"\"........\"\"\"\"\""
  "\"^^^^\".......\"\"\"\"\"\"\"\"..\"\n"
  "            |......\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"......"
  "..\"\"\"\"\"....\"\"\"\"\"..\"\"...\"\"\".\n\n"
  "            You're dead.  Better luck in the next life.\n\n\n";



void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "       [-i|--image <pgm>] [-s|--save]\n",
          name);

  exit(-1);
}

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed, do_image;
  uint32_t long_arg;
  char *save_file;
  char *pgm_file;
  WINDOW *my_win;
  int starty,startx,up_x,up_y,room,down_y,down_x;
   

  memset(&d, 0, sizeof (d));

  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = 0;
  do_seed = 1;
  save_file = NULL;
  d.max_monsters = 10;
  

  /* The project spec requires '--load' and '--save'.  It's common  *
   * to have short and long forms of most switches (assuming you    *
   * don't run out of letters).  For now, we've got plenty.  Long   *
   * forms use whole words and take two dashes.  Short forms use an *
`   * abbreviation after a single dash.  We'll add '--rand' (to     *
   * specify a random seed), which will take an argument of it's    *
   * own, and we'll add short forms for all three commands, '-l',   *
   * '-s', and '-r', respectively.  We're also going to allow an    *
   * optional argument to load to allow us to load non-default save *
   * files.  No means to save to non-default locations, however.    *
   * And the final switch, '--image', allows me to create a dungeon *
   * from a PGM image, so that I was able to create those more      *
   * interesting test dungeons for you.                             */
 
 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            save_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
        case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%hu", &d.max_monsters)) {
            usage(argv[0]);
          }
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }
/////////////////////////////////////
   initscr();
   cbreak();
   curs_set(FALSE);
   keypad(stdscr, TRUE);
   ///////
   starty = 0;
   startx =0;
  //srand(time(NULL));

   my_win =newwin(24, 80, starty, startx);
   //newwin(1,80,1,0);
  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Seed is %ld.\n", seed);
  srand(seed);

  init_dungeon(&d); //init

    ///
  if (do_load) {
    read_dungeon(&d, save_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }
////
    
  config_pc(&d); // put PC in?
  gen_monsters(&d, d.max_monsters, 0); //gen monster
    refresh();
    char c='x';

     room = rand_range(1, d.num_rooms - 1);
    
    up_y= rand_range(d.rooms[room].position[dim_y],
                            (d.rooms[room].position[dim_y] +
                             d.rooms[room].size[dim_y] - 1));
    up_x=rand_range(d.rooms[room].position[dim_x],
                            (d.rooms[room].position[dim_x] +
                             d.rooms[room].size[dim_x] - 1));
      
                   up_x=getPositionX(d.pc)+1;
                   up_y=getPositionY(d.pc)+1;
    

    room = rand_range(1, d.num_rooms - 1);
    down_y= rand_range(d.rooms[room].position[dim_y],
                            (d.rooms[room].position[dim_y] +
                             d.rooms[room].size[dim_y] - 1));
    down_x=rand_range(d.rooms[room].position[dim_x],
                            (d.rooms[room].position[dim_x] +
                             d.rooms[room].size[dim_x] - 1));
   
    printw("Auto play can be on by press 'a' or any other key to conitune");
    refresh();
    if(c !='a'){
    
    c=getchar();
  }
  while (pc_is_alive(&d) && dungeon_has_npcs(&d)) {
    /*
      printw("Here");
      refresh();
      sleep(2);
     */
    if((getPositionY(d.pc)==up_y && getPositionX(d.pc)==up_x)
      ||(getPositionY(d.pc)==down_y &&getPositionX(d.pc)==down_x)){
      init_dungeon(&d);
      gen_dungeon(&d);
      config_pc(&d);
      gen_monsters(&d, d.max_monsters, 0);
      room = rand_range(1, d.num_rooms - 1);
    up_y= rand_range(d.rooms[room].position[dim_y],
                            (d.rooms[room].position[dim_y] +
                             d.rooms[room].size[dim_y] - 1));
    up_x=rand_range(d.rooms[room].position[dim_x],
                            (d.rooms[room].position[dim_x] +
                             d.rooms[room].size[dim_x] - 1));

    room = rand_range(1, d.num_rooms - 1);
    down_y= rand_range(d.rooms[room].position[dim_y],
                            (d.rooms[room].position[dim_y] +
                             d.rooms[room].size[dim_y] - 1));
    down_x=rand_range(d.rooms[room].position[dim_x],
                            (d.rooms[room].position[dim_x] +
                             d.rooms[room].size[dim_x] - 1));
   
    }

    
    render_dungeon(&d);
    mvaddch(up_y,up_x,'<');
    mvaddch(down_y,down_x,'>');
    refresh();
    
    if(c !='a'){
      c=getchar();
    }
    
  if(c =='m'){
    int r=0,count=0;
     clear();
    //refresh();
   
   scrollok(stdscr,true);
   // refresh();
    while(r != 27){
     int k=0;
     int i=0,j=0;
    for(i=0; i<DUNGEON_Y; i++){
      for( j=0; j<DUNGEON_X; j++){
        
        if(d.character[i][j] !=NULL && getSymbol(d.character[i][j]) !='@'){
              char str[80];
             sprintf(str, "\n %c x %d y %d \n",getSymbol(d.character[i][j]),j,i);
              printw(str);
        }
      }
     }
   

     
    
     
     
     if(r == KEY_DOWN){

      count--;
     scrl(count);
     }
     else if(r == KEY_UP){
      count++;
      scrl(count);
     }
     r=getch();
      refresh();
    }
    c=r;

   
    
  }

   if(c == 27){
      clear();
    }
    else if(c =='s'){
      endwin();
      return 0;
    }
else{
  if(c == 'a'){
    usleep(250000);
  }
    
    /*
    printw("alkdjfakjndfla");
    refresh();
    sleep(2);
    */
    do_moves(&d,&c);
    refresh();
   
  }

  }


  refresh();
  render_dungeon(&d);

  if (do_save) {
    write_dungeon(&d);
  }

   refresh();
   endwin();
  printf(pc_is_alive(&d) ? victory : tombstone);
   
   //sleep(10);


  pc_delete(d.pc);
  

  delete_dungeon(&d);


  return 0;
}
