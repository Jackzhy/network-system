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

int lkcost2[4];		/*the link cost between node 2 and other nodes*/
struct distance_table /*define distance table*/
{
  int costs[4][4];
} dt2;


extern void printdt2(struct distance_table *dtptr);
extern void linkhandler2(int linkid, int newcost);


int min2Change;// keep track of mincost change

/* students to write the following two routines, and maybe some others */

//a helper function return min of 2 int
int m2(int x, int y){
  if (x <= y) {
    return x;
  }
  return y;
}

//A helper function that will posion any via node in the table that will lead back.
void posion2(){
  int i;
  int k;
  for ( i = 0; i < 4; i++) {
    if (i != 2) {
      for ( k = 0; k < 4; k++) {
        // if the distance to destination via the sender is further than the shortest path form receiver to destination plus linkcost of receiver to sender plus sender's shortest path to receiver.
        if (dt2.costs[k][2] + lkcost2[i] + m2(m2(m2(dt2.costs[i][0], dt2.costs[i][1]), dt2.costs[i][3]), lkcost2[i]) <= dt2.costs[k][i]) {
          // posion the via sender node path on the table , set distance to INFINITY.
          dt2.costs[k][i] = INFINITY;
          printf("Table changed\n");
        }
      }
    }
  }
  printdt2(&dt2);
}

// A helper functin that will sent the latest mincosts to neighbor
void sendToNeighbors2(){
  struct rtpkt packet;
  int i;
  for ( i = 0; i < 4; i++) {
    packet.mincost[i] = dt2.costs[i][2];
  }
  packet.sourceid = 2;
  packet.destid = 0;
  tolayer2(packet);
  packet.destid = 1;
  tolayer2(packet);
  packet.destid = 3;
  tolayer2(packet);
  min2Change = false;
}



// init table and send its link costs to neighbor
void rtinit2()
{
  printf("################ rtinit2 is called! ##############\n");
  printf("#The time is : %lf\n", clocktime);
  //update link costs
  lkcost2[0] = 1;
  lkcost2[1] = 10;
  lkcost2[2] = 0;
  lkcost2[3] = 2;
  int i;
  int j;
  int k;
  //Init table value to INFINITY
  //Then set it to link costs
  for ( i = 0; i < 4; i++) {
    for ( j = 0; j < 4; j++) {
      dt2.costs[i][j] = INFINITY;
    }
  }
  for ( k = 0; k < 4; k++) {
    dt2.costs[k][2] = lkcost2[k];
  }
  // print the table and sent link cost to neighbour
  printdt2(&dt2);
  sendToNeighbors2();
}

// Update the cost table when receive a packet from neighbor
void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  printf("@@@@@@@@@@@@rtupdate2 is called!@@@@@@@@@@@@@\n");
  printf("#The time is : %lf\n", clocktime);
  printf("packet is send from: %d\n", rcvdpkt->sourceid);
  // renew the latest shortest path via sender to destination on the table.
  int i;
  for ( i = 0; i < 4; i++) {
    // if there is any change on table update and print.
    if (dt2.costs[i][rcvdpkt->sourceid] != rcvdpkt->mincost[i] + lkcost2[rcvdpkt->sourceid] ) {
      dt2.costs[i][rcvdpkt->sourceid] = rcvdpkt->mincost[i] + lkcost2[rcvdpkt->sourceid];
      printf("Table changed\n");
      printdt2(&dt2);
    }
  }
  // Find the the shortest path to destination form table (via any other node or just the link cost).
  int j;
  for ( j = 0; j < 4; j++) {
    // If there is any change on shortest path , update and set minchange to true for later use.
    if (dt2.costs[j][2] != m2(m2(m2(dt2.costs[j][0], dt2.costs[j][1]), dt2.costs[j][3]), lkcost2[j])) {
      min2Change = true;
      dt2.costs[j][2] = m2(m2(m2(dt2.costs[j][0], dt2.costs[j][1]), dt2.costs[j][3]), lkcost2[j]);
    }
  }
  // Call the posion helper function to set posion to other node if needed.
  posion2();
  // If there is any change on the shortest path to every destination, send information to neighbors.
  if (min2Change == true) {
    sendToNeighbors2();
  }
}


void printdt2(dtptr)
  struct distance_table *dtptr;
{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][1],dtptr->costs[3][3]);

}


/* called when cost from 2 to linkid changes from current value to newcost*/
void linkhandler2(linkid, newcost)
  int linkid, newcost;
{

}
