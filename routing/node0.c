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

int lkcost0[4];				/*The link cost between node 0 and other nodes*/
struct distance_table 		/*Define distance table*/
{
  int costs[4][4];
} dt0;


extern void printdt0(struct distance_table *dtptr);
extern void linkhandler0(int linkid, int newcost);


int min0Change;// keep track of mincost change

/* students to write the following two routines, and maybe some others */

//a helper function return min of 2 int
int m0(int x, int y){
  if (x <= y) {
    return x;
  }
  return y;
}

//A helper function that will posion any via node in the table that will lead back.
void posion0(){
  int i;
  int k;
  for ( i = 0; i < 4; i++) {
    if (i != 0) {
      for ( k = 0; k < 4; k++) {
        // if the distance to destination via the sender is further than the shortest path form receiver to destination plus linkcost of receiver to sender plus sender's shortest path to receiver.
        if (dt0.costs[k][0] + lkcost0[i] + m0(m0(m0(dt0.costs[i][1], dt0.costs[i][2]), dt0.costs[i][3]),lkcost0[i]) <= dt0.costs[k][i]) {
          // posion the via sender node path on the table , set distance to INFINITY.
          dt0.costs[k][i] = INFINITY;
          printf("Table changed\n");
        }
      }
    }
  }
  printdt0(&dt0);
}

// A helper functin that will sent the latest mincosts to neighbor
void sendToNeighbors0(){
  struct rtpkt packet;
  int i;
  for ( i = 0; i < 4; i++) {
    packet.mincost[i] = dt0.costs[i][0];
  }
  packet.sourceid = 0;
  packet.destid = 1;
  tolayer2(packet);
  packet.destid = 2;
  tolayer2(packet);
  min0Change = false;
}

// init table and send its link costs to neighbor
void rtinit0()
{
  printf("################ rtinit0 is called! ##############\n");
  printf("#The time is : %lf\n", clocktime);
  //update link costs
  lkcost0[0] = 0;
  lkcost0[1] = 1;
  lkcost0[2] = 1;
  lkcost0[3] = INFINITY;
  int i;
  int j;
  int k;
  //Init table value to INFINITY
  //Then set it to link costs
  for ( i = 0; i < 4; i++) {
    for ( j = 0; j < 4; j++) {
      dt0.costs[i][j] = INFINITY;
    }
  }
  for ( k = 0; k < 4; k++) {
    dt0.costs[k][0] = lkcost0[k];
  }
  // print the table and sent link cost to neighbour
  printdt0(&dt0);
  sendToNeighbors0();
}

// Update the cost table when receive a packet from neighbor
void rtupdate0(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  printf("@@@@@@@@@@@@rtupdate0 is called!@@@@@@@@@@@@@\n");
  printf("#The time is : %lf\n", clocktime);
  printf("packet is send from: %d\n", rcvdpkt->sourceid);
  // renew the latest shortest path via sender to destination on the table.
  int i;
  for ( i = 0; i < 4; i++) {
    // if there is any change on table update and print.
    if (dt0.costs[i][rcvdpkt->sourceid] != rcvdpkt->mincost[i] + lkcost0[rcvdpkt->sourceid]) {
      dt0.costs[i][rcvdpkt->sourceid] = rcvdpkt->mincost[i] + lkcost0[rcvdpkt->sourceid];
      printf("Table changed\n");
      printdt0(&dt0);
    }
  }
  // Find the the shortest path to destination form table (via any other node or just the link cost).
  int j;
  for ( j = 0; j < 4; j++) {
    // If there is any change on shortest path , update and set minchange to true for later use.
    if (dt0.costs[j][0] != m0(m0(m0(dt0.costs[j][1], dt0.costs[j][2]), dt0.costs[j][3]),lkcost0[j])) {
      min0Change = true;
      dt0.costs[j][0] = m0(m0(m0(dt0.costs[j][1], dt0.costs[j][2]), dt0.costs[j][3]),lkcost0[j]);
    }
  }
  // Call the posion helper function to set posion to other node if needed.
  posion0();
  // If there is any change on the shortest path to every destination, send information to neighbors.
  if (min0Change == true) {
    sendToNeighbors0();
  }
}


void printdt0(dtptr)
  struct distance_table *dtptr;
{
  printf("                via     \n");
  printf("   D0 |    1     2 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d \n",dtptr->costs[1][1], dtptr->costs[1][2]);
  printf("dest 2|  %3d   %3d \n",dtptr->costs[2][1], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d \n",dtptr->costs[3][1], dtptr->costs[3][2]);
}


/* called when cost from 0 to linkid changes from current value to newcost*/
void linkhandler0(linkid, newcost)
  int linkid, newcost;
{
  printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>> linkhandler0 is called! <<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
  printf("#The time is : %lf\n", clocktime);
  // update the link cost and reset the shortest path to each destination and then send the shortest path info to neighbors.
  lkcost0[linkid] = newcost;
  dt0.costs[linkid][0] = newcost;
  sendToNeighbors0();
}
