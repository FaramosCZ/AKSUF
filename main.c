/* -----------------------------------------------------------------------------

   ABOUT:
     Program created for cryptographic/puzzle competition Fuška 2018

   AUTHOR:
     Michal Schorm <mschorm@redhat.com>

   DESCRIPTITON:
     Reverse generator for "Bejeweled" game.
     Program works with a field of "gems". When 3 or more same "gems" are in
     straight line right next to each other, the triplet disappears. If there
     were other gems above those who vanished, they "fall down". (Like in a
     Bejeweled game)

     This program gets board with remaining "gems" placed (no more triplets can
     be made) and it fills the playing board back - move by move - in such a
     way, that there will always be *only one* solution for the whole game.

   CONSTRAINTS:
     - The generated board MUST have only one solution.
     - Use only triplets (avoid 4 or 5 in a row)

   -----------------------------------------------------------------------------
*/
/* -----------------------------------------------------------------------------

   TODO:
     Generate first triplet on the board
       Write it properly to the history
     Create template for generating all other moves
     Create template for history operations
     Iterate through all available positions for the first triplet

   MISC:
     Global variables and macros written with capital letters

   -----------------------------------------------------------------------------
*/



#include <stdio.h>
#include <stdlib.h>



// -----------------------------------------------------------------------------
// MACROS, GLOBAL VARIABLES, DATA TYPES

#define HEIGHT 10                                //
#define WIDTH 7                                  // Dimensions of the playing field
int SIZE = HEIGHT*WIDTH;                         //

#define PLACEHOLDER '.'                          // Character that fills empty fields (for pritnf)
#define COLOR_COUNT 7                            // Number of different types of "gems" (7 is deafult number in Bejeweled)

// Create structure for history operations
typedef struct history
{
 char board[HEIGHT][WIDTH];                      // Each history entry will hold the whole playing board status
 struct history * history_older;                 // Pointer to "older" history - a history closer to the game END
 int history_stack;                              // Serial number of the history entry (mostly for debugging)

 int orientation, position, combination, type;   // Some info about the triplet
 int x1, y1, x2, y2, x3, y3;                     // As well as coordinates of all 3 gems of that triplet
} history;

history * HISTORY_LATEST = NULL;                 // Pointer to the history one-way chained list



// -----------------------------------------------------------------------------
// FUNCTIONS FOR THE HISTORY OPERATIONS

void initialize_history()
{
 HISTORY_LATEST = malloc( sizeof(history) );
   if( HISTORY_LATEST==NULL ){fprintf(stderr, "\n\t UNSUCCESSFUL MALLOC CALL \n" ); exit(1);}

 // Fill the board with placeholder character
 for( int x=0 ; x<HEIGHT ; x++) for( int y=0 ; y<WIDTH ; y++) HISTORY_LATEST->board[x][y]=PLACEHOLDER;
 // Prepare other variables
 HISTORY_LATEST->history_older = NULL;
 HISTORY_LATEST->history_stack = 0;
 HISTORY_LATEST->orientation = HISTORY_LATEST->position = HISTORY_LATEST->combination = HISTORY_LATEST->type = 0;
 HISTORY_LATEST->x1 = HISTORY_LATEST->x2 = HISTORY_LATEST->x3 = HISTORY_LATEST->y1 = HISTORY_LATEST->y2 = HISTORY_LATEST->y3 = 0;

 // HERE is the place to set initial field pattern:

 HISTORY_LATEST->board[HEIGHT-4][WIDTH/2-1] = 'C';
 HISTORY_LATEST->board[HEIGHT-3][WIDTH/2] = 'C';
 HISTORY_LATEST->board[HEIGHT-2][WIDTH/2] = 'C';
 HISTORY_LATEST->board[HEIGHT-1][WIDTH/2] = 'C';
 HISTORY_LATEST->board[HEIGHT-1][WIDTH/2-1] = 'A';
} // end of initialize_history()



void delete_last_history_entry()
{
 if( HISTORY_LATEST == NULL ){fprintf(stderr, "\n\t NO HISTORY ENTRIES TO DELETE \n" ); exit(1);}

 history * history_entry_for_deletion = HISTORY_LATEST;
 HISTORY_LATEST = HISTORY_LATEST->history_older;
 free( history_entry_for_deletion );
} // end of delete_last_history_entry()



// -----------------------------------------------------------------------------
// CLI AND FUNCTIONS FOR THE BOARD PRINTING

void print_board_specific( history * history_entry )
{
 printf("\n");
 for( int x=0 ; x<HEIGHT ; x++)
  {
   printf("| ");
   for( int y=0 ; y<WIDTH ; y++) printf("%c ", history_entry->board[x][y]);
   printf("|\n");
  }

  // bottom border
  for( int x=0 ; x<=WIDTH ; x++) printf("--");
  printf("-\n");
}



void print_board() { print_board_specific( HISTORY_LATEST ); }



// -----------------------------------------------------------------------------
// CHECK FUNCTIONS

