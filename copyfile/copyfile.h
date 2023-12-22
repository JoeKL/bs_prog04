#ifndef COPYFILE_H_   /* Include guard */
#define COPYFILE_H_

int copyUsingBuffer(const char *file_src, const char *file_dest); 

int copyUsingMap(const char *file_src, const char *file_dest); 

int copyUsingSystemCall(const char *file_src, const char *file_dest); 

#endif // COPYFILE_H_