
#include "msh.h"

extern char input[];
extern char prompt[];
extern Slist *head;


int insert_at_first(Slist **head, pid_t pid, char *input)
{
    Slist *new = malloc(sizeof(Slist)); 
    if (new == NULL)
    {
        return 0;
    }
    new->pid = pid;
    strcpy(new->str, input);
    new->link = NULL;
    if (*head == NULL)
    {
        *head = new; 
        return 1;
    }
    else
    {
        Slist *temp = *head;
        *head = new;
        new->link = temp;
    }
    return 1;
}


void print_list(Slist *head)
{
    if (head == NULL)
    {
        printf(ANSI_COLOR_YELLOW "INFO : No stopped process." ANSI_COLOR_RESET "\n");
    }
    else
    {
        printf(ANSI_COLOR_BLUE"PID\tSTATUS\t\tNAME\n"ANSI_COLOR_RESET);
        while (head)
        {
            if(head->pstate==stop)
            {
                printf(ANSI_COLOR_MAGENTA"%d\t" ANSI_COLOR_RESET, head->pid);
                printf(ANSI_COLOR_RED "Stopped\t" ANSI_COLOR_RESET);
                printf(ANSI_COLOR_MAGENTA "\t%s" ANSI_COLOR_RESET "\n\n", head->str);
            }
            else if(head->pstate==run)
            {
                printf(ANSI_COLOR_MAGENTA"%d\t" ANSI_COLOR_RESET, head->pid);
                printf(ANSI_COLOR_GREEN"Running\t" ANSI_COLOR_RESET);
                printf(ANSI_COLOR_MAGENTA"\t%s" ANSI_COLOR_RESET "\n\n", head->str);
            }
            head = head->link;
        }
    }
}



int sl_delete_first(Slist **head)
{
    if (*head == NULL)
    {
        printf("List is Empty.\n");
        return 0;
    }
    Slist *temp = *head;
    if (temp->link == NULL)
    {
        free(temp);
        *head = NULL;
        return 1;
    }
    else
    {
        *head = temp->link;
        free(temp);
        return 1;
    }
}