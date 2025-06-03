#include "msh.h"

extern pid_t sig_pid;
extern int prompt_flag;
extern char input[];
extern char prompt[];
extern Slist *head;
void signal_handler(int signum)
{
    if (signum == SIGINT)
    {
        if (sig_pid <= 0)
        {
            printf(ANSI_COLOR_CYAN"\033[1m\n%s \033[0m" ANSI_COLOR_RESET, prompt); 
            fflush(stdout);
        }
        else
            printf("\n");
    }
    else if (signum == SIGTSTP)
    {
        if (sig_pid <= 0)
        {
            prompt_flag=1;
            printf(ANSI_COLOR_CYAN"\033[1m\n%s \033[0m" ANSI_COLOR_RESET, prompt); 
            fflush(stdout);
        }
        else if (sig_pid > 0)
        {
            insert_at_first(&head, sig_pid, input); 
            printf(ANSI_COLOR_GREEN" %d\t%s"ANSI_COLOR_RESET, sig_pid, input);
            printf(ANSI_COLOR_RED"\t\tStopped\n"ANSI_COLOR_RESET);
        }
    }
    else if (signum == SIGCHLD)
    {
        int status;
        while (waitpid(-1, &status, WNOHANG) > 0)
        {
            sl_delete_first(&head); 
        }
    }
}