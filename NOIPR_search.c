/******The program is designed by BlackHawk with C language in 2006/08/15********************************/
         /**************************************************/
/******In this program, the major arithmetic is supported by WuXin.******/
         /**************************************************/
/*******It can search the fullerene structure of special NOIPR carbon numbers through the .w3d file****/


/**************The net of different functions******** */
/****
                     main(int argc,char*argv[])
                        /      |     \      \
                       /       \       \       \       
                      /        \          \        \
                     /          \             \        \
                    /            \               \        showHelp()
                   /              \                  \
                  /                \                     \ 
                /                   \                        \
readGjfInputFormFile(FILE*)   dealWithSearchContion(char*)     \
               |                                                  readW3DFile(FILE*)
               |                                             /    /  \          \
               |                                           /      /   \          \
             dealWith32_10                              /       /    \           \
                                                      /        /     \           \
                                                    /         /      \           \
                                                  /          /       \           \
                         getFileNum(char*,int ,char)       /         \           \
                                                          /         \            \
                                     dealWithSingleRow(char*)       \            saveGJFFile(W3DDate *,int,int)                                   
                                                                    \
                                       dealWithSingleGroup(const W3DDate*)
                                                          |
                                                          |
                                                          |
                                         dealWithSinglePoint( const W3DDate*,int )
                                                          |
                                                          |
                                                     pow_int(int,int)                                                            
***************************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<time.h>
#include<errno.h>
/**********/
typedef struct PentagonHexagon 
      {
	      int PentagonNum;
	      int HexagonNum;
	    }PenHex;
	    
/****declare the tree data structure******/
typedef struct TS 
      {
	     struct TS* PreBranchPointer;
	     int BranchData;
	    }TreeStruct;
/*******declare data structure by myself****/
typedef struct RDW3D
      {
      	double CoordinateXYZ[3];
      	int Category[4];   /**Category[0] save the data of tree point*/
      	
      }W3DDate;
typedef struct PentagonNum2PentagonNum3
      {
      	int Pen2Num;
      	int Pen3Num;
      	int PenMore2Num;
      }Pen2Pen3;
typedef struct SearchCondition
      {
      	int Condition_One;
      	int Condition_Two;
      }SearchCon;
typedef struct hk
	   { 
	   	  int RowNum;
	   	  int BlankSeriNum;
	   }InputForm;
typedef struct fil
     {
     	   unsigned long curFilePos;
     	   unsigned int pen2;
     	   unsigned int pen3;
     }FilePos;
/**********search mode*********/
enum SearchMode{PEN2_OR_PEN3,PEN2_ONLY,PEN2_AND_PEN3};
enum DEAL_FPOS_MODE{NOT_USING_FPOS,USING_FPOS_TEMP,USING_FPOS_ARRAY};
enum BOOLEN{FALSE_BOOL,TRUE_BOOL};
/********macro declare****/
#define READLENGTH  130
#define ListSeparator "/"    /**linux****/
#define MAX 150     
#define SHOW(X,Y) fprintf(stdout,"The program in %s for %d\n",X,Y)
#define SHOWSTDOUT(X) fprintf(stdout,"%s \n",X)
#define SHOWPSTATUS_1(X,Y) fprintf(ProcessStatusFile,"%s for %d\n",X,Y)
#define SHOWPSTATUS_2(X) fprintf(ProcessStatusFile,"%s \n",X)

/*****Declare the variables****/
int TotalNumber;
W3DDate SingRowTempDate;          /***variable send data***/
char gjfNoteString[READLENGTH];          /**get the gjf noting line from gjf input file**/
int Pentagon_2_Num;
enum SearchMode Search_Mode;
enum DEAL_FPOS_MODE MyFPosMode;
SearchCon MySearchContion;       /***the contion of searching**/	  
char gjfResultFolderNameStr[50]; /**Result folder of gjf file**/ 
PenHex singlePointData;
char*gjfInputForm[MAX];
InputForm gjf_Input_Form;

int ConditionNumber;    /*FILE* SaveFilePoint; test*/
int*SearchContionData=NULL;
FILE* ProcessStatusFile;
int OneTimeSearchNumber;
FilePos*myCurrFilePos;
unsigned int CurrFilePosNum;
unsigned int SetFilePosArrayLengh;
FILE*TempFilePoint;
char tempFileName[ ]="template-XXXXXX";

time_t starttime;

/***declare the function**/

/*#define BUG                /***/


int dealWithSinglePoint( const W3DDate*,int );
int dealWithSingleGroup(const W3DDate *);
/*int ReadPDBFile(FILE*);*/
int readW3DFile(FILE*);
int saveGJFFile(W3DDate *,int);              
int dealWithSingleRow(char*);             
void showHelp(); 
void readGjfInputFormFile(FILE*);                
char* getFileNum(char*,int ,char);                         
long int pow_int(int,int);
int dealWithSearchContion(char*);
int getDataFormFilePos(FILE*,long);
int getDataFromArray(FILE*,int,int);
int getDataFormTempFilePos(FILE*,int,int);
void writeLastStatus(enum BOOLEN);
/*****define funciton****/