int check_number_of_triplets_active_specific( history * history_entry )
{
 int count = 0; // Final count of triplets found

 // Check for:   0
 //              0
 //              0
 for( int x=0 ; x<HEIGHT-2 ; x++ )
  {
   for( int y=0 ; y<WIDTH ; y++ )
    {
     if( history_entry->board[x][y] == PLACEHOLDER ) continue;
     if( history_entry->board[x][y] == history_entry->board[x+1][y] && history_entry->board[x+1][y] == history_entry->board[x+2][y] ) count++;
    }
  }

 // Check for:
 //              0 0 0
 //
 for( int x=0 ; x<HEIGHT ; x++ )
  {
   for( int y=0 ; y<WIDTH-2 ; y++ )
    {
     if( history_entry->board[x][y] == PLACEHOLDER ) continue;
     if( history_entry->board[x][y] == history_entry->board[x][y+1] && history_entry->board[x][y+1] == history_entry->board[x][y+2] ) count++;
    }
  }

 return count;
}



int check_number_of_triplets_one_move_away_specific( history * history_entry )
{
 int count = 0; // Final count of triplets found

 // check for:   0      0       0
 //              0        0       0
 //                0    0         0
 for( int x=0 ; x<HEIGHT-2 ; x++)
  {
   for( int y=0 ; y<WIDTH-1 ; y++)
    {
     if(history_entry->board[x][y]==PLACEHOLDER) continue;
     if(history_entry->board[x][y] == history_entry->board[x+1][y] && history_entry->board[x+1][y] == history_entry->board[x+2][y+1]) count++;
     if(history_entry->board[x][y] == history_entry->board[x+1][y+1] && history_entry->board[x+1][y+1] == history_entry->board[x+2][y+1]) count++;
     if(history_entry->board[x][y] == history_entry->board[x+1][y+1] && history_entry->board[x+1][y+1] == history_entry->board[x+2][y]) count++;
    }
  }

 // check for:   0      0       0
 //              0    0       0
 //            0        0     0
 for( int x=0 ; x<HEIGHT-2 ; x++)
  {
   for( int y=1 ; y<WIDTH ; y++)
    {
     if(history_entry->board[x][y]==PLACEHOLDER) continue;
     if(history_entry->board[x][y] == history_entry->board[x+1][y] && history_entry->board[x+1][y] == history_entry->board[x+2][y-1]) count++;
     if(history_entry->board[x][y] == history_entry->board[x+1][y-1] && history_entry->board[x+1][y-1] == history_entry->board[x+2][y-1]) count++;
     if(history_entry->board[x][y] == history_entry->board[x+1][y-1] && history_entry->board[x+1][y-1] == history_entry->board[x+2][y]) count++;
    }
  }

 // check for:
 //             0 0      0   0    0
 //                 0      0        0 0
 for( int x=0 ; x<HEIGHT-1 ; x++)
  {
   for( int y=0 ; y<WIDTH-2 ; y++)
    {
     if(history_entry->board[x][y]==PLACEHOLDER) continue;
     if(history_entry->board[x][y] == history_entry->board[x][y+1] && history_entry->board[x][y+1] == history_entry->board[x+1][y+2]) count++;
     if(history_entry->board[x][y] == history_entry->board[x+1][y+1] && history_entry->board[x+1][y+1] == history_entry->board[x+1][y+2]) count++;
     if(history_entry->board[x][y] == history_entry->board[x+1][y+1] && history_entry->board[x+1][y+1] == history_entry->board[x][y+2]) count++;
    }
  }

 // check for:     0       0        0 0
 //             0 0      0   0    0
 //
 for( int x=1 ; x<HEIGHT ; x++)
  {
   for( int y=0 ; y<WIDTH-2 ; y++)
    {
     if(history_entry->board[x][y]==PLACEHOLDER) continue;
     if(history_entry->board[x][y] == history_entry->board[x][y+1] && history_entry->board[x][y+1] == history_entry->board[x-1][y+2]) count++;
     if(history_entry->board[x][y] == history_entry->board[x-1][y+1] && history_entry->board[x-1][y+1] == history_entry->board[x-1][y+2]) count++;
     if(history_entry->board[x][y] == history_entry->board[x-1][y+1] && history_entry->board[x-1][y+1] == history_entry->board[x][y+2]) count++;
    }
  }

 return count;
}



int check_number_of_triplets_active(){ return check_number_of_triplets_active_specific( HISTORY_LATEST ); }
int check_number_of_triplets_one_move_away(){ return check_number_of_triplets_one_move_away_specific( HISTORY_LATEST ); }
int check_triplets_specific( history * history_entry ){ return check_number_of_triplets_one_move_away_specific( history_entry ) + check_number_of_triplets_active_specific( history_entry ); }
int check_triplets(){ return check_triplets_specific( HISTORY_LATEST ); }



// -----------------------------------------------------------------------------
// MAIN

int main()
  {

   // Prepare initial history entry; load beginning state of playing board
   initialize_history();

   print_board();

   printf("\nNuber of active triplets: %d\n", check_number_of_triplets_active() );
   printf("\nNuber of triplets one move away: %d\n", check_number_of_triplets_one_move_away() );
   printf("\nTotal ^: %d\n", check_triplets() );

   return 0;
  }
