/*----------------------------------------------*/
/*PROGRAM Peg Solitaire:                        */
/* Peg solitaire is a traditional game,played   */
/* with marbles(O) sat on wooden board.The pegs */
/* can take another peg by jumping it going up, */
/* down,left or right.                          */
/*----------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CSEC (double) (CLOCKS_PER_SEC)

struct move{
	char *board;  /*board is stored as string*/
	int n;
	struct move *parent;
};     /*store a board and information allowing its parent board to be identified */ 
typedef struct move Move;

struct save_pointer{
	Move *n;
	struct save_pointer *next;
};  /* link board*/
typedef struct save_pointer Save;

struct tree{
	struct tree *left,*right;
};  /*store characters from board */
typedef struct tree Tree;

Move *AllocateMove();
Move *read_board(FILE *fp,int x, int y);  /*read board from file */
void print_board(int x,int y,Move *last_board);   /* print solution into file */
int move_board(int x,int y,Move *p);  /* jump pegs and estimate possible ways */
int compare_board(Move *m,int x,int y,Tree *tree);  /* find repeated boards */
int completed_board(int x,int y,Move *last_board);  /*estimate the final situation */
void Addlist(Move *m,Save **endp);   /* add boards into link list */
int count_board(int x,int y,Move *p);  /* count the number of pegs in the current boards*/
Move *create_newboard(Save *parent_board,int x,int y);  /*create new board*/
void onemove(int cond, int startpos, int pegpos, int gappos, int x, int y,Save *temp, Save **endp);
/*onemove function is used to estimate the situation of pegs' movement*/

int main(int argc,char **argv)
{
	FILE *fp;
	Move *head;
    clock_t c1,c2;
    int width,length,value=0;
    if (argc==2){
	  printf("--------------------------------------------------------------\n");
	  printf("          Welcome to the game of Peg Solitaire.\n");
	  printf("  You should enter the size of board which you want to play :)\n");
	  printf("--------------------------------------------------------------\n");
	  printf("Please enter the size of board(e.g. 5 5):");
	  value=scanf("%d %d",&width,&length);
	  if (value!=2){
	 	printf("Please enter the right form as e.g.!\n");
		return 1;
	  }
	  if (width % 2==0){
		printf("\nSorry, the size of board should be odd number!\n");
		return 1;
	  }
	  c1=clock();   /* test running time */
	  if ((fp=fopen("input.txt","r"))==NULL){ /*open a file for reading*/
		printf("\nCan not open file\n");
		return 1;
	  }
	  head=read_board(fp,length,width);   /*read the initial board, size x*y, from file fp*/
	  move_board(length,width,head);      /* jump pegs and estimate possible ways*/
	}
   else{
     printf("\nYou gave a wrong form!Try e.g. pegboard input.txt test.txt!\n");
     return 1;
   }
   c2=clock();    /* test running time */
   printf("\nRunning time:%f\n",(double)(c2-c1)/CSEC);
   return 0;
}

/*read the initial board from file and check whether it is valid or not*/

Move *read_board(FILE *fp,int x,int y)
{
	int i,j;
	char ch;
	Move *p;
    i=0;
    j=0;
	p=AllocateMove();
	p->board=(char *) malloc(x*y);
	/*read the board and check whether it is valid or not*/
    while (ch!=EOF && i<x*y){
       ch=fgetc(fp);
       if (ch=='\n'){
		 if (i % x!=0){
			printf("\nSorry,the board is invalid!\nPlease check your board first.\n");
			exit(1);
		 }
	   }
       if (ch=='O' || ch=='.'){
	      *(p->board+i)=ch;
	      i++;
       }
    } 
    if (i!=x*y){
       printf("\nThe width of board is wrong!\n");
       exit(1);
    }
    fclose(fp);
	p->parent=NULL;
    return p;
}

/* create spaces */

Move *AllocateMove()
{
	Move *p;
	p=(Move *)malloc(sizeof(Move));
	return p;
}

/* count the numbers of pegs in the current board */

int count_board(int x,int y,Move *p)
{
	int count,i;
	count=0;
	for (i=0;i<x*y;i++){
	   if (*(p->board+i)=='O'){
			count++;
	   }
	}
	return count;
}

/*This is the main function of Peg Solitaire */

