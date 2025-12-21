//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
static int smm_board_nr;
static int smm_food_nr;
static int smm_festival_nr;
static int smm_player_nr;


typedef struct {
    char name[MAX_CHARNAME];
    int pos;
    int credit;
    int energy;
    int flag_graduated;
    int flag_doingexp;//doing experiment:1, Not doing experiment:0
} smm_player_t;

smm_player_t *smm_players;

//
typedef struct {
    char name[MAX_CHARNAME];//food name
    int energy;//food energy
} smm_food_t;

smm_food_t *smm_foods;

typedef struct {
    char name[MAX_CHARNAME]; //festival name
} smm_festival_t;

//function prototypes
int isGraduated(void); //check if any player is graduated
void generatePlayers(int n, int initEnergy);
void printPlayerStatus(void); //print all player status at the beginning of each turn
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void actionNode(int player);//action code when a player stays at a node
void printGrades(int player); //print grade history of the player 
float calcAverageGrade(int player); //calculate average grade of the player
void printGrades(int player); //print all the grade history of the player

#if 0

smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)

#endif

float calcAverageGrade(int player) //calculate average grade of the player//(refers to findGrade, actionNode)
{
      int size = smmdb_len(LISTNO_OFFSET_GRADE + player);
      int i;
      void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
      int grade = smmObj_getObjectGrade(ptr);
      float grade_sum = 0.0;  
          for(i=0;i<size;i++)
          {   
              switch(grade)//change grade word to numerical score
              {
                  case GRADE_Ap :{
                  return 4.3;
                  break;
                  }
                  case GRADE_A0 :{
                  return 4.0;
                  break;
                  }
                  case GRADE_Am :{     
                  return 3.7;
                  break;
                  }
                  case GRADE_Bp :{
                  return 3.3;
                  break;
                  }
                  case GRADE_B0 :{
                  return 3.0;
                  break;
                  }
                  case GRADE_Bm :{
                  return 2.7;
                  break;
                  }
                  case GRADE_Cp :{
                  return 2.3;
                  break;
                  }
                  case  GRADE_C0 :{
                  return 2.0;
                  break;
                  }
                  case  GRADE_Cm :{
                  return 1.7;
                  break;
                  }
                  default :
                  break;
              }   
            grade_sum = grade_sum + smmObj_getObjectGrade(ptr);//total previous accumulated score + next accumulated score
           }
      return (grade_sum/size);//score average
}


void printGrades(int player)//print grade history of the player
{
     int size = smmdb_len(LISTNO_OFFSET_GRADE + player);
     int i;
     
     for(i=0;i<size;i++)
     {
     void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
     printf(" %s, %c, %f \n", smmObj_getObjectName(ptr), smmObj_getObjectGrade(ptr), smmObj_getObjectGrade(ptr));// print Lecture name, grade, gradescore 
     }
}



void* findGrade(int player, char *lectureName) //find the grade from the player's grade history
{
     int size = smmdb_len(LISTNO_OFFSET_GRADE + player);//the number of player's grade
     int i;
     
     for (i=0;i<size;i++)
     {
         void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         if (strcmp(smmObj_getObjectName(ptr), lectureName)==0)//( a, b) are identical
         {
              return ptr;
         }
     }
}
  
      
int isGraduated(void) //check if any player is graduated
{
    int i;
    for(i=0;i<smm_player_nr;i++)
    {
         if (smm_players[i].flag_graduated == 1)
         return 1;
    }
    return 0;
}


void goForward(int player, int step)//make player go "step" steps on the board
{
     int i;
     void *ptr;
     
     ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
     printf("-> die result : %i\n", step);
     printf("start from %i(%s) (%i)\n", smm_players[player].pos,
                                        smmObj_getObjectName(ptr), step);
     for (i=0;i<step;i++)               
     {
          smm_players[player].pos = (smm_players[player].pos + 1)%smm_board_nr;
          ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
          printf("  =>moved to %i(%s)\n", smm_players[player].pos, smmObj_getObjectName(ptr));
     }
}


void generatePlayers(int n, int initEnergy) //generate a new player
{
     int i;
     
     smm_players = (smm_player_t*)malloc(n*sizeof(smm_player_t));
     for (i=0;i<n;i++)
     {
          smm_players[i].pos = 0;
          smm_players[i].credit = 0;
          smm_players[i].energy = initEnergy;
          smm_players[i].flag_graduated = 0;
          smm_players[i].flag_doingexp = 0;
         
          printf("Input %i-th player name:\n", i+1);
          scanf("%s", &smm_players[i].name[0]);
          fflush(stdin);
          
     }
}


