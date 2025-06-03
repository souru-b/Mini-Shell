
/* name:sourabh bahubali baragale
   date:16/5/25
   project:minishell
*/

#include <stdio.h>
#include "msh.h"
char prompt[20] = "minishell$";   
char input[30]; 
int main()
{
    scan_input(prompt, input);
    return 0;
}