/*******deal with single point********/
int dealWithSinglePoint(const W3DDate*GroupData,int n)  
{
    int i,j,k,m,g;
    int num;
    int C5Num,C6Num;
    PenHex Result;
    TreeStruct*TreePointData[7];
    C5Num=0;C6Num=0;num=0;
    
    
    TreePointData[0]=(TreeStruct*)malloc(sizeof(TreeStruct));
    TreePointData[1]=(TreeStruct*)calloc(3,sizeof(TreeStruct));
    for(i=2;i<7;i++)
    {
    	 TreePointData[i]=(TreeStruct*)calloc(3*pow_int(2,i-1),sizeof(TreeStruct));
    }
    TreePointData[0][0].PreBranchPointer=NULL;
    TreePointData[0][0].BranchData=GroupData[n].Category[0];
    for(i=0;i<3;i++)
    {
    	 TreePointData[1][i].BranchData=GroupData[n].Category[i+1];
    	 TreePointData[1][i].PreBranchPointer=TreePointData[0];
    }
    for (i=2;i<7;i++)
      for (j=0;j<3*pow_int(2,i-2);j++)    /**circle the father tree points*******/
          for(k=0;k<TotalNumber;k++)
            if ((TreePointData[i-1][j].BranchData)==(GroupData[k].Category[0]))
            	  {
            	  	num=j*2;
            	  	for (m=1;m<4;m++)
            	  	  if ((TreePointData[i-1][j].PreBranchPointer)->BranchData!=GroupData[k].Category[m])
            	  	  	{
            	  	  		 TreePointData[i][num].BranchData=GroupData[k].Category[m];
            	  	  		 TreePointData[i][num].PreBranchPointer=&(TreePointData[i-1][j]);
            	  	  		 num++;
            	  	  	}
            	  	}
            	  	
   /****caculate the num of Pentagon and Hexagon******/ 
      for(g=0;g<3*pow_int(2,4);g++)
        if(TreePointData[5][g].BranchData ==TreePointData[0][0].BranchData )
        	     	  C5Num++;
      for(g=0;g<3*pow_int(2,5);g++)
        if(TreePointData[6][g].BranchData ==TreePointData[0][0].BranchData )
        	     	  C6Num++;	  	
      singlePointData.PentagonNum=C5Num/2;
      singlePointData.HexagonNum=C6Num/2;
      
      for(i=0;i<7;i++)
         free(TreePointData[i]);    /****release the memory***/
         
      return 1;
    
}
  /****get the N th substring in the string***/
char* getFileNum(char*p,int n,char c)     
{
	char*start,*end;
  char result[100];
  char *q;
  start=p;
  q=result;
  end=(char*)calloc(strlen(p)+1,sizeof(char));
  
  while(*start )
  {
      if  ((int)(*start)==(int)c)
      {   
          start++;}
      else
          {
            
           strcpy(end,start);
           
           while((*start)!='\0' &&(int) (*start)!=(int)c )start++;
           
           if (!(--n)){ 
           	
            strncpy(q,end,strlen(end)-strlen(start));
            q[strlen(end)-strlen(start)]='\0';
            break;}
           }
          
   }
  free(end); 
  return q; 
}


/*******Read w3d File*****/
int readW3DFile(FILE*pF)
{
	   char p[READLENGTH],*q;
	   int i,j;
	   int numm;
	   
	   W3DDate*SingGroupData;
	   numm=0;
	   
	   
	   #ifdef BUG
	      SHOWSTDOUT("come in readW3DFile");
	   #endif
	   
	   /*if ((TESTPF=fopen("testresult","a"))==NULL) { fprintf(stdout,"%s\n","error in open file testresult");return 1;}*/
	   	
	   if (!feof(pF)) 
	   {
	   	  q=fgets(p,READLENGTH,pF);
	   	  if (! strcmp(q,">>writegraph3d<<")) 
	   	  	{
	   	  		fclose(pF);
	   	  		SHOWPSTATUS_2("Format Error in the readW3DFile function");
	   	  		writeLastStatus(FALSE_BOOL); 
	   	  		exit(0);
	   	    }
	   }
	   else
	   	  {
	   	  	SHOWPSTATUS_2("W3D File is empty in the readW3DFile function");
	   	  	writeLastStatus(FALSE_BOOL); 
	   	  	exit(0);
	   	  }
	   
	   SingGroupData=(W3DDate*)calloc(TotalNumber,sizeof(W3DDate));
	   while(!feof(pF))
	   {    
	   	    switch (MyFPosMode)
   	     {
   	   	   case USING_FPOS_ARRAY:
   	           myCurrFilePos[CurrFilePosNum].curFilePos=ftell(pF);
   	           break;
   	       case USING_FPOS_TEMP:
   	       	   myCurrFilePos->curFilePos=ftell(pF);
   	       	   break;
   	       default:
   	       	   break;
   	    }
	   	    /*SHOW("int readW3DFile while",nu++);**/
	   	    
	   	    if(feof(pF)) goto Free_Deal;
	   	    if (fgets(gjfNoteString,READLENGTH,pF)==NULL)goto Free_Deal;;
	   	    
	   	   
	   	    /*SHOWPSTATUS_2(gjfNoteString);*/
	   	    
	   	    numm=atoi(getFileNum(gjfNoteString,2,' '));
	   	    
	   	    if(feof(pF))goto Free_Deal;
	   	    
	   	    
	   	    for(i=0;i<TotalNumber;i++)
	   	    {
	   	    		if(feof(pF)){goto Free_Deal;}
	   	    		if (fgets(p,READLENGTH,pF)==NULL)goto Free_Deal;
	   	    		/*SHOWPSTATUS_2(p);*/
              if (dealWithSingleRow(p)) 
	             {
	             	  SingGroupData[i].Category[0]=SingRowTempDate.Category[0];
	             	  for(j=0;j<3;j++)
	             	  {
	             	  	SingGroupData[i].CoordinateXYZ[j]=SingRowTempDate.CoordinateXYZ[j];
	             	  	SingGroupData[i].Category[j+1]=SingRowTempDate.Category[j+1];
	             	  }
	             	  
	             	}
	             else
	             {
	             	  SHOWPSTATUS_2("The process of reading SingleRow is error in the readW3DFile function.");
	             	  return(0);
	             }
	         }
	         /*SHOW("over",nu);*/
	         if(feof(pF)) {goto Free_Deal;}
	         if (fgets(p,READLENGTH,pF)==NULL)goto Free_Deal;
	         if (atoi(p)!=0){
	         	  SHOWPSTATUS_2("The format of .w3d file is error in the readW3DFile function.");
	         	  writeLastStatus(FALSE_BOOL); 
	         	  exit(0);
	         	}
	          if (dealWithSingleGroup(SingGroupData)) 
	         	{
	         		  saveGJFFile(SingGroupData,numm);
	         		  
	         		  OneTimeSearchNumber++;  
	          }
	          if(feof(pF)) {goto Free_Deal;}  
	   }
	  
Free_Deal:
	       
	       free(SingGroupData);
	         	 
	        
	   #ifdef BUG
	      SHOWSTDOUT("leave from readW3DFile");
	   #endif
	   return(1);
}