void printPlayerStatus(void) //print all player status at the beginning of each turn
{
     int i;
     void *ptr; 
     ptr = smmdb_getData(LISTNO_NODE, smm_players[i].pos);
     printf("\n\n=====================PLAYER STATUS=====================\n");
     
     for (i=0;i<smm_player_nr;i++)
     {
         //eexperiment status : doing experiment
         if(smm_players[i].flag_doingexp == 1)
         printf("%s - position: %i(%s), credit: %i, energy: %i, Doing experiment\n",
                    smm_players[i].name, smm_players[i].pos, smmObj_getObjectName(ptr), smm_players[i].credit, smm_players[i].energy);
         
         //experiment status : not doing experiment           
         else if(smm_players[i].flag_doingexp == 0)
         printf("%s - position: %i(%s), credit: %i, energy: %i, Not doing experiment\n",
                    smm_players[i].name, smm_players[i].pos, smmObj_getObjectName(ptr), smm_players[i].credit, smm_players[i].energy);
     }
     printf("=====================PLAYER STATUS=====================\n");
}
    
int rolldie(int player)
{
     char c;
     printf(" Press any key to roll a die (press g to see grade):");
     c = getchar();
     fflush(stdin);
#if 0
     if (c == 'g')
         printGrades(player);
#endif
    
     return (rand()%MAX_DIE + 1);
}


