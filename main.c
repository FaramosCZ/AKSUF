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
     Add function to generate all other moves
     Explain the mathematic model of the generator

   MISC:
     Global variables and macros written with capital letters

   -----------------------------------------------------------------------------
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>	// memcpy



// -----------------------------------------------------------------------------
// MACROS, GLOBAL VARIABLES, DATA TYPES

#define HEIGHT 10                                //
#define WIDTH 7                                  // Dimensions of the playing field
int SIZE = HEIGHT*WIDTH;                         //

#define PLACEHOLDER '.'                          // Character that fills empty fields (for pritnf)
#define COLOR_COUNT 7                            // Number of different types of "gems" (7 is deafult number in Bejeweled)

#define VERTICAL 0                               // Triplet orientation
#define HORIZONTAL 1                             // Triplet orientation

// Create structure for history operations
typedef struct history
{
 char board[HEIGHT][WIDTH];                      // Each history entry will hold the whole playing board status
 struct history * history_older;                 // Pointer to "older" history - a history closer to the game END

 int history_stack, position, combination, type; // Serial number of the history entry (mostly for debugging); and some info about the triplets		// yet UNUSED
 int orientation;                                // Some info about the triplet    // USED

 int x1, y1, x2, y2, x3, y3;                     // As well as coordinates of all 3 gems of that triplet
} history;

history * HISTORY_LATEST = NULL;                 // Pointer to the history one-way chained list



// -----------------------------------------------------------------------------
// FUNCTION HEADERS

int check_push_space(history * history_entry, int maximal_x, int minimal_y);



// -----------------------------------------------------------------------------
// FUNCTIONS FOR THE HISTORY OPERATIONS

void history_initialize()
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



void history_delete_last_entry()
{
 if( HISTORY_LATEST == NULL ){fprintf(stderr, "\n\t NO HISTORY ENTRIES TO DELETE \n" ); exit(1);}

 history * history_entry_for_deletion = HISTORY_LATEST;
 HISTORY_LATEST = HISTORY_LATEST->history_older;
 free( history_entry_for_deletion );
} // end of delete_last_history_entry()



int history_push(int x1, int x2, int x3, int y1, int y2, int y3, int color)
{
 history * history_entry = malloc( sizeof(history) );
   if( history_entry==NULL ){fprintf(stderr, "\n\t UNSUCCESSFUL MALLOC CALL \n" ); exit(1);}

 // Copy the board from the last history entry
 memcpy(history_entry->board, HISTORY_LATEST->board, SIZE);
 // Prepare other variables
 // history_entry->history_stack = HISTORY_LATEST->history_stack++;
 history_entry->history_older = HISTORY_LATEST;

 // Find the edge values
 int maximal_x = x1, minimal_y = y1;
 if( x1>x2 && x1>x3 ){maximal_x = x1; history_entry->orientation=VERTICAL;}
 if( x2>x1 && x2>x3 ){maximal_x = x2; history_entry->orientation=VERTICAL;}
 if( x3>x2 && x3>x1 ){maximal_x = x3; history_entry->orientation=VERTICAL;}
 if( y1<y2 && y1<y3 ){minimal_y = y1; history_entry->orientation=HORIZONTAL;}
 if( y2<y1 && y2<y3 ){minimal_y = y2; history_entry->orientation=HORIZONTAL;}
 if( y3<y2 && y3<y1 ){minimal_y = y3; history_entry->orientation=HORIZONTAL;}

 // Check rules of game, if the move can be really placed
 if( check_push_space(history_entry, maximal_x, minimal_y) != 0 ){ free(history_entry); return -1; }

 history_entry->x1 = x1;
 history_entry->x2 = x2;
 history_entry->x3 = x3;
 history_entry->y1 = y1;
 history_entry->y2 = y2;
 history_entry->y3 = y3;

 // Make space for the triplet
 if( history_entry->orientation == VERTICAL ){ for( int x=3 ; x<=maximal_x ; x++ ) history_entry->board[x-3][y1]=history_entry->board[x][y1]; }
 else{ for( int y=minimal_y ; y<=minimal_y+2 ; y++ ){for( int x=1 ; x<=maximal_x ; x++ ) history_entry->board[x-1][y]=history_entry->board[x][y];} }

 // Place the triplet oo the playing board
 history_entry->board[x1][y1] = history_entry->board[x2][y2] = history_entry->board[x3][y3] = color+65; // The "+65" will move the gem number in ASCII


 HISTORY_LATEST = history_entry;

 return 0;
}



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