/*************Deal With Singel Group data*****/
int dealWithSingleGroup(const W3DDate*SingleGroupData)
{
	  PenHex*TotalGroupPenHex;
	  
	  int i,j;
	  int pen_2_Num,pen_3_Num;
	  pen_2_Num=0;
	  pen_3_Num=0;
	  
	  
	  
	  TotalGroupPenHex=(PenHex*)calloc(TotalNumber,sizeof(PenHex));
	  for(i=0;i<TotalNumber;i++)
	  {
	  	  if(dealWithSinglePoint(SingleGroupData,i))
	  	  {
	  	    TotalGroupPenHex[i].PentagonNum=singlePointData.PentagonNum;
	  	    TotalGroupPenHex[i].HexagonNum=singlePointData.HexagonNum;
	  	  
	  	  }
	  	  else
	  	  	{
	  	  		SHOWPSTATUS_2("dealWithSinglePoint is error");
	  	  		writeLastStatus(FALSE_BOOL);
	  	  		exit(0);
	  	  	}
	  }
	  for(j=0;j<TotalNumber;j++)
	  {
	  	  if (TotalGroupPenHex[j].PentagonNum==2) pen_2_Num++;
	  	  if (TotalGroupPenHex[j].PentagonNum==3) pen_3_Num++;
	  }
   switch (MyFPosMode)
   {
	    case USING_FPOS_ARRAY:
  	      myCurrFilePos[CurrFilePosNum].pen2=pen_2_Num;
  	      myCurrFilePos[CurrFilePosNum].pen3=pen_3_Num;
  	      
  	      /*fprintf(ProcessStatusFile,"%ld     %d      %d \n",myCurrFilePos[CurrFilePosNum].curFilePos,myCurrFilePos[CurrFilePosNum].pen2,myCurrFilePos[CurrFilePosNum].pen3);*/
  	      CurrFilePosNum++;
  	      if (CurrFilePosNum>=SetFilePosArrayLengh)
  	      	 {
  	      	 	   SetFilePosArrayLengh=CurrFilePosNum+100;
  	      	 	   myCurrFilePos=(FilePos*)realloc(myCurrFilePos,SetFilePosArrayLengh*sizeof(FilePos));
  	      	 	   if(myCurrFilePos==NULL)
  	      	 	   	 {
  	      	 	   	    fprintf(ProcessStatusFile,"In the process of allot memory(realloc function),there is %s\n","error");
  	      	 	   	    writeLastStatus(FALSE_BOOL);
  	      	 	   	    exit(0);
  	      	 	   	 }
  	      	 	   	
   	      	 }
  	      break;
	   case USING_FPOS_TEMP:
  	      myCurrFilePos->pen2=pen_2_Num;
  	      myCurrFilePos->pen3=pen_3_Num;
  	      fprintf(TempFilePoint,"%ld  %d  %d\n",myCurrFilePos->curFilePos,myCurrFilePos->pen2,myCurrFilePos->pen3);
  	      break;
	   default:
	   	  break;
	 }
	 
	      
	  switch (Search_Mode)
	  {
	  	case PEN2_OR_PEN3:
	  		   if (MySearchContion.Condition_One==(pen_2_Num+pen_3_Num)) 
	  		   	   return 1;
	  		   else
	  		   	   return 0;
	  		   break;
	  	case PEN2_ONLY:
	  	     if (MySearchContion.Condition_One==pen_2_Num && pen_3_Num==0)
	  	     	   return 1;
	  	     else
	  	     	   return 0;
	  	     break;
	  	case PEN2_AND_PEN3:
	         if (MySearchContion.Condition_One==pen_2_Num && MySearchContion.Condition_Two==pen_3_Num)
	             return 1;
	  	     else
	  	     	   return 0;
	  	     break;
	  	default:
	  		   SHOWPSTATUS_2("no search mode,please input the search mode!");
	 }
	 free(TotalGroupPenHex);
}
/**************dealWithSingleRow***********/
int dealWithSingleRow( char*RowString)	   /***SingRowTempDate send data to function readW3DFile***/
{
	    int i;
	    char*start,*end;
	    
	    start=RowString;
	    end=RowString;
	    
	    SingRowTempDate.Category[0]=strtol(start,&end,10);
	    start=end;
	    for(i=0;i<3;i++)
	    {
	         SingRowTempDate.CoordinateXYZ[i]= strtod(start,&end);
	         start=end;
	    }
	    for(i=1;i<4;i++)
	    {
	    	  SingRowTempDate.Category[i]=strtol(start,&end,10);
	    	  start=end;
	    }
	    return(1);
}
int saveGJFFile(W3DDate *Coordinate_Data,int serialnum)                          /***save as gjf file**/
{
	  char *p;
	  char AbsoluDir[200];
	  char buff[10];
	  char buff2[10];
	  FILE*pF;
	  int i,j;
	  
	  #ifdef BUG
	   SHOWSTDOUT("come in saveGJFFile");
	  #endif
	                              /*SHOW("saveGJFFile",2);*/
	  if (getcwd(AbsoluDir,100)==NULL)
	  	{
	  		printf("the getting AbsoluDir is error\n");
	  		exit(1);
	  	}
	  p=AbsoluDir;
	                 
	  strcat(p,ListSeparator);
	  strcat(p,gjfResultFolderNameStr);
	  if(access(p,0)!=0)
	  {
	     if(mkdir(p,S_IRWXU|S_IRWXG|S_IRWXO)!=0)
	        fprintf(ProcessStatusFile,"!Createing the dir %s is error.\n",p);
	  }	 	
	  strcat(p,ListSeparator);
	  sprintf(buff,"C%d_",TotalNumber);
	  strcat(p,buff);
	  sprintf(buff,"%d.gjf",serialnum);
	  strcat(p,buff);                       /**处理结果文件路径**/
	   
	  sprintf(buff2,"C%d_%d.chk",TotalNumber,serialnum);
	                                 /*SHOW("saveGJFFile",3);*/
	  if((pF=fopen(p,"w"))==NULL)
	  	{
	  		 SHOWPSTATUS_2("Open the savefile is error in saveGJFFile function");
	  		 writeLastStatus(FALSE_BOOL);
	  		 exit(1);
	  	}
	  fprintf(pF,"%s%s\n","%chk=",buff2);	
	  for(i=1;i<gjf_Input_Form.RowNum;i++)
	  {
	  	 if ((i-1)==gjf_Input_Form.BlankSeriNum) fprintf(pF,"%s",gjfNoteString);
	  	 fprintf(pF,"%s",gjfInputForm[i]);
	  }
	                /*SHOW("saveGJFFile",4);*/
	  
	  for(i=0;i<TotalNumber;i++)
	  {
	  	fprintf(pF,"%s      ","C");
	  	j=0;
	  	do
	  	{
	  	   fprintf(pF,"%.6f       " ,Coordinate_Data[i].CoordinateXYZ[j]); 
	       j++;
	    
	    }while(j<3);
	    fprintf(pF,"%s","\n");
	  }
	  fprintf(pF,"%s","\n");
	  fclose(pF);
	  
	  #ifdef BUG
	     SHOWSTDOUT(" leave from saveGJFFile");
	  #endif                
}

