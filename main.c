#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// int main( int argc, char *argv[] ) {
//   DIR *pDIR;
//   struct dirent *pDirEnt;
//   struct stat fileStat;
//   pDIR = opendir(".");

//   if ( pDIR == NULL ) {
//     fprintf( stderr, "%s %d: opendir() failed (%s)\n", __FILE__, __LINE__, strerror( errno ));
//     exit( -1 );
//   }

//   pDirEnt = readdir( pDIR );

//   while ( pDirEnt != NULL ) {
//     //  printf( "%s %i\n", pDirEnt->d_name, pDirEnt->d_reclen );
//     stat(pDirEnt->d_name, &fileStat);
//     printf( "%s %ld\n", pDirEnt->d_name, fileStat.st_size);
//     pDirEnt = readdir( pDIR );
//   }

//   closedir( pDIR );
//   return 0;
// }

struct executedCommand {
  int optionsNum;
  char* options;
  bool isPathToDirPassed;
  char* pathToDir;
};

void determineIfPathToDirIsPassed(int passedArgumentsCount, char* lastPassedOption, struct executedCommand* commandInfo) {
  if(passedArgumentsCount > 1 && (lastPassedOption[0] == '.' || lastPassedOption[0] == '/'))
    commandInfo->isPathToDirPassed = true;
  else
    commandInfo->isPathToDirPassed = false;
}

void readPathToDir(char* lastPassedOption, struct executedCommand* commandInfo) {
  if(commandInfo->isPathToDirPassed) {
    size_t pathSize = strlen(lastPassedOption);
    commandInfo->pathToDir = malloc(sizeof(char) * pathSize + 1);
    strncpy(commandInfo->pathToDir, lastPassedOption, pathSize + 1);
  } else {
    commandInfo->pathToDir = NULL;
  }
}

void getPathToDir(int passedArgumentsCount, char* lastPassedOption, struct executedCommand* commandInfo) {
  determineIfPathToDirIsPassed(passedArgumentsCount, lastPassedOption, commandInfo);
  readPathToDir(lastPassedOption, commandInfo);
}

void addOption(char optionCharacter, struct executedCommand* commandInfo) {
  int oldOptionsNum = commandInfo->optionsNum;
  ++commandInfo->optionsNum;

  char* temp = malloc(sizeof(char) * commandInfo->optionsNum + 1);
  strncpy(temp, commandInfo->options, oldOptionsNum);
  temp[commandInfo->optionsNum - 1] = optionCharacter;
  temp[commandInfo->optionsNum] = '\0';

  free(commandInfo->options);
  commandInfo->options = temp;
}

void unpackOption(char* option, struct executedCommand* commandInfo) {
  if(option[0] != '-') {
    printf("Each options should be proceeded with -\n");
    exit(-1);
  }

  for(size_t i = 1; i < strlen(option); i++)
    addOption(option[i], commandInfo);
}

void getOptions(int argc, char *argv[], struct executedCommand* commandInfo) {
  int lastCheckedOptionIndex = argc - 1;

  if(commandInfo->isPathToDirPassed)
    --lastCheckedOptionIndex;
  
  for(int i = 1; i <= lastCheckedOptionIndex; i++)
    unpackOption(argv[i], commandInfo);
}

void getCommandInfoData(int argc, char *argv[], struct executedCommand* commandInfo) {
  getPathToDir(argc, argv[argc - 1], commandInfo);
  getOptions(argc, argv, commandInfo);
}

void freeExecutedCommandStructure(struct executedCommand* commandInfo) {
  free(commandInfo->options);
  free(commandInfo->pathToDir);
}

int main( int argc, char *argv[] ) {
  struct executedCommand commandInfo = {0 , NULL, false, NULL};

  getCommandInfoData(argc, argv, &commandInfo);
  freeCommandInfo(&commandInfo);
}


