/******************** OPTIONS.C *********************/
/*
 * Rory Barnes, Wed May  7 16:27:19 PDT 2014
 *
 * All subroutines necessary to read in all options. Also
 * monitor input files for mistakes and log all option data.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "vplanet.h"
#include "options.h"
#include "output.h"

/*
 *
 * Utility Subroutines
 *
 */

/* Is the current input file the primary, i.e. the one on the command
   line */

void NotPrimaryInput(int iFile,char cName[],char cFile[],int iLine,int iVerbose) {
  if (iLine > 0 && iFile == 0) {
    if (iVerbose >= VERBINPUT)
      fprintf(stderr,"ERROR: %s is not allowed in file %s.\n",cName,cFile);
    LineExit(cFile,iLine);
  }
}

/* Returns the line with the desiried options AND asserts no duplicate 
   entries. cLine is the entire text of the line, iLine is the line 
   number. */ 

/* Is the first non-white space a #? I so, return 1 */
int CheckComment(char cLine[],int iLen) {
  int i;

  for (i=0;i<iLen;i++) {
    if (!isspace(cLine[i])) {
      if (cLine[i] == 35)
	return 1;
    } else
      return 0;
  }
  return 0;
}

void GetLine(char cFile[],char cOption[],char cLine[],int *iLine,int iVerbose) {
  int iLen,done=0,n,i;
  char cWord[OPTLEN],cTmp[LINE];
  FILE *fp;

  iLen=strlen(cOption);

  fp=fopen(cFile,"r");
  i=0;
  memset(cLine,'\0',LINE);
  memset(cTmp,'\0',LINE);

  while(fgets(cTmp,LINE,fp) != NULL) {
    if (!CheckComment(cTmp,iLen)) {
      sscanf(cTmp,"%s",cWord);
      if (memcmp(cWord,cOption,iLen+1) == 0) {
	/* Parameter Found! */
	if (done) {
	    if (iVerbose > VERBINPUT) 
		fprintf(stderr,"Multiple occurences of parameter %s found.\n",cOption);
	  fprintf(stderr,"\t%s, lines: %d and %d\n",cFile,(*iLine+1),i+1);
	  exit(1);
	}
	strcpy(cLine,cTmp);
	*iLine=i;
	done=1;
      }
    }
    i++;
    memset(cTmp,'\0',LINE);
    memset(cWord,'\0',OPTLEN);
  }
  fclose(fp);
}

/* If the previous line ended in $, must find the next valid line
   (the next lines could be a # or blank). The search is made 
   recursively, and cLine and *iLine are the line and line number,
   respectively. */

void GetNextValidLine(char cFile[],int iStart,char cLine[],int *iLine) {
  FILE *fp;
  int i,ok=1;

  fp=fopen(cFile,"r");
  *iLine=0;

  for (i=0;i<iStart;i++) {
    fgets(cLine,LINE,fp);
    (*iLine)++;
  }

  /* If EOF, return */
  if (fgets(cLine,LINE,fp) == NULL) {
    sprintf(cLine,"null");
    fclose(fp);
    return;
  }

  /* Now check for blank line, comment (#), or continue ($) */

  for (i=0;i<LINE;i++) {
    if (cLine[i] == 36 || cLine[i] == 35 || cLine[i] == 10) { 
      /* First character is a $, # or \n: continue */
      GetNextValidLine(cFile,iStart+1,cLine,iLine);
      fclose(fp);
      return;
    }
    if (!isspace(cLine[i])) { 
      /* Found next valid line */
      fclose(fp);
      return;
    }
  }
  /* If made it here, line was blank */
  GetNextValidLine(cFile,iStart+1,cLine,iLine);
  fclose(fp);
}

/* Where is the first non-white-space character in a line? */

int GetPos(char cLine[]) {
  int i;

  for (i=0;i<strlen(cLine);i++) 
    if (!isspace(cLine[i]))
      return i;

  /* Shouldn't be possible to get here */
  return 0;
}

/* Separate a line into words. cInput is an array of strings, each
   containing one word. This routine also checks if the final word has
   a trailing $, if so, it is an array that continues to the next
   line. */

void GetWords(char cLine[],char cInput[MAXARRAY][OPTLEN],int *iNumWords,int *bContinue) {
  int i,j,n;
  char cTmp[OPTLEN];

  n=0;
  /* Use GetPos to avoid white space */
  for (i=GetPos(cLine);i<strlen(cLine);i++) {
    j=0;
    while (!isspace(cLine[i])) {
      if (cLine[i] != 35) {
	/* Fill word in */
	cInput[n][j] = cLine[i];
	j++;
	i++;
      } else {
	/* If at the start of the word, we must decrement n
	   so that when it is incremented at the end of the loop
	   the correct number of words is returned. If at the end 
	   of a word, everything should be fine. */
	if (j==0) n--;

	i=strlen(cLine);
	break;
      }
    }
    /* Now advance to next word */
    while (isspace(cLine[i]))
      i++;
    
    i--;
    n++;
  }
  /* Is the last character a $? If so, remove it and adjust iNumWords */
  if (cInput[n-1][strlen(cInput[n-1])-1] == 36) {
    *bContinue=1;
    if (strlen(cInput[n-1]) == 1) 
      *iNumWords = n-1;
    else
      *iNumWords = n;
    cInput[n-1][strlen(cInput[n-1])-1] = '\0';
  } else {
    *bContinue=0;
    *iNumWords=n;
  }
}

/* If a parameter wasn't found, print a warning if iVerbose is large
   enough. */

void PrintDefault(char cName[],char cDefault[],char cFile[],int iVerbose,int iVerbThreshold) {
  if (iVerbose >= iVerbThreshold) 
    fprintf(stderr,"WARNING: %s not set in file %s, defaulting to %s.\n",cName,cFile,cDefault);
}

double dNegativeDouble(OPTIONS options,char cFile[],int iVerbose) {
  if (iVerbose >= VERBUNITS)
    fprintf(stderr,"WARNING: %s < 0 in file %s, units assumed to be %s.\n",options.cName,cFile,options.cNeg);
  return -options.dNeg;
}

/* Get all fields in a string array. The fields are stored in saInput,
   and the lines which were read are in iNumLines. If a parameter is
   not found, saInput is an array of empty strings, and iLine is
   unchanged. */

void AddOptionStringArray(char cFile[],char cOption[],char saInput[MAXARRAY][OPTLEN], int *iNumIndices,int *iNumLines,int *iLine,int iVerbose) {
  char cLine[LINE],cTmp[MAXARRAY][OPTLEN];
  int i,j,istart,ok,done=0,n=0,num,bContinue,iNumWords;
  FILE *fp;

  /* iLine=malloc(MAXLINES*sizeof(int)); */

  /* Fencepost problem. If cInput continues across multiple lines, 
     then we must keep getting more lines. For the first line, we 
     remove the first word, as it is cOption. iLine must come 
     preassigned and set to 0. */

  /* Initialize the array to have nothing in it. */
  for (i=0;i<MAXARRAY;i++) 
    memset(cTmp[i],'\0',OPTLEN);

  GetLine(cFile,cOption,cLine,&iLine[0],iVerbose);
  GetWords(cLine,cTmp,&iNumWords,&bContinue);
  *iNumLines=1;
  
  for (i=0;i<iNumWords-1;i++) {
    memset(saInput[i],'\0',OPTLEN);
    strcpy(saInput[i],cTmp[i+1]);
    /* Reset cTmp string: If the next time cTmp is filled, the 
       new string is longer than the old, then vestigial characters 
       can remain after a trailing $. */
    memset(cTmp[i+1],'\0',OPTLEN);
  }	
  /* Now subtract off OptionName */
  *iNumIndices=iNumWords-1;
  /* Reset first cTmp string */
  memset(cTmp[0],'\0',OPTLEN);
  
  /* Now keep getting lines until done */
  while (bContinue) {
    GetNextValidLine(cFile,iLine[*iNumLines-1]+1,cLine,&iLine[*iNumLines]);
    if (memcmp(cLine,"null",4)) {
      GetWords(cLine,cTmp,&iNumWords,&bContinue);
      for (i=0;i<iNumWords;i++) {
	strcpy(saInput[*iNumIndices+i],cTmp[i]);
	memset(cTmp[i],'\0',OPTLEN);
      }
      *iNumIndices += iNumWords;
      (*iNumLines)++;
    }
  }
}    

/* Get all fields in a double array. The fields are stored in daInput,
   and the lines which were read are in iNumLines. If a parameter is
   not found, daInput is empty, and iLine is unchanged. */

void AddOptionDoubleArray(char cFile[],char cOption[],double *daInput, int *iNumIndices,int *iNumLines,int *iLine,int iVerbose) {
  int i;
  char cTmp[MAXARRAY][OPTLEN];

  AddOptionStringArray(cFile,cOption,cTmp,iNumIndices,iNumLines,iLine,iVerbose);
  for (i=0;i<*iNumIndices;i++)
    daInput[i]=atof(cTmp[i]);
}

