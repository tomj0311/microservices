#include <stdio.h>

int main() 
{ 
    FILE* file_; 
    char buffer[1024]; 
    file_ = fopen("test.txt", "r"); 
    int ntimes = 0;
    while (!feof(file_)) // to read file 
    { 
        ntimes++;
        // fucntion used to read the contents of file 
        fread(buffer, sizeof(buffer), 1, file_); 
    } 
    printf("%d", ntimes);
    return 0; 
} 