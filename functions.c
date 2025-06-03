#include "msh.h"


char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
                    "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
                    "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "jobs", "fg", "bg", NULL};


char *external_commands[200]; 


char command[20]; 

Slist *temp=NULL;

extern char prompt[]; 
pid_t sig_pid;        
int ext_cmd_size;     
Slist *head = NULL;   
extern char input[];  
int run_flag=0;
int child_flag=0;
int prompt_flag=0;

void scan_input(char *prompt, char *input)
{
    extract_external_commands(external_commands); 
    signal(SIGINT, signal_handler);    
    signal(SIGTSTP, signal_handler);   
    while (1)
    {
       
        if(!prompt_flag)
            printf(ANSI_COLOR_CYAN"\033[1m%s \033[0m" ANSI_COLOR_RESET, prompt);
        prompt_flag=0;
        memset(input, '\0', 30); 
        scanf("%[^\n]", input); 
        getchar();               
        if (input[0] != '\0')
        {
            int ret = check_input_PS1(input, prompt);
            if (ret == 0)
            {
                char *cmd = get_command(input);
                int res = check_command_type(cmd); 
                if (res == BUILTIN)
                {
                    execute_internal_commands(input); 
                }
                else if (res == EXTERNAL)
                {
                    sig_pid = fork(); 
                    int sig_status;
                    if (sig_pid > 0)
                    {
                        child_flag=1;
                        waitpid(sig_pid, &sig_status, WUNTRACED); 
                        sig_pid = 0;
                    }
                    else if (sig_pid == 0)
                    {
                        signal(SIGINT, SIG_DFL);   
                        signal(SIGTSTP, SIG_DFL);  
                        execute_external_commands(input); 
                        exit(0);
                    }
                    child_flag=0;
                }
                else if (res == NO_COMMAND)
                {
                    printf(ANSI_COLOR_RED "Info : Command not found!" ANSI_COLOR_RESET "\n");
                }
            }
        }
    }
}


void extract_external_commands(char **ext_cmd)
{
    int fd = open("commands.txt", O_RDONLY); 
    if (fd == -1)
    {
        printf("INFO: ");
        if (errno == ENOENT)
            printf("Named file doesn't exit\n");
        else if (errno == EACCES)
            printf("File doesn't have permissiob\n");
        else
            printf("may be other reason\n");
    }
    else
    {
        char ch;
        char buff2[50];
        int count = 0, i, j;
        i = j = 0;
        while (read(fd, &ch, 1) != 0)
        {
            if (ch == '\n')
            {
                buff2[i] = '\0';
                ext_cmd[j] = malloc(sizeof(char) * count); 
                strcpy(ext_cmd[j], buff2);                 
                j++;
                count = 0;
                i = 0;
            }
            else
            {
                if (ch == EOF)
                    break;

                buff2[i] = ch;
                i++;
                count++;
            }
        }
        buff2[i] = '\0';
        ext_cmd[j] = malloc(sizeof(char) * count);
        strcpy(ext_cmd[j], buff2);
        ext_cmd[++j] = NULL;
    }
    close(fd);
}


char *get_command(char *input)
{
    int j = 0;
    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] == ' ')
            break;
        else
            command[j++] = input[i];
    }
    command[j] = '\0';
    return command;
}


void execute_external_commands(char *input)
{
    char *ext_cmd[20];
    int flag = 0;
    convert_2D_array(input, ext_cmd); 
    for (int i = 0; ext_cmd[i] != NULL; i++)
    {
        if (strcmp("|", ext_cmd[i]) == 0)
        {
            flag = 1; 
            break;
        }
    }
    if (!flag)
        execvp(ext_cmd[0], ext_cmd); 
    else if (flag)
    {
        int pipe_count = 0;
        int j = 0;
        int cmd_pos[ext_cmd_size];
        cmd_pos[j++] = 0;
        for (int i = 0; i < ext_cmd_size; i++)
        {
            if (strcmp(ext_cmd[i], "|") == 0)
            {
                ext_cmd[i] = NULL;
                pipe_count++;
                cmd_pos[j++] = i + 1; 
            }
        }
        int std_fd = dup(0); 
        int fd[2];
        int status2;
        for (int i = 0; i < pipe_count + 1; i++)
        {
            if (i < pipe_count)
                pipe(fd); 
            pid_t pid2 = fork();
            if (pid2 > 0)
            {
                wait(&status2); 
                if (i < pipe_count)
                {
                    close(fd[1]);
                    dup2(fd[0], 0); 
                    close(fd[0]);
                }
            }
            else if (pid2 == 0)
            {
                if (i < pipe_count)
                {
                    close(fd[0]);
                    dup2(fd[1], 1); 
                }
                execvp(ext_cmd[cmd_pos[i]], ext_cmd + cmd_pos[i]); 
            }
        }
        dup2(std_fd, 0);
    }
}