/**major function **/
int main(int argc,char*argv[])
{
	
	char DataFileNameString[50],buffer[20],gjfInputFileName[50];
	char *point_FileName,*point_gjfFileName,*point_FolderName;
	char buffFolderName[50],*buffFolder;
	char *judstr,*pname;
	enum BOOLEN IsDeleteTemFile;
	enum BOOLEN DEAL_WITH_FILE_POS;
	unsigned long int TotalNOIPRNumber;
	FILE *pF1,*pFgjf;
	
	int i;
	
	point_FolderName=gjfResultFolderNameStr;
	point_FileName=DataFileNameString;
	point_gjfFileName=gjfInputFileName;
	buffFolder=buffFolderName;
	IsDeleteTemFile=TRUE_BOOL;
	TotalNOIPRNumber=0;
/*if((SaveFilePoint=fopen("savefile.txt","a+"))==NULL){SHOW("FILE IS ERROR",1);return 0;}*/

	if((ProcessStatusFile=fopen("ProcStatus.out","w+"))==NULL)         /****ProcessStatusFile***/
		{
			SHOWSTDOUT("The creating the ProcessStatusFile.out is error");
			writeLastStatus(FALSE_BOOL);
			exit(0);  
		}
	fprintf(ProcessStatusFile,"%s\n","******The program is designed by BlackHawk and WuXin******");
	fprintf(ProcessStatusFile,"%s\n","");
	fprintf(ProcessStatusFile,"%s\n","Note:welcome to give me advice by email(glzhuang0119@163.com)");
	fprintf(ProcessStatusFile,"%s\n","");
	time(&starttime);
	fprintf(ProcessStatusFile,"The program begins with the time %s\n",ctime(&starttime));
	SHOWPSTATUS_2("**********The status and result in the search process******");
	fclose(ProcessStatusFile);
	                                                                /*SHOW("INT MAIN",0);*/
	if((ProcessStatusFile=fopen("ProcStatus.out","a"))==NULL)
		{
			SHOWSTDOUT("The appending  the ProcessStatusFile.out is error");
			writeLastStatus(FALSE_BOOL);
			exit(0);  
		}
		
	             /*SHOW("INT MAIN",1);*/
if(argc<=2) 
		{
			if(strcasecmp(argv[1],"-H")==0)
				{
				 showHelp();
				 exit(0);
				}
				 
			else
				{
			   printf("Plearse input:\"IPR -H|-h\" for the help. \n");
			   exit(0);
			  }
		}
else if(argc>=6 && argc<8)
  	{
    		if(strcasecmp(argv[1],"-m")==0)
    			       {
    		   		  	 Search_Mode=PEN2_OR_PEN3;
    		   		  	 SHOWPSTATUS_2("NOTE: the search mode is PEN2_OR_PEN3.");
    		   		   }
    		else if (strcasecmp(argv[1],"-a")==0)
    			      {
    		   		    Search_Mode=PEN2_AND_PEN3;
    		   		    SHOWPSTATUS_2("NOTE: the search mode is PEN2_AND_PEN3.");
    		   		  }
    		else if (strcasecmp(argv[1],"-o")==0)
    			      { 
    		   	       Search_Mode=PEN2_ONLY;
    		   	       SHOWPSTATUS_2("NOTE: the search mode is PEN2_ONLY.");
    		   	    }
    	  else
    	  	  { 
    	  	  	printf(" The searche mode is error.\n");
    	  	  	SHOWPSTATUS_2(" The searche mode is error.");
    	  	  	exit(1);
    	  	  }
    	  
    	  TotalNumber=atoi(argv[2]);
    	  if (TotalNumber==0) 
    	  {
    	  	  printf("The carbon number of searching is error.\n");
    	  	  SHOWPSTATUS_2("The carbon number of searching is error.");
    	  	  writeLastStatus(FALSE_BOOL);
    	     	     exit(0);
    	  }
    	  strcpy(point_FolderName,"C");
    	  sprintf(buffer,"%d",TotalNumber);
    	  strcat(point_FolderName,buffer);
    	  strcat(point_FolderName,"_NOIPR_");
    	  strcat(point_FolderName,argv[1]);
    	  strcpy(buffFolder,point_FolderName);
  	  
    	  dealWithSearchContion(argv[3]);                      /*deal with SearchMode string*/
    	  
    	  if(ConditionNumber ==1)                                  /*DO deal with fpos mode?*/
    	    {	
      	  	MyFPosMode=NOT_USING_FPOS;
      	  	DEAL_WITH_FILE_POS=FALSE_BOOL;
    	  	}
    	 else
    		{
      	  	DEAL_WITH_FILE_POS=TRUE_BOOL;
      	  	if((argc==7)&&(strcmp(argv[6],"-q")==0))
      	  	   MyFPosMode= USING_FPOS_ARRAY;
      	    else
      	  	   MyFPosMode= USING_FPOS_TEMP;
    	  }            
    	  
    	  judstr=argv[4]+(strlen(argv[4])-3);
    	  if (strcasecmp(judstr,"w3d")!=0)
    	  	{
    	  	     fprintf(stdout,"In the main() the format of file %s is error\n",point_FileName);
    	  	     fprintf(ProcessStatusFile,"In the main() the format of file %s is error\n",point_FileName);
    	  	     writeLastStatus(FALSE_BOOL);
    	         exit(0);
    	    }
    	                 
    	  strcpy(point_FileName,argv[4]);
    	  if ((pF1=fopen(point_FileName,"r"))==NULL)
    	  {
    	  	printf("It cannot open the file %s.\n",point_FileName);
    	  	fprintf(ProcessStatusFile,"It cannot open the file %s.\n",point_FileName);
    	  	writeLastStatus(FALSE_BOOL);
    	  	exit(0);
    	  }
  	                      
  	    strcpy(point_gjfFileName,argv[5]);
  	  	 if ((pFgjf=fopen(point_gjfFileName,"r"))==NULL)
  	      {
  	  	   printf("It cannot open the gjf file %s.\n",point_gjfFileName);
  	  	   fprintf(ProcessStatusFile,"It cannot open the gjf file %s.\n",point_gjfFileName);
  	  	   writeLastStatus(FALSE_BOOL);
  	  	   exit(0);
  	      }
  	                  
  	      readGjfInputFormFile(pFgjf);                /*read gjf input file form pFgjf file pointer*/
  	  
  	   if(DEAL_WITH_FILE_POS==TRUE_BOOL)                        /**deal with MyFPosMode*/
  	   	{
    	     switch (MyFPosMode) 
    	     {
    	     	  case  USING_FPOS_ARRAY:
    	           SetFilePosArrayLengh=1000;
    	           myCurrFilePos=(FilePos*)calloc(SetFilePosArrayLengh,sizeof(FilePos));
    	           if(myCurrFilePos==NULL)
  	      	 	   	 {
  	      	 	   	    fprintf(ProcessStatusFile,"In the process of allot memory(calloc function),there is %s\n","error");
  	      	 	   	    exit(0);
  	      	 	   	 }
    	           break;
    	     	 case USING_FPOS_TEMP:
    	     	 	   mkstemp(tempFileName);
    	     	 	   
    	           myCurrFilePos=(FilePos*)malloc(sizeof(FilePos));
    	           if((TempFilePoint=fopen(tempFileName,"a"))==NULL)
    	           	{
    	        	      fprintf(ProcessStatusFile,"Opening temp file %s is error\n",tempFileName);
    	        	      writeLastStatus(FALSE_BOOL);
    	        	      exit(0);
    	        	  }
    	        	 break;
    	       default:
    	       	   break;
    	      }
    	 }
      for(i=0;i<ConditionNumber;i++)
    	  {  
        	  	 MySearchContion.Condition_One=SearchContionData[i];
        	  	 if(Search_Mode==PEN2_OR_PEN3)
        	  	     SHOWPSTATUS_1("Begin with search the structure of  NOIPR num ",SearchContionData[i]);
        	  	 else if (Search_Mode==PEN2_AND_PEN3)
        	  	 	   fprintf(ProcessStatusFile,"Begin with search the structure of NOIPR pen2 num %d and NOIPR pen3 num %d.\n",SearchContionData[i],MySearchContion.Condition_Two);
        	     else 
        	     	    fprintf(ProcessStatusFile,"Begin with search the structure of NOIPR pen2 num %d and NOIPR pen3 num %d.\n",SearchContionData[i],0);
        	     OneTimeSearchNumber=0;     
        	     if ( (MySearchContion.Condition_One<0) ||(MySearchContion.Condition_One>TotalNumber))
        	      {
          	  	  SHOWPSTATUS_2("The NOIPR C Number of searching is wrong.");
          	  	  writeLastStatus(FALSE_BOOL);
          	     	exit(0);
        	      }	
        	      if (Search_Mode==PEN2_AND_PEN3)
        	         sprintf(buffer,"_%d_%d",MySearchContion.Condition_One,MySearchContion.Condition_Two);   /***define the name of folder**/
        	      else
        	      	 sprintf(buffer,"_%d",MySearchContion.Condition_One);
        	      strcpy(point_FolderName,buffFolder);
        	      strcat(point_FolderName,buffer);
        	     
        	     /**dealwith search mode**/ 
       	      if (i==0) 
       	      {
          	      	readW3DFile(pF1);
          	      	fprintf(ProcessStatusFile,"   Searching result:   NOIPR structure number is   %d.\n",OneTimeSearchNumber);
          	      	TotalNOIPRNumber=TotalNOIPRNumber+OneTimeSearchNumber;
          	      	switch (MyFPosMode)
          	      	{
              	        case USING_FPOS_TEMP:
              	        	   fclose(TempFilePoint);
              	        	   break;
              	        case USING_FPOS_ARRAY:
              	      	  myCurrFilePos=(FilePos*)realloc(myCurrFilePos,CurrFilePosNum*sizeof(FilePos));
              	      	   break;
              	      	default:
              	      		 break;
          	      	}
          	   }
       	      else
       	      	{
       	      		switch (MyFPosMode)
          	      	{
          	      		case USING_FPOS_ARRAY:
       	      		        getDataFromArray(pF1,MySearchContion.Condition_One,MySearchContion.Condition_Two);
       	      		        fprintf(ProcessStatusFile,"   Searching result:   NOIPR structure number is   %d.\n",OneTimeSearchNumber);
       	      		        TotalNOIPRNumber=TotalNOIPRNumber+OneTimeSearchNumber;
       	      		        break;
       	      		    case USING_FPOS_TEMP:
       	      		        getDataFormTempFilePos(pF1,MySearchContion.Condition_One,MySearchContion.Condition_Two);
       	      		        fprintf(ProcessStatusFile,"   Searching result:   NOIPR structure number is   %d.\n",OneTimeSearchNumber);
       	      	          TotalNOIPRNumber=TotalNOIPRNumber+OneTimeSearchNumber;
       	      	          break;
       	      	      default:
          	      		  break;
          	      	} 
       	      	}
       	       		     
    	   }	
     free(SearchContionData);
	   SHOWPSTATUS_2("----------------TotalNumber for this Searching--------------");
	   fprintf(ProcessStatusFile,"   Total Searching result:  TotalNumber is   %ld.\n", TotalNOIPRNumber);
	   writeLastStatus(TRUE_BOOL);   /**write last status to ProcessStatusFile**/
  
    /*SHOW("INT MAIN",7);*/
   	   fclose(pF1);
   	   fclose(pFgjf);
   	   fclose(ProcessStatusFile);
   	 /*SHOW("INT MAIN",8);  */
   	   switch (MyFPosMode)
   	   {
   	   	   case USING_FPOS_TEMP:
   	           free(myCurrFilePos);
   	           fclose(TempFilePoint);
   	           if (IsDeleteTemFile==TRUE_BOOL) unlink(tempFileName);
   	           break;
   	       case USING_FPOS_ARRAY:
   	       	   free(myCurrFilePos);
   	       	   break;
   	       default:
   	       	   break;
   	   }
  	      
  }
	else
   	   {
   	   	 printf("The input parameter is wrong.\n");	
   	   	 showHelp();
   	   	}
	   
   	  
	return(1);
}