int check_push_space(history * history_entry, int maximal_x, int minimal_y)
{
 int free_space, x, y;

 if( history_entry->orientation==VERTICAL ) // vertical triplet
   {
    free_space=0;
    for( x=0 ; x<=maximal_x ; x++ ){if( history_entry->board[x][minimal_y] == PLACEHOLDER ) free_space++; }
    // If there is not enough space verticaly for the triplet, fail.
    if( free_space < 3 ) return -1;
    for( free_space=0 ; x<HEIGHT ; x++ ){if( history_entry->board[x][minimal_y] == PLACEHOLDER ) free_space++; }
    // If there is any free space in the column anywhere below the triplet, fail.
    if( free_space != 0 ) return -1;
   }
 else // horizontal triplet
   {
    for( y=0 ; y<3 ; y++ ) // 3 columns
      {
       free_space=0;
       for( x=0 ; x<=maximal_x ; x++ ){if( history_entry->board[x][minimal_y+y] == PLACEHOLDER ) free_space++; }
       // If there is not enough space horizontaly for the triplet, fail.
       if( free_space < 1 ) return -1;
       for( free_space=0 ; x<HEIGHT ; x++ ){if( history_entry->board[x][minimal_y+y] == PLACEHOLDER ) free_space++; }
       // If there is any free space in the column anywhere below the triplet, fail.
       if( free_space != 0 ) return -1;
      }
   }

 return 0;
}

// -----------------------------------------------------------------------------
// GENERATE MOVE

int generate_move()
{

   printf("generate_move():");
   print_board();
   printf("\nNuber of active triplets: %d\n", check_number_of_triplets_active() );
   printf("\nNuber of triplets one move away: %d\n", check_number_of_triplets_one_move_away() );
   printf("\nTotal ^: %d\n", check_triplets() );

 return 0;
}



// -----------------------------------------------------------------------------
// GENERATE FIRST MOVE
//   On the empty (but default gems) board, the initial triplet must be placed
//   first. Such triplet is placed in an unique way, so it deservers separate
//   function.
//   This function will iterate through *every possible* initial moves and for
//   every such a move, it will call function to generate the rest of the
//   board.
//   Return value is number of all fully filled playing boards, that has only
//   one unique way to be solved for given default gems placement.
//
//   The fact that this is teh "first move" means, that the triplet can be only
//   placed on the "bottom" of the playing board or on gems given by deafult
//   (which has to be placed on the bottom too)
//   This first move will also generate first usable history entry

int generate_first_move()
{
 int count_of_results = 0;

 // Iterate through the bottom of the playing field from left to right
 for( int y=0 ; y<WIDTH ; y++ )
   {
    // Iterate through the column from the bottom
    for( int x=HEIGHT-1 ; x>=0 ; x-- )
      {
       // Check if there is enough place above (minus places already taken by gems)
       if( HEIGHT>=3 && HISTORY_LATEST->board[0][y] == PLACEHOLDER && HISTORY_LATEST->board[1][y] == PLACEHOLDER && HISTORY_LATEST->board[2][y] == PLACEHOLDER )
         {
          // Try all "gems color" combinations
          for( int color=0 ; color<COLOR_COUNT ; color++ )
            {
             if( history_push(x, x-1, x-2, y, y, y, color) != 0 ) break; // wrong push
             // If, after first triplet placed, there is any other move possible, the result cannot be generated successfuly, finish now.
             if( check_triplets() == 1 ) count_of_results += generate_move();
             history_delete_last_entry();
             if( HISTORY_LATEST == NULL ){fprintf(stderr, "\n\t WRONG NUMBER OF HISTORY ENTRIES - generate_first_move() \n" ); exit(1);}
            }
         }
       // Check if there is enough place above (atleast the top field has to be empty) in all 3 collumns
       if( WIDTH>=y+3 && HISTORY_LATEST->board[0][y] == PLACEHOLDER && HISTORY_LATEST->board[0][y+1] == PLACEHOLDER && HISTORY_LATEST->board[0][y+2] == PLACEHOLDER )
         {
          // Try all "gems color" combinations
          for( int color=0 ; color<COLOR_COUNT ; color++ )
            {
             if( history_push(x, x, x, y, y+1, y+2, color) != 0 ) break; // wrong push
             // If, after first triplet placed, there is any other move possible, the result cannot be generated successfuly, finish now.
             if( check_triplets() == 1 ) count_of_results += generate_move();
             history_delete_last_entry();
             if( HISTORY_LATEST == NULL ){fprintf(stderr, "\n\t WRONG NUMBER OF HISTORY ENTRIES - generate_first_move() \n" ); exit(1);}
            }
         }
       // If there is no "gem" placed on the current field, no need to go higher
       if( HISTORY_LATEST->board[x][y] == PLACEHOLDER ) break;
      }
   }
 return count_of_results;
}



// -----------------------------------------------------------------------------
// MAIN

int main()
  {

   // Prepare initial history entry; load beginning state of playing board
   history_initialize();

   print_board();

   printf("\nNuber of active triplets: %d\n", check_number_of_triplets_active() );
   printf("\nNuber of triplets one move away: %d\n", check_number_of_triplets_one_move_away() );
   printf("\nTotal ^: %d\n", check_triplets() );

   printf("\n\n\n TOTAL NUMBER OF SOLUTIONS: %d\n\n", generate_first_move() );

   return 0;
  }