void AddOptionDouble(char cFile[],char cOption[],double *dInput,int *iLine,int iVerbose) {
  char cTmp[OPTLEN],cLine[LINE];

  GetLine(cFile,cOption,cLine,iLine,iVerbose);
  sscanf(cLine,"%s %s",cTmp,cTmp);
  *dInput = atof(cTmp);
}

void AddOptionInt(char cFile[],char cOption[],int *iInput,int *iLine,int iVerbose) {
  char cTmp[OPTLEN],cLine[LINE];

  GetLine(cFile,cOption,cLine,iLine,iVerbose);
  sscanf(cLine,"%s %s",cTmp,cTmp);
  *iInput = atoi(cTmp);
}

void AddOptionBool(char cFile[],char cOption[],int *iInput,int *iLine,int iVerbose) {

  AddOptionInt(cFile,cOption,iInput,iLine,iVerbose);
  if (*iInput == 0 || *iInput == 1) 
    return;
  else {
    if (iVerbose >= VERBERR) {
      fprintf(stderr,"ERROR: %s must be either 0 or 1.\n",cOption);
      LineExit(cFile,*iLine);
    }
  }
}

void AddOptionString(char cFile[],char cOption[],char cInput[],int *iLine,int iVerbose) {
  char cTmp[OPTLEN],cLine[LINE];

  GetLine(cFile,cOption,cLine,iLine,iVerbose);
  sscanf(cLine,"%s %s",cTmp,cInput);
}


int GetNumOut(char infile[],char name[],int iLen,int *nline,int iExit) {
  char line[256],word[24];
  int i,j,istart,ok,done=0,n=0,num;
  FILE *fp;

  fp=fopen(infile,"r");
  if (fp == NULL) {
    fprintf(stderr,"Unable to open %s.\n",infile);
    exit(iExit);
  }

  while(fgets(line,256,fp) != NULL) {
    /* Check for # sign */
    if (memcmp(line,"#",1) != 0) {
      /* Check for desired parameter */
      sscanf(line,"%s",word);
      if (memcmp(word,name,iLen) == 0) {
	/* Parameter Found! */
	if (done) {
	  fprintf(stderr,"ERROR: Multiple occurences of parameter %s found.\n",name);
	  fprintf(stderr,"\t%s, lines: %d and %d\n",infile,*nline,n);
	  exit(iExit);
	}
	done=1;
	*nline=n;

	num=0;
	istart=0;
	ok=1;
	for (i=1;i<256;i++) { /* Ignore first character, as it makes conditional well-defined */
	  /* printf("%d ",line[i]); */ 
	  if (ok) {
	    if (line[i] == 35) {
	      /* Pound sign! */
	      ok=0;
	      num++;
	    }
	    if (isspace(line[i]) && !isspace(line[i-1]))
	      num++;
	  }
	}
      }
    }
    n++;
    for (i=0;i<256;i++) 
      line[i]=0;
  }
  /* Lose the input parameter */
  num--;
  return num;
}

int iGetNumLines(char infile[]) {
  int n;
  FILE *fp;
  char line[256];

  fp=fopen(infile,"r");
  if (fp == NULL) {
    fprintf(stderr,"Unable to open %s.\n",infile);
    exit(EXIT_INPUT);
  }
  while(fgets(line,256,fp) != NULL) {
    n++;
  }
  return n;
}


void InitializeInput(INFILE *input) {
  int n,i,bad;
  FILE *fp;
  char cLine[LINE];

  fp=fopen(input->cIn,"r");
  if (fp == NULL) {
    fprintf(stderr,"Unable to open %s.\n",input->cIn);
    exit(EXIT_INPUT);
  }
  input->iNumLines = iGetNumLines(input->cIn);
  input->bLineOK = malloc(input->iNumLines*sizeof(int));

  for (n=0;n<input->iNumLines;n++) {
    /* Initialize bLineOK */
    input->bLineOK[n] = 0;
    
    /* Now find those lines that are comments or blank */
    for (i=0;i<LINE;i++) 
      cLine[i]='\0';

    fgets(cLine,LINE,fp);
    /* Check for # sign or blank line */
    if (CheckComment(cLine,256)) 
      /* Line is OK */
      input->bLineOK[n] = 1;
    else {
      bad=0;
      for (i=0;i<LINE;i++) {
	if (!isspace(cLine[i]) && cLine[i] != '\0') {
	  bad=1;	
	}
      }	
      if (!bad) input->bLineOK[n] = 1;
    }
  }
}



void Unrecognized(FILES files) {
  FILE *fp;
  char line[256],word[16];
  int i,j,iExit=0,iCheck; /* Assume don't exit */

  for (i=0;i<files.iNumInputs;i++) {
    fp=fopen(files.Infile[i].cIn,"r");

    j=0;
    while (fgets(line,256,fp) != NULL) {
      if (!files.Infile[i].bLineOK[j]) {
	/* Bad line */
	sscanf(line,"%s",word);	
	fprintf(stderr,"ERROR: Unrecognized parameter \"%s\" in %s, line %d.\n",word,files.Infile[i].cIn,j+1);
	iExit=1;
      }
      j++;
    }
  }
  if (iExit)
    exit(EXIT_INPUT);

}

void UpdateFoundOption(INFILE *input,OPTIONS *options,int iLine,int iFile) {
  input->bLineOK[iLine] = 1;
  options->iLine[iFile] = iLine;
  strcpy(options->cFile[iFile],input->cIn);
}

void UpdateFoundOptionMulti(INFILE *input,OPTIONS *options,int *iLine,int iNum,int iFile) {
  int i;

  /*
    For now options->iLine is a scalar, so just use the first line. 
     The user should be able to figure it out from there.
  */
  options->iLine[iFile] = iLine[0];
  strcpy(options->cFile[iFile],input->cIn);
  for (i=0;i<iNum;i++) {
    input->bLineOK[iLine[i]] = 1;
  }
}

void CheckDuplication(OPTIONS options,FILES files,char cFile[],int iLine,int iVerbose) {
    int i,num;

    if (options.iMultiFile) {
	fprintf(stderr,"ERROR: CheckDuplication called, but options.iMultiFile = %d\n",options.iMultiFile);
	exit(EXIT_INPUT);
    }

    num=0;
    for (i=0;i<files.iNumInputs;i++) {
      if (options.iLine[i] >= 0) {
	/* Found previously set location */
	if (iVerbose >= VERBERR) 
	    fprintf(stderr,"ERROR: Option %s found in multiple files\n",options.cName);
	fprintf(stderr,"\t%s, Line: %d\n",files.Infile[i].cIn,options.iLine[i]);
	fprintf(stderr,"\t%s, Line: %d\n",cFile,iLine);
	exit(EXIT_INPUT);
      }
    }

}

/*
 *
 * Initial Option Subroutines
 *
 */

void ReadVerbose(OPTIONS *options,int *iVerbose,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int iTmp;

  AddOptionInt(files->Infile[iNumFile].cIn,options->cName,&iTmp,&lTmp,VERBALL);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,VERBALL);
    if (iTmp < 0 || iTmp > VERBALL) {
      fprintf(stderr,"ERROR: %s must be in the range [0,%d]\n",options->cName,VERBALL);
      LineExit(files->Infile[iNumFile].cIn,lTmp);
    }	
    if (*iVerbose == VERBALL) {
      fprintf(stderr,"WARNING: -v set at command line, but %s option set.\n",options->cName);
      fprintf(stderr,"iVerbose is set to %d\n",VERBALL);
    } else {
      *iVerbose = iTmp;
    }
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
     *iVerbose = atoi(options->cDefault);
}