/********deal With Search Condition numbers*********/
int dealWithSearchContion(char*str)
{
	
	char*p,C;
	
	int maxdata,mindata,m,i;
	maxdata=0;
	mindata=0;
	C=*str;  
	
	p=str;
	ConditionNumber=0;
	#ifdef BUG
	   SHOWSTDOUT("come in dealWithSearchContion");
	#endif
	 if (C=='S'||C=='s')
	   	 { 
	   	  	str++;str++;
	   	  	 while ((*p)!='\0')
          { 
            if (*p=='+')
	   	        ConditionNumber++;
            p++;
          }
              ConditionNumber++;
	   	  	   
	   	  	SearchContionData=(int*)calloc(ConditionNumber,sizeof(int));
	   	  	 for(i=0;i<ConditionNumber;i++)
	   	  	     SearchContionData[i]=atoi(getFileNum(str,i+1,'+'));
	   	  }
	   	  
	   else if(C=='T'||C=='t')
	   	  {
	   	 	  str++;str++;
	   	 	  maxdata=atoi(getFileNum(str,1,':'));
	   	 	  mindata=atoi(getFileNum(str,2,':'));
	   	 	  if (maxdata<mindata){
	   	 	  	m=mindata;
	   	 	  	mindata=maxdata;
	   	 	  	maxdata=m;   }
	   	 	  ConditionNumber=maxdata-mindata+1;
	   	 	  SearchContionData=(int*)calloc(ConditionNumber,sizeof(int));
	   	 	  m=0;
	   	 	  for(i=mindata;i<=maxdata;i++)
	   	  	     SearchContionData[m++]=i;
	   	   }
	   	  
	  else if(C=='E'||C=='e')
	   	   {
	   	   	  str++;
	   	   	  str++;
	   	 	    maxdata=atoi(getFileNum(str,1,':'));
	   	 	    mindata=atoi(getFileNum(str,2,':'));
	   	 	   if (maxdata<mindata){
	   	 	      	m=mindata;
	   	 	  	    mindata=maxdata;
	   	 	  	    maxdata=m;   }	
	   	 	   if (maxdata%2==1) maxdata--;
	   	 	   if (mindata%2==1) mindata++;
	   	 	   ConditionNumber=(maxdata-mindata)/2+1;
	   	 	   SearchContionData=(int*)calloc(ConditionNumber,sizeof(int));
	   	 	   m=0;
	   	 	   for (i=mindata;i<=maxdata;i=i+2)
	   	 	       SearchContionData[m++]=i;
	   	 	  }
	   	 	 
	   else if(C=='O'||C=='o')
	        {
	          str++;str++;
	        	maxdata=atoi(getFileNum(str,1,':'));
	   	 	    mindata=atoi(getFileNum(str,2,':'));
	   	 	    /*fprintf(stdout,"%d %d\n",maxdata,mindata);*/
	   	 	   if (maxdata<mindata){
	   	 	      	m=mindata;
	   	 	  	    mindata=maxdata;
	   	 	  	    maxdata=m;   }	
	   	 	   if (maxdata%2==0) maxdata--;
	   	 	   if (mindata%2==0) mindata++;
	   	 	   ConditionNumber=(maxdata-mindata)/2+1; 
	   	 	   SearchContionData=(int*)calloc(ConditionNumber,sizeof(int));
	   	 	   m=0;
	   	 	   for (i=mindata;i<=maxdata;i=i+2)
	   	 	       SearchContionData[m++]=i;
	   	 	  } 
	   else if(C=='A'||C=='a')
	   	    {
	   	    	str++;str++;
	   	    	maxdata=atoi(getFileNum(str,1,'+'));
	   	 	    mindata=atoi(getFileNum(str,2,'+'));
	   	 	    /*fprintf(stdout,"%d %d\n",maxdata,mindata);*/
	   	 	    ConditionNumber=1;
	   	 	    SearchContionData=(int*)calloc(ConditionNumber,sizeof(int));
	   	 	    SearchContionData[0]=maxdata;
	   	 	    MySearchContion.Condition_Two=mindata;
	   	    }
	   else {
	   	   
	   	   	  ConditionNumber=1;
	   	   	  SearchContionData=(int*)calloc(ConditionNumber,sizeof(int));
	   	   	  SearchContionData[0]=atoi(str);
	   	   	}
	 
	 #ifdef BUG
	   SHOWSTDOUT("leave from dealWithSearchContion");
	 #endif 
	  	
	 return 1;  	  	
}

