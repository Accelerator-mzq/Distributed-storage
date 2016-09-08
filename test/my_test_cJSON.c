#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

int main(int argc, char *argv[])
{

    const char *src[3] = {"marong", "fengjie", "chunBro"}; 
    cJSON *root;
    char *out = NULL;
    root=cJSON_CreateStringArray(src, 3);

    root=cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString("itcast"));
    cJSON_AddNumberToObject(root, "age",        18);
    cJSON_AddNumberToObject(root, "id",        123);
    cJSON_AddItemToObject(root, "girls", cJSON_CreateStringArray(src, 3));


	out=cJSON_Print(root);
    cJSON_Delete(root);	
    printf("%s\n",out);	
    free(out);	/* Print to text, Delete the cJSON, print it, release the string. */
}
