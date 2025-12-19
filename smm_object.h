//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h

/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/
#define SMMNODE_TYPE_LECTURE           0
#define SMMNODE_TYPE_RESTAURANT        1
#define SMMNODE_TYPE_LABORATORY        2
#define SMMNODE_TYPE_HOME              3
#define SMMNODE_TYPE_GOTOLAB           4
#define SMMNODE_TYPE_FOODCHANCE        5
#define SMMNODE_TYPE_FESTIVAL          6

#define SMMNODE_OBJTYPE_BOARD          0
#define SMMNODE_OBJTYPE_GRADE          1
#define SMMNODE_OBJTYPE_FOOD           2
#define SMMNODE_OBJTYPE_FEST           3

#define GRADE_Ap                       0
#define GRADE_A0                       1
#define GRADE_Am                       2
#define GRADE_Bp                       3
#define GRADE_B0                       4
#define GRADE_Bm                       5
#define GRADE_Cp                       6
#define GRADE_C0                       7
#define GRADE_Cm                       8
/* grade :
    A+,
    A0,
    A-,
    B+,
    B0,
    B-,
    C+,
    C0,
    C-
*/
#define SMMNODE_MAX_GRADE       9


//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade);
char* smmObj_getObjectName(void *ptr);
int smmObj_getObjectType(void *ptr);
int smmObj_getObjectEnergy(void *ptr);
char* smmObj_getTypeName(void *ptr);
int smmObj_getObjectGrade(void *ptr);
//member retrieving


//element to string




#endif /* smm_object_h */