/*****Get serial number of Blank line in gjf input file*****/
int dealWith32_10(char*p)           
{
   int i,bol=0;
   #ifdef BUG
	   SHOWSTDOUT("come in dealWith32_10");
	 #endif
   for(i=0;i<strlen(p);i++)
      if ((int)(p[i])!=32 && (int)(p[i])!=10 ){bol=1;break;}
     if(bol)
       return(0);
     else 
   #ifdef BUG
	   SHOWSTDOUT("leave in dealWith32_10");
	 #endif
       return(1);
       
 }
 
/****Read gjf input file***/ 
void readGjfInputFormFile(FILE*pF)            
{
	int i,rownum;
	rownum=0;
	                         /*SHOW("IN  readGjfInputFormFile",0);*/
	#ifdef BUG
	   SHOWSTDOUT("come in readGjfInputFormFile");
	 #endif
	for (i=0;i<MAX;i++)
	  gjfInputForm[i]=(char*)calloc(MAX,sizeof(char));
	while(!feof(pF))
	    fgets(gjfInputForm[rownum++],130,pF);
	                         /*SHOW("IN  readGjfInputFormFile",1); */ 
	    gjf_Input_Form.RowNum=rownum;
	for(i=rownum;i<MAX;i++)
	    free(gjfInputForm[i]);
	for(i=0;i<rownum;i++)
	    if(dealWith32_10(gjfInputForm[i]))
	    	{
	    		gjf_Input_Form.BlankSeriNum=i ;
	    		break;
	    	}
	                       /*SHOW("IN  readGjfInputFormFile",2);*/
	 #ifdef BUG
	   SHOWSTDOUT("leave in readGjfInputFormFile");
	 #endif 
}

