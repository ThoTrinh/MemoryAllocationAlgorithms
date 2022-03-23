Tho Trinh

You would run this like so:
make
./project2 BESTFIT N ex75.txt
./project2 FIRSTFIT N ex75.txt
./project2 NEXTFIT N ex75.txt
./project2 WORSTFIT N ex76.txt

This program implements 4 memory allocation algorithms; N (an integer) will specify the total Memory size that you will allow, and the last argument is your script file. This assumes the script file is in the same directory you are testing in.

My Implementation:

Each process is represented by a node. I did not keep track of any nodes that represents empty spaces, as this can be done easily with a little data processing. For each node, I kept track of where it starts, its size, its name, and the next node. So, to find space, you just find the spaces between the start of each node and the thing before it: whether it is the start of the list, another node, or something else. For the last node, you check if the end of its space (start_of_process + size_of_process) is equal to totalMemoryAmount. If it's not, you just subtract it from totalMemoryAmount. The rest of my implementation is written clearly in the function descriptions below.

Function Descriptions:
I have a structure that I used for each node of my linked list. Each node contains where it starts, its size, its name, and a pointer to the next node in the listed

node createNode() creates an empty node that has none of the structure elements assigned and returns it

listAssigned(node head) lists the assigned processes in the program. It lists the processes as "name, size, relative start allocation"

addNode(node head, int value, char* label) adds a process to our linked list. Add is relatively the same (for different memory allocation algorithms ) when adding to an empty list. However, adding after that depends on the memory allocation algorithm chosen. There is a huge if else if statement that executes depending on the algorithm chosen. It then adds to the list, then returns the head. This is in case the head might change during adding.

node deleteNode(node  head, char* label) deletes a node. It finds the node in the list and sees if it is there. If it is, it is deleted. If not, it prints FAIL then exits the Function. If the program removes the only node, it will return NULL

int findNode(node head, char* label) finds a node. This runs on the same logic as delete node. It prints FAULT when not finding the node though. And, it prints out the nodes information upon locating it.

listAvailable(node head) lists all the available spaces in the program. I used logic to determine where the spaces are instead of keeping nodes for empty spaces.

main is my last method. It does all the file processing. It finds the arguments which you specified to it and assigns memory algorithm as well as totalMemoryAmount. It then opens the script and deciphers each line to see what to do with it. It then calls a function to keep track of the data via linkedlist. I then free all the memory I allocated to the system.
