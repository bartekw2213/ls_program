#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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

void openCorrectDirectory(DIR **pDirectory, char* pathToDir) {
  *pDirectory = opendir(pathToDir);

  if ( *pDirectory == NULL ) {
    fprintf( stderr, "%s %d: opendir() failed (%s)\n", __FILE__, __LINE__, strerror( errno ));
    exit( -1 );
  }
}

void logDirectoryName(char* dirName) {
  printf("\033[33m%s", dirName);
  printf("\033[0m:\n");
}

bool doesStringContainChar(char* string, char a) {
  size_t upperBound = strlen(string);
  for(size_t i = 0; i < upperBound; i++)
    if(string[i] == a)
      return true;
  return false;
}

void logDetailedInfoAboutFile(struct dirent* pDirEnt) {
  struct stat fileStats;
  stat(pDirEnt->d_name, &fileStats);

  printf("%ld ", fileStats.st_size);
}

void logInfoAboutFile(struct executedCommand* commandInfo, struct dirent* pDirEnt) {
  if(doesStringContainChar(commandInfo->options, 'l'))
    logDetailedInfoAboutFile(pDirEnt);

  printf("%s\n", pDirEnt->d_name);
}

void logInfoAboutEachFileInDir(struct executedCommand* commandInfo, DIR **pDirectory) {
  struct dirent *pDirEnt;

  pDirEnt = readdir( *pDirectory );

  while ( pDirEnt != NULL ) {
    logInfoAboutFile(commandInfo, pDirEnt);
    pDirEnt = readdir( *pDirectory );
  }

  printf("\n");
}

void logDirectoryContent(struct executedCommand* commandInfo, char* pathToDir) {
  DIR *pDirectory;

  openCorrectDirectory(&pDirectory, pathToDir);
  logDirectoryName(pathToDir);
  logInfoAboutEachFileInDir(commandInfo, &pDirectory);
  
  closedir(pDirectory);
}

void freeCommandInfo(struct executedCommand* commandInfo) {
  free(commandInfo->options);
  free(commandInfo->pathToDir);
}

int main( int argc, char *argv[] ) {
  struct executedCommand commandInfo = {0 , NULL, false, NULL};

  getCommandInfoData(argc, argv, &commandInfo);

  if(commandInfo.isPathToDirPassed) {
    logDirectoryContent(&commandInfo, commandInfo.pathToDir);
  } else {
    logDirectoryContent(&commandInfo, ".");
  }

  freeCommandInfo(&commandInfo);
}