void actionNode(int player)//action code when a player stays at a node
{
     void *ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
     int type = smmObj_getObjectType(ptr);
     int credit = smmObj_getObjectCredit(ptr);
     int energy = smmObj_getObjectEnergy(ptr);
     int grade;
     void* gradePtr;
     int i;
     int threshold = 0;
    
     printf(" --> player %s, pos : %i, type : %s, credit : %i, energy : %i\n", 
                   smm_players[player].name, smm_players[player].pos, smmObj_getTypeName(ptr), credit, smm_players[player].energy);
      
         switch(type)
         { 
              case SMMNODE_TYPE_LECTURE: //have enough energy and new lecture, possible to choose join of drop
              {
                  char choice;
                  char join = '1';
                  int drop = '0';
                  ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
           
                  //case 1: have enough energy, new lecture
                  if((smm_players[player].energy >= smmObj_getObjectEnergy(ptr))&&(findGrade(player, smmObj_getObjectName(ptr))!= NULL ))//////////////////////////////////////
                  {
                       do{
                       //choose drop or join
                       printf("Lecture %s(credit:%i, energy:%i) starts! are you going to join(press : 1)? or drop(press : 0)?\n", 
                                                        smmObj_getObjectName(ptr), smmObj_getObjectCredit(ptr), smmObj_getObjectEnergy(ptr));
                       scanf(" %c", &choice);
                       //case 1-1: join the lecture and get credit(require energy)
                       if(choice == join)
                       {     
                           smm_players[player].credit += credit;
                           smm_players[player].energy -= energy;
                           grade = rand()%SMMNODE_MAX_GRADE;
            
                           gradePtr = smmObj_genObject(smmObj_getObjectName(ptr), SMMNODE_OBJTYPE_GRADE, type, credit, energy, grade);
                           smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
                           printf("%s successfully takes the lecture %s with grade %c (average : %i), remained energy : %i)\n", 
                             smm_players[player].name, smmObj_getObjectName(ptr), gradePtr, calcAverageGrade(smm_players[player].name), smm_players[player].energy);  
                       }//평균 학점 이상한 듯  
                       //case 1-2: drop
                       else if(choice == drop)
                       printf("Player %s drops the lecture %s\n", smm_players[player].name, smmObj_getObjectName(ptr));
                       //case 1-3: user input invalid value
                       else                                                                                   
                       printf("Invalid input! input \"drop\" or \"join\"\n");
                       }while(choice != join && choice != drop);
                
                 }
              //case 2: leck of energy
              else if(energy < smmObj_getObjectEnergy(ptr))
              printf("%s is too hungry to take the lecture %s (remained:%i, required:%i)\n",
                                             smm_players[player].name, smmObj_getObjectName(ptr), smm_players[player].energy, smmObj_getObjectEnergy(ptr)); 
               
              //case3: already took the lecture                                                                               
              else if((findGrade(player, smmObj_getObjectName(ptr))== NULL ))
              printf("%s already took a class\n", smm_players[player].name);//////////멘트 확인  
              else ; 
               
              break;
              }
              case SMMNODE_TYPE_RESTAURANT://add energy from restaurant(current energy + restaurant food energy)
              {
                  smm_players[player].energy += energy;//charge energy by restaurant food(current energy + restaurant food energy)
                  smmdb_getData(LISTNO_NODE, smm_players[player].pos);
                  printf("Let's eat in %s and charge %i energies(remained energy : %i)\n", 
                                                smmObj_getObjectName(ptr), smmObj_getObjectEnergy(ptr), smm_players[player].energy);
                  break;
               }
            
              case SMMNODE_TYPE_LABORATORY://roll die when player's status is doing experiment
                                                               //ㄴ(die result>=threshold: experiment end/ die result< threshold: stay NODE LABORATORY and do experience)
              { 
                  //case1 : status Doing experiment and reached to the LABORATORY NODE
                  if(smm_players[player].flag_doingexp == 1)
                  {
                      int exp_result;//experiment result
                  
                      printf(" ->Experiment time! Let's see if you can satisfy professor (threshold: %i)\n", threshold);
                      exp_result = rand()%MAX_DIE + 1;//to get random experiment result
            
                      if(exp_result >= threshold)
                      {
                          //case1-1: success experiment--> exit the lab
                          printf(" -> Experiment result : %i, success! %s can exit this lab!\n", exp_result, smm_players[player].name);
                          smm_players[player].flag_doingexp = 0;
                      }
                      //case1-2: fail experiment
                      else
                      printf(" -> Experiment result : %i, fail T_T %s needs more experiment.....\n", exp_result, smm_players[player].name);
                  }
                  //case2 : status Not doing experiment and reached to the LABORATORY NODE
                  else
                  printf("This is not experiment time. You can go through this lab.\n");
                       
                  break;
              }
         
              case SMMNODE_TYPE_HOME: //pass through and get energy
              {
                  smm_players[player].energy +=energy;//charge energy from HOME(current energy + energy from HOME)
                  printf("returned to HOME! energy charged by 18(total energy : %i)\n", smm_players[player].energy);
                  
                  //case: graduated(current pos: HOME and player's credit is higher than GRADUATE_CREDIT)
                  if (smm_players[player].credit >= GRADUATE_CREDIT)
                  {
                      smm_players[player].flag_graduated = 1;
                  }
                  break;                               
              }   
            
        
              case SMMNODE_TYPE_GOTOLAB: //status change(doing experiments), and move to LABORATORY, decide standard value of success experiment for random
                                                                              //(refers to 1.Boardfonfiging, goForward, case SMMNODE_TYPE_FOODCHANCE)
              {
                   int boardnode = rand()%smm_board_nr;//to get random node
                   void* Boardptr = smmdb_getData(LISTNO_NODE, boardnode);
                   void* ptr;
                   
                   printf("OMG! This is experiment time!! Player %s goes to the lab.\n", smm_players[player].name);
        
                   //status change(doing an experiment)
                   smm_players[player].flag_doingexp = 1;
                   
                   //move to LABORATORY(currunt pos--> LAB NODE)
                   do
                   {
                       boardnode = rand()%smm_board_nr;//to get random board node
                       Boardptr = smmdb_getData(LISTNO_NODE, boardnode);
                   }while(smmObj_getObjectType(Boardptr) != SMMNODE_TYPE_LABORATORY);//do random until to get LABORATORY node
                   
                   smm_players[player].pos = boardnode;//players position-->LABORATORY NODE
                   
                   //decide threshold for random(stand value of success experiment)
                   threshold = rand()%MAX_DIE + 1;
            
                   break;
              }
              
          
              case SMMNODE_TYPE_FOODCHANCE: //get random food and charge energy//(refers to findGrade, rolldie, case RESTAURANT)
              { 
                  char pick_food;//for scanf(store user's input)
                  int foodCount = smmdb_len(LISTNO_FOODCARD);//number of food card  
                  int foodcard = rand()%foodCount;//to get random index(food card)
                  void* foodptr = smmdb_getData(LISTNO_FOODCARD, foodcard);
            
                  printf("%s gets a food chance! ", smm_players[player].name);
                  printf("press any key to pick a food card:%c\n");
                  scanf(" %c", &pick_food); 
                  fflush(stdin);
               
                  smm_players[player].energy += smmObj_getObjectEnergy(foodptr);//increase accumulated energy by energy of current node
                  smmdb_getData(LISTNO_NODE, smm_players[player].pos);
                  printf("%s picks %s and charges %i(remained energy : %i)\n", 
                                        smm_players[player].name, smmObj_getObjectName(foodptr), smmObj_getObjectEnergy(foodptr), smm_players[player].energy);
                    
              break;
              }
            
            
              case SMMNODE_TYPE_FESTIVAL: //get random festival card (and do misson)//(refers to case SMMNODE_TYPE_FESTIVAL)
              {
                  char pick_fest;//for scanf(store user's input)
                  int festCount = smmdb_len(LISTNO_FESTCARD);//number of festival card
                  int festcard = rand()%festCount;//to get random index(festivl card)
                  void* festptr = smmdb_getData(LISTNO_FESTCARD, festcard);
                
                  printf("%s participates to Snow Festival!  ", smm_players[player].name);  
                  printf("press any key to pick a festival card\n");
                  scanf(" %c", &pick_fest);
                  fflush(stdin);
            
                  printf("MISSION : %s !!\n(Press any key when mission is ended.)\n", smmObj_getObjectName(festptr)); 
            
                  break;
              }
           
              default:
              break; 
         }
}    



