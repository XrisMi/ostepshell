#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<ctype.h>

int batch=0;
int pathChanged=0;
char *path;
int CLOSED=0;
int pathEmpty=0;
int spaceFlag = 0;
static int SIZE = 512;
char multiPath[512][512];
int numberMultiPath=0;
 
void printError(){
		char error_message[30] = "An error has occurred\n";
	        write(STDERR_FILENO, error_message, strlen(error_message));
		        exit(1);
			        
}
void printPrompt(){
	        write(STDOUT_FILENO, "wish> ", strlen("wish> "));                
}
int newProcess(char *myargs[]) {
	int rc=fork(); //new fork
	if(rc<0){ 
		  printError();
		  exit(1); 
		  }
		 else if(rc==0 && pathEmpty!=1){ // In the child process
		  if(pathChanged==0){
		  	path=strdup("/bin/");
		  	path=strcat(path,myargs[0]);
		  	if(access(path,X_OK)!=0 && pathChanged==0){ //check binary access
		  		path=strdup("/usr/bin/");
		  		path=strcat(path,myargs[0]);
		  		if(access(path,X_OK)!=0){
					printError();
		  			exit(0);
		  }
		 }             
		}
		  else if(pathChanged==1 && numberMultiPath==0){
		  	path=strcat(path,myargs[0]);
		  	if(access(path,X_OK)!=0){
		  		printError();
				exit(0);
		  	}
		  }
		 else{
		  	for(int x=0;x<numberMultiPath;x++){
		  	strcat(multiPath[x],myargs[0]);
		  	if(access(multiPath[x],X_OK)==0){
		  		strcpy(path,multiPath[x]);
		  		break;
		  	}
		  }
	}
		  if(execv(path,myargs)==-1){// check if binary was successfully executed 
		  	printError();
		  	exit(0);
		  }
		 }
		  else {
		 	 int returnStatus=0;
		  }
		  return rc;
	 }
		  
int preProcess(char *buffer){
	int stdout_copy=0;
	int rc;
	if(strstr(buffer,">")!=NULL){ // Redirect
		int a=0;
		char* multiRedirect[sizeof(char)*512];
		multiRedirect[0]= strtok(strdup(buffer)," \n\t>");
		while(multiRedirect[a]!=NULL){
			a++;
			multiRedirect[a]=strtok(NULL," \n\t>");
		}	    
		        if(a==1){ //no output file 
				printError();
				exit(0);    
			}	      

			int i=0;
			char* myargs[sizeof(buffer)];
			myargs[0]= strtok(buffer,"\n\t>");
			while(myargs[i]!=NULL){
				      i++;
				      myargs[i]=strtok(NULL," \n\t>"); 
				  }
			if(i>2){
				 printError();
				 exit(0);    
			}
			int x=0;
			char* tokenize[sizeof(myargs[1])];
			tokenize[0]= strtok(myargs[1]," \n\t");
			while(tokenize[x]!=NULL){
				 x++;
				 tokenize[x]=strtok(NULL," \n\t"); 
			}
			char *fout=strdup(tokenize[0]);
			stdout_copy=dup(1);

			int out=open(fout,O_WRONLY|O_CREAT|O_TRUNC,0666);
			int error=open(fout,O_WRONLY|O_CREAT|O_TRUNC,0666);
			fflush(stdout);
			dup2(out,STDOUT_FILENO);
			dup2(out,STDERR_FILENO);
			close(out);
			CLOSED=1;
			if(out==-1 || error==-1 || x>1 || i>2){
				      printError();   
				      exit(0);
				   }
			myargs[i+1]=NULL;
			tokenize[x+1]=NULL;
			strcpy(buffer,myargs[0]);
			}
			if(buffer[0] != '\0' && buffer[0] != '\n') {
				      char *command[sizeof(buffer)];
				      command[0] = strtok(buffer, " \t\n");
				      int p=0;
				      while(command[p]!=NULL){
				     	 p++;
				      	 command[p]=strtok(NULL, " \n\t");
				     }
				      command[p+1]=NULL;
				      if(strcmp(command[0],"cd") == 0){//cd
				      	if(p==2){
				      		if(chdir(command[1])!=0){
							printError();
				      		}
				     	}
				      else{ 
				        printError();
				       }
				      }  
				      else if(strcmp(command[0],"path") == 0){
				      	pathChanged=1;
				      	if(p==2){
				      		pathEmpty=0;
				      		path=strdup(command[1]);
				      	if(path[strlen(path)-1]!='/'){
				      		strcat(path,"/");
				            	 }      
				      	}
				      else if(p==1){
				      	pathEmpty=1;
				      }
				     else {
				      	pathEmpty=0;
				      for(int i=1;i<p;i++){
				      	char *temp=strdup(command[i]);
				      	if(temp[strlen(temp)-1]!='/'){
				      		strcat(temp,"/");
				      		strcpy(multiPath[i-1],temp);
				      		numberMultiPath++;
				      		}
				      	}
				}}
				else if(strcmp(command[0],"exit") == 0) {
				      	if(p==1){
						fflush(stdin);
				      		exit(0);
				      }
				      else{ 
				      	printError();
				      }
				     }    
				      else{
				      	if(pathEmpty==1)
				      		printError();
					else
				      		rc=newProcess(command);
				      }
				     }
				      if(CLOSED==1){
				      	dup2(stdout_copy,1);
				      	close(stdout_copy);
				      }
				     return rc;
				}
int main(int argc, char* argv[]){
	
	FILE *file = NULL;
	path=(char*) malloc(SIZE);
	char buffer[SIZE];
				     
       	if(argc==1){ //Not in batch mode
		file=stdin; 
		printPrompt();
	} 
	else if(argc==2){ // Batch mode
			char *bFile= strdup(argv[1]);
			file = fopen(bFile, "r");
			if (file == NULL) {
				   printError();
			}
		       batch=1;
		}
			else{
				printError();
			}
		     while(fgets(buffer, SIZE, file)){ //file write to buffer
				      CLOSED=0;


				      for(int i=0;i<strlen(buffer);i++){ // check if space
				      	if(isspace(buffer[i])==0){ 
						spaceFlag=1;
						break;
					}
					continue;
				      }
				      if(strstr(buffer,"&")!=NULL){// Concurrency
				      	int j=0;
				      	char *myargs[sizeof(buffer)];
				      	myargs[0]= strtok(buffer,"\n\t&");                            
				      	while(myargs[j]!=NULL){
				      		j++;
				      		myargs[j]=strtok(NULL,"\n\t&"); 
				      // every call with NULL uses saved user_input 
				      // value and returns next substring
				      }
				      myargs[j+1]=NULL;
				      int pid[j];
				      for(int i=0;i<j;i++){
				      	pid[i]=preProcess(myargs[i]);
				      	for(int x=0;x<j;i++){
				      		int returnStatus=0;
				      		waitpid(pid[x],&returnStatus,0);                        
				      		if (returnStatus == 1){
				      			printError();    
				      			}		                       
				      	}
				      }
				    }
				      else{
					      preProcess(buffer);
				      }
				      if(argc == 1) {
				      	printPrompt();
				      }
			      }
	 }