int move_board(int x,int y, Move *p)
{
	int i,j,search;
	Save *temp,*List,**endp,*end;
	List=(Save *)malloc(sizeof(Save));
    List->n=p;
    List->next=NULL;
	temp=List;
	end=List;
	endp=&end;
	search=0;
	while(temp!=NULL){  
        search=completed_board(x,y,temp->n);
        if (search==1){
			print_board(x,y,temp->n);
            return 0;
        }
        else{
		  for (i=0;i<y;i++){
		    for (j=0;j<x;j++){
		      if (*(temp->n->board+i*x+j)=='O'){ 
                onemove(i-2>=0,i*x+j,(i-1)*x+j,(i-2)*x+j,x,y,temp,endp);
				onemove(i+2<y,i*x+j,(i+1)*x+j,(i+2)*x+j,x,y,temp,endp);
				onemove(j-2>=0,i*x+j,i*x+j-1,i*x+j-2,x,y,temp,endp);
				onemove(j+2<x,i*x+j,i*x+j+1,i*x+j+2,x,y,temp,endp);
			  }
			}
		  }
		}
		temp=temp->next;
	}
	if (search!=1)
		printf("This Peg Solitaire has no solution!\n");
	return 0;
}
void onemove(int cond, int startpos, int pegpos, int gappos, int x, int y,Save *temp, Save **endp)
{
	Move *new_board;
	Tree *tree;
	tree=(Tree *)malloc(sizeof(Tree));
	tree->left=NULL;
	tree->right=NULL;
	if (cond && *(temp->n->board+pegpos)=='O' && *(temp->n->board+gappos)=='.'){
	  	new_board=create_newboard(temp,x,y);
		*(new_board->board+gappos)='O';
		*(new_board->board+pegpos)='.';
		*(new_board->board+startpos)='.';
		new_board->parent=temp->n;
		if (compare_board(new_board,x,y,tree)==1){
			Addlist(new_board,endp);
		}
	}
}

/*add generable and unrepeated board into the list*/

void Addlist(Move *m,Save **endp)
{   
   Save *temp,*new_list;
   temp=*endp;
   new_list=(Save *) malloc(sizeof(Save));
   new_list->n=m;
   temp->next=new_list;
   new_list->next=NULL;
   *endp=new_list;
}

/*create new board*/

Move *create_newboard(Save *parent_board,int x,int y)
{
   Move *new_board;
   new_board=AllocateMove();
   new_board->board=(char *) malloc(x*y);
   strncpy(new_board->board,parent_board->n->board,x*y);
   return new_board;
}

/* estimate that whether it finds the solution or not */

int completed_board(int x,int y,Move *last_board)
{
   int i,n;
   n=0;
   for (i=0;i<x*y;i++){
	   if (*(last_board->board+i)=='O'){
			 n++;
	   }
   }
   if (n==1 && *(last_board->board+(x*y-1)/2)=='O'){ /*only one peg and also in the centre of board*/
        return 1;
   }
   else{
       return 0;
   }
}

/*compare characters on the board*/

int compare_board(Move *m,int x,int y,Tree *tree)
{
   int i;
   int compare=0;
   for (i=0;i<x*y;i++){
    if (*(m->board+i)=='.'){
     if (tree->left==NULL){
      compare=1;
      tree->left=(Tree *)malloc(sizeof(Tree));
      tree->left->left=NULL;
      tree->left->right=NULL;
     }
      tree=tree->left;
    }
    else{
     if (tree->right==NULL){
      compare=1;
      tree->right=(Tree *)malloc(sizeof(Tree));
      tree->right->left=NULL;
      tree->right->right=NULL;
     }
     tree=tree->right;
    }
   }
   return compare;
}

/*print board into a file*/

void print_board(int x,int y,Move *last_board)
{
	int i,j,n;
    FILE *output;
	n=-1;
	if ((output=fopen("output.txt","w"))==NULL){  /*open a file for writing*/
		printf("Can not open file\n");
		exit(0);
	}
	fprintf(output,"The solution of this Peg Solitaire is in reverse order,please look at it from the bottom:\n");
	fprintf(output,"\n");
	while (last_board!=NULL){
	   for (i=0;i<y;i++){
	  	 for (j=0;j<x;j++){
			fprintf(output,"%2c",*(last_board->board+i*x+j));
		 }
		 fprintf(output,"\n");
	   }
	   fprintf(output,"\n");
	   n++;
	   last_board=last_board->parent;
	}
	fprintf(output,"The total steps are %d.\n",n);
	fclose(output);
}
