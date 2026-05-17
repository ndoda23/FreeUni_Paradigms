using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"
#include <cstring>

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const { 
  int numActors = *(int*)actorFile;
  
  int left = 0;
  int right = numActors - 1;
  
  while(left <= right) {
    int middle = (left + right) / 2;
    int offset = *((int*)actorFile + middle + 1);
    char* name = (char*)actorFile + offset;
    
    if(strcmp(player.c_str(), name) == 0) {
      int nameLength = strlen(name) + sizeof(char);

      if(nameLength%2==1){
        nameLength++;
      }

      short* filmsNumberPointer = (short*)(name + nameLength);
      short filmsNumber = *filmsNumberPointer;

      int extraspace = 0;
      if((nameLength+2)%4 == 2){
        extraspace = 2;
      }else{
        extraspace = 0;
      }

      int* startingMovie = (int*) ((char*)filmsNumberPointer + sizeof(short) + extraspace); 
      
      for(int k = 0; k < filmsNumber;k++){
        char* movie = (char*)movieFile + *(startingMovie + k);
        film mv;
        mv.title = movie;
        unsigned char year = *((unsigned char*)movie + strlen(movie) + sizeof(char));
        mv.year  = 1900 + year;
        films.push_back(mv);
      }

      return true;
    }
    
    if(strcmp(player.c_str(), name) < 0) {
      right = middle - 1;
    } else {
      left = middle + 1;
    }
  }
  
  return false;
}


bool imdb::getCast(const film& movie, vector<string>& players) const {
  int numMovies = *(int*)movieFile;
  
  int left = 0;
  int right = numMovies - 1;
  
  while(left <= right) {
    int middle = (left + right) / 2;
    int offset = *((int*)movieFile + middle + 1);
    char* movStart = (char*)movieFile + offset;

    film currentFilm; 
    currentFilm.title = movStart;

    int nameLength = strlen(movStart) + sizeof(char);
    int startingYear = 1900;

    unsigned char* plusYear = (unsigned char*)movStart + nameLength;
    int yeaR = startingYear + *plusYear;

    currentFilm.year = yeaR;

    if(currentFilm == movie){
      int extraspace = 0;
      if((nameLength + sizeof(char))%2==1){
        extraspace = 1;
      }

      short* numActorsP = (short*)(movStart+nameLength+sizeof(char)+extraspace);
      short numActors = *numActorsP;

      int secExtraSpace = 0;
      if((nameLength + sizeof(char) + extraspace + sizeof(short))%4==2){
        secExtraSpace = 2;
      }

      int* startOfActors = (int*)(movStart + nameLength + sizeof(char) + extraspace + sizeof(short)+secExtraSpace);
      int* startPointer = startOfActors;
      
      for(int k = 0 ; k < numActors; k++){
        char* name = ((char*)actorFile + *startPointer);
        players.push_back(name);
        startPointer++;
      }
      return true;
    }
    
    if(currentFilm < movie) {
      left = middle + 1;
    } else {
      right = middle - 1;
    }
  }
  
  return false;  
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}






// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