/************get data from .w3d file by use of FILE Pointer***************************/
int getDataFormFilePos(FILE*pF,long offset)
{
	 W3DDate *SingGroupData;
	 char p[READLENGTH];
	 
	 #ifdef BUG
	   SHOWSTDOUT("come in getDataFromArray");
	 #endif
	 int i,j,numm;
	 
	 SingGroupData=(W3DDate*)calloc(TotalNumber,sizeof(W3DDate));
	 numm=0;i=0;
	   
	    rewind(pF);  
	 		fseek(pF,offset,0);        /*switch file point to pos1*/
	 		
	 		fgets(gjfNoteString,READLENGTH,pF);
	   	numm=atoi(getFileNum(gjfNoteString,2,' '));
	   	for(i=0;i<TotalNumber;i++)
	   	    {
	   	    		if(feof(pF))goto Deal_H1;
	   	    		fgets(p,READLENGTH,pF);
	   	    		/*SHOW(p,k++);*/
              if (dealWithSingleRow(p)) 
	             {
	             	  SingGroupData[i].Category[0]=SingRowTempDate.Category[0];
	             	  for(j=0;j<3;j++)
	             	  {
	             	  	SingGroupData[i].CoordinateXYZ[j]=SingRowTempDate.CoordinateXYZ[j];
	             	  	SingGroupData[i].Category[j+1]=SingRowTempDate.Category[j+1];
	             	  }
	             }
	           else
	             {
	             	  SHOWPSTATUS_2("The process of reading SingleRow is error in the readW3DFile function.");
	             	  return(0);
	             }
	         }
	     if(feof(pF)) goto Deal_H1;
	     fgets(p,130,pF);
	     if (atoi(p)!=0)goto Deal_H1;
Deal_H1:
	     saveGJFFile(SingGroupData,numm);
	     
	     #ifdef BUG
	        SHOWSTDOUT("leave from getDataFromArray");
	     #endif 
}

/*******get the FILE Pointer from the user-defined structure Array*/
int getDataFromArray(FILE*pF,int Con1,int Con2)
{
	int i;
	#ifdef BUG
	   SHOWSTDOUT("come in getDataFromArray");
	#endif
	
	OneTimeSearchNumber=0;
	for(i=0;i<CurrFilePosNum;i++)
	{
		 switch (Search_Mode)
		 {
		   case PEN2_OR_PEN3:
		 	   if (Con1==(myCurrFilePos[i].pen2+myCurrFilePos[i].pen3))
		 	   	 {
		 	   	 	 getDataFormFilePos(pF,myCurrFilePos[i].curFilePos); 
		 	   	   OneTimeSearchNumber++;
		 	   	 }
		 	     break;
		 	 case PEN2_ONLY:
		 	 	 if ((Con1==myCurrFilePos[i].pen2)&&(myCurrFilePos[i].pen3==0))
		 	 	 	{
		 	       getDataFormFilePos(pF,myCurrFilePos[i].curFilePos);
		 	       OneTimeSearchNumber++;
		 	    }
		 	     break;
		 	 case PEN2_AND_PEN3:
		 	 	 if ((Con1==myCurrFilePos[i].pen2)&&(myCurrFilePos[i].pen3==Con2))
		 	 	 	{
		 	       getDataFormFilePos(pF,myCurrFilePos[i].curFilePos);
		 	       OneTimeSearchNumber++;
		 	    }
		 	     break;
		 	 default:
		 	 	   break;
		 }
	}
	#ifdef BUG
	   SHOWSTDOUT("leave in getDataFromArray");
	#endif
	return 1;
}

