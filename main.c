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

   -----------------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

int main()
  {
   printf("Let's begin!");

   return 0;
  }