int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int turn = 0;
    int i;
    
    smm_board_nr = 0;
    smm_food_nr = 0;
    smm_festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {    
         printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
         getchar();
         return -1;
    }
    
    printf("Reading board component......\n");
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
         //store the parameter set
         void* ptr;
         printf("%s %i %i %i\n" , name, type, credit, energy);
         ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_BOARD, type, credit, energy, 0);
         //smm_board_nr = smmdb_addTail(LISTNO_NODE, ptr);
         smmdb_addTail(LISTNO_NODE , ptr);//add the new nodes
         smm_board_nr++;//count board nodes
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", smm_board_nr);
    
    
   
    //2. food card config  //(refers to 1. boardConfig)
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
         printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
         return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (fscanf(fp, "%s %i", name, &energy) == 2) //read a food parameter set
    {
         //store the parameter set
         void* ptr;
         printf("%s %i\n", name, energy);
         ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FOOD, 0, 0, energy, 0);
         //smm_food_nr = smmdb_addTail(LISTNO_FOODCARD, ptr);
         smmdb_addTail(LISTNO_FOODCARD, ptr);//add new nodes
         smm_food_nr++;//count food cards
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", smm_food_nr);
    
   
    //3. festival card config //refers to 1. boardConfig)
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
         printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
         return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fscanf(fp, "%s", name) == 1) //read a festival card string
    {
         //store the parameter set
         void *ptr;
         printf("%s\n", name);
         ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FEST, 0, 0, 0, 0);
         //smm_festival_nr = smmdb_addTail(LISTNO_FESTCARD, ptr);
         smmdb_addTail(LISTNO_FESTCARD, ptr);//add new nodes
         smm_festival_nr++;//count festival cards
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", smm_festival_nr);
    
  
    
    //2. Player configuration ---------------------------------------------------------------------------------
    //opening
    printf("========================================================\n");
    printf("--------------------------------------------------------\n");
    printf("  Sookmyung Marble!! Let's Graduate(total credit : 30)  \n");
    printf("--------------------------------------------------------\n");
    printf("========================================================\n");
    
    do
    {
        //input player number to player_nr
        printf("Input player number:");
        scanf("%i", &smm_player_nr);
       
            if(smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER)
            printf("Invalid player number!\n");
        fflush(stdin);
    }
    while (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER);
    
    generatePlayers(smm_player_nr, smmObj_getObjectEnergy(smmdb_getData(SMMNODE_OBJTYPE_BOARD, 0)));
    
  
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    
    while (isGraduated() == 0)//is anybody graduated?
    {
         int die_result;
        
         //4-1. initial printing
         printPlayerStatus();
        
         //4-2. die rolling (if not in experiment)
         if(smm_players[i].flag_doingexp == 0)
         die_result =  rolldie(turn);
        
         //4-3. go forward
         goForward(turn, die_result);
        
		 //4-4. take action at the destination node of the board
         actionNode(turn);
        
         //4-5. next turn
         turn = (turn + 1)%smm_player_nr;
    }
    //game end(check graduate, graduated person printing)
    if(smm_players[i].flag_graduated)
    {        
    printf("%s is graduated !!", smm_players[i].name); 
    printGrades(i);
    }
            
    free(smm_players);
    
    system("PAUSE");
    return 0;
}