void convert_2D_array(char *input, char **ext_cmd)
{
    char buff[50];
    int j = 0, k = 0;
    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] != ' ')
        {
            buff[j++] = input[i];
        }
        else
        {
            buff[j] = '\0';
            ext_cmd[k] = malloc(sizeof(char) * j);
            strcpy(ext_cmd[k], buff);
            k++;
            j = 0;
        }
    }
    buff[j] = '\0';
    ext_cmd[k] = malloc(sizeof(char) * j);
    strcpy(ext_cmd[k], buff);
    ext_cmd[++k] = NULL;
    ext_cmd_size = k; 
}


int check_command_type(char *cmd)
{
    for (int i = 0; builtins[i] != NULL; i++)
    {
        if (strcmp(cmd, builtins[i]) == 0)
        {
            return BUILTIN;
        }
    }
    for (int i = 0; external_commands[i] != NULL; i++)
    {
        if (strcmp(cmd, external_commands[i]) == 0)
        {
            return EXTERNAL;
        }
    }
    return NO_COMMAND;
}


void execute_internal_commands(char *input)
{
    if (!strcmp("exit", input))
    {
        exit(0);
    }
    else if (!strcmp("pwd", input))
    {
        char buff[50];
        getcwd(buff, 50); 
        printf(ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET "\n", buff);
    }
    else if (!strncmp("cd", input, 2))
    {
        chdir(input + 3); 
    }
    else if (!strcmp("echo $$", input))
    {
        printf(ANSI_COLOR_MAGENTA "PID : %d" ANSI_COLOR_RESET "\n", getpid()); 
    }
    else if (!strcmp("echo $?", input))
    {
        int status;
        wait(&status); 
        printf(ANSI_COLOR_GREEN"exit status : %d\n"ANSI_COLOR_RESET, WEXITSTATUS(status));
    }
    else if (strcmp("echo $SHELL", input) == 0)
    {
        printf(ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET "\n", getenv("SHELL")); 
    }
    else if (!strcmp("jobs", command))
    {
        print_list(head); 
    }
    else if (!strcmp("fg", command))
    {
        if (head != NULL)
        {
            int status;
            kill(head->pid, SIGCONT); 
            head->pstate=run;
            waitpid(head->pid, &status, WUNTRACED);
            sl_delete_first(&head);  
        }
    }
    else if (!strcmp("bg", command))
    {
        static int i=0;
        if(head!=NULL)
        {
            if(i++==0)
            {
                temp=head;
            }
            signal(SIGCHLD, signal_handler);
            kill(temp->pid, SIGCONT); 
            temp->pstate=run;
            if(temp->link!=NULL)
                temp=temp->link;
        }
    }
    else
    {
        printf(ANSI_COLOR_RED "Invalid command usage!" ANSI_COLOR_RESET "\n");
    }
}


int check_input_PS1(char *input, char *prompt)
{
    if (!strncmp("PS1=", input, 4))
    {
        char new_prompt[50];
        int j = 0;
        for (int i = 4; input[i] != '\0'; i++)
        {
            if (input[i] == ' ')
                return 0;
            new_prompt[j++] = input[i];
        }
        new_prompt[j] = '\0';
        strcpy(prompt, new_prompt); 
        return 1;
    }
    return 0;
}



