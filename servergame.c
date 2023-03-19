#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#define num 3 
#define MAX 256
#define SA struct sockaddr 
#define tcp_portnum 6666
void sig_chld(int signo);
char check_win(char arr[num][num]);
int edit_and_check_arr(char(*arr)[num], int position ,char x_or_o, int just_check ); // by default it send it as refernce 
void position_draw_after_edit(char arr[num][num]);
int main(){
    struct sockaddr_in server_addr, client_1,client_2;
    socklen_t client1_len, client2_len;
    pid_t pid;
    char buff[MAX];                                
    int pos, tcp_sfd,udp_sfd,connfd1,connfd2; 
    memset(&server_addr,'\0', sizeof(server_addr));
    memset(&client_1,'\0', sizeof(client_1));
    memset(&client_2,'\0', sizeof(client_2));
    tcp_sfd= socket(AF_INET, SOCK_STREAM,0);
    if (tcp_sfd < 0 )
    {
        perror("error while open socket \n ");
        exit(0);
    }
    udp_sfd = socket(AF_INET, SOCK_DGRAM,0);
    if (udp_sfd < 0 )
    {
        perror("error while open socket \n ");
        exit(0);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(tcp_portnum);
    server_addr.sin_addr.s_addr =htonl(INADDR_ANY); 
    if(bind(tcp_sfd, (SA*)&server_addr, sizeof(server_addr)) < 0 )
    {
        perror("error while binding  \n ");
        exit(0);
    }

    if(bind(udp_sfd, (SA*)&server_addr, sizeof(server_addr)) < 0 )
    {
        perror("error while binding  \n ");
        exit(0);
    }
    listen(tcp_sfd, 7);
    fd_set UDP_TCP_set; 
    FD_ZERO(&UDP_TCP_set);
    int maximum_for_array = tcp_sfd +1;
    FD_SET(tcp_sfd, &UDP_TCP_set);
    struct timeval _sec25; 
    _sec25.tv_sec =5 ; //// 25 seconds 
    signal(SIGCHLD, sig_chld);////// to deal with zombie proccesses   
    for(;;)
    {
      
        select(maximum_for_array, &UDP_TCP_set, NULL,NULL,(struct timeval*)&_sec25); 
        client1_len = sizeof(client_1); 
        while((connfd1 = accept(tcp_sfd, (SA*)&client_1, &client1_len))<0);
        printf("one Player connected wait for the second ! \n");
        write(connfd1, "wait for your opponent\n",sizeof("wait for your opponent\n"));
        client2_len = sizeof(client_2); 
        while((connfd2 =  accept(tcp_sfd, (SA*)&client_2, &client2_len))<0);
        printf("the Game started ! \n");
        write(connfd1,"The Game Started Lets Goooo!\n", sizeof("The Game Started Lets Goooo!\n"));    
        write(connfd2,"The Game Started Lets Goooo!\n", sizeof("The Game Started Lets Goooo!\n"));        
        if((pid =fork())==0)
        {   
            close(tcp_sfd);
            char arr[num][num] ,readwrite[MAX],x_or_o  ; 
            int temp =0 ; 
            memset(&arr, '\0',sizeof(arr)); 
            srand(time(0));
            int r = rand()%100;//1-99
            if(r %2 ==0 )
            {
                temp = connfd2; 
                connfd2 = connfd1; 
                connfd1= temp; 
            }

            int pos = 0;                                
            memset(&arr, '\0',sizeof(arr)); 
            memset(&readwrite, '\0',MAX); 
            char win = '\0';
            int check_position;               
            for(;;)
            {   
                if(FD_ISSET(tcp_sfd,&UDP_TCP_set))
                {  
                    sleep(1);
                    write(connfd1, "Your Turn!\n", sizeof("Your Turn!\n"));
                    memset(&readwrite,'\0',MAX);
                    memset(&buff,'\0',MAX);
                    snprintf((char*)&buff,MAX,"msg\n");  
                    check_position =1; 
                    while(check_position ==1)
                    {
                        memset(&readwrite,'\0',MAX);
                        while(read(connfd1,readwrite, MAX )<1);
                        if(strcmp(buff, readwrite)==0)
                        {   
                            write(connfd2, "You Got msg!\n", sizeof("You Got msg!\n"));
                            memset(&readwrite,'\0',MAX);
                            sleep(1);
                            while(read(connfd1,readwrite, MAX)<0);
                            sleep(1);
                            write(connfd2, readwrite, MAX); 
                            }
                        else
                        {
                            check_position = edit_and_check_arr(arr,atoi(readwrite),'O', 1);
                            if(check_position ==1 )
                                write(connfd1 , "The Position taken by the other player !!\n",sizeof("The Position taken by the other player !!\n"));
                            else 
                                write(connfd1 , "Correct Position\n",sizeof("Correct Position\n"));
                        }
                    }
                    edit_and_check_arr(arr,atoi(readwrite),'O', 0);
                    write(connfd2,"Change 2D-array\n", sizeof("Change 2D-array\n"));
                    sleep(1);
                    write(connfd2,"X is your character\n", sizeof("X is your character\n"));
                    sleep(1);
                    write(connfd2, readwrite, sizeof(readwrite));
                    win = check_win(arr);
                    if(win == 'O')
                    {
                        write(connfd1,"Congratulations. You won!\n",sizeof("Congratulations. You won!\n") );
                        write(connfd2,"Game Over. You Lose!\n" ,sizeof("Game Over. You Lose!\n"));
                    } 
                    if(win == 'D')
                    {
                        write(connfd1,"Draw!\n",sizeof("Draw!\n") );
                        write(connfd2,"Draw!\n",sizeof("Draw!\n") );
                    } 
                    /////////////////////////////////////client2 ///////////////// 
                    write(connfd2, "Your Turn!\n", sizeof("Your Turn!\n"));
                    memset(&buff,'\0',MAX);
                    snprintf((char*)&buff,MAX,"msg\n"); 
                    check_position =1; 
                    while(check_position ==1)
                    {
                        memset(&readwrite,'\0',MAX);
                        while(read(connfd2,readwrite, MAX )<1);
                        if(strcmp(buff, readwrite)==0)
                        {   
                            write(connfd1, "You Got msg!\n", sizeof("You Got msg!\n"));
                            memset(&readwrite,'\0',MAX);
                            sleep(1);
                            while(read(connfd2,readwrite, MAX)<0);
                            sleep(1);
                            write(connfd1, readwrite, MAX); 

                        }
                        else 
                        {
                            check_position = edit_and_check_arr(arr,atoi(readwrite),'X', 1);

                            if(check_position ==1 )
                                write(connfd2 , "The Position taken by the other player !!\n",sizeof("The Position taken by the other player !!\n"));
                            else 
                                write(connfd2 , "Correct Position\n",sizeof("Correct Position\n"));
                        }
                    }
                    edit_and_check_arr(arr,atoi(readwrite),'X',0);
                    write(connfd1,"Change 2D-array\n", sizeof("Change 2D-array\n"));
                    sleep(1);
                    write(connfd1,"O is your character\n", sizeof("O is your character\n"));
                    sleep(1);
                    write(connfd1, readwrite, sizeof(readwrite));
                    win = check_win(arr); 
                    if(win == 'X')
                    {
                        write(connfd2,"Congratulations. You won!\n",sizeof("Congratulations. You won!\n") );
                        write(connfd1,"Game Over. You Lose!\n" ,sizeof("Game Over. You Lose!\n"));
                    } 
                    if(win == 'D')
                    {
                        write(connfd1,"Draw!\n",sizeof("Draw!\n") );
                        write(connfd2,"Draw!\n",sizeof("Draw!\n") );
                    } 
                }
            }
        }
        close(connfd1);
        close(connfd2);  
    }
        
    return 0; 
}
char check_win(char arr[num][num])
{
    for(int i = 0 ; i < num ; i++)
    {   
        if((arr[i][0]==arr[i][1])&&(arr[i][1]==arr[i][2])&&(arr[i][0]!='\0'))
        {
            return arr[i][0];
        }
    }
    for(int i = 0 ; i < num ; i++)
    {   
        if((arr[0][i]==arr[1][i])&&(arr[1][i]==arr[2][i])&&(arr[0][i]!='\0'))
        {
            return arr[1][i];
        }
    }
    if((arr[0][0]==arr[1][1])&&(arr[1][1]==arr[2][2])&&(arr[0][0]!='\0'))
    {
            return arr[0][0];
    }
     if((arr[0][2]==arr[1][1])&&(arr[1][1]==arr[2][0])&&(arr[0][2]!='\0'))
    {
        return arr[0][2];
    }
    int check_draw = 1; 
    for(int i = 0 ; i < num; i++)
    {
        for(int j = 0 ; j< num ; j++)
        {
            if(arr[i][j] == '\0')
            {
                check_draw = 0 ;
            }
        }
    }
    if(check_draw == 1 )
    {
        return 'D';
    }
    return 'N'; // nothing 
}
int edit_and_check_arr(char(*arr)[num], int position ,char x_or_o, int just_check )
{   

    int pos = 1; 
    for (int i =0 ; i <num; i++)
    {
        for(int j=0;j<num; j++)
        {
            if(pos == position)
            {
                if(arr[j][i]== '\0') 
                {
                    if(just_check == 0)
                    {
                        arr[j][i] = x_or_o;  
                    }
                    return 0; //successed 
                }
            }
            pos ++ ; 
        }
    }
    return 1; 
}
void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    wait(&stat);
    printf("the following child with %d pid has terminate\n", pid);
}