int iAssignMassUnit(char cTmp[],int iVerbose,char cFile[],char cName[],int iLine) {
  if (memcmp(sLower(cTmp),"g",1) == 0) 
    return 0;
  else if (memcmp(sLower(cTmp),"k",1) == 0)
    return 1;
  else if (memcmp(sLower(cTmp),"s",1) == 0)
    return 2;
  else if (memcmp(sLower(cTmp),"e",1) == 0)
    return 3;
  else if (memcmp(sLower(cTmp),"j",1) == 0)
    return 4;
  else if (memcmp(sLower(cTmp),"n",1) == 0)
    return 5;
  else {
    if (iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are: gm, kg, solar, Earth, Jupiter, Neptune.\n",cName,cTmp);
    LineExit(cFile,iLine);
  }

  /* Whoops! */
  return 1./0;
}

void ReadUnitMass(OPTIONS *options,CONTROL *control,FILES *files,int iNumFile) {
  /* Mass Unit 
     0=gm
     1=kg
     2=solar
     3=Earth
     4=Jup
     5=Neptune */
  int i,lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (iNumFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->iVerbose >= VERBINPUT)
	fprintf(stderr,"WARNING: %s set in %s, all bodies will use this unit.\n",options->cName,files->Infile[iNumFile].cIn);
      control->Units[iNumFile].iMass = iAssignMassUnit(cTmp,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
      UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);

      for (i=1;i<files->iNumInputs;i++) {
	control->Units[i].iMass = control->Units[iNumFile].iMass;
	/* Negative sign for lTmp indicated that the parameter was found in another file */
	/* UpdateFoundOption(&files->Infile[i],options,-lTmp,iNumFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* Assigned in body file */
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
	/* Assigned in primary file */
	if (control->iVerbose >= VERBERR)
	  fprintf(stderr,"ERROR: %s found in primary and body files!\n",options->cName);
	fprintf(stderr,"\t%s, Line: %d\n",files->Infile[0].cIn,options->iLine[0]);
	fprintf(stderr,"\t%s, Line: %d\n",files->Infile[iNumFile].cIn,lTmp);
	exit(EXIT_INPUT);
      } else {
	/* Wasn't assigned in primary */
	control->Units[iNumFile].iMass = iAssignMassUnit(cTmp,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
	UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
      }  
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
	/* No, assign default */
	if (control->iVerbose >= VERBUNITS)
	  fprintf(stderr,"WARNING: %s not set in file %s, defaulting to %s.\n",options->cName,files->Infile[iNumFile].cIn,options->cDefault);
	control->Units[iNumFile].iMass = iAssignMassUnit(options->cDefault,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read */
    }
  }
}

int iAssignUnitTime(char cTmp[],int iVerbose,char cFile[],char cName[], int iLine) {
  if (memcmp(sLower(cTmp),"s",1) == 0) {
    return 0;
  } else if (memcmp(sLower(cTmp),"d",1) == 0) {
    return 1;
  } else if (memcmp(sLower(cTmp),"y",1) == 0) {
    return 2;
  } else if (memcmp(sLower(cTmp),"m",1) == 0) {
    return 3;
  } else if (memcmp(sLower(cTmp),"g",1) == 0) {
    return 4;
  } else {
    if (iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are seconds, days, years, Myr, or Gyr.\n",cName,cTmp);
    LineExit(cFile,iLine);
  }

  /* Whoops! */
  return 1./0;
}


void ReadUnitTime(OPTIONS *options,CONTROL *control,FILES *files,int iNumFile) {
  /* Time Unit 
     0=s
     1=d
     2=yr
     3=Myr
     4=Gyr */
  int i,lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (iNumFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->iVerbose >= VERBINPUT)
	fprintf(stderr,"WARNING: %s set in %s, all bodies will use this unit.\n",options->cName,files->Infile[iNumFile].cIn);
      control->Units[iNumFile].iTime = iAssignUnitTime(cTmp,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
      UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);

      for (i=1;i<files->iNumInputs;i++) {
	control->Units[i].iTime = control->Units[iNumFile].iTime;
	/* UpdateFoundOption(&files->Infile[i],options,-lTmp,iNumFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
	if (control->iVerbose >= VERBERR)
	  fprintf(stderr,"ERROR: %s found in primary and body files!\n",options->cName);
	fprintf(stderr,"\t%s, Line: %d\n",options->cFile[0],options->iLine[0]);
	fprintf(stderr,"\t%s, Line: %d\n",files->Infile[iNumFile].cIn,lTmp);
	exit(EXIT_INPUT);
      } else {
	/* Wasn't assigned in primary */
	control->Units[iNumFile].iTime = iAssignUnitTime(cTmp,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
	UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
      }  
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
	/* No, assign default */
	if (control->iVerbose >= VERBUNITS)
	  fprintf(stderr,"WARNING: %s not set in file %s, defaulting to %s.\n",options->cName,files->Infile[iNumFile].cIn,options->cDefault);
	control->Units[iNumFile].iTime = iAssignUnitTime(options->cDefault,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read */
    }      
  }
}

int iAssignUnitAngle(char cTmp[],int iVerbose,char cFile[],char cName[], int iLine) {
  if (memcmp(sLower(cTmp),"r",1) == 0) {
    return 0;
  } else if (memcmp(sLower(cTmp),"d",1) == 0) {
    return 1;
  } else {
    if (iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are radians or degrees.\n",cName,cTmp);
    LineExit(cFile,iLine);
  }

  /* Whoops! */
  return 1./0;
}
   
void ReadUnitAngle(OPTIONS *options,CONTROL *control,FILES *files,int iNumFile) {
  /* Angle Unit 
     0=rad
     1=deg */
  int i,lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (iNumFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->iVerbose >= VERBINPUT)
	fprintf(stderr,"WARNING: %s set in %s, all bodies will use this unit.\n",options->cName,files->Infile[iNumFile].cIn);
      control->Units[iNumFile].iAngle = iAssignUnitAngle(cTmp,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
      UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);

      for (i=1;i<files->iNumInputs;i++) {
	control->Units[i].iAngle = control->Units[iNumFile].iAngle;
	/* UpdateFoundOption(&files->Infile[i],options,-lTmp,iNumFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
	if (control->iVerbose >= VERBERR)
	  fprintf(stderr,"ERROR: %s found in primary and body files!\n",options->cName);
	fprintf(stderr,"\t%s, Line: %d\n",options->cFile[0],options->iLine[0]);
	fprintf(stderr,"\t%s, Line: %d\n",files->Infile[iNumFile].cIn,lTmp);
	exit(EXIT_INPUT);
      } else {
	/* Wasn't assigned in primary */
	control->Units[iNumFile].iAngle = iAssignUnitAngle(cTmp,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
	UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
      }  
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
	/* No, assign default */
	if (control->iVerbose >= VERBUNITS)
	  fprintf(stderr,"WARNING: %s not set in file %s, defaulting to %s.\n",options->cName,files->Infile[iNumFile].cIn,options->cDefault);
	control->Units[iNumFile].iAngle = iAssignUnitAngle(options->cDefault,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read */
    }
  }
}

int iAssignUnitLength(char cTmp[],int iVerbose,char cFile[],char cName[], int iLine) {
  if (memcmp(sLower(cTmp),"c",1) == 0) {
    return 0;
  } else if (memcmp(sLower(cTmp),"m",1) == 0) {
    return 1;
  } else if (memcmp(sLower(cTmp),"k",1) == 0) {
    return 2;
  } else if (memcmp(sLower(cTmp),"s",1) == 0) {
    return 3;
  } else if (memcmp(sLower(cTmp),"e",1) == 0) {
    return 4;
  } else if (memcmp(sLower(cTmp),"j",1) == 0) {
    return 5;
  } else if (memcmp(sLower(cTmp),"a",1) == 0) {
    return 6;
  } else {
    if (iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are cm, m, km, solar, Earth, Jupiter, AU.\n",cName,cTmp);
    LineExit(cFile,iLine);
  }

  /* Whoops! */
  return 1./0;
}

void ReadUnitLength(OPTIONS *options,CONTROL *control,FILES *files,int iNumFile) {
  /* Length Unit
     0=cm
     1=m
     2=km
     3=R_sun
     4=R_earth
     5=R_Jup
     6=AU */
  int i,lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (iNumFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->iVerbose >= VERBINPUT)
	fprintf(stderr,"WARNING: %s set in %s, all bodies will use this unit.\n",options->cName,files->Infile[iNumFile].cIn);
      control->Units[iNumFile].iLength = iAssignUnitLength(cTmp,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
      UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);

      options->iLine[iNumFile] = lTmp;
      for (i=1;i<files->iNumInputs;i++) {
	control->Units[i].iLength = control->Units[iNumFile].iLength;
	/* UpdateFoundOption(&files->Infile[i],options,lTmp,iNumFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
	if (control->iVerbose >= VERBERR)
	  fprintf(stderr,"ERROR: %s found in primary and body files!\n",options->cName);
	fprintf(stderr,"\t%s, Line: %d\n",options->cFile[0],options->iLine[0]);
	fprintf(stderr,"\t%s, Line: %d\n",files->Infile[iNumFile].cIn,lTmp);
	exit(EXIT_INPUT);
      } else {
	/* Wasn't assigned in primary */
	control->Units[iNumFile].iLength = iAssignUnitLength(cTmp,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
	UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
      }  
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
	/* No, assign default */
	if (control->iVerbose >= VERBUNITS)
	  fprintf(stderr,"WARNING: %s not set in file %s, defaulting to %s.\n",options->cName,files->Infile[iNumFile].cIn,options->cDefault);
	control->Units[iNumFile].iLength = iAssignUnitLength(options->cDefault,control->iVerbose,files->Infile[iNumFile].cIn,options->cName,lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read */
    }
  }
}

void ReadSystemName(OPTIONS *options,CONTROL *control,SYSTEM *system,FILES *files,int iNumFile) {
  /* System Name */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    strcpy(system->cName,cTmp);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  }
}

void ReadBodyFileNames(OPTIONS *options,CONTROL *control,FILES *files) {
  int i,iNumIndices=0,iNumLines=0;
  int *lTmp;
  char saTmp[MAXARRAY][OPTLEN];

  lTmp=malloc(MAXLINES*sizeof(int));

  AddOptionStringArray(files->Infile[0].cIn,options->cName,saTmp,&iNumIndices,&iNumLines,lTmp,control->iVerbose);
  if (lTmp[0] >= 0) {
    if (iNumIndices == 0) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: No files supplied for option %s.\n",options->cName);
      LineExit(files->Infile[0].cIn,lTmp[0]);
    }
    files->iNumInputs=iNumIndices+1;
    control->iNumBodies=iNumIndices;
    for (i=0;i<=iNumIndices;i++)
      strcpy(files->Infile[i+1].cIn,saTmp[i]);
    UpdateFoundOptionMulti(&files->Infile[0],options,lTmp,iNumLines,0);
  } else {
    if (control->iVerbose >= VERBERR) 
      fprintf(stderr,"ERROR: Option %s is required in file %s.\n",options->cName,files->Infile[0].cIn);
    exit(EXIT_INPUT);
  }

  free(lTmp);
}

/*
 *
 * Master subroutine for initial options
 *
 */

void ReadInitialOptions(CONTROL *control,OPTIONS *options,FILES *files,SYSTEM *system) {
  int i;

  /* Initialize primary input file */
  InitializeInput(&files->Infile[0]);

  /* First find input files */
  ReadBodyFileNames(&options[OPT_BODYFILES],control,files);

  /* Is iVerbose set in primary input? */
  ReadVerbose(&options[OPT_VERBOSE],&control->iVerbose,files,0);

  /* Now we can search through files for all options. First we scan the files for Verbosity */
  /* Initialize other input files */
  for (i=1;i<files->iNumInputs;i++) {
    InitializeInput(&files->Infile[i]);
    ReadVerbose(&options[OPT_VERBOSE],&control->iVerbose,files,i);
  }

  /* Now initialize arrays */
  control->Units = malloc(files->iNumInputs*sizeof(UNITS));

  /* Next we must find the units and system name */
  for (i=0;i<files->iNumInputs;i++) {
    ReadUnitMass(&options[OPT_UNITMASS],control,files,i);
    ReadUnitTime(&options[OPT_UNITTIME],control,files,i);
    ReadUnitAngle(&options[OPT_UNITANGLE],control,files,i);
    ReadUnitLength(&options[OPT_UNITLENGTH],control,files,i);
    ReadSystemName(&options[OPT_SYSTEMNAME],control,system,files,i);
  }

  /* XXX Should check this file here */

}

void AssignDefaultDouble(double *dOption,OPTIONS options,int iNumFiles) {
  int i;

  for (i=0;i<iNumFiles;i++) 
    if (options.iLine[i] != -1)
      /* Options already input in other file */
      return;

  /* If made it here, not input already, so assign default */
  *dOption = options.dDefault;
}

void AssignDefaultInt(int *iOption,OPTIONS options,int iNumFiles) {
  int i;

  for (i=0;i<iNumFiles;i++) 
    if (options.iLine[i] != -1)
      /* Options already input in other file */
      return;

  /* If made it here, not input already, so assign default */
  *iOption = atoi(options.cDefault);
}

void AssignDefaultString(char cOption[],OPTIONS options,int iNumFiles) {
  int i;

  for (i=0;i<iNumFiles;i++) 
    if (options.iLine[i] != -1)
      /* Options already input in other file */
      return;

  /* If made it here, not input already, so assign default */
  strcpy(cOption,options.cDefault);
}

int bOptionAlreadyFound(int *iLine,int iNumFiles) {
  int i;
  
  for (i=0;i<iNumFiles;i++) {
    if (iLine[i] >= 0)
      return 1;
  }
  return 0;
}


/*
 *
 * A
 *
 */

/* Age */

void ReadAge(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;
  
  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) {
      system->dAge = dTmp*dNegativeDouble(*options,files->Infile[iNumFile].cIn,control->iVerbose);
    } else 
      system->dAge = dTmp*fdTimeUnit(control->Units[iNumFile].iTime);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&system->dAge,*options,files->iNumInputs);
}