/*******get the FILE Pointer from the temporary file*/
int getDataFormTempFilePos(FILE*pF,int Con1,int Con2)
{
	int get_Con_1,get_Con_2;
	long get_Pos;
	OneTimeSearchNumber=0;
	#ifdef BUG
	      SHOWSTDOUT("come in getDataFormTempFilePos");
	#endif
	
	if((TempFilePoint=fopen(tempFileName,"r"))==NULL)
	    fprintf(stdout,"In getDataFormTempFilePos opening temp file %s is error\n",tempFileName);
	
	while(!feof(TempFilePoint))
	{
		
		 fscanf(TempFilePoint,"%ld  %d  %d\n",&get_Pos,&get_Con_1,&get_Con_2);
		                          /*fprintf(ProcessStatusFile,"%ld   %d   %d\n",get_Pos,get_Con_1,get_Con_2);*/
		 switch (Search_Mode)
		 {
		   case PEN2_OR_PEN3:
		 	   if (Con1==(get_Con_1+get_Con_2))
		 	   	{
		 	   		 getDataFormFilePos(pF,get_Pos);
		 	   	   OneTimeSearchNumber++; 
		 	   	 }
		 	     break;
		 	 case PEN2_ONLY:
		 	 	 if ((Con1==get_Con_1)&&(get_Con_2==0))
		 	     {
		 	        getDataFormFilePos(pF,get_Pos);
		 	        OneTimeSearchNumber++;
		 	     } 
		 	     break;
		 	 case PEN2_AND_PEN3:
		 	 	 if ((Con1==get_Con_1)&&(Con2==get_Con_2))
		 	     {
		 	     	  getDataFormFilePos(pF,get_Pos);
		 	        OneTimeSearchNumber++; 
		 	     }
		 	     break;
		 	 default:
		 	 	   break;
		 }
  }
  #ifdef BUG
	      SHOWSTDOUT("leave from getDataFormTempFilePos");
	#endif
	        	
}

/***showHelp function for giving users some advice**/

void showHelp()
{
	char*Title="The help of NOIPR program is as follow:";
	char*List;
	
	fprintf(stdout,"%s\n",Title);
	fprintf(stdout,"%s\n","");
	List="The format of command lines:";
	fprintf(stdout,"%s\n",List);
	List="IPR -m|-a|-o AtomNumber NOIPRNumberFormat InputfilePath gjfInputFilePath [-q]\n";
	fprintf(stdout,"%s\n",List);
	
	List="IPR is program name which goes through (gcc -o IPR NOIPR.c)";
	fprintf(stdout,"   %s\n",List);
	
	List="-m|-a|-o is Search mode\n";
	fprintf(stdout,"   %s",List);
	
	List="-m is PEN2_OR_PEN3\n";
	fprintf(stdout,"        %s",List);
	List="-a is PEN2_AND_PEN3\n";
	fprintf(stdout,"        %s",List);
	List="-o is PEN2_ONLY\n";
	fprintf(stdout,"        %s",List);
	
	List="AtomNumber  is the total carbon number of fullerene.";
	fprintf(stdout,"   %s\n",List);  
	
	List="NOIPRNumberFormat is number format of NOIPR carbon in fullerene.";
	fprintf(stdout,"   %s\n",List); 
	List="There are five NOIPRNumberFormats in the program which is as follow:";
	fprintf(stdout,"   %s\n",List);
	List=" A:     \"T:n1:n2\"  ";
	fprintf(stdout,"         %s\n",List);
	List="which means that NOIPRNumber is all numbers with range from n1 to n2.";
	fprintf(stdout,"         %s\n",List);
	
	List=" B:     \"E:n1:n2\" ";
	fprintf(stdout,"         %s\n",List);
	List="which means that NOIPRNumber is even numbers with range from n1 to n2.";
	fprintf(stdout,"         %s\n",List);
	
	List=" C:     \"O:n1:n2\" ";
	fprintf(stdout,"         %s\n",List);
	List="which means that NOIPRNumber is odd numbers with range from n1 to n2.";
	fprintf(stdout,"         %s\n",List);
	
	List=" D:     \"S:n1+n2+n3+n4....\" ";
	fprintf(stdout,"         %s\n",List);
	List="which means that NOIPRNumber is n1,n2,n3,n4 and so on.";
	fprintf(stdout,"         %s\n",List);
	
	List=" E:     \"A:n1+n2\" ";
	fprintf(stdout,"         %s\n",List);
	List="which means that Pen2 number is n1 and Pen3 number is n2.";
	fprintf(stdout,"         %s\n",List);
	List="Note:this format is only for \"-a\" searchmode.";
	fprintf(stdout,"         %s\n",List);
	List="InputfilePath is the path of the .w3d input file.";
	fprintf(stdout,"   %s\n",List);
	
	List="gjfInputFilePath is the path of the input file int accord with gjf format.";
	fprintf(stdout,"   %s\n",List);
	List="[-q] isnot a required but optional parameter which can speed processing.\n";
	fprintf(stdout,"   %s\n",List);
	fprintf(stdout,"%s\n"," ");
	List="If you have some question,give me email(glzhuang0119@163.com).\n   Thank you for using the software.\n";
	fprintf(stdout,"   %s\n",List);
}
long int pow_int(int a,int b)
{
 long int sum=1;
	while(b>0)
	     {sum=sum*a;b--;}
 return sum;
}
void writeLastStatus(enum BOOLEN StatusBool)
{
	  time_t lasttime;
	   SHOWPSTATUS_2("*************************************************************");
	   if (StatusBool==TRUE_BOOL)
	      SHOWPSTATUS_2("The program is finished successfully.");
	   else
	   	  SHOWPSTATUS_2("The program is aborted for some errors.");
	   time(&lasttime);
	   fprintf(ProcessStatusFile,"The elapsed time is %f seconds.\n",difftime(lasttime,starttime));
}
