// AUTHOR : HONGYU ZHOU
// compile instruction:
// Put node0.c, node1.c, node2.c, node3.c and prog3.c in the same folder.
// Change dictionary in terminal into the folder
// command
// gcc *.c -Wall -o prog3
// ./prog3

#include <stdio.h>
#define INFINITY 9999

typedef int bool;
#define true 1
#define false 0

struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
};

extern void tolayer2(struct rtpkt packet);

extern int TRACE;
extern double clocktime;

int lkcost3[4];		/*the link cost between node 3 and other nodes*/
struct distance_table /*define distance table*/
{
  int costs[4][4];
} dt3;

extern void printdt3(struct distance_table *dtptr);
extern void linkhandler3(int linkid, int newcost);



int min3Change;// keep track of mincost change

/* students to write the following two routines, and maybe some others */

//a helper function return min of 2 int
int m3(int x, int y){
  if (x <= y) {
    return x;
  }
  return y;
}

//A helper function that will posion any via node in the table that will lead back.
void posion3(){
  int i;
  int k;
  for ( i = 0; i < 4; i++) {
    if (i != 3) {
      for ( k = 0; k < 4; k++) {
        // if the distance to destination via the sender is further than the shortest path form receiver to destination plus linkcost of receiver to sender plus sender's shortest path to receiver.
        if (dt3.costs[k][3] + lkcost3[i] + m3(m3(m3(dt3.costs[i][0], dt3.costs[i][1]), dt3.costs[i][2]), lkcost3[i]) <= dt3.costs[k][i]) {
          // posion the via sender node path on the table , set distance to INFINITY.
          dt3.costs[k][i] = INFINITY;
          printf("Table changed\n");
        }
      }
    }
  }
  printdt3(&dt3);
}

// A helper functin that will sent the latest mincosts to neighbor
void sendToNeighbors3(){
  struct rtpkt packet;
  int i;
  for ( i = 0; i < 4; i++) {
    packet.mincost[i] = dt3.costs[i][3];
  }
  packet.sourceid = 3;
  packet.destid = 1;
  tolayer2(packet);
  packet.destid = 2;
  tolayer2(packet);
  min3Change = false;
}

// init table and send its link costs to neighbor
void rtinit3()
{
  printf("################ rtinit3 is called! ##############\n");
  printf("#The time is : %lf\n", clocktime);
  //update link costs
  lkcost3[0] = INFINITY;
  lkcost3[1] = 7;
  lkcost3[2] = 2;
  lkcost3[3] = 0;
  int i;
  int j;
  int k;
  //Init table value to INFINITY
  //Then set it to link costs
  for ( i = 0; i < 4; i++) {
    for ( j = 0; j < 4; j++) {
      dt3.costs[i][j] = INFINITY;
    }
  }
  for ( k = 0; k < 4; k++) {
    dt3.costs[k][3] = lkcost3[k];
  }
  // print the table and sent link cost to neighbour
  printdt3(&dt3);
  sendToNeighbors3();
}

// Update the cost table when receive a packet from neighbor
void rtupdate3(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  printf("@@@@@@@@@@@@rtupdate3 is called!@@@@@@@@@@@@@\n");
  printf("#The time is : %lf\n", clocktime);
  printf("packet is send from: %d\n", rcvdpkt->sourceid);
  // renew the latest shortest path via sender to destination on the table.
  int i;
  for ( i = 0; i < 4; i++) {
    // if there is any change on table update and print.
    if (dt3.costs[i][rcvdpkt->sourceid] != rcvdpkt->mincost[i] + lkcost3[rcvdpkt->sourceid]) {
      dt3.costs[i][rcvdpkt->sourceid] = rcvdpkt->mincost[i] + lkcost3[rcvdpkt->sourceid];
      printf("Table changed\n");
      printdt3(&dt3);
    }
  }
  // Find the the shortest path to destination form table (via any other node or just the link cost).
  int j;
  for ( j = 0; j < 4; j++) {
    // If there is any change on shortest path , update and set minchange to true for later use.
    if (dt3.costs[j][3] != m3(m3(m3(dt3.costs[j][0], dt3.costs[j][1]), dt3.costs[j][2]), lkcost3[j])) {
      min3Change = true;
      dt3.costs[j][3] = m3(m3(m3(dt3.costs[j][0], dt3.costs[j][1]), dt3.costs[j][2]), lkcost3[j]);
    }
  }
  // Call the posion helper function to set posion to other node if needed.
  posion3();
  // If there is any change on the shortest path to every destination, send information to neighbors.
  if (min3Change == true) {
    sendToNeighbors3();
  }
}


void printdt3(dtptr)
  struct distance_table *dtptr;
{
  printf("                via     \n");
  printf("   D3 |    1     2 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][1],dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][1],dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][1],dtptr->costs[2][2]);

}

/* called when cost from 3 to linkid changes from current value to newcost*/
void linkhandler3(linkid, newcost)
  int linkid, newcost;
{

}
