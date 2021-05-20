#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

struct executedCommand {
  int optionsNum;
  char* options;
  bool isPathToDirPassed;
  char* pathToDir;
};

struct myFileStats {
  long int size;
  char modificationMonth[4];
  int modificationDay;
  int modificationHour;
  int modificationMinutes;
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

void convertFileModifiactionMonthToString(struct myFileStats* fileStats, int monthNum) {
  switch(monthNum) {
    case 0: strncpy(fileStats->modificationMonth, "Jan", 3); break;
    case 1: strncpy(fileStats->modificationMonth, "Feb", 3); break;
    case 2: strncpy(fileStats->modificationMonth, "Mar", 3); break;
    case 3: strncpy(fileStats->modificationMonth, "Apr", 3); break;
    case 4: strncpy(fileStats->modificationMonth, "May", 3); break;
    case 5: strncpy(fileStats->modificationMonth, "Jun", 3); break;
    case 6: strncpy(fileStats->modificationMonth, "Jul", 3); break;
    case 7: strncpy(fileStats->modificationMonth, "Aug", 3); break;
    case 8: strncpy(fileStats->modificationMonth, "Sep", 3); break;
    case 9: strncpy(fileStats->modificationMonth, "Oct", 3); break;
    case 10: strncpy(fileStats->modificationMonth, "Nov", 3); break;
    case 11: strncpy(fileStats->modificationMonth, "Dec", 3); break;
  }

  fileStats->modificationMonth[3] = '\0';
}

void convertFileModificationTime(struct stat* stats, struct myFileStats* fileStats) {
  struct timespec ts = stats->st_mtim;    
  struct tm *timeInfo = localtime(&ts.tv_sec);

  fileStats->modificationDay = timeInfo->tm_mday;
  fileStats->modificationHour = timeInfo->tm_hour;
  fileStats->modificationMinutes = timeInfo->tm_min;
  convertFileModifiactionMonthToString(fileStats, timeInfo->tm_mon);
}

void convertStatsAboutFile(struct stat* stats, struct myFileStats* fileStats) {
  fileStats->size = stats->st_size;
  convertFileModificationTime(stats, fileStats);
}

void logDetailedInfoAboutFile(struct dirent* pDirEnt) {
  struct stat stats;
  struct myFileStats fileStats;

  stat(pDirEnt->d_name, &stats);
  convertStatsAboutFile(&stats, &fileStats);

  printf("%8ld %s %02d %02d:%02d ", fileStats.size, fileStats.modificationMonth, fileStats.modificationDay, 
    fileStats.modificationHour, fileStats.modificationMinutes);
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


