#include <stdio.h> //used for FILE
#include <stdlib.h> //used for sizeof
#include <string.h> //string for strings
#include <math.h> //need math for round
#include "leak_detector_c.h"
#include "cop3502_as1.h"

void readMonster(FILE *ifp, monster **monsterList, int monsterNum){
    char nameTemp[25]; //have to consider how long names could be
    char elementTemp[10]; //consider how long element name could be
    int populationTemp;

    for(int i = 0; i< monsterNum; i++){
        monsterList[i] = malloc(sizeof(monster));//creates memory space for the actual struct
        //goes through each monster line
        fscanf(ifp, "%s %s %d\n", nameTemp, elementTemp, &populationTemp); //SCANS LINES OF FILE
        monsterList[i]->id = i+1; //KEEPS TRACK OF ID
        monsterList[i]->name = malloc((strlen(nameTemp) + 1) * sizeof(char)); //MAKES SPACE FOR THE NAME TO BE STORED
        strcpy(monsterList[i]->name, nameTemp); //COPIES THE NAME TO THE LIST
        monsterList[i]->element = malloc((strlen(elementTemp) + 1) * sizeof(char)); //MAKES SPACE FOR THE ELEMENT TO BE STORED
        strcpy(monsterList[i]->element, elementTemp); //COPIES THE ELEMENT TO THE LIST
        monsterList[i]->population = populationTemp; //how many monsters of that name and type
    }
}
monster *findMonster(char *name, monster **monsterList, int nmonsters){
    for (int i = 0; i<nmonsters; i++){
        if (strcmp(name, monsterList[i]->name) == 0){
                return monsterList[i];
        }
    }
    return NULL;
}

region *findRegion(char *name, region **regionList, int nregions){
    for (int i = 0; i<nregions; i++){
        if (strcmp(name, regionList[i]->name) == 0){
                return regionList[i];
        }
    }
    return NULL;
}

