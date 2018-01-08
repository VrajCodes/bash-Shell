/**
 * Machine Problem: Shell
 * CS 241 - Fall 2017
 */
#include "format.h"
#include "shell.h"
#include "vector.h"


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct process {
    char *command;
    char *status;
    pid_t pid;
} process;

vector *currproc;
vector *history;

extern char** enviorn;


int checkinput(char* s)
{   
    for(unsigned long i=0; i< (strlen(s)); i++)
    {
        if(ispunct(s[i]) && (s[i] !='_')   )
            return 0;
        if( (isalpha(s[i])) || (isdigit(s[i])) || (s[i] =='_')  )
        {}
        else
            return 0;
     
    }
    return 1;

}


void sig_handler(int signal)
{

	process *temp2;
	size_t sizev= vector_size(currproc);
	int status;
	pid_t newid = waitpid(-1, &status, WNOHANG);

	if( !WIFSIGNALED(status)  )
	{
		for(size_t w=0; w<sizev; w++)
		{
			temp2=vector_get(currproc,w);
			if( (temp2->pid)== newid)
			{
				vector_erase(currproc,w);
				return;
			}


		}
	}

}


int shell(int argc, char *argv[]) {


	signal(SIGCHLD, sig_handler);

	// TODO: This is the entry point for your shell.

	char getoptc='x';
	int getflagc=-999;
	char *currdir = calloc(100, sizeof(char));
	char *token = calloc(100, sizeof(char));
	char *prefix = calloc(100, sizeof(char));
	size_t get =0;
	int argwalk=0;
	int found=0;
	int findex=0;
	int exits=0;
	int counter=0;
	int file=0;
	int hist=0;
	ssize_t ctrld=0;
	int background=0;

	currproc = vector_create(NULL,NULL,NULL);

	history = vector_create(string_copy_constructor, string_destructor,string_default_constructor);
	

	process *p =(process*)malloc(sizeof(process));
	p->command= strdup(argv[0]);
	p->status= STATUS_RUNNING;
	p->pid= getpid();

	vector_push_back(currproc,p);

	FILE *fp;
	FILE *ff;
	opterr =0;

	char flagh ='\0';
	char flagf ='\0';
	while(1)
	{
		int ifsucc=1;

		found=0;
		//print_prompt( getcwd(currdir,100) , getpid());
		do
		{
			getflagc =getopt(argc, argv,"f:h:");
			getoptc=getflagc;

			if(getoptc == 'f')
			{
				flagf='f';
				argwalk=+2;	
				file=argwalk;
				ff=fopen(argv[argwalk], "r");

				if(ff==NULL)
				{
					print_script_file_error();
					exits=1;
					exit(1);
				}


			}
			else if(getoptc == 'h') 
			{
				flagh='h';
				argwalk=+2;
				fp=fopen(argv[argwalk], "r");

				if(fp==NULL)
				{
					print_history_file_error();
					exits=1;
					exit(1);
				}
				hist=argwalk;
				

			}
			else if( (getoptc != 'f')&&(getoptc !='h') && (getoptc == '?')) 
			{
				if( !(getoptc != '?'))
				{
					print_usage();
					exit(1);
				}
				break;
			}
		}while(getflagc != -1);


		char **cmd = calloc(100, sizeof(char*));

		for(int k=0; k<100; k++)
		{
			cmd[k]=calloc(100,sizeof(char));
		}
		char *readin = calloc(256, sizeof(char));
		char *readincpy = calloc(256, sizeof(char));
		char *logiccpy = calloc(256, sizeof(char));

		char *varname = calloc(100, sizeof(char));
		char *value = calloc(100, sizeof(char));
		char *token2=calloc(100, sizeof(char));
		char *reference = calloc(100, sizeof(char));
		char *colon = calloc(100, sizeof(char));



	
		
			if(flagf == 'f')
			{
					
				ctrld=getline(&readin,&get,ff);
					
				if(strcmp("",readin)==0)
					exit(0);
				print_prompt( getcwd(currdir,100) , getpid());
				fprintf(stdout,"%s",readin);
				fflush(stdout);
	
			}
			else
			{
				print_prompt( getcwd(currdir,100) , getpid());
				ctrld=getline(&readin,&get,stdin);
			}
			

			strcpy(logiccpy,readin);
		
			if( readin[ strlen(readin)-1 ] == '\n')
				readin[ strlen(readin)-1 ] = '\0';
	



			strcpy(readincpy,readin);
			background=0;

			if( (strstr(readin,"&") != NULL) &&(strstr(readin,"\\")==NULL) )
			{
					background =1;
					//size_t strlength=strlen(readin);

					// for(int e=strlength-1; e>0; e--)
					// {
					// 	if(( readin[e]=' ')&&( isalpha(readin[e-1])==0))
					// 		readin[e]='\0';

					// }

					readin[ strlen(readin)-1 ] = '\0';
					readincpy[ strlen(readincpy)-1 ] = '\0';

					//fprintf(stderr,"herere\n");
			}

			if( strncmp("!", readin,1) == 0 )
			{

				if( strcmp("!history",readin) == 0)
				{
					for(size_t r=0; r< vector_size(history); r++)
					{
						print_history_line(r,vector_get(history,r));	

					}
				}
				else
				{
					if(readin[1] != '\0')
					{
						token=strtok(readincpy, "!");



						for(int r= (int)(vector_size(history)-1); r>=0; r--)
						{
							prefix = vector_get(history,r);
							//for(size_t k = 0; k<strlen(readin); k++)

							if( strncmp(prefix,token, strlen(token)) ==0   )
							{
								findex=r;
								found =1;
								break;
							}
						}

						if( found == 1)
						{

							print_command(vector_get(history,findex));	

							readin = vector_get(history,findex);
							strcpy(readincpy,readin);
							token=strtok(readincpy, " \0");
							while(token != NULL)
							{
								strcpy( cmd[counter] , token);
								counter++;
								token=strtok(NULL, " \0");
							}

							pid_t child = fork();
							if(child == -1)
							{
								print_fork_failed();
								return 1;

							}
							else if(child > 0) // This is the parent ( need to wait and then exit)
							{
								int status;

								waitpid(child, &status, 0);
								if(status != 0)
									exit(1);

							}
							else if( child ==0) // child process 
							{
								//PUT A EXIT RIGHT AFTER EXEC IN CASE IT FAILS

								print_command_executed(getpid());
								execvp(cmd[0], cmd);	
								print_exec_failed(cmd[0]);
								ifsucc =0;
								exit(1);		

							}

							
						}
						else
						{
							print_no_history_match();
							exits=1;
						}

					}
					else
					{
						print_command(vector_get(history,vector_size(history)-1));	

						readin = vector_get(history, vector_size(history)-1 );
						findex=((vector_size(history))-1);
						
						strcpy(readincpy,readin);
						token=strtok(readincpy, " \0");
						while(token != NULL)
						{
							strcpy( cmd[counter] , token);
							counter++;
							token=strtok(NULL, " \0");
						}

						pid_t child = fork();
						if(child == -1)
						{
							print_fork_failed();
							return 1;

						}
						else if(child > 0) // This is the parent ( need to wait and then exit)
						{
							int status;

							waitpid(child, &status, 0);
							if(status != 0)
								exit(1);

						}
						else if( child ==0) // child process 
						{
							//PUT A EXIT RIGHT AFTER EXEC IN CASE IT FAILS

							print_command_executed(getpid());
							execvp(cmd[0], cmd);	
							print_exec_failed(cmd[0]);
							ifsucc =0;
							exit(1);		

						}

						
							
					}

					vector_push_back(history,vector_get(history,findex));
					

				}

			}
			else if((strcmp("exit",readin)==0) || (ctrld==-1))
			{

				if(flagh=='h')
				{
						fp=fopen(argv[argwalk], "a+");
						
						size_t s= (vector_size(history));
						if(s != 0)
						{
							for(size_t t= 0; t<s; t++)
							{
								const char* lines = (const char*)vector_get(history,t);
								fprintf(fp,"%s\n", lines);

							
							}
							fclose(fp);
						}

				}

				if(ctrld==-1)
					printf("\n");


					exit(0);
			}
			else if( strncmp("export",readin,6) == 0)
			{
				vector_push_back(history,readin);

				token= strtok(readin," \0");
				token= strtok(NULL,"=\0");
				strcpy(varname,token);	
				//fprintf(stderr,"varname:%s\n",varname);
				
				token= strtok(NULL,"\0");
				strcpy(value,token);
				
				if(  ((strlen(token)>=1) && (checkinput(varname)) ))
				{	
					if(token[0]=='$')
					{
						token[strlen(token)]='\0';
						token++;
						token2=strtok(token,":\0");
						strcpy(reference,token2);
						
						if(token2 !=NULL)
						{
							
							token2=strtok(NULL,"\0");
							strcat(colon,":");
							
							strcat(colon,token2);
						}
							//fprintf(stderr,"colon:%s\n",colon);
							value=getenv(reference);
							strcpy(token,value);
							strcat(value,colon);


					}

					// fprintf(stderr,"value:%s\n",value);
					// fprintf(stderr,"token:%s\n",token);
					setenv(varname,value,1);
					setenv(reference,token,1);

				}

			}
			else if( strncmp("kill",readin,4) == 0)
			{
				
				char *procid= calloc(256, sizeof(char));
				int is=0;
				size_t sizev= vector_size(currproc);	
			
				token=strtok(readin," ");
				procid=strtok(NULL,"\0");
				
				vector_push_back(history,readincpy);
	
				if(procid==NULL)
				{
					print_invalid_command(readincpy);
					//continue or exit

				}

				sizev= vector_size(currproc);

				process *temp;

				for(size_t w=0; w<sizev; w++)
				{
					temp=vector_get(currproc,w);
					if((temp->pid)== (atoi(procid)) )
						is=1;

				}
			
				if(is==1)
				{
					kill((atoi(procid)), SIGTERM);	
					print_killed_process((atoi(procid)),readincpy);


					for(size_t w=0; w<sizev; w++)
					{
						temp=vector_get(currproc,w);
						if( (temp->pid)== (atoi(procid)))
							vector_erase(currproc,w);

					}

				}
				if(is==0)
				{
					print_no_process_found((atoi(procid)));
				}



			}
			else if( strncmp("stop",readin,4) == 0)
			{
				
				char *procid= calloc(256, sizeof(char));
				int is=0;	
			
				token=strtok(readin," ");
				procid=strtok(NULL,"\0");
				vector_push_back(history,readincpy);
	
				if(procid==NULL)
				{
					print_invalid_command(readincpy);
					//continue or exit

				}



				size_t sizev= vector_size(currproc);

				process *temp;

				for(size_t w=0; w<sizev; w++)
				{
					temp=vector_get(currproc,w);
					if((temp->pid)== (atoi(procid)) )
						is=1;

				}
			
				if(is==1)
				{
					kill((atoi(procid)), SIGTSTP);	
					print_stopped_process((atoi(procid)),readincpy);
					for(size_t w=0; w<sizev; w++)
					{
						temp=vector_get(currproc,w);
						if( (temp->pid)== (atoi(procid)))
							temp->status= STATUS_STOPPED;

					}
				}
				if(is==0)
				{
					print_no_process_found((atoi(procid)));
				}

			}
			else if( strncmp("cont",readin,4) == 0)
			{
				
				char *procid= calloc(256, sizeof(char));
				int is=0;	
			
				token=strtok(readin," ");
				procid=strtok(NULL,"\0");
				vector_push_back(history,readincpy);
	
				if(procid==NULL)
				{
					print_invalid_command(readincpy);
					//continue or exit

				}



				size_t sizev= vector_size(currproc);

				process *temp;

				for(size_t w=0; w<sizev; w++)
				{
					temp=vector_get(currproc,w);
					if((temp->pid)== (atoi(procid)) )
						is=1;

				}
			
				if(is==1)
				{
					kill((atoi(procid)), SIGCONT);	
					for(size_t w=0; w<sizev; w++)
					{
						temp=vector_get(currproc,w);
						if( (temp->pid)== (atoi(procid)))
							temp->status= STATUS_RUNNING;

					}
					
				}
				if(is==0)
				{
					print_no_process_found((atoi(procid)));
				}

			}
			else if( strncmp("cd",readin, 2) ==0)
			{
				int chdirsuc=0;	
				vector_push_back(history,readin);

				token=strtok(readin, " \0");
				while(token != NULL)
				{
					strcpy( cmd[counter] , token);
					counter++;
					token=strtok(NULL, " \0");
				}
				
				chdirsuc= chdir(cmd[1]);

				//fprintf(stderr,"cmd1:%s\n",cmd[1]);
				if(chdirsuc == 0)
				{
					

				}
				else 
					print_no_directory(cmd[1]);

			}
			else if( strncmp("#",readin, 1) ==0)
			{
				int afteratoi=0;
				token=strtok(readin, "#\0");
				afteratoi= atoi(token);

				//fprintf(stderr,"afteratoi:%d\n", afteratoi);

				if( (afteratoi <0)||(afteratoi > ((int)vector_size(history)-1)))
				{
					print_invalid_index();
				}
				else
				{
					print_command(vector_get(history,(size_t)afteratoi));	

					readin = vector_get(history,(size_t)afteratoi);
					strcpy(readincpy,readin);
					token=strtok(readincpy, " \0");
					while(token != NULL)
					{
						strcpy( cmd[counter] , token);
						counter++;
						token=strtok(NULL, " \0");
					}


					if( strcmp("cd",cmd[0]) == 0)
					{

						int chdirsuc=0;	

						token=strtok(readin, " \0");
						while(token != NULL)
						{
							strcpy( cmd[counter] , token);
							counter++;
							token=strtok(NULL, " \0");
						}
						
						chdirsuc= chdir(cmd[1]);

						//fprintf(stderr,"cmd1:%s\n",cmd[1]);
						if(chdirsuc == 0)
						{
							vector_push_back(history,readin);

						}
						else 
							print_no_directory(cmd[1]);


					}
					else
					{
						pid_t child = fork();
						if(child == -1)
						{
							print_fork_failed();
							return 1;

						}
						else if(child > 0) // This is the parent ( need to wait and then exit)
						{
							

							int status;
							if(background==0)
								waitpid(child, &status, 0);
							else
							{
								if (setpgid(child, child) == -1) {
	   							print_setpgid_failed();
	    						exit(1);
	  							}

								waitpid(child, &status, WNOHANG);
								process *p =(process*)malloc(sizeof(process));
								p->command= strdup(readincpy);
								p->status= STATUS_RUNNING;
								p->pid= child;
								vector_push_back(currproc,p);

							}


							

						}
						else if( child ==0) // child process 
						{
							//PUT A EXIT RIGHT AFTER EXEC IN CASE IT FAILS

							print_command_executed(getpid());
							execvp(cmd[0], cmd);	
							print_exec_failed(cmd[0]);
							ifsucc =0;
							exit(1);		

						}

					
							vector_push_back(history,readin);

					}
				}

			}
			else if(  ((strstr(readin,"&&") != NULL) || (strstr(readin,"||") != NULL) || (strstr(readin,";") != NULL)) && (strstr(readin,"\\")==NULL) )
			{
				

				int and=0;
				int or=0;
				int semi=0;
				int turn=0;
				char *occur=NULL;
				char* logic = calloc(100,sizeof(char));
				exits=0;

				if( logiccpy[ strlen(logiccpy)-1 ] == '\n')
					logiccpy[ strlen(logiccpy)-1 ] = '\0';


				occur= strstr(logiccpy,"&&");
				// fprintf(stderr,"token:%s\n", token);
				// 	fprintf(stderr,"logic:%s\n", logic);

				if(occur != NULL)
				{
					//fprintf(stderr,"readin:%s\n", readin);
					and=1;
					token=strtok(logiccpy, "&&");
	
					logic=strtok(NULL,"\0");
					logic++;
					logic++;
					
					//fprintf(stderr,"logic:%s\n", logic);
				}
				occur= strstr(logiccpy,"||");	
				if(occur != NULL)
				{
					or=1;
					token=strtok(logiccpy, "||");
					logic=strtok(NULL,"\0");
					//token[ strlen(token)-1 ] = '\0';
					//logic[ strlen(logic)-1 ] = '\0';
					logic++;
					logic++;
					//fprintf(stderr,"token:%s\n", token);
					//fprintf(stderr,"logic:%s\n", logic);

				}
				occur= strstr(logiccpy,";");	
				if(occur != NULL)
				{
					semi=1;
					token=strtok(logiccpy, ";");
					logic=strtok(NULL,"\0");
					//token[ strlen(token)-1 ] = '\0';
					//logic[ strlen(logic)-1 ] = '\0';
					logic++;
				}
				for(int w=0; w<=1;w++)
				{
					turn++;
					counter=0;
					char **cmd = calloc(100, sizeof(char*));
					char* piece = calloc(100,sizeof(char));
		
					for(int k=0; k<100; k++)
					{
						cmd[k]=calloc(100,sizeof(char));
					}

					if(and == 1)
					{

						if(turn ==1)
							strcpy(piece, token);
						if(turn ==2)
						{
							strcpy(piece, logic);
						
						}
						piece=strtok(piece, " \0");
						while(piece != NULL)
						{
							strcpy( cmd[counter] , piece);
							counter++;
							piece=strtok(NULL, " \0");
						}

						if(exits != 1)
						{
							pid_t child = fork();
							if(child == -1)
							{
								print_fork_failed();
								return 1;

							}
							else if(child > 0) // This is the parent ( need to wait and then exit)
							{
								int status;

								waitpid(child, &status, 0);
								if(status != 0)
									exits=1;

							}
							else if( child ==0) // child process 
							{
								//PUT A EXIT RIGHT AFTER EXEC IN CASE IT FAILS

								print_command_executed(getpid());
								execvp(cmd[0], cmd);	
								print_exec_failed(cmd[0]);
								exit(1);
								ifsucc =0;
								exits=1;		

							}

						}


					}
	
					if(or== 1)
					{

						if(turn ==1)
							strcpy(piece, token);
						if(turn ==2)
							strcpy(piece, logic);

						piece=strtok(piece, " \0");
						while(piece != NULL)
						{
							strcpy( cmd[counter] , piece);
							counter++;
							piece=strtok(NULL, " \0");
						}

					
						if((exits == 1)||(turn != 2))
						{
							pid_t child = fork();
							if(child == -1)
							{
								print_fork_failed();
								return 1;

							}
							else if(child > 0) // This is the parent ( need to wait and then exit)
							{
								int status;

								waitpid(child, &status, 0);
								if(status != 0)
									exits=1;

							}
							else if( child ==0) // child process 
							{
								//PUT A EXIT RIGHT AFTER EXEC IN CASE IT FAILS

								print_command_executed(getpid());
								execvp(cmd[0], cmd);	
								print_exec_failed(cmd[0]);
								exit(1);
								ifsucc =0;
								exits=1;		

							}


						}


					}

					if(semi == 1)
					{

						if(turn ==1)
						strcpy(piece, token);
						if(turn ==2)
						strcpy(piece, logic);

						piece=strtok(piece, " \0");
						while(piece != NULL)
						{
							strcpy( cmd[counter] , piece);
							counter++;
							piece=strtok(NULL, " \0");
						}

							pid_t child = fork();
							if(child == -1)
							{
								print_fork_failed();
								return 1;

							}
							else if(child > 0) // This is the parent ( need to wait and then exit)
							{
								int status;

								waitpid(child, &status, 0);
								

							}
							else if( child ==0) // child process 
							{
								//PUT A EXIT RIGHT AFTER EXEC IN CASE IT FAILS

								print_command_executed(getpid());
								execvp(cmd[0], cmd);	
								print_exec_failed(cmd[0]);
								exit(1);
								ifsucc =0;
										

							}


					}
					for(int k=0; k<100; k++)
					{
						free(cmd[k]);
					}
						free(cmd);

				}
				
					vector_push_back(history,readincpy);
			

			
			}
			else if( (strncmp("ps",readin,2)==0))
			{
				//void print_process_info(const char *status, int pid, const char *command);
				//void *vector_get(vector *this, size_t n);
				size_t sizev= vector_size(currproc);

				process *temp;

				for(size_t w=0; w<sizev; w++)
				{
					temp=vector_get(currproc,w);
					print_process_info(temp->status, temp->pid, temp->command);

				}
				vector_push_back(history,readincpy);


			}

			else if((strstr(readin,"\\") != NULL))
			{
				char *catted=calloc(100,sizeof(char));
				char sym='\0';

				token=strtok(readin, " \0");
				strcpy(cmd[counter] , token);
				counter++;

				token = strtok(NULL,"\0");

				int i= (int)strlen(token);
				for(int k=0;k<i;k++)
				{
					if(token[k]=='\\')
					{
					}
					else
					{
						sym=token[k];
						strcat(catted,&sym);
					}

				}
				strcpy(cmd[counter] , catted);
				
				
				pid_t child = fork();
				if(child == -1)
				{
					print_fork_failed();
					return 1;

				}
				else if(child > 0) // This is the parent ( need to wait and then exit)
				{


					

					int status;
					if(background==0)
						waitpid(child, &status, 0);
					else
					{
						if (setpgid(child, child) == -1) {
						print_setpgid_failed();
						exit(1);
						}


						waitpid(child, &status, WNOHANG);
						process *p =(process*)malloc(sizeof(process));
						p->command= strdup(readincpy);

						p->status= STATUS_RUNNING;
						p->pid= child;
						vector_push_back(currproc,p);

					}

			

				}
				else if( child ==0) // child process 
				{
					//PUT A EXIT RIGHT AFTER EXEC IN CASE IT FAILS
					
					print_command_executed(getpid());
					execvp(cmd[0], cmd);	
					print_exec_failed(cmd[0]);
					ifsucc =0;
					exit(1);		

				}

				
					vector_push_back(history,readincpy);
			}
			else
			{
				
				char *tok2=calloc(100,sizeof(char));
				char *val=calloc(100,sizeof(char));
				char *ref=calloc(100,sizeof(char));
				char *rd=calloc(100,sizeof(char));
				char *cols=calloc(100,sizeof(char));
				char *origval=calloc(100,sizeof(char));

				token=strtok(readin, " \0");
				while(token != NULL)
				{


					if(token[0]=='$')
					{
						strcat(cols,":");
						strcat(rd,token);
						rd++;
						//fprintf(stderr,"rd:%s\n",rd);
						tok2=strtok(rd,":\0");
						strcpy(ref,tok2);
						val=getenv(tok2);
						origval=getenv(tok2);
						//fprintf(stderr,"val1:%s\n",origval);
						tok2=strtok(NULL,"\0");
						if(tok2!=NULL)
						{
							strcat(cols,tok2);
							//fprintf(stderr,"cols:%s\n",cols);
							strcat(val,cols);
							//fprintf(stderr,"val:%s\n",val);
						}
						
						strcpy(cmd[counter] , val);
						
						setenv(ref,origval,1);

					}
					else
						strcpy(cmd[counter] , token);

					//fprintf(stderr,"token:%s\n",token);
					counter++;
					token=strtok(NULL, " \0");
				}
				// for(int i=0; i<counter; i++)
				//  {
				//  		fprintf(stderr,"cmd%d:%s\n",i,cmd[i]);

				//  }
				cmd[counter]= (char*)NULL;
				pid_t child = fork();
					

			
				if(child == -1)
				{
					print_fork_failed();
					return 1;

				}
				else if(child > 0) // This is the parent ( need to wait and then exit)
				{



					int status;
					if(background==0)
						waitpid(child, &status, 0);
					else
					{


						if (setpgid(child, child) == -1) {
						print_setpgid_failed();
						exit(1);
							}

						waitpid(child, &status, WNOHANG);
						process *p =(process*)malloc(sizeof(process));
						p->command= strdup(readincpy);
						p->status= STATUS_RUNNING;
						
						p->pid=child;
							
						

						vector_push_back(currproc,p);

					}


				}
				else if( child ==0) // child process 
				{
					//PUT A EXIT RIGHT AFTER EXEC IN CASE IT FAILS
					
					print_command_executed(getpid());
					execvp(cmd[0], cmd);
					print_exec_failed(cmd[0]);
					ifsucc =0;
					exit(1);		

				}

					vector_push_back(history,readincpy);

				 // free(tok2);
				 // free(val);
				 // free(rd);
				 // free(cols);
			}

		for(int k=0; k<100; k++)
		{
			free(cmd[k]);
		}
		free(cmd);
		free(readin);
		free(readincpy);
		counter=0;
		exits=0;

	}


	free(currdir);
	free(token);
	free(prefix);
	return 0;
}
