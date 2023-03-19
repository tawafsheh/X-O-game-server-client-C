#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#define portnum 6666
#define MAX 256
#define num 3 
#define SA struct sockaddr
void server_terminated();
void position_draw_after_edit(char arr[num][num]);
int edit_and_check_arr(char(*arr)[num], int position ,char x_or_o, int just_check);
int array_is_null(char arr[num][num]);
int main()
{  

    struct sockaddr_in server_addr;
    int tcp_sfd;
    socklen_t slen= sizeof(server_addr) ;
    char readwrite[MAX],check[MAX],msg[MAX],positionbuff[MAX],letsplay[num][num];
    tcp_sfd = socket(AF_INET, SOCK_STREAM,0);
    if(tcp_sfd<0)
    {
        perror("error while open socket \n ");
        exit(0);
    }
    memset(&server_addr,'\0', sizeof(server_addr));
    memset(&readwrite,'\0', MAX);
    memset(&check,'\0', MAX);
    memset(&letsplay,'\0', sizeof(letsplay));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnum);
    server_addr.sin_addr.s_addr =htonl(INADDR_ANY); 
    if (connect(tcp_sfd,(SA*)&server_addr, sizeof(server_addr))<0)
    {
        perror("error while connecting \n ");
        exit(0);
    }

    snprintf((char*)&check,MAX,"wait for your opponent\n");     
    while (read(tcp_sfd, readwrite,MAX )<0);
    if (strcmp(readwrite,check)==0)
    {   
 
        puts(readwrite);
        bzero(&readwrite, MAX);
        while(read(tcp_sfd, readwrite,MAX )<0); 
        puts(readwrite);
    }
    else
    { 
        puts(readwrite);
        bzero(&readwrite, MAX);
    } 
    char X_or_O = '\0';
    int check_the_postion =1;
    int bytesreads=1; 
    for(;;)
    {

        memset(&readwrite,'\0', MAX);
        while ((bytesreads =read(tcp_sfd, readwrite,MAX ))<1)
        {
            if(bytesreads == 0)
            {
                server_terminated();
            }
        } 
        ////////////////////////check if msg /////////            
        memset(&check,'\0', MAX);
        snprintf((char*)&check,MAX,"You Got msg!\n");
        if(strcmp(readwrite,check)==0)
        {
            printf("you got this message from oppenent :  ");
            memset(&msg,'\0',MAX );
            while((bytesreads= read(tcp_sfd, msg,MAX))<0)
            {
                if(bytesreads == 0)
                {
                    server_terminated();
                }
            } 
            puts(msg);  
        }
        //////////change the 2D array  send by the other player ///////////
        memset(&check,'\0', MAX);
        snprintf((char*)&check,MAX,"Change 2D-array\n");
        if(strcmp(readwrite,check)==0)
        {
            memset(&readwrite,'\0', MAX);
            while ((bytesreads= read(tcp_sfd, readwrite,MAX ))<0)
            {
                if(bytesreads == 0)
                {
                    server_terminated();
                }
            } 
            memset(&check,'\0', MAX);
            snprintf((char*)&check,MAX,"X is your character\n");
            if(strcmp(readwrite,check)==0)
            {

                X_or_O = 'X';
                memset(&readwrite,'\0', MAX);
                while ((bytesreads =read(tcp_sfd, readwrite,MAX ))< 0 )
                {
                    if(bytesreads == 0)
                    {
                        server_terminated();
                    }
                } 

                edit_and_check_arr(letsplay, atoi(readwrite),'O',0); 
                memset(&readwrite, '\0', MAX);    
            }
            else 
            {
                memset(&readwrite,'\0', MAX);
                while((bytesreads= read(tcp_sfd, readwrite,MAX ))<0)
                {
                    if(bytesreads == 0)
                    {
                        server_terminated();
                    }
                } 
                edit_and_check_arr(letsplay, atoi(readwrite),'X',0); 
                memset(&readwrite, '\0', MAX);    
            }
        }
        /////check your turn //////////////
        memset(&check,'\0', MAX);
        snprintf((char*)&check,MAX,"Your Turn!\n");
        if(strcmp(readwrite,check)==0)
        {
            printf("      ");
            puts(readwrite);
            position_draw_after_edit(letsplay);
            if( array_is_null(letsplay)== 1)
            {
               X_or_O = 'O';
            }
            check_the_postion =1; 
            memset(&check,'\0', MAX);
            snprintf((char*)&check,MAX,"The Position taken by the other player !!\n");
            memset(&msg,'\0', MAX);
            snprintf((char*)&msg,MAX,"msg\n");
            while(check_the_postion ==1)
            {
                memset(&positionbuff, '\0', MAX);
                memset(&readwrite, '\0', MAX);
                printf("ENTER 'msg' to send a message to the oponent\n");
                printf("your character is %c please choose availabe position: ", X_or_O); 
                fgets(positionbuff,MAX,stdin );
                if(strcmp(msg,positionbuff)==0)
                {   
                    write(tcp_sfd,positionbuff,MAX);
                    printf("enter what to send: ");
                    fgets(readwrite,MAX,stdin );
                    write(tcp_sfd,readwrite,MAX);
                    printf("\n");
                }
                else 
                {
                    write(tcp_sfd, positionbuff, MAX);
                    while((bytesreads =read(tcp_sfd, readwrite,MAX ))<0)
                    {
                        if(bytesreads == 0)
                        {
                            server_terminated();
                        }
                    } 
                    if(strcmp(readwrite,check)==0)
                    {
                        puts(readwrite);   
                    }
                    else 
                    {
                        check_the_postion= 0;
                        edit_and_check_arr(letsplay, atoi(positionbuff), X_or_O, 0);
                        printf("\n      Nice Move!\n");
                        position_draw_after_edit(letsplay);
                        printf("Wait Your Oponent Move...");
                    }
                }
            }
            memset(&readwrite,'\0',MAX);
            printf("\n");
        }
        memset(&check,'\0', MAX);
        snprintf((char*)&check,MAX,"Congratulations. You won!\n");
        if(strcmp(readwrite,check) ==0)
        {
            position_draw_after_edit(letsplay);
            puts(readwrite);
            exit(0);
        }
        memset(&check,'\0', MAX);
        snprintf((char*)&check,MAX,"Game Over. You Lose!\n");
        if(strcmp(readwrite,check) ==0)
        {
            position_draw_after_edit(letsplay);
            puts(readwrite);
            exit(0);
        }
        memset(&check,'\0', MAX);
        snprintf((char*)&check,MAX,"Draw!\n");
        if(strcmp(readwrite,check) ==0)
        {
            position_draw_after_edit(letsplay);
            puts(readwrite);
            exit(0);
        }
    }
    return 0 ; 
}
void position_draw_after_edit(char arr[num][num])
{   int pos= 1; 
    for(int i = 0 ; i < num;i ++)
    {
        printf("        ");
        printf("     |     |     \n");
        printf("        ");
        for(int j = 0 ; j< num ; j++)
        {
           
            if(arr[j][i]== '\0')
            {
                if(j < (num-1))
                {
                    printf("  %d  |", pos);
                }
                else
                {
                    printf("  %d  ", pos);
                }
            }
            else
            {
                if(j < (num-1) )
                    printf("  %c  |", arr[j][i]);
                else
                {
                    printf("  %c  ", arr[j][i]);
                }
            }
            pos ++; 
        }
        if(i<(num-1))
        {
            printf("\n        ");
            printf("_____|_____|_____\n");
        }
    }   
    printf("\n");
    printf("        ");
    printf("     |     |     \n");
    printf("\n");
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
int array_is_null(char arr[num][num])
{
    for(int i =0 ; i<num ; i++)
    {
        for(int j= 0 ; j<num ;j++ )
        {
            if(arr[j][i] != '\0')
            {
                return 0; 
            }        
        }
    }
    return 1;
}
void server_terminated()//////////////// check if server terminted or not ////////////////////
{
    perror("server reset the connetion  !!\n");
    exit(0);
}


    //   bzero(buff,MAXLINE);
    //    int n = read(clfd,buff, sizeof(buff));
    
    //     // Receives the tic tac toe box from the server and stores it in 'buff'
    //    while(read(clfd, buff, sizeof(buff))<0);
    //     //bzero(&buff, MAXLINE);
    //    puts(buff);
       
    //     // atoi(buff);
    //     // Outputs the tic tac toe box 
    //    // printf("TIC TAC TOE BOX:%s\n", atoi(buff));
    //     // Player's move
    //     printf("Choose a # from 1 - 9 to replace");
   
    //     // Store Player's move in 'buff'
    //     fgets(buff,MAXLINE,stdin);
    //     // Sends player's move in 'buff 'to server
    //     write(clfd,buff,sizeof(buff));
    //     // Get result from the server
    //     bzero(&buff, MAXLINE);
    //     read(clfd,buff,MAXLINE);
    //     // Output the results received
    //     puts(buff);