/*
 *
 * B
 *
 */

/* Do Backward Integration? */

void ReadDoBackward(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    control->Integr.bDoBackward = atoi(options->cDefault);
}

/* Output File Name */

void ReadOutFile(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file -- Each body has an output file */
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    strcpy(files->Outfile[iNumFile-1].cOut,cTmp);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      AssignDefaultString(files->Outfile[iNumFile-1].cOut,*options,files->iNumInputs);
}

/* Backward Eta */

void ReadEta(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp <= 0) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);
    }
    control->Integr.dEta = dTmp;
    if (control->Integr.dEta > 1 && control->iVerbose >= VERBALL)
      fprintf(stderr,"WARNING: %s > 1 is not advised (%s:%d).\n",options->cName,files->Infile[iNumFile].cIn,lTmp);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    AssignDefaultDouble(&control->Integr.dEta,*options,files->iNumInputs);
}
  
/* Backward integration output interval */

void ReadOutputTime(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) {
      if (control->iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);
    }
    /* Convert timestep to cgs */
    control->dOutputTime = dTmp*fdTimeUnit(control->Units[iNumFile].iTime);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&control->dOutputTime,*options,files->iNumInputs);
}

/* Backward integration stop time */

void ReadStopTime(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) {
      if (control->iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);
    }
    /* Convert timestep to cgs */
    control->Integr.dStopTime = dTmp*fdTimeUnit(control->Units[iNumFile].iTime);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&control->Integr.dStopTime,*options,files->iNumInputs);
}

/* Integration timestep */

void ReadTimeStep(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);
    }
    /* Convert timestep to cgs */
    control->Integr.dTimeStep = dTmp*fdTimeUnit(control->Units[iNumFile].iTime);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&control->Integr.dTimeStep,*options,files->iNumInputs);
}
  
/* Do variable timestep? */

void ReadVarDt(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->Integr.bVarDt = bTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    AssignDefaultInt(&control->Integr.bVarDt,*options,files->iNumInputs);
}

/* Body Name */

void ReadBodyName(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file -- Each body has an output file */
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    strcpy(body[iNumFile-1].cName,cTmp);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      sprintf(body[iNumFile-1].cName,"%d",iNumFile);
}

/*
 *
 * D
 *
 */

/* Digits */

void ReadDigits(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int iTmp;

  AddOptionInt(files->Infile[iNumFile].cIn,options->cName,&iTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->iDigits = iTmp;
    if (control->iDigits < 0) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be non-negative.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,options->iLine[iNumFile]);
    }
    if (control->iDigits > 16) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be less than 17.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,options->iLine[iNumFile]);
    }
    control->iDigits = iTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultInt(&control->iDigits,*options,files->iNumInputs);
}


/*
 *
 * E
 *
 */

/* Eccentricity */

void ReadEcc(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* Cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,options->iLine[iNumFile],control->iVerbose);
    if (dTmp < 0 || dTmp >= 1) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be in the range [0,1).\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    body[1].dEcc = dTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&body[1].dEcc,*options,files->iNumInputs);
    
}

/*
 *
 * F
 *
 */


/* Forward integration? */

void ReadDoForward(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->Integr.bDoForward = bTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    /* Set to default */
    AssignDefaultInt(&control->Integr.bDoForward,*options,files->iNumInputs);
}

/*
 *
 *   HALT
 *
 */


/* Maximum Eccentricity */

void ReadHaltMaxEcc(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;
  
  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Halt.dMaxEcc < 0 || control->Halt.dMaxEcc > 1) {
      if (control->iVerbose >= VERBERR)
	  fprintf(stderr,"ERROR: %s must be in the range (0,1).\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    control->Halt.dMaxEcc = dTmp;
    control->Halt.bHalt = 1;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&control->Halt.dMaxEcc,*options,files->iNumInputs);
}

/* Merge */

void ReadHaltMerge(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->Halt.bMerge = bTmp;
    if (control->Halt.bMerge)
      control->Halt.bHalt = 1;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else {
    AssignDefaultInt(&control->Halt.bMerge,*options,files->iNumInputs);
    /* Default is to search for merge, so also set Halt.bHalt to 1 */
    control->Halt.bHalt = 1;
  }

}

/* Minimum Eccentricity */
  
void ReadHaltMinEcc(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be in the range (0,1).\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    control->Halt.dMinEcc = dTmp;
    control->Halt.bHalt = 1;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&control->Halt.dMinEcc,*options,files->iNumInputs);
}


/* Need to add ReadHaltDblSync function, since halt.bDblSync is never initialized anywhere! */


/* Minimum obliquity */
  
void ReadHaltMinObl(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file */
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
	if (control->iVerbose >= VERBINPUT) 
	  fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
	if (control->iVerbose >= VERBINPUT) 
	  fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      } 
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    control->Halt.dMinObl[iNumFile-1] = dTmp;
    control->Halt.bHalt = 1;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    if (iNumFile > 0)
      control->Halt.dMinObl[iNumFile-1] = options->dDefault;
}

/* Minimum Semi-Major Axis */

void ReadHaltMinSemi(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp <= 0) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be larger than 0.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    control->Halt.dMinSemi = dTmp*fdLengthUnit(control->Units[iNumFile].iLength);
    control->Halt.bHalt = 1;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    AssignDefaultDouble(&control->Halt.dMinSemi,*options,files->iNumInputs);
}

/* Positive de/dt */

void ReadHaltPosDeDt(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->Halt.bPosDeDt = bTmp;
    control->Halt.bHalt = 1;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultInt(&control->Halt.bPosDeDt,*options,files->iNumInputs);
}


