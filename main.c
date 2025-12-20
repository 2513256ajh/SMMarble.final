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
    int flag_doingexp;
} smm_player_t;

smm_player_t *smm_players;

typedef struct {
    char name[MAX_CHARNAME];
    int energy;
} smm_food_t;

smm_food_t *smm_foods;

typedef struct {
    char name[MAX_CHARNAME];
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

float calcAverageGrade(int player) //calculate average grade of the player
{
      int size = smmdb_len(LISTNO_OFFSET_GRADE + player);
      int i;
      void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
      int grade = smmObj_getObjectGrade(ptr);
      float grade_sum = 0.0;//이거 확인. 0.0000 ??    
          for(i=0;i<size;i++)
          {   
              switch(grade)//actionNode는 왜 된 거야 ????  
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
            grade_sum = grade_sum + smmObj_getObjectGrade(ptr);
           }
      return grade_sum/size;
}


void printGrades(int player)//print grade history of the player
{
     int size = smmdb_len(LISTNO_OFFSET_GRADE + player);
     int i;
     
     for(i=0;i<size;i++)
     {
     void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
     printf(" %s, %c, %f \n", smmObj_getObjectName(ptr), smmObj_getObjectGrade(ptr), smmObj_getObjectGrade(ptr));//Lecture name, grade, gradescore 
     }
}



void* findGrade(int player, char *lectureName) //find the grade from the player's grade history
{
     int size = smmdb_len(LISTNO_OFFSET_GRADE + player);
     int i;
     
     for (i=0;i<size;i++)
     {
         void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         if (strcmp(smmObj_getObjectName(ptr), lectureName)==0)
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


void goForward(int player, int step)//make player go "step" steps on the board(check if player is graduated)
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
     int i,j;
     void *ptr; 
     ptr = smmdb_getData(LISTNO_NODE, smm_players[j].pos);
     printf("\n\n=====================PLAYER STATUS=====================\n");
     
     for (i=0;i<smm_player_nr;i++)
     {
         printf("%s - position: %i(%s), credit: %i, energy: %i\n",
                    smm_players[i].name, smm_players[i].pos, smmObj_getObjectName(ptr), smm_players[i].credit, smm_players[i].energy);
                    ////////////////////// 실험 중 미해결(실험중, 실험 안 함 지정해야할까? )  
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
    
     printf(" --> player %s, pos : %i, type : %s, credit : %i, energy : %i\n", smm_players[player].name, smm_players[player].pos, smmObj_getTypeName(ptr), credit, smm_players[player].energy);
     
     switch(type)
     {
          case SMMNODE_TYPE_LECTURE: //현재 에너지가 필요에너지 이상 있고 이전에 
        //듣지 않은 강의이면 수강 가능, 수강 혹은 드랍 선택할 수 있
          {
          char choice;
          char join;
          char drop;
          void* ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
           
          //enough energy, new lecture--> choose join/drop
          if((energy >= smmObj_getObjectEnergy(ptr))&&(findGrade(player, smmObj_getObjectName(ptr))== NULL ))//////////////////////////////////////
               {
               do{
               printf("Lecture %s(credit:%i, energy:%i) starts! are you going to join? or drop?\n", smmObj_getObjectName(ptr),
                                                                                                  smmObj_getObjectCredit(ptr), smmObj_getObjectEnergy(ptr));
               scanf(" %c", &choice);
                   if(choice == join)
                   {
                   smm_players[player].credit += credit;
                   smm_players[player].energy -= energy;
            
                   grade = rand()%SMMNODE_MAX_GRADE;
            
                   gradePtr = smmObj_genObject(smmObj_getObjectName(ptr), SMMNODE_OBJTYPE_GRADE, type, credit, energy, grade);
                   smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
                   printf("%s successfully takes the lecture %s with grade %c (average : %i), remained energy : %i)\n", smm_players[player].name, smmObj_getObjectName(ptr),
                                                                                                                      gradePtr, calcAverageGrade(smm_players[player].name), smm_players[player].energy);
                   }//평균 학점  
                   else if(choice == drop) 
                   printf("Player %s drops the lecture %s\n", smm_players[player].name, smmObj_getObjectName(ptr));
                  
                   else                                                                                   

                   printf("Invalid input! input \"drop\" or \"join\"/n");
                 }while(choice != join || choice != drop);
               }
               else if(energy < smmObj_getObjectEnergy(ptr))
               printf("%s is too hungry to take the lecture %s (remained:%i, required:%i)\n", smm_players[player].name, smmObj_getObjectName(ptr),
                                                                                              smm_players[player].energy, smmObj_getObjectEnergy(ptr));//required 멘트확인  
               else if((findGrade(player, smmObj_getObjectName(ptr))!= NULL ))
               printf("");//////////멘트 확인 
               
               break;
          }
          case SMMNODE_TYPE_RESTAURANT: //멘트 확인 
               smm_players[player].energy += energy;
               smmdb_getData(LISTNO_NODE, smm_players[player].pos);
               printf("Let's eat in %s and charge %i energies(remained energy : %i)\n", smmObj_getObjectName(ptr), smmObj_getObjectEnergy(ptr), smm_players[player].energy);
               break;
            
          case SMMNODE_TYPE_LABORATORY:
          { 
               int exp_result;
                   if(smm_players[player].flag_doingexp == 1)
                   {
                   printf(" ->Experiment time! Let's see if you can satisfy professor (threshold: 5)\n");
                   exp_result = rolldie(player); //////////////이거 잘 모르겠음  
            
                       if(exp_result >= 5)
                       {
                       printf(" -> Experiment result : %i, success! %s can exit this lab!\n", exp_result);
                       smm_players[player].flag_doingexp == 0;
                       }
            
                       else
                       printf(" -> Experiment result : %i, fail T_T %s needs more experiment.....\n", exp_result);//die result 자리  
                       }
                       else
                       printf("This is not experiment time. You can go through this lab.\n");
                       
                       break;
           }
                   
                   
           case SMMNODE_TYPE_HOME: 
               smm_players[player].energy +=energy;
                   if (smm_players[player].credit <= GRADUATE_CREDIT)
                   {
                   smm_players[player].flag_graduated = 1;
                   
                   break;                               
                   }
            
        
           case SMMNODE_TYPE_GOTOLAB: 
               printf("OMG! This is experiment time!! Player %s goes to the lab.\n", smm_players[player].name);
               smm_players[player].flag_doingexp == 1;
               break;
         
           case SMMNODE_TYPE_FOODCHANCE: //get random food and charge energy
           { 
               char pick_food;
               int foodCount = smmdb_len(LISTNO_FOODCARD);//number of food card  
               int foodcard = rand()%foodCount;//to get random index(food card)
               void* foodptr = smmdb_getData(LISTNO_FOODCARD, foodcard);
            
               printf("%s gets a food chance! ", smm_players[player].name);
               printf("press any key to pick a food card: %c\n");
               scanf(" %c", &pick_food); 
               fflush(stdin);
               
               smm_players[player].energy += smmObj_getObjectEnergy(ptr);//increase accumulated energy by energy of current node
               
               printf("%s picks %s and charges %i(remained energy : %i)\n", smm_players[player].name, smmObj_getObjectName(foodptr),
                                                                            smmObj_getObjectEnergy(foodptr), smm_players[player].energy);
                    
            break;
            }
            
            
            case SMMNODE_TYPE_FESTIVAL: //get random festival card (and do misson)
            {
                char pick_fest;
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
            system("PAUSE");
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
         smmdb_addTail(LISTNO_NODE, ptr);
         smm_board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", smm_board_nr);
    
    
   
    //2. food card config 
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
         smm_food_nr = smmdb_addTail(LISTNO_FOODCARD, ptr);
         
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", smm_food_nr);
    
   
    //3. festival card config 
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
         ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FEST, 0, 0, 0, 0);////////////축제는 에너지 소모 없나??? 
         smm_festival_nr = smmdb_addTail(LISTNO_FESTCARD, ptr);
         
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
         die_result =  rolldie(turn);
        
         //4-3. go forward
         goForward(turn, die_result);
         //pos = pos+2;
        
		 //4-4. take action at the destination node of the board
         actionNode(turn);
        
         //4-5. next turn
         turn = (turn + 1)%smm_player_nr;
    }
    //game end(graduated person printing)
    
    if(smm_players[i].flag_graduated)
    {        
    printf("%s is graduated !!", smm_players[i].name);/////////////여기 이름이 나와야해 졸업한 사람의!!  
    printGrades(i);
    }
            
    free(smm_players);
    
    system("PAUSE");
    return 0;
}
