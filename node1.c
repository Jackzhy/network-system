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

int lkcost1[4];		/*the link cost between node 1 and other nodes*/
struct distance_table /*define distance table*/
{
  int costs[4][4];
} dt1;

struct rtpkt packet;

extern void printdt1(struct distance_table *dtptr);
extern void linkhandler1(int linkid, int newcost);


int min1Change;// keep track of mincost change

/* students to write the following two routines, and maybe some others */

//a helper function return min of 2 int
int m1(int x, int y){
  if (x <= y) {
    return x;
  }
  return y;
}

//A helper function that will posion any via node in the table that will lead back.
void posion1(){
  int i;
  int k;
  for ( i = 0; i < 4; i++) {
    if (i != 1) {
      for ( k = 0; k < 4; k++) {
        // if the distance to destination via the sender is further than the shortest path form receiver to destination plus linkcost of receiver to sender plus sender's shortest path to receiver.
        if (dt1.costs[k][1] + lkcost1[i] + m1(m1(m1(dt1.costs[i][0], dt1.costs[i][2]), dt1.costs[i][3]), lkcost1[i]) <= dt1.costs[k][i]) {
          // posion the via sender node path on the table , set distance to INFINITY.
          dt1.costs[k][i] = INFINITY;
          printf("Table changed\n");
        }
      }
    }
  }
  printdt1(&dt1);
}

// A helper functin that will sent the latest mincosts to neighbor
void sendToNeighbors1(){
  struct rtpkt packet;
  int i;
  for ( i = 0; i < 4; i++) {
    packet.mincost[i] = dt1.costs[i][1];
  }
  packet.sourceid = 1;
  packet.destid = 0;
  tolayer2(packet);
  packet.destid = 2;
  tolayer2(packet);
  packet.destid = 3;
  tolayer2(packet);
  min1Change = false;
}

// init table and send its link costs to neighbor
void rtinit1()
{
  printf("################ rtinit1 is called! ##############\n");
  printf("#The time is : %lf\n", clocktime);
  //update link costs
  lkcost1[0] = 1;
  lkcost1[1] = 0;
  lkcost1[2] = 10;
  lkcost1[3] = 7;
  int i;
  int j;
  int k;
  //Init table value to INFINITY
  //Then set it to link costs
  for ( i = 0; i < 4; i++) {
    for ( j = 0; j < 4; j++) {
      dt1.costs[i][j] = INFINITY;
    }
  }
  for ( k = 0; k < 4; k++) {
    dt1.costs[k][1] = lkcost1[k];
  }
  // print the table and sent link cost to neighbour
  printdt1(&dt1);
  sendToNeighbors1();
}

// Update the cost table when receive a packet from neighbor
void rtupdate1(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
  printf("@@@@@@@@@@@@rtupdate1 is called!@@@@@@@@@@@@@\n");
  printf("#The time is : %lf\n", clocktime);
  printf("packet is send from: %d\n", rcvdpkt->sourceid);
  // renew the latest shortest path via sender to destination on the table.
  int i;
  for ( i = 0; i < 4; i++) {
    // if there is any change on table update and print.
    if (dt1.costs[i][rcvdpkt->sourceid] != rcvdpkt->mincost[i] + lkcost1[rcvdpkt->sourceid]) {
      dt1.costs[i][rcvdpkt->sourceid] = rcvdpkt->mincost[i] + lkcost1[rcvdpkt->sourceid];
      printf("Table changed\n");
      printdt1(&dt1);
    }
  }
  // Find the the shortest path to destination form table (via any other node or just the link cost).
  int j;
  for ( j = 0; j < 4; j++) {
    // If there is any change on shortest path , update and set minchange to true for later use.
    if (dt1.costs[j][1] != m1(m1(m1(dt1.costs[j][0], dt1.costs[j][2]), dt1.costs[j][3]), lkcost1[j])) {
      min1Change = true;
      dt1.costs[j][1] = m1(m1(m1(dt1.costs[j][0], dt1.costs[j][2]), dt1.costs[j][3]), lkcost1[j]);
    }
  }
  // Call the posion helper function to set posion to other node if needed.
  posion1();
  // If there is any change on the shortest path to every destination, send information to neighbors.
  if (min1Change == true) {
    sendToNeighbors1();
  }
}


void printdt1(dtptr)
  struct distance_table *dtptr;
{
  printf("                via     \n");
  printf("   D1 |    0     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2],dtptr->costs[0][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2],dtptr->costs[3][3]);

}


/* called when cost from 1 to linkid changes from current value to newcost*/
void linkhandler1(linkid, newcost)
  int linkid, newcost;
{
  printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>> linkhandler1 is called! <<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
  printf("#The time is : %lf\n", clocktime);
  // update the link cost and reset the shortest path to each destination and then send the shortest path info to neighbors.
  lkcost1[linkid] = newcost;
  dt1.costs[linkid][1] = newcost;
  sendToNeighbors1();
}