/*
 *
 * I
 *
 */

/* Integration Method */
void ReadIntegrationMethod(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int i,lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (memcmp(sLower(cTmp),"e",1) == 0) 
      control->iOneStep = EULER;
    else if (memcmp(sLower(cTmp),"r",1) == 0) 
      control->iOneStep = RUNGEKUTTA;
    else {
      if (control->iVerbose >= VERBERR) {
	fprintf(stderr,"ERROR: Unknown argument to %s: %s.\n",options->cName,cTmp);
	fprintf(stderr,"Options are Euler.\n");
      }
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    } 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } 
  /* If not input, VerifyIntegration assigns default */
}

/*
 *
 *   LOG
 *
 */

/* Do log file? */
 
void ReadDoLog(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    control->bLog = bTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultInt(&control->bLog,*options,files->iNumInputs);
}
  
/* Log file name */

void ReadLogFile(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int i,lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    strcpy(files->cLog,cTmp);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else {
    /* Assign Default */
    if (iNumFile == files->iNumInputs-1) {
      for (i=0;i<files->iNumInputs;i++) 
	if (options->iLine[i] != -1)
	  /* Was aaigned, return */
	  return;
    }
    /* Wasn't entered, assign default */
    sprintf(files->cLog,"%s.log",system->cName);
  }
}

/*
 *
 *   M
 *
 */

/* Mass */

void ReadMass(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Density and MassRad */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0)
      body[iNumFile-1].dMass = dTmp*dNegativeDouble(*options,files->Infile[iNumFile].cIn,control->iVerbose);
    else
      body[iNumFile-1].dMass = dTmp*fdMassUnit(control->Units[iNumFile].iMass);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    if (iNumFile > 0)
      body[iNumFile-1].dMass = options->dDefault;
}

/* Mass-Radius relationship */

void ReadMassRad(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Density and Mass */
  int lTmp=-1;
  char cTmp[OPTLEN];
    
  AddOptionString(files->Infile[iNumFile].cIn,options->cName,cTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (memcmp(sLower(cTmp),"r",1) == 0) {
      /* Reid & Hawley 2000 */	
      control->iMassRad[iNumFile]=1;
    } else if (memcmp(sLower(cTmp),"g",1) == 0) {
      /* Gorda and Svenchnikov 1999 */
      control->iMassRad[iNumFile]=2;
    } else if (memcmp(sLower(cTmp),"b",1) == 0) {
      /* Bayless & Orosz 2006 */
      control->iMassRad[iNumFile]=3;
    } else if (memcmp(sLower(cTmp),"s",1) == 0) {
      /* Sotin et al 2007 */
      control->iMassRad[iNumFile]=4;
    } else {
      if (control->iVerbose >= VERBERR) {
	fprintf(stderr,"ERROR: Unknown argument to %s: %s.\n",options->cName,cTmp);
	fprintf(stderr,"Options are GS99 (Gorda & Svechnikov 1999), BO06 (Bayless & Orosz 2006), Sotin07 (Sotin et al. 2007), or RH00 (Reid & Hawley 2000).\n");
      }
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    } 
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else {
    /* This one is weird, since the default is "none", but if this option
       is not set, then we need to set this variable to 0 for logging
       purposes. */
    control->iMassRad[iNumFile] = 0;
  }
}

/* Mean Motion */

void ReadMeanMotion(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp <= 0) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    body[1].dMeanMotion = dTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&body[1].dMeanMotion,*options,files->iNumInputs);
}

/* Minimum Value */

void ReadMinValue(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    if (dTmp < 0) {
      if (control->iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be larger than 0.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    control->Integr.dMinValue = dTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&control->Integr.dMinValue,*options,files->iNumInputs);
}  

/*
 *
 * O
 *
 */

/* Obliquity */

void ReadObliquity(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->Units[iNumFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
	if (control->iVerbose >= VERBERR)
	    fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
	LineExit(files->Infile[iNumFile].cIn,lTmp);	
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iNumFile-1].dObliquity = dTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    if (iNumFile > 0)
      body[iNumFile-1].dObliquity = options->dDefault;
}

/*
 * Output Order
 */


void ReadOutputOrder(OPTIONS *options,OUTPUT *output,FILES *files,int iNumFile,int iVerbose) {
  int i,j,count,iLen,iNumIndices=0,bNeg[MAXARRAY],ok=0;
  int k,iOut,lTmp=-1;
  char saTmp[MAXARRAY][OPTLEN],cTmp[OPTLEN],cOption[MAXARRAY][OPTLEN],cOut[OPTLEN];
  int iLen1,iLen2;

  AddOptionStringArray(files->Infile[iNumFile].cIn,options[OPT_OUTPUTORDER].cName,saTmp,&iNumIndices,&files->Infile[iNumFile].iNumLines,&lTmp,iVerbose);

  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options[OPT_OUTPUTORDER].cName,files->Infile[iNumFile].cIn,lTmp,iVerbose);

    /* First remove and record negative signs */
    for (i=0;i<iNumIndices;i++) {
      if (saTmp[i][0] == 45) {
	/* Option is negative */
	bNeg[i] = 1;
	/* Now remove negative sign */
	for (j=0;j<strlen(saTmp[i]);j++) 
	  saTmp[i][j] = saTmp[i][j+1];
	saTmp[i][strlen(saTmp[i])] = 0;
      } else
	bNeg[i] = 0;
    }
    
    /* Check for ambiguity */
    for (i=0;i<iNumIndices;i++) {
      count=0; /* Number of possibilities */
      for (j=0;j<OPTLEN;j++)
	cTmp[j]=0;
      strcpy(cTmp,saTmp[i]);
      for (j=0;j<NUMOUT;j++) {
	for (k=0;k<OPTLEN;k++)
	   cOut[k]=0;
	strcpy(cOut,output[j].cName);
	iLen1=strlen(cOut);
	iLen2=strlen(cTmp);
	/* Check for perfect match */
	if ( (iLen1 == iLen2) && (memcmp(sLower(cTmp),sLower(cOut),strlen(cOut)) == 0)) {
	  /* Output option found! */
	  strcpy(cOption[count],output[j].cName);
	  count = 1;
	  iOut = j;
	  j = NUMOUT; /* Poor man's break! */
	} else {
	  if (iLen1 < iLen2)
	    iLen=strlen(cOut);
	  else
	    iLen=strlen(cTmp);
	  
	  if (memcmp(sLower(cTmp),sLower(cOut),iLen) == 0 && iLen1 > iLen2) {
	    /* Output option found! */
	    strcpy(cOption[count],output[j].cName);
	    count++;
	    iOut = j;
	  }
	}
      }
      
      if (count > 1) {
	/* More than one possibility */
	if (iVerbose >= VERBERR) {
	  fprintf(stderr,"ERROR: Output option \"%s\" is ambiguous. Options are ",saTmp[i]);
	  for (j=0;j<count;j++) {
	    fprintf(stderr,"%s",cOption[j]);
	    if (j < count-1)
	      fprintf(stderr,", ");
	  }
	  fprintf(stderr,".\n");
	}
	LineExit(files->Infile[iNumFile].cIn,lTmp);
      }
      
      if (!count) {
	/* Option not found */
	if (iVerbose >= VERBERR) 
	  fprintf(stderr,"ERROR: Unknown output option \"%s\".\n",saTmp[i]);
	LineExit(files->Infile[iNumFile].cIn,lTmp);
      }
      
      if (count == 1) {
	/* Unique option */
	/* If negative, is that allowed? */
	if (bNeg[i] && !output[iOut].bNeg) {
	  /* No */
	  if (iVerbose >= VERBERR) {
	    fprintf(stderr,"ERROR: Output option %s ",saTmp[i]);
	    if (strlen(saTmp[i]) < strlen(output[iOut].cName)) 
	      fprintf(stderr,"(= %s) ",output[iOut].cName);
	    fprintf(stderr,"cannot be negative.\n");
	  }
	  LineExit(files->Infile[iNumFile].cIn,lTmp);
	} else if (bNeg[i] && output[iOut].bNeg) {
	  /* Yes */
	  output[iOut].bDoNeg[iNumFile-1] = 1;
	}
	files->Outfile[iNumFile-1].caCol[i][0]='\0';
	strcpy(files->Outfile[iNumFile-1].caCol[i],output[iOut].cName);
      }
    }
    files->Outfile[iNumFile-1].iNumCols = iNumIndices;
    UpdateFoundOption(&files->Infile[iNumFile],&options[OPT_OUTPUTORDER],lTmp,iNumFile);
  }

}

void ReadOverwrite(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iNumFile].cIn,options->cName,&bTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (control->bOverwrite > -1) {
      if (control->iVerbose >= VERBERR)
        fprintf(stderr,"WARNING: -f set at command line, and %s set (%s:%d). Overwrite authorized.\n",options->cName,files->Infile[iNumFile].cIn,lTmp);
    } else {
      control->bOverwrite = bTmp;
      UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
    }
  } else
    if (control->bOverwrite == -1)
      AssignDefaultInt(&control->bOverwrite,*options,files->iNumInputs);
}