void readRegion(FILE *ifp, region **regionList, monster **monsterList, int regionNum, int monsterNum){
    char nameTemp[25]; //have to consider how long names could be
    int nmonstersTemp;
    char trash[10];
    int total;

    for(int i = 0; i<regionNum; i++){
        total = 0;
        regionList[i] = malloc(sizeof(region)); //allocates space for regionlist

        fscanf(ifp, "%s\n", nameTemp); //scans name
        regionList[i]->name = malloc((strlen(nameTemp) + 1) * sizeof(char)); //allocates space for names
        strcpy(regionList[i]->name, nameTemp); //stores in space made

        fscanf(ifp, "%d %s\n", &nmonstersTemp, trash); //scans address of number of monsters
        regionList[i]->nmonsters = nmonstersTemp; //assigns nmonsters to the temp that was found

        regionList[i]->monsters = malloc(sizeof(monster *) * nmonstersTemp); //allocates space for the regionlist monsters double pointers

        for(int j = 0; j<nmonstersTemp; j++){
            fscanf(ifp, "%s\n", nameTemp); //scans in the names of monsters

            //regionList[i]->monsters[j] = malloc(sizeof(monster));
            regionList[i]->monsters[j] = findMonster(nameTemp, monsterList, monsterNum);

            //find regionList[i]->total_population using regionList[i]->monsterList
            total = regionList[i]->monsters[j]->population + total;
        }
        regionList[i]->total_population = total; //store total population in total
        fscanf(ifp, "\n"); //skip blank line
    }
}
void readTrainers(FILE *ifp, trainer **trainerList, region **regionList, int regionNum, int trainerNum, itinerary **itineraryList){
    char nameTemp[25]; //have to consider how long names could be
    int nregionsTemp;
    int captureNumTemp;
    char trash[10];

    for(int i = 0; i<trainerNum; i++) { //creates for loop for how many trainers there are
        trainerList[i] = malloc(sizeof(trainer)); //creates space for trainerList struct

        fscanf(ifp, "%s\n", nameTemp); //scans name of trainer
        trainerList[i]->name = malloc((strlen(nameTemp) + 1) * sizeof(char)); //creates space for trainer
        strcpy(trainerList[i]->name, nameTemp); //copies trainer name into the space created for them

        itineraryList[i] = malloc(sizeof(itinerary)); //creates space for trainerList struct
        fscanf(ifp, "%d %s\n", &captureNumTemp, trash); //scan how many captures they want
        itineraryList[i]->captures = captureNumTemp;

        itineraryList[i]->regions = malloc(sizeof(region *) * nregionsTemp); //makes space for ptr to region

        fscanf(ifp, "%d %s\n", &nregionsTemp, trash); //scans number of regions after captures
        itineraryList[i]->nregions = nregionsTemp; //nregionsTemp is stored in nregions of itinerary

        for(int j = 0; j<nregionsTemp; j++){ //for loop to know how many regions to read before it goes to the next trainer

            fscanf(ifp, "%s\n", nameTemp); //scans name of region
            //itineraryList[i]->regions[j] = malloc(sizeof(region));
            itineraryList[i]->regions[j] = findRegion(nameTemp, regionList, regionNum);
        }

        trainerList[i]->visits = itineraryList[i]; //visits in trainerList points to the itineraryList struct
        fscanf(ifp, "\n"); //skips next line after first trainers regions
    }
}
void calculations(FILE *ofp, monster **monsterList, int monsterNum, region **regionList, int regionNum, trainer **trainerList, int trainerNum)
{
    for (int i = 0; i < trainerNum; i++)
    {
        fprintf(ofp, "%s\n", trainerList[i]->name); //print trainer names
        for(int j = 0; j < trainerList[i]->visits->nregions; j++) //for the amount of regions print region names in visits wanted
        {
            fprintf(ofp, "%s\n", trainerList[i]->visits->regions[j]->name); //print name of regions that want to be visited
            {
                for (int k = 0; k < trainerList[i]->visits->regions[j]->nmonsters; k++)
                {
                    int relPop = trainerList[i]->visits->regions[j]->monsters[k]->population; //Monster population = population of single monster/ relative pop
                    int captures = trainerList[i]->visits->captures; //how many captures are wanted
                    double monNum = (double)relPop / trainerList[i]->visits->regions[j]->total_population; //monNum is relPop/totalPop have to use float and double to get decimals
                    monNum = monNum * captures; //relativePop/totalPop * number of captures raw/divided * captures = monster
                    int catches = round(monNum); //catches = monNum rounded using math.h
                    if (catches != 0) //if the monster catches is not = 0 print the monster name
                    {
                        fprintf(ofp, "%d %s\n", catches, trainerList[i]->visits->regions[j]->monsters[k]->name); //prints monster name if catches != 0
                    }
                }
            }
        }
          fprintf(ofp, "\n"); //print blank line between each trainer
    }
}
int main()
{
    //scan in file
    atexit(report_mem_leak); //memory leak report
    FILE *ifp = fopen("as1-sample-input-2.txt", "r");

    //error check: file read correctly
    if (ifp == NULL){
        printf("fail\n");
    }
    //create monster, region, and trainer list
   // monster **monsterList;
    monster **monsterList;
    trainer **trainerList;
    region **regionList;
    itinerary **itineraryList;

    //sizes for each array
    int monsterNum, regionNum, trainerNum; //first number read to store
    char trash[10]; //monster, regions, trainers after first number gets trashed

    //read monsters
    fscanf(ifp, "%d %s\n", &monsterNum, trash);
    monsterList = malloc(sizeof(monster *) * monsterNum);

    readMonster(ifp, monsterList, monsterNum);


    //read regions
    fscanf(ifp, "\n");
    fscanf(ifp, "%d %s\n", &regionNum, trash);
    regionList = malloc(sizeof(region *) * regionNum);

    fscanf(ifp, "\n");
    readRegion(ifp, regionList, monsterList, regionNum, monsterNum);


    //read trainers
    fscanf(ifp, "\n");
    fscanf(ifp, "%d %s\n", &trainerNum, trash);
    trainerList = malloc(sizeof(trainer *) * trainerNum);

    fscanf(ifp, "\n"); //skip line after trainers

    itineraryList = malloc(sizeof(itinerary *) *trainerNum);
    readTrainers(ifp, trainerList, regionList, regionNum, trainerNum, itineraryList); //function to read whats under tainers

    //print out list of monsters to check
    printf("\n MONSTER LIST:\n");
    for(int k = 0; k<monsterNum; k++){
          printf("Contents of structure: %s is %s, Population: %d, ID: %d\n", monsterList[k]->name, monsterList[k]->element,monsterList[k]->population, monsterList[k]->id);
    }

    printf("\n REGION LIST:\n");
    for(int k = 0; k<regionNum; k++){
          printf("Contents of structures: %s Number monsters: %d Population: %d \n", regionList[k]->name, regionList[k]->nmonsters, regionList[k]->total_population);
          for(int m = 0; m < regionList[k]->nmonsters; m++){
            printf("%s ", monsterList[m]->name);
          }
          printf("\n");
    }

    printf("\n TRAINER LIST:\n");
   for(int k = 0; k<trainerNum; k++){
          printf("Contents of structures: %s Regions: %d Captures int: %d\n", trainerList[k]->name, trainerList[k]->visits->nregions, itineraryList[k]->captures);
          printf("\n");
    }
    //calculations
    FILE *ofp = fopen("cop3502-as1-output-bachleda-michal-2.txt", "w"); //open outputfile
    calculations(ofp, monsterList, monsterNum, regionList, regionNum, trainerList, trainerNum);

    //deallocating all memory
 for (int i = 0; i < monsterNum; i++)
    {
        free(monsterList[i]->name);
        free(monsterList[i]->element);
        free(monsterList[i]);
    }

    free(monsterList);

    for (int i = 0; i < regionNum; i++)
    {
        free(regionList[i]->name);
        free(regionList[i]->monsters);
        free(regionList[i]);
    }

    free(regionList);

    for (int i = 0; i < trainerNum; i++)
    {
        free(trainerList[i]->name);
        free(trainerList[i]->visits->regions);
        free(trainerList[i]->visits);
        free(trainerList[i]);
    }

    free(trainerList);
    free(itineraryList);

    printf("End of file");

    fclose(ifp);
    fclose(ofp);
    return 0;
}
