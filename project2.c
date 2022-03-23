/*
``@author Tho Trinh
  @info   This program implements memory allocation algorithms used by operating systems.
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<limits.h>

// 1 = BESTFIT 2 = FIRSTFIT 3 = NEXTFIT 4 = WORSTFIT
int fit; // global variable to keep track of which memory allocation algorithm chosen
int totalMemoryAmount;
int last_left_off; // this is a "pointer" to keep track of where we should add for NEXTFIT

struct LinkedList {
    int start; // where the first memory starts
    int size; // how many bytes the process takes up
    char name[16]; // name of the process
    struct LinkedList *next;
 };
typedef struct LinkedList *node; //Define node as pointer of data type struct LinkedList

node createNode() { // creates an empty node for linked list
    node temp;
    temp = (node) malloc(sizeof(struct LinkedList)); // allocate memory for node
    temp->next = NULL;
    return temp; // returns the empty node
}

void listAssigned( node head ){ // list the assigned nodes
    node p;
    p = head; // node used to print out each node
    if(p == NULL) { // No allocated blocks
        printf("NONE");
    }
    while(p != NULL){ // print out each, until NULL
        printf("(%s, %d, %d) ", p->name, p->size, p->start);
        p = p->next;
    }
    printf("\n");
}

node addNode(node head, int value, char* label){ // addNode method
    node process, traverse;
    process = createNode(); // create empty node
    process->size = value; // assign size of process
    strcpy(process->name, label); // name the process

    if(head == NULL){ //if empty list
  	  // when adding the first process is larger than the totalMemoryAmount Allocated
      if(process->size > totalMemoryAmount){
    		printf("FAIL REQUEST %s %d\n", process->name, process->size);
    		free(process);
    		return NULL;
	     }
    	// otherwise, just add like normal
    	head = process;
      head->start = last_left_off; // add at pointer not 0, learned that the long way
    	printf("ALLOCATED %s %d\n", process->name, head->start);
    	if(fit == 3) {
    		last_left_off = head->start + head->size; //case for NEXTFIT ONLY
    	}
    	return head;
    }
    traverse  = head; // create a node to traverse the lsit
    // This depends on which fit we chose
    int prevStart = 0;
  	if(fit == 1){ // BESTFIT-smallest space that is big enough
   	  // bestfit
  		int index = 0;
  		int smallestIndex = -1;
  		int smallestSize = INT_MAX;
  		if((head->start - 0) >= value) { // should only be 0 when there is enough space in front of front node to place
  			smallestIndex = 0; // if can add before head, consider the head space in counting
  			smallestSize = process->size;
  		}
		  while(traverse->next != NULL) { //loop through list
        prevStart = traverse->start;
			  // if there is a space that fits in list, compare it to old one to see which is smaller
        if((traverse->next->start - (prevStart+traverse->size)) >= value){
          int currentSize = traverse->next->start - (prevStart+traverse->size);
				  if(currentSize < smallestSize){
					smallestSize = currentSize;
					smallestIndex = index;
				  }
	  	   }
			   index++; // used to keep track of where we should add in list
         traverse = traverse->next;
       }
		   // now add to the smallest spot
	   	 int i = 0;
		   if(smallestIndex == 0 && smallestSize == process->size){ // if insert before head
  		     process->next = head;
  			   process->start = 0;
  			   head = process;
  			   printf("ALLOCATED %s %d\n", process->name, process->start);
  			   return head;
		   }
		   if(smallestIndex != -1) { // if there was space between the nodes
         int spaceAtEnd = totalMemoryAmount-(traverse->start + traverse->size); // but the space is still bigger than space at end
         if(smallestSize >= spaceAtEnd && (process->size <= spaceAtEnd)){ // if end is smallest space, and it fits
           smallestIndex = -1; // do this so it adds at end
         }
        }
		    if(smallestIndex == -1){ // there was no space in between nodes, so just put at end
    			process->start = traverse->start + traverse->size;
    			if((process->start + process->size) > totalMemoryAmount){ // if we cant fit it in at the end
    			   printf("FAIL REQUEST %s %d\n", process->name, process->size);
    				 free(process);
    				 return head;
			    }
		     	// otherwise just put it at the end
          traverse->next = process;
          printf("ALLOCATED %s %d\n", process->name, process->start);
			    return head;
		      }
		     // insert middle
		     node middleAdd = head;
		     while(i != smallestIndex){ // traverse list to we get to the index where we kep track the smallest space
		   	     i++;
			       middleAdd = middleAdd->next;
		     }
		     // now add in the middle
		     process->next = middleAdd->next;
		     process->start = middleAdd->start + middleAdd->size;
		     middleAdd->next = process;
		     printf("ALLOCATED %s %d\n", process->name, process->start);
		     return head;
	} else if (fit == 2) { // FIRSTFIT
		if((head->start - 0) >= value){ // insert before head
      process->next = head;
			process->start = 0;
			head = process;
			printf("ALLOCATED %s %d\n", process->name, process->start);
			return head;
		}
		while(traverse->next != NULL){ // add in middle
      prevStart = traverse->start;
			// loop until we can fit in the process
      if((traverse->next->start - (prevStart+traverse->size)) >= value){
        process->next = traverse->next;
				process->start = traverse->start + traverse->size;
				traverse->next = process;
        printf("ALLOCATED %s %d\n", process->name, process->start);
				return head;
      }
      traverse = traverse->next;
    }
		// no space, so add at the end
		process->start = traverse->start + traverse->size;
		if((process->start + process->size) > totalMemoryAmount){ // if no room at the end
      printf("FAIL REQUEST %s %d\n", process->name, process->size);
      free(process);
      return head;
    }
		traverse->next = process;
		printf("ALLOCATED %s %d\n", process->name, process->start);
		return head;
	} else if(fit == 3){ // NEXTFIT
		node prevNode; // helpful node to utilise when adding to the end
		while(traverse != NULL){ // loop until we get to the where the pointer was
      if(last_left_off < traverse->start+traverse->size){
  		break;
  	  }
			prevNode = traverse;
			traverse = traverse->next;
    }
		// if reaches the end of list
		if(traverse == NULL){
			// if it wont fit at the end, start back at beginning
			if((last_left_off + process->size) > totalMemoryAmount){
				traverse = head; // back at the beggining
				if((head->start - 0) >= value){ // insert before head, if it fits
        	process->next = head;
        	process->start = 0;
        	head = process;
        	printf("ALLOCATED %s %d\n", process->name, process->start);
        	last_left_off = process->start + process->size;
        	return head;
      	}
			} else { // if it fits, just add at end of list
				// this is a special case where we add in the middle of empty space at end
				if(last_left_off > (prevNode->start + prevNode->size)){
					process->start = last_left_off;
        	prevNode->next = process;
        	last_left_off = process->start+process->size;
        	printf("ALLOCATED %s %d\n", process->name, process->start);
        	return head;
				}
				// add at end, dont care about no middle of empty space
				process->start = prevNode->start + prevNode->size;
    		prevNode->next = process;
    		last_left_off = process->start+process->size;
    		printf("ALLOCATED %s %d\n", process->name, process->start);
   		 	return head;
			}
		}
		if((traverse->start - last_left_off) >= process->size){
			process->start = last_left_off;
			prevNode->next = process;
			process->next = traverse;
			last_left_off = process->start+process->size;
			printf("ALLOCATED %s %d\n", process->name, process->start);
      return head;
		}
		// if you cant add, then keep on traversing until you can
    while(traverse->next != NULL){ // add in middle

      prevStart = traverse->start;
			// if we can ever add, we add, then return head immediately
      if((traverse->next->start - (prevStart+traverse->size)) >= value) {

        process->next = traverse->next;
        process->start = traverse->start + traverse->size;
        traverse->next = process;
        printf("ALLOCATED %s %d\n", process->name, process->start);
				last_left_off = process->start + process->size;
				return head;
      }
    traverse = traverse->next;
    }
      // if we couldnt add yet, just stick it at end
      process->start = traverse->start + traverse->size;
      if((process->start + process->size) > totalMemoryAmount) { // see if we can even add at end
        printf("FAIL REQUEST %s %d\n", process->name, process->size);
        free(process);
        return head;
      }
  		traverse->next = process;
  		last_left_off = process->start+process->size;
      printf("ALLOCATED %s %d\n", process->name, process->start);
		  return head;
	  } else { // WORSTFIT
        int index = 0;
        int biggestIndex = -1;
        int biggestSize = INT_MIN;
        // biggestIndex example   0|1|2|3|4| where | = nodes and indexes indicate space between
        // find where is spot in list to insert
        if((head->start - 0) >= value) { //should only be 0 when there is enough space in front of front node to place
          biggestIndex = 0;
          biggestSize = process->size;
        }
        // if cant fit before head, loop and find the biggest size
		    while(traverse->next != NULL) {
          prevStart = traverse->start;
          if((traverse->next->start - (prevStart+traverse->size)) >= value) { // if it fits
            int currentSize = traverse->next->start - (prevStart+traverse->size);
            if(currentSize > biggestSize) { // check to see if this space is bigger than previous space
              biggestSize = currentSize;
              biggestIndex = index; //if it is, keep track of where biggest space is
            }
          }
          index++; // used to keep track where to add,
          traverse = traverse->next;
        }
          // now add to the smallest spot
          int i = 0;
          if(biggestIndex == 0 && biggestSize == head->start) { // insert before head
            process->next = head;
            process->start = 0;
            head = process;
            printf("ALLOCATED %s %d\n", process->name, process->start);
			      return head;
        }
		    if(biggestIndex != -1) { // if there was space between the nodes
			     int spaceAtEnd = totalMemoryAmount-(traverse->start + traverse->size); // but the space is still not as big as space at end
			     if(biggestSize <= spaceAtEnd && (process->size <= spaceAtEnd)){ // if end is biggest space, and it fits
				       biggestIndex = -1; // do this so it adds at end
			     }
	       }
         if(biggestIndex == -1) { // there was no space in between nodes, so just put at end
            process->start = traverse->start + traverse->size;
            if((process->start + process->size) > totalMemoryAmount){ // checks to see if there is still space at end to add
              printf("FAIL REQUEST %s %d\n", process->name, process->size);
              free(process);
              return head;
            }
			    // if not, just put that node at the end
    			traverse->next = process;
          printf("ALLOCATED %s %d\n", process->name, process->start);
    			return head;
          }
              // insert middle
              node middleAdd = head;
              while(i != biggestIndex) { // loop till we get back to the index with the biggest size
                  i++;
                  middleAdd = middleAdd->next;
              }
              process->next = middleAdd->next;
              process->start = middleAdd->start + middleAdd->size;
              middleAdd->next = process;
              printf("ALLOCATED %s %d\n", process->name, process->start);
		          return head;
	   }
}

node deleteNode(node  head, char* label) {
  node traverse, prev;
  traverse = head;
	// delete head of list, and if node is not NULL
      if(traverse != NULL && (strcmp(traverse->name, label) ==0 )){
          if(traverse->next != NULL){
			        head = traverse->next;
			        traverse->next = NULL;
			        printf("FREE %s %d %d\n", traverse->name, traverse->size, traverse->start);
        	    free(traverse);
        	    return head;
		      } else { //if head was only thing in list
        			printf("FREE %s %d %d\n", traverse->name, traverse->size, traverse->start);
        			free(head);
        			return NULL;
		      }
      }
	    // traverse list  until we find match
      while(traverse != NULL && (strcmp(traverse->name, label) != 0)){
		      prev = traverse;
          traverse = traverse->next;
      }
	    // not in list
      if(traverse == NULL) {
		      printf("FAIL RELEASE %s\n", label);
          return head;
	    }

      // delete node in list
	     prev->next = traverse->next;
	     traverse->next = NULL;
	     printf("FREE %s %d %d\n", traverse->name, traverse->size, traverse->start);
       free(traverse);

       return head;
}

int findNode(node head, char* label) {
    node traverse;
	  traverse = head;
	  // traverse until we find the node
	  while(traverse != NULL && (strcmp(traverse->name, label) != 0)){
		    traverse = traverse->next;
	  }
	  // past end of the list
	  if(traverse == NULL){ //node not found
		    printf("FAULT\n");
		    return 0;
	  }
	  // else we found it, so just print it
	  printf("(%s, %d, %d)\n", label, traverse->size, traverse->start);
	  return 1; // node was found maybe switch this with if statement so !=
}

void listAvailable(node head){
	  node traverse = head;
	  if(head == NULL){ // if nothing in list, just list entire memory
	     printf("(%d, %d) ", totalMemoryAmount, 0);
		   printf("\n");
		   return;
	   }
	   int count = 0;	// used to keep track if there is not space at all
	   if(head->start != 0){ // if head is not at 0, print the size
		     count++; // space before head, so there is space
		     printf("(%d, %d) ", head->start-0, 0);
	   }
	   while(traverse->next != NULL){ // traverse list and list every space
		     int emptySpace = traverse->start + traverse->size;
		     if(emptySpace != traverse->next->start){ // sees if there are spaces between the nodes
			       count++; // space between nodes, so there is space
			       printf("(%d, %d) ", traverse->next->start - emptySpace, emptySpace);
		     }
		 traverse = traverse->next;
	   }
	   if((traverse->size + traverse->start) != totalMemoryAmount){ // if the end has space
		     printf("(%d, %d) ", totalMemoryAmount - (traverse->start+traverse->size), traverse->size + traverse->start);
	  } else { // else we are at the end
		     if(count == 0){ // if we didnt print any space at all, it is FULL
		         printf("FULL");
		     }
	  }
	printf("\n");
}

int main(int argc, char** argv){
	// check which memory allocation algorithm to use
	if(strcmp(argv[1], "BESTFIT") == 0){
		fit = 1;
	} else if(strcmp(argv[1], "FIRSTFIT") == 0) {
		fit = 2;
	} else if(strcmp(argv[1], "NEXTFIT") == 0){
		fit = 3;
	} else if(strcmp(argv[1], "WORSTFIT") == 0){
		fit = 4;
	}
	// transform the specificated memory size to totalMemory we can allocate
	totalMemoryAmount = atoi(argv[2]);

	// open the script and read instructions
	FILE *fp;
	fp = fopen(argv[3], "r");
	if(fp == NULL){
		perror("File not Found");
		exit(EXIT_FAILURE);
	}

	last_left_off = 0; // intialize this global just to make sure
	node head = NULL;
	char* buffer = NULL; // used to read in each word
	size_t buffer_size = 0;
	char* word;
	while(getline(&buffer, &buffer_size, fp) >= 0){ // read in each word
		word = strtok(buffer, " ,.-"); // read in the first word of the line
		if(strcmp(word, "#") != 0){ // if comment ignore line, and go next
			char* command = NULL;
			char process_name[16];
			int process_size = 0;
			int i = 0;
			while(word != NULL){// delimit the line by spaces
				if(strcmp(word, "\n") != 0){ // if line is not new line, then read
					if(i == 0) { command = word; } // assign first word to the command
					else if(i == 1) {strcpy(process_name, word); } // second word is always name
					else { process_size = atoi(word); } // sometimes we might not have this
				}
				word = strtok(NULL, " ,.-"); // read next word
				++i; // keeps track of which word this is
			}
			// after read in instruction, simulate it
			if(strcmp(command, "REQUEST") == 0){
				head = addNode(head, process_size, process_name);
			} else if(strcmp(command, "RELEASE") == 0){
				process_name[strlen(process_name) -1] = '\0'; // have to do this when last word is the process name
				head = deleteNode(head, process_name);
			} else if(strcmp(command, "LIST") == 0){
				// another if statement
				process_name[strlen(process_name) -1] = '\0';// have to do this when last word is the process name
				if(strcmp(process_name, "AVAILABLE") == 0) {
					// List Available
					listAvailable(head);
				} else if (strcmp(process_name, "ASSIGNED") == 0) {
					// List Assigned
					listAssigned(head);
				}
			} else if(strcmp(command, "FIND") == 0){
				process_name[strlen(process_name) -1] = '\0';// have to do this when last word is the process name
				findNode(head, process_name);
			}
		}
	}

	node current = head;
	node next;
	while(current!=NULL){
		next = current->next;
		free(current);
		current = next;
	}
	head = NULL;
}