/*
 *
 * P
 *
 */


/* Orbital Period */

void ReadOrbPeriod(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify with Semi and MeanMotion */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) 
      body[1].dOrbPeriod = dTmp*dNegativeDouble(*options,files->Infile[iNumFile].cIn,control->iVerbose);
    else
      body[1].dOrbPeriod = dTmp*fdTimeUnit(control->Units[iNumFile].iTime);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&body[1].dOrbPeriod,*options,files->iNumInputs);
}

/*
 *
 * R
 *
 */

/* Radius */

void ReadRadius(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Mass and MassRad */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) 
      body[iNumFile-1].dRadius = dTmp*dNegativeDouble(*options,files->Infile[iNumFile].cIn,control->iVerbose);
    else
      body[iNumFile-1].dRadius = dTmp*fdLengthUnit(control->Units[iNumFile].iLength);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    if (iNumFile > 0)
      body[iNumFile-1].dRadius = options->dDefault;
}

/* Radius of Gyration */

void ReadRadiusGyration(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) {
      if (control->iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than zero.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    body[iNumFile-1].dRadGyra = dTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    if (iNumFile > 0)
      body[iNumFile-1].dRadGyra = options->dDefault;
}

/* Rotation Period */

void ReadRotPeriod(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify in conjunction with RotRate, RotVel and ForceEqSpin */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) 
      body[iNumFile-1].dRotPer = dTmp*dNegativeDouble(*options,files->Infile[iNumFile].cIn,control->iVerbose);
    else
      if (iNumFile > 0)
	body[iNumFile-1].dRotPer = dTmp*fdTimeUnit(control->Units[iNumFile].iTime);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  }
}

/* Rotational Frequency */

void ReadRotRate(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify in conjunction with RotPer, RotVel and ForceEqSpin */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be non-negative.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    body[iNumFile-1].dRotRate = dTmp/fdTimeUnit(control->Units[iNumFile].iTime);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  }
}

/* Rotational Velocity */

void ReadRotVel(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify in conjunction with RotPer, RotRate and ForceEqSpin */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iNumFile,options->cName,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0)
      body[iNumFile-1].dRotVel = dTmp*dNegativeDouble(*options,files->Infile[iNumFile].cIn,control->iVerbose);
    else 
      body[iNumFile-1].dRotVel = dTmp*fdLengthUnit(control->Units[iNumFile].iLength)/fdTimeUnit(control->Units[iNumFile].iTime);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  }
}

/*
 *
 * S
 *
 */

/* Scientific Notation */

void ReadSciNot(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int iTmp;

  AddOptionInt(files->Infile[iNumFile].cIn,options->cName,&iTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (iTmp < 0) {
      if (control->iVerbose >= VERBERR)
	fprintf(stderr,"ERROR: %s must be non-negative.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    if (iTmp > 16) {
      if (control->iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be less than 16.\n",options->cName);
      LineExit(files->Infile[iNumFile].cIn,lTmp);	
    }
    control->iSciNot = iTmp;
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else 
    AssignDefaultInt(&control->iSciNot,*options,files->iNumInputs);
}

/* Semi-Major Axis */

void ReadSemiMajorAxis(CONTROL *control,BODY *body,SYSTEM *system,OPTIONS *options,FILES *files,int iNumFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;
    
  AddOptionDouble(files->Infile[iNumFile].cIn,options->cName,&dTmp,&lTmp,control->iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(*options,*files,files->Infile[iNumFile].cIn,lTmp,control->iVerbose);
    if (dTmp < 0) 
      body[1].dSemi = dTmp*dNegativeDouble(*options,files->Infile[iNumFile].cIn,control->iVerbose);
    else 
      body[1].dSemi = dTmp*fdLengthUnit(control->Units[iNumFile].iLength);
    UpdateFoundOption(&files->Infile[iNumFile],options,lTmp,iNumFile);
  } else
    AssignDefaultDouble(&body[1].dSemi,*options,files->iNumInputs);
}


void ReadOptionsGeneral(CONTROL *control,OPTIONS *options,BODY *body,SYSTEM *system,FILES *files,fnReadOption fnRead[]) {
  /* Now get all other options, if not in VPLANET mode */
  int iOpt,iFile;

  /* For each option, search the file for its occurence */
  /* is iLine necessary? */

  for (iFile=0;iFile<files->iNumInputs;iFile++) {
    /* Start at 100 because 0-99 are reserved for initial options */
    for (iOpt=100;iOpt<NUMOPT;iOpt++) 
      /* OutputOrder is special */
      if (options[iOpt].iType != -1 && iOpt != OPT_OUTPUTORDER) {
	fnRead[iOpt](control,body,system,&options[iOpt],files,iFile);
      }
  }
}

void InitializeControl(CONTROL *control) {

  control->iMassRad = malloc(control->iNumBodies*sizeof(int));

  /* This needs to be vectorized VPL */
  InitializeControlEqtide(control);

}
/*
 *
 * Master Read Options Subroutine
 *
 */

void ReadOptions(OPTIONS *options,CONTROL *control,VPLANET *vplanet,BODY **body,UPDATE **update,FILES *files, SYSTEM *system,OUTPUT *output,fnReadOption fnRead[]) {
  int iFile;

  /* Read options for files, units, verbosity, and system name. */
  ReadInitialOptions(control,options,files,system);

  /* Now that we know how many bodies there are, initialize more features */
  *body = malloc(control->iNumBodies*sizeof(BODY));
  *update = malloc(control->iNumBodies*sizeof(UPDATE));

  /* This needs to be vectorized VPL */
  InitializeControl(control);
  InitializeHalt(&control->Halt,vplanet,files->iNumInputs);


  /* Now read in remaining options */
  ReadOptionsGeneral(control,options,*body,system,files,fnRead);

  /* This needs to be vectorized VPL */
  ReadOptionsEqtide(control,options,*body,system,files,fnRead);

  /* Read in utput order -- merge into ReadGeneralOptions? */
  for (iFile=1;iFile<files->iNumInputs;iFile++)
    ReadOutputOrder(options,output,files,iFile,control->iVerbose);

  /* Any unrecognized options? */
  Unrecognized(*files);
}

/*
 *
 * Initialize Options
 *
 */

void InitializeOptionsGeneral(OPTIONS *options,fnReadOption fnRead[]) {

    /* Do System Name first as it is used by other options. */

    sprintf(options[OPT_SYSTEMNAME].cName,"sSystemName");
    sprintf(options[OPT_SYSTEMNAME].cDescr,"System Name");
    sprintf(options[OPT_SYSTEMNAME].cDefault,"None - must be supplied");
    options[OPT_SYSTEMNAME].iType = 3;

    /*
     *
     *   A
     *
     */

    sprintf(options[OPT_AGE].cName,"dAge");
    sprintf(options[OPT_AGE].cDescr,"System Age");
    sprintf(options[OPT_AGE].cDefault,"0");
    options[OPT_AGE].iType = 2;
    options[OPT_AGE].dNeg = 1e9*YEARSEC;
    sprintf(options[OPT_AGE].cNeg,"Gyr");
    fnRead[OPT_AGE] = &ReadAge;

    /*
     *
     *   B
     *
     */
    
    sprintf(options[OPT_BACK].cName,"bDoBackward");
    sprintf(options[OPT_BACK].cDescr,"Do Backward Integration?");
    sprintf(options[OPT_BACK].cDefault,"No");
    options[OPT_BACK].iType = 0;
    fnRead[OPT_BACK] = &ReadDoBackward;

    sprintf(options[OPT_OUTFILE].cName,"sOutFile");
    sprintf(options[OPT_OUTFILE].cDescr,"Name of Output File");
    sprintf(options[OPT_OUTFILE].cDefault,"cSystemName.backward");
    options[OPT_OUTFILE].iType = 3;
    options[OPT_OUTFILE].iMultiFile = 1;
    fnRead[OPT_OUTFILE] = &ReadOutFile;

    sprintf(options[OPT_ETA].cName,"dEta");
    sprintf(options[OPT_ETA].cDescr,"Variable Time Step Coefficient");
    sprintf(options[OPT_ETA].cDefault,"1");
    options[OPT_ETA].dDefault = 1;
    options[OPT_ETA].iType = 2;
    fnRead[OPT_ETA] = &ReadEta;

    sprintf(options[OPT_OUTPUTTIME].cName,"dOutputTime");
    sprintf(options[OPT_OUTPUTTIME].cDescr,"Output Interval");
    sprintf(options[OPT_OUTPUTTIME].cDefault,"1 year");
    options[OPT_OUTPUTTIME].dDefault = YEARSEC;
    options[OPT_OUTPUTTIME].iType = 2;
    options[OPT_OUTPUTTIME].dNeg = YEARSEC;
    sprintf(options[OPT_OUTPUTTIME].cNeg,"Years");
    fnRead[OPT_OUTPUTTIME] = &ReadOutputTime;

    sprintf(options[OPT_STOPTIME].cName,"dStopTime");
    sprintf(options[OPT_STOPTIME].cDescr,"Integration Stop Time");
    sprintf(options[OPT_STOPTIME].cDefault,"10 years");
    options[OPT_STOPTIME].dDefault = 10*YEARSEC;
    options[OPT_STOPTIME].dNeg = YEARSEC;
    sprintf(options[OPT_STOPTIME].cNeg,"Years");
    options[OPT_STOPTIME].iType = 2;
    fnRead[OPT_STOPTIME] = &ReadStopTime;

    sprintf(options[OPT_TIMESTEP].cName,"dTimeStep");
    sprintf(options[OPT_TIMESTEP].cDescr,"Integration Timestep");
    sprintf(options[OPT_TIMESTEP].cDefault,"1 year");
    options[OPT_TIMESTEP].dDefault = YEARSEC;
    options[OPT_TIMESTEP].iType = 2;
    options[OPT_TIMESTEP].dNeg = YEARSEC;
    sprintf(options[OPT_TIMESTEP].cNeg,"Years");
    fnRead[OPT_TIMESTEP] = &ReadTimeStep;

    sprintf(options[OPT_VARDT].cName,"bVarDt");
    sprintf(options[OPT_VARDT].cDescr,"Use variable timestep?");
    sprintf(options[OPT_VARDT].cDefault,"None");
    options[OPT_VARDT].iType = 0;
    fnRead[OPT_VARDT] = &ReadVarDt;

    sprintf(options[OPT_BODYFILES].cName,"saBodyFiles");
    sprintf(options[OPT_BODYFILES].cDescr,"Input files for each body");
    sprintf(options[OPT_BODYFILES].cDefault,"None");
    options[OPT_BODYFILES].iType = 13;

    sprintf(options[OPT_BODYNAME].cName,"sName");
    sprintf(options[OPT_BODYNAME].cDescr,"Body's Name");
    sprintf(options[OPT_BODYNAME].cDefault,"Integer of Input Order, i.e. 1");
    options[OPT_BODYNAME].iType = 3;
    fnRead[OPT_BODYNAME] = &ReadBodyName;


    /*
     *
     *   E
     *
     */

    /*
     *   FORWARD
     *
     */

    sprintf(options[OPT_FORW].cName,"bDoForward");
    sprintf(options[OPT_FORW].cDescr,"Do Forward Integration?");
    sprintf(options[OPT_FORW].cDefault,"0");
    options[OPT_FORW].iType = 0;
    fnRead[OPT_FORW] = &ReadDoForward;

    /* 
     *
     *   HALT
     *
     */


    sprintf(options[OPT_HALTMAXECC].cName,"dHaltMaxEcc");
    sprintf(options[OPT_HALTMAXECC].cDescr,"Maximum Eccentricity Value that Halts Integration");
    sprintf(options[OPT_HALTMAXECC].cDefault,"1");
    options[OPT_HALTMAXECC].dDefault = 1;
    options[OPT_HALTMAXECC].iType = 2;
    fnRead[OPT_HALTMAXECC] = &ReadHaltMaxEcc;

    sprintf(options[OPT_HALTMERGE].cName,"bHaltMerge");
    sprintf(options[OPT_HALTMERGE].cDescr,"Halt at Merge");
    sprintf(options[OPT_HALTMERGE].cDefault,"1");
    options[OPT_HALTMERGE].iType = 0;
    fnRead[OPT_HALTMERGE] = &ReadHaltMerge;

    sprintf(options[OPT_HALTMINECC].cName,"dHaltMinEcc");
    sprintf(options[OPT_HALTMINECC].cDescr,"Minimum Eccentricity Value that Halts Integration");
    sprintf(options[OPT_HALTMINECC].cDefault,"0");
    options[OPT_HALTMINECC].dDefault = 0;
    options[OPT_HALTMINECC].iType = 2;
    fnRead[OPT_HALTMINECC] = &ReadHaltMinEcc;

    sprintf(options[OPT_HALTMINOBL].cName,"dHaltMinObl");
    sprintf(options[OPT_HALTMINOBL].cDescr,"Minimum Obliquity Value that Halts Integration");
    sprintf(options[OPT_HALTMINOBL].cDefault,"-1 degrees");
    options[OPT_HALTMINOBL].dDefault = -DEGRAD;    
    options[OPT_HALTMINOBL].iType = 2;
    options[OPT_HALTMINOBL].iMultiFile = 1;
    fnRead[OPT_HALTMINOBL] = &ReadHaltMinObl;

    sprintf(options[OPT_HALTMINSEMI].cName,"dHaltMinSemi");
    sprintf(options[OPT_HALTMINSEMI].cDescr,"Minimum Semi-Major Axis Value that Halts Integration");
    sprintf(options[OPT_HALTMINSEMI].cDefault,"0");
    options[OPT_HALTMINSEMI].dDefault = 0;
    options[OPT_HALTMINSEMI].iType = 2;
    options[OPT_HALTMINSEMI].dNeg = AUCM;
    sprintf(options[OPT_HALTMINSEMI].cNeg,"AU");
    fnRead[OPT_HALTMINSEMI] = &ReadHaltMinSemi;

    sprintf(options[OPT_HALTPOSDEDT].cName,"bHaltPosDeDt");
    sprintf(options[OPT_HALTPOSDEDT].cDescr,"Halt if de/dt > 0?");
    sprintf(options[OPT_HALTPOSDEDT].cDefault,"0");
    options[OPT_HALTPOSDEDT].iType= 0;
    fnRead[OPT_HALTPOSDEDT] = &ReadHaltPosDeDt;


    /*
     *
     *   I
     *
     */

    sprintf(options[OPT_INTEGRATIONMETHOD].cName,"sIntegrationMethod");
    sprintf(options[OPT_INTEGRATIONMETHOD].cDescr,"Integration Method: Euler, Runge-Kutta4 (Default = Runge-Kutta4)");
    sprintf(options[OPT_INTEGRATIONMETHOD].cDefault,"Runge-Kutta4");
    options[OPT_INTEGRATIONMETHOD].iType=4;
    fnRead[OPT_INTEGRATIONMETHOD] = &ReadIntegrationMethod;

    /*
     *
     *   L
     *
     */

    sprintf(options[OPT_LOG].cName,"bDoLog");
    sprintf(options[OPT_LOG].cDescr,"Write Log File?");
    sprintf(options[OPT_LOG].cDefault,"0");
    options[OPT_LOG].iType = 0;
    fnRead[OPT_LOG] = &ReadDoLog;

    sprintf(options[OPT_LOGFILE].cName,"sLogFile");
    sprintf(options[OPT_LOGFILE].cDescr,"Log File Name");
    sprintf(options[OPT_LOGFILE].cDefault,"tide.log");
    options[OPT_LOGFILE].iType = 3;
    fnRead[OPT_LOGFILE] = &ReadLogFile;

    /*
     *
     *   M
     *
     */

    sprintf(options[OPT_MINVALUE].cName,"dMinValue");
    sprintf(options[OPT_MINVALUE].cDescr,"Minimum Non-Zero Value of Eccentricity and Obliquities");
    sprintf(options[OPT_MINVALUE].cDefault,"0");
    options[OPT_MINVALUE].dDefault = 0;
    options[OPT_MINVALUE].iType = 2;
    fnRead[OPT_MINVALUE] = &ReadMinValue;

    /*
     *
     *   N
     *
     */

    /*
     *
     *   O
     *
     */

    sprintf(options[OPT_OUTDIGITS].cName,"iDigits");
    sprintf(options[OPT_OUTDIGITS].cDescr,"Number of Digits After Decimal Point");
    sprintf(options[OPT_OUTDIGITS].cDefault,"4");
    options[OPT_OUTDIGITS].iType = 1;
    fnRead[OPT_OUTDIGITS] = &ReadDigits;

    sprintf(options[OPT_OUTPUTORDER].cName,"saOutputOrder");
    sprintf(options[OPT_OUTPUTORDER].cDescr,"Output Parameter(s)");
    sprintf(options[OPT_OUTPUTORDER].cDefault,"None");
    options[OPT_OUTPUTORDER].iType = 14;
    options[OPT_OUTPUTORDER].iMultiFile = 1;


    sprintf(options[OPT_OUTSCINOT].cName,"iSciNot");
    sprintf(options[OPT_OUTSCINOT].cDescr,"Logarithm to Change from Standard to Scientific Notation");
    sprintf(options[OPT_OUTSCINOT].cDefault,"4");
    options[OPT_OUTSCINOT].iType = 1;
    fnRead[OPT_OUTSCINOT] = &ReadSciNot;

    sprintf(options[OPT_OVERWRITE].cName,"bOverwrite");
    sprintf(options[OPT_OVERWRITE].cDescr,"Permit file overwrite?");
    sprintf(options[OPT_OVERWRITE].cDefault,"0");
    options[OPT_OVERWRITE].iType = 0;
    fnRead[OPT_OVERWRITE] = &ReadOverwrite;

    /*
     *
     *   ORBIT
     *
     */

    sprintf(options[OPT_ORBECC].cName,"dEcc");
    sprintf(options[OPT_ORBECC].cDescr,"Orbital Eccentricity");
    sprintf(options[OPT_ORBECC].cDefault,"0");
    options[OPT_ORBECC].dDefault = 0;
    options[OPT_ORBECC].iType = 2;
    fnRead[OPT_ORBECC] = &ReadEcc;

    sprintf(options[OPT_ORBMEANMOTION].cName,"dMeanMotion");
    sprintf(options[OPT_ORBMEANMOTION].cDescr,"Orbital Mean Motion");
    sprintf(options[OPT_ORBMEANMOTION].cDefault,"1 /yr");
    options[OPT_ORBMEANMOTION].dDefault = 1./YEARSEC;    
    options[OPT_ORBMEANMOTION].iType = 2;
    options[OPT_ORBMEANMOTION].dNeg = 1./YEARSEC;
    sprintf(options[OPT_ORBMEANMOTION].cNeg,"/Year");
    fnRead[OPT_ORBMEANMOTION] = &ReadMeanMotion;

    sprintf(options[OPT_ORBPER].cName,"dOrbPeriod");
    sprintf(options[OPT_ORBPER].cDescr,"Orbital Period");
    sprintf(options[OPT_ORBPER].cDefault,"1 year");
    options[OPT_ORBPER].dDefault = YEARSEC;
    options[OPT_ORBPER].iType = 2;
    options[OPT_ORBPER].dNeg = DAYSEC;
    sprintf(options[OPT_ORBPER].cNeg,"Days");
    fnRead[OPT_ORBPER] = &ReadOrbPeriod;

    sprintf(options[OPT_ORBSEMI].cName,"dSemi");
    sprintf(options[OPT_ORBSEMI].cDescr,"Semi-Major Axis");
    sprintf(options[OPT_ORBSEMI].cDefault,"1 AU");
    options[OPT_ORBSEMI].dDefault = AUCM;
    options[OPT_ORBSEMI].iType = 2;
    options[OPT_ORBSEMI].dNeg = AUCM;
    sprintf(options[OPT_ORBSEMI].cNeg,"AU");
    fnRead[OPT_ORBSEMI] = &ReadSemiMajorAxis;

    /*
     *
     *   BODY PROPERTIES
     *
     */



    sprintf(options[OPT_MASS].cName,"dMass");
    sprintf(options[OPT_MASS].cDescr,"Mass");
    sprintf(options[OPT_MASS].cDefault,"1 Earth Mass");
    options[OPT_MASS].dDefault = MEARTH;
    options[OPT_MASS].iType = 2;
    options[OPT_MASS].iMultiFile = 1;
    options[OPT_MASS].dNeg = MEARTH;
    sprintf(options[OPT_MASS].cNeg,"Earth masses");
    fnRead[OPT_MASS] = &ReadMass;

    sprintf(options[OPT_MASSRAD].cName,"sMassRad");
    sprintf(options[OPT_MASSRAD].cDescr,"Mass-Radius Relationship for Central Body: GS99 RH00 BO06 Sotin07 ");
    sprintf(options[OPT_MASSRAD].cDefault,"None");
    options[OPT_MASSRAD].iType = 3;
    options[OPT_MASSRAD].iMultiFile = 1;
    fnRead[OPT_MASSRAD] = &ReadMassRad;


    sprintf(options[OPT_OBL].cName,"dObliquity");
    sprintf(options[OPT_OBL].cDescr,"Obliquity");
    sprintf(options[OPT_OBL].cDefault,"0");
    options[OPT_OBL].dDefault = 0;
    options[OPT_OBL].iType = 2;
    options[OPT_OBL].iMultiFile = 1;
    fnRead[OPT_OBL] = &ReadObliquity;


    sprintf(options[OPT_RADIUS].cName,"dRadius");
    sprintf(options[OPT_RADIUS].cDescr,"Radius");
    sprintf(options[OPT_RADIUS].cDefault,"1 Earth Radius");
    options[OPT_RADIUS].dDefault = REARTH;
    options[OPT_RADIUS].iType = 2;
    options[OPT_RADIUS].iMultiFile = 1;
    options[OPT_RADIUS].dNeg = REARTH;
    sprintf(options[OPT_RADIUS].cNeg,"Earth radii");
    fnRead[OPT_RADIUS] = &ReadRadius;

    sprintf(options[OPT_RG].cName,"dRadGyra");
    sprintf(options[OPT_RG].cDescr,"Radius of Gyration");
    sprintf(options[OPT_RG].cDefault,"0.5");
    options[OPT_RG].dDefault = 0.5;
    options[OPT_RG].iType = 2;
    options[OPT_RG].iMultiFile = 1;
    fnRead[OPT_RG] = &ReadRadiusGyration;

    sprintf(options[OPT_ROTPER].cName,"dRotPeriod");
    sprintf(options[OPT_ROTPER].cDescr,"Rotation Period");
    sprintf(options[OPT_ROTPER].cDefault,"1 Day");
    options[OPT_ROTPER].dDefault = DAYSEC;
    options[OPT_ROTPER].iType = 2;
    options[OPT_ROTPER].iMultiFile = 1;
    options[OPT_ROTPER].dNeg = DAYSEC;
    sprintf(options[OPT_ROTPER].cNeg,"Days");
    fnRead[OPT_ROTPER] = &ReadRotPeriod;

    sprintf(options[OPT_ROTRATE].cName,"dRotRate");
    sprintf(options[OPT_ROTRATE].cDescr,"Rotational Angular Frequency");
    sprintf(options[OPT_ROTRATE].cDefault,"1 /day");
    options[OPT_ROTRATE].dDefault = 1./DAYSEC;
    options[OPT_ROTRATE].iType = 2;
    options[OPT_ROTRATE].iMultiFile = 1;
    options[OPT_ROTRATE].dNeg = 1./DAYSEC;
    sprintf(options[OPT_ROTRATE].cNeg,"/Day");
    fnRead[OPT_ROTRATE] = &ReadRotRate;
    


    sprintf(options[OPT_ROTVEL].cName,"dRotVel");
    sprintf(options[OPT_ROTVEL].cDescr,"Rotational Velocity");
    sprintf(options[OPT_ROTVEL].cDefault,"0");
    options[OPT_ROTVEL].dDefault = 0;
    options[OPT_ROTVEL].iType = 2;
    options[OPT_ROTVEL].iMultiFile = 1;
    options[OPT_ROTVEL].dNeg = 1e5;
    sprintf(options[OPT_ROTVEL].cNeg,"km/s");
    fnRead[OPT_ROTVEL] = &ReadRotVel;

    /*
     *
     *   UNITS
     *
     */

    sprintf(options[OPT_UNITANGLE].cName,"sUnitAngle");
    sprintf(options[OPT_UNITANGLE].cDescr,"Angle Units: Degrees Radians");
    sprintf(options[OPT_UNITANGLE].cDefault,"Radians");
    options[OPT_UNITANGLE].iType = 3;

    sprintf(options[OPT_UNITLENGTH].cName,"sUnitLength");
    sprintf(options[OPT_UNITLENGTH].cDescr,"Length Units: cm m km Solar Earth Jupiter AU");
    sprintf(options[OPT_UNITLENGTH].cDefault,"cm");
    options[OPT_UNITLENGTH].iType = 3;

    sprintf(options[OPT_UNITMASS].cName,"sUnitMass");
    sprintf(options[OPT_UNITMASS].cDescr,"Mass Units: Grams Kilograms Solar Earth Jupiter Saturn");
    sprintf(options[OPT_UNITMASS].cDefault,"grams");
    options[OPT_UNITMASS].iType = 3;

    sprintf(options[OPT_UNITTIME].cName,"sUnitTime");
    sprintf(options[OPT_UNITTIME].cDescr,"Time Units: Seconds, Days Years Myr Gyr");
    sprintf(options[OPT_UNITTIME].cDefault,"Seconds");
    options[OPT_UNITTIME].iType = 3;

    /*
     *
     *   V
     *
     */

    sprintf(options[OPT_VERBOSE].cName,"iVerbose");
    sprintf(options[OPT_VERBOSE].cDescr,"Verbosity Level: 1-5");
    sprintf(options[OPT_VERBOSE].cDefault,"3");
    options[OPT_VERBOSE].iType = 1;

}

void InitializeOptions(OPTIONS *options,fnReadOption fnRead[]) {
  int iOpt,iFile;

  /* Initialize all parameters describing the option's location */
  for (iOpt=0;iOpt<OPTEND;iOpt++) {
    sprintf(options[iOpt].cName,"null");
    options[iOpt].iLine = malloc(MAXFILES*sizeof(int));
    options[iOpt].iMultiFile=0;
    options[iOpt].iMultiIn=0;
    options[iOpt].iType = -1;
    
    for (iFile=0;iFile<MAXFILES;iFile++) {
      options[iOpt].iLine[iFile] = -1;
      sprintf(options[iOpt].cFile[iFile],"null");
    }
  }

  InitializeOptionsGeneral(options,fnRead);

  /* Needs to be vectorized VPL */
  InitializeOptionsEqtide(options,fnRead);
}


