
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
  int iPos;

  for (iPos=0;iPos<iLen;iPos++) {
    if (!isspace(cLine[iPos])) {
      if (cLine[iPos] == 35) // # is ASCII code 35
      return 1;
    } else
      return 0;
  }
  return 0;
}

void GetLine(char cFile[],char cOption[],char cLine[],int *iLine,int iVerbose) {
  int iLen,bDone=0,iLineTmp=0;
  char cWord[OPTLEN],cTmp[LINE];
  FILE *fp;

  iLen=strlen(cOption);

  fp=fopen(cFile,"r");
  memset(cLine,'\0',LINE);
  memset(cTmp,'\0',LINE);
  memset(cWord,'\0',OPTLEN);

  while(fgets(cTmp,LINE,fp) != NULL) {
    if (!CheckComment(cTmp,iLen)) {
      sscanf(cTmp,"%s",cWord);
      // XXX Add check for comments embedded in the option here
      if (memcmp(cWord,cOption,iLen+1) == 0) {
        /* Parameter Found! */
        if (bDone) {
            if (iVerbose > VERBINPUT)
            fprintf(stderr,"Multiple occurences of parameter %s found.\n",cOption);
          fprintf(stderr,"\t%s, lines: %d and %d\n",cFile,(*iLine+1),iLineTmp+1);
          exit(1);
        }
        strcpy(cLine,cTmp);
        *iLine=iLineTmp;
        bDone=1;
      }
    }
    iLineTmp++;
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
  int iPos,iLineTmp,ok=1;

  fp=fopen(cFile,"r");
  *iLine=0;

  for (iLineTmp=0;iLineTmp<iStart;iLineTmp++) {
    fgets(cLine,LINE,fp);
    (*iLine)++;
  }

  /* If EOF, return */
  if (fgets(cLine,LINE,fp) == NULL) {
    sprintf(cLine,"null");
    fclose(fp);
    return;
  }

  /* Now check for blank line, comment (# = 35), continue ($ = 36)
     or blank line (line feed = 10). */

  for (iPos=0;iPos<LINE;iPos++) {
    if (cLine[iPos] == 36 || cLine[iPos] == 35 || cLine[iPos] == 10) {
      /* First character is a $, # or \n: continue */
      GetNextValidLine(cFile,iStart+1,cLine,iLine);
      fclose(fp);
      return;
    }
    if (!isspace(cLine[iPos])) {
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
  int iPos;

  for (iPos=0;iPos<strlen(cLine);iPos++)
    if (!isspace(cLine[iPos]))
      return iPos;

  /* Shouldn't be possible to get here */
  return 0;
}

/* Separate a line into words. cInput is an array of strings, each
   containing one word. This routine also checks if the final word has
   a trailing $, if so, it is an array that continues to the next
   line. */

void GetWords(char cLine[],char cInput[MAXARRAY][OPTLEN],int *iNumWords,int *bContinue) {
  int iPos,iPosStart,iWord;
  char cTmp[OPTLEN];

  //iPos0=GetPos(cLine);
  iWord=0;
  /* Use GetPos to avoid white space */
  for (iPos=GetPos(cLine);iPos<strlen(cLine);iPos++) {
  //for (iPos=GetPos(cLine);iPos<strlen(cLine)-GetPos(cLine);iPos++) {
    /* MEM: Something is wrong here, but it is intermittent. Sometimes a call
       here produces a memory error with valgrind. On 12/14/16 a run without the
       next print statements produced an error, but with them in, the error
       disappeared. After commenting out again, the problem was still gone. */
    /* DEBUG
    printf("%s\n",cLine);
    printf("%d %d %d\n",(int)strlen(cLine),GetPos(cLine),iPos);
    fflush(stdout);
    */
    iPosStart=0;
    while (!isspace(cLine[iPos])) {
      if (cLine[iPos] != 35) { // 35 is ASCII code for #
        /* Fill word in */
        cInput[iWord][iPosStart] = cLine[iPos];
        iPosStart++;
        iPos++;
      } else {
        /* If at the start of the word, we must decrement iWord
          so that when it is incremented at the end of the loop
          the correct number of words is returned. If at the end
          of a word, everything should be fine. */
        if (iPosStart==0) iWord--;

        iPos=strlen(cLine);
        break;
      }
    }
    /* Now advance to next word */
    while (isspace(cLine[iPos]))
      iPos++;

    iPos--;
    iWord++;
  }
  /* Is the last character a $? If so, remove it and adjust iNumWords */
  if (cInput[iWord-1][strlen(cInput[iWord-1])-1] == 36) {
    *bContinue=1;
    if (strlen(cInput[iWord-1]) == 1)
      *iNumWords = iWord-1;
    else
      *iNumWords = iWord;
    cInput[iWord-1][strlen(cInput[iWord-1])-1] = '\0';
  } else {
    *bContinue=0;
    *iNumWords=iWord;
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
  int iPos,iWord,bContinue,iNumWords;
  FILE *fp;

  memset(cLine,'\0',LINE);

  /* iLine=malloc(MAXLINES*sizeof(int)); */

  iLine[0]=-1;

  /* Fencepost problem. If cInput continues across multiple lines,
     then we must keep getting more lines. For the first line, we
     remove the first word, as it is cOption. iLine must come
     preassigned and set to 0. */

  /* Initialize the array to have nothing in it. */
  for (iPos=0;iPos<MAXARRAY;iPos++)
    memset(cTmp[iPos],'\0',OPTLEN);

  GetLine(cFile,cOption,cLine,&iLine[0],iVerbose);
  GetWords(cLine,cTmp,&iNumWords,&bContinue);
  *iNumLines=1;

  for (iWord=0;iWord<iNumWords-1;iWord++) {
    memset(saInput[iWord],'\0',OPTLEN);
    strcpy(saInput[iWord],cTmp[iWord+1]);
    /* Reset cTmp string: If the next time cTmp is filled, the
       new string is longer than the old, then vestigial characters
       can remain after a trailing $. */
    memset(cTmp[iWord+1],'\0',OPTLEN);
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
      for (iWord=0;iWord<iNumWords;iWord++) {
        strcpy(saInput[*iNumIndices+iWord],cTmp[iWord]);
        memset(cTmp[iWord],'\0',OPTLEN);
      }
      *iNumIndices += iNumWords;
      (*iNumLines)++;
    } else {
      if (iVerbose >= VERBINPUT) {
	fprintf(stderr,"WARNING: Trailing $ found without a subsequent valid line for option %s in file %s.\n",cOption,cFile);
	bContinue=0;
      }
    }
  }
}

/* Get all fields in a double array. The fields are stored in daInput,
   and the lines which were read are in iNumLines. If a parameter is
   not found, daInput is empty, and iLine is unchanged. */

void AddOptionDoubleArray(char cFile[],char cOption[],double *daInput, int *iNumIndices,int *iNumLines,int *iLine,int iVerbose) {
  int iIndex;
  char cTmp[MAXARRAY][OPTLEN];

  AddOptionStringArray(cFile,cOption,cTmp,iNumIndices,iNumLines,iLine,iVerbose);
  for (iIndex=0;iIndex<*iNumIndices;iIndex++)
    daInput[iIndex]=atof(cTmp[iIndex]);
}

void AddOptionDouble(char cFile[],char cOption[],double *dInput,int *iLine,int iVerbose) {
  char cTmp[OPTLEN],cLine[LINE];

  GetLine(cFile,cOption,cLine,iLine,iVerbose);
  if(*iLine >= 0)
      sscanf(cLine,"%s %lf",cTmp, dInput);
}

void AddOptionInt(char cFile[],char cOption[],int *iInput,int *iLine,int iVerbose) {
  char cTmp[OPTLEN],cLine[LINE];

  GetLine(cFile,cOption,cLine,iLine,iVerbose);
  if(*iLine >= 0)
      sscanf(cLine,"%s %d",cTmp,iInput);
}

void AddOptionBool(char cFile[],char cOption[],int *iInput,int *iLine,int iVerbose) {

  AddOptionInt(cFile,cOption,iInput,iLine,iVerbose);
  if (*iLine == -1 )   //PED 4/9/15
      return;
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


int GetNumOut(char cFile[],char cName[],int iLen,int *iLineNum,int iExit) {
  char cLine[LINE],cWord[NAMELEN];
  int iPos,j,ok,bDone=0,iLine=0,iNumOut;
  FILE *fp;

  fp=fopen(cFile,"r");
  if (fp == NULL) {
    fprintf(stderr,"Unable to open %s.\n",cFile);
    exit(iExit);
  }

  while(fgets(cLine,LINE,fp) != NULL) {
    /* Check for # sign */
    if (memcmp(cLine,"#",1) != 0) {
      /* Check for desired parameter */
      sscanf(cLine,"%s",cWord);
      if (memcmp(cWord,cName,iLen) == 0) {
        /* Parameter Found! */
        if (bDone) {
          fprintf(stderr,"ERROR: Multiple occurences of parameter %s found.\n",cName);
          fprintf(stderr,"\t%s, lines: %d and %d\n",cFile,*iLineNum,iLine);
          exit(iExit);
        }
        bDone=1;
        *iLineNum=iLine;

        iNumOut=0;
        ok=1;
        for (iPos=1;iPos<LINE;iPos++) { /* Ignore first character, as it makes conditional well-defined */
          /* printf("%d ",cLine[iPos]); */
          if (ok) {
            if (cLine[iPos] == 35) { // 35 is ASCII code for #
              /* Pound sign! */
              ok=0;
              iNumOut++;
            }
            if (isspace(cLine[iPos]) && !isspace(cLine[iPos-1]))
              iNumOut++;
          }
        }
      }
    }
    iLine++;
    for (iPos=0;iPos<LINE;iPos++)
      cLine[iPos]=0;
  }
  /* Lose the input parameter */
  iNumOut--;
  return iNumOut;
}

int iGetNumLines(char cFile[]) {
  int iNumLines = 0,iChar,bFileOK = 1;
  int bComment,bReturn;
  FILE *fp;
  char cLine[LINE];

  fp=fopen(cFile,"r");
  if (fp == NULL) {
    fprintf(stderr,"Unable to open %s.\n",cFile);
    exit(EXIT_INPUT);
  }

  while(fgets(cLine,LINE,fp) != NULL) {
    iNumLines++;

    /* Check to see if line is too long. The maximum length of a line is set
       by LINE. If a carriage return is not found in the first LINE
       characters *and* is not preceded by a comment, the line is too long. */
    bComment=0;
    bReturn=0;
    for (iChar=0;iChar<LINE && cLine[iChar] != '\0';iChar++) {
      if (cLine[iChar] == 35) { // 35 is ASCII code for #
	bComment = 1;
      }
      // Maybe unnecessary with the second conditional in the loop initialization?
      if (cLine[iChar] == 10) { // 10 is ASCII code for line feed
	bReturn = 1;
      }

    }

    if (!bReturn && !bComment) {
      if (iChar >= LINE) {
	fprintf(stderr,"ERROR: Line %s:%d is longer than allowed (%d characters).\n",cFile,iNumLines,LINE);
	bFileOK = 0;
      }
    }
  }

  if (!bFileOK)
    exit(EXIT_INPUT);

  return iNumLines;
}

void InitializeInput(INFILE *input) {
  int iLine,iPos,bBlank;
  FILE *fp;
  char cLine[LINE];

  fp=fopen(input->cIn,"r");
  if (fp == NULL) {
    fprintf(stderr,"Unable to open %s.\n",input->cIn);
    exit(EXIT_INPUT);
  }
  input->iNumLines = iGetNumLines(input->cIn);
  input->bLineOK = malloc(input->iNumLines*sizeof(int));
  input->cSpecies[0] = 0;
  input->cReactions[0] = 0;

  for (iLine=0;iLine<input->iNumLines;iLine++) {
    /* Initialize bLineOK */
    input->bLineOK[iLine] = 0;

    /* Now find those lines that are comments or blank */
    for (iPos=0;iPos<LINE;iPos++)
      cLine[iPos]='\0';

    fgets(cLine,LINE,fp);
    /* Check for # sign or blank line */
    if (CheckComment(cLine,LINE))
      /* Line is OK */
      input->bLineOK[iLine] = 1;
    else {
      // Is it a blank line?
      bBlank=0;
      for (iPos=0;iPos<LINE;iPos++) {
        if (!isspace(cLine[iPos]) && cLine[iPos] != '\0') {
          bBlank=1;
        }
      }
      if (!bBlank) input->bLineOK[iLine] = 1;
    }
  }
}

void Unrecognized(FILES files) {
  FILE *fp;
  char cLine[LINE],cWord[NAMELEN];
  int iFile,iLine,bExit=0; /* Assume don't exit */

  for (iFile=0;iFile<files.iNumInputs;iFile++) {
    fp=fopen(files.Infile[iFile].cIn,"r");

    iLine=0;
    while (fgets(cLine,LINE,fp) != NULL) {
      if (!files.Infile[iFile].bLineOK[iLine]) {
        /* Bad line */
        sscanf(cLine,"%s",cWord);
        fprintf(stderr,"ERROR: Unrecognized option \"%s\" in %s, line %d.\n",cWord,files.Infile[iFile].cIn,iLine+1);
        bExit=1;
      }
      iLine++;
    }
  }
  if (bExit)
    exit(EXIT_INPUT);

}

void UpdateFoundOption(INFILE *input,OPTIONS *options,int iLine,int iFile) {
  input->bLineOK[iLine] = 1;
  options->iLine[iFile] = iLine;
  strcpy(options->cFile[iFile],input->cIn);
}

void UpdateFoundOptionMulti(INFILE *input,OPTIONS *options,int *iLine,int iNumLines,int iFile) {
  int iLineNow;

  /*
    For now options->iLine is a scalar, so just use the first line.
    The user should be able to figure it out from there.
  */
  options->iLine[iFile] = iLine[0];
  strcpy(options->cFile[iFile],input->cIn);
  for (iLineNow=0;iLineNow<iNumLines;iLineNow++) {
    input->bLineOK[iLine[iLineNow]] = 1;
  }
}

void CheckDuplication(FILES *files,OPTIONS *options,char cFile[],int iLine,int iVerbose) {
  int iFile;

  if (options->iMultiFile) {
    fprintf(stderr,"ERROR: CheckDuplication called, but options.iMultiFile = %d\n",options->iMultiFile);
    exit(EXIT_INPUT);
  }

  for (iFile=0;iFile<files->iNumInputs;iFile++) {
    if (options->iLine[iFile] >= 0 && memcmp(files->Infile[iFile].cIn,cFile,strlen(cFile)) != 0) {
      /* Found previously set location */
      if (iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: Option %s found in multiple files\n",options->cName);
      fprintf(stderr,"\t%s, Line: %d\n",files->Infile[iFile].cIn,options->iLine[iFile]);
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

void ReadVerbose(FILES *files,OPTIONS *options,int *iVerbose,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn,options->cName,&iTmp,&lTmp,VERBALL);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,VERBALL);
    if (iTmp < 0 || iTmp > VERBALL) {
      fprintf(stderr,"ERROR: %s must be in the range [0,%d]\n",options->cName,VERBALL);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    if (*iVerbose == VERBALL) {
      fprintf(stderr,"WARNING: -v set at command line, but %s option set.\n",options->cName);
      fprintf(stderr,"\tiVerbose is set to %d.\n",VERBALL);
    } else if (*iVerbose == 0) {
      /*
      These lines defeat the point of setting the ``-q`` flag!
      fprintf(stderr,"WARNING: -q set at command line, but %s option set.\n",options->cName);
      fprintf(stderr,"\tiVerbose is set to 0.\n");
      */
    } else {
      *iVerbose = iTmp;
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else if (*iVerbose == -1) // Was not set at command line, so set to default
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
  assert(0);
}

void ReadUnitMass(CONTROL *control,FILES *files,OPTIONS *options,int iFile) {
  int iFileNow,lTmp=-1;
  char cTmp[OPTLEN];
  /* Mass Unit
     0=gm
     1=kg
     2=solar
     3=Earth
     4=Jup
     5=Neptune */

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (iFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->Io.iVerbose >= VERBINPUT)
        fprintf(stderr,"WARNING: %s set in %s, all bodies will use this unit.\n",options->cName,files->Infile[iFile].cIn);
      control->Units[iFile].iMass = iAssignMassUnit(cTmp,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);

      for (iFileNow=1;iFileNow<files->iNumInputs;iFileNow++) {
        control->Units[iFileNow].iMass = control->Units[iFile].iMass;
        /* Negative sign for lTmp indicated that the parameter was found in another file */
        /* UpdateFoundOption(&files->Infile[iFileNow],options,-lTmp,iFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* Assigned in body file */
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
        /* Assigned in primary file */
        if (control->Io.iVerbose >= VERBERR)
          fprintf(stderr,"ERROR: %s found in primary and body files!\n",options->cName);
        fprintf(stderr,"\t%s, Line: %d\n",files->Infile[0].cIn,options->iLine[0]);
        fprintf(stderr,"\t%s, Line: %d\n",files->Infile[iFile].cIn,lTmp);
        exit(EXIT_INPUT);
      } else {
        /* Wasn't assigned in primary */
        control->Units[iFile].iMass = iAssignMassUnit(cTmp,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
        UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
      }
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
        /* No, assign default */
        if (control->Io.iVerbose >= VERBUNITS)
          fprintf(stderr,"WARNING: %s not set in file %s, defaulting to %s.\n",options->cName,files->Infile[iFile].cIn,options->cDefault);
        control->Units[iFile].iMass = iAssignMassUnit(options->cDefault,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
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
  assert(0);
}


void ReadUnitTime(CONTROL *control,FILES *files,OPTIONS *options,int iFile) {
  int iFileNow,lTmp=-1;
  char cTmp[OPTLEN];
  /* Time Unit
     0=s
     1=d
     2=yr
     3=Myr
     4=Gyr */

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (iFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->Io.iVerbose >= VERBINPUT)
        fprintf(stderr,"WARNING: %s set in %s, all bodies will use this unit.\n",options->cName,files->Infile[iFile].cIn);
      control->Units[iFile].iTime = iAssignUnitTime(cTmp,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);

      for (iFileNow=1;iFileNow<files->iNumInputs;iFileNow++) {
        control->Units[iFileNow].iTime = control->Units[iFile].iTime;
        /* UpdateFoundOption(&files->Infile[iFileNow],options,-lTmp,iFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
        if (control->Io.iVerbose >= VERBERR)
          fprintf(stderr,"ERROR: %s found in primary and body files!\n",options->cName);
        fprintf(stderr,"\t%s, Line: %d\n",options->cFile[0],options->iLine[0]);
        fprintf(stderr,"\t%s, Line: %d\n",files->Infile[iFile].cIn,lTmp);
        exit(EXIT_INPUT);
      } else {
        /* Wasn't assigned in primary */
        control->Units[iFile].iTime = iAssignUnitTime(cTmp,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
        UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
      }
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
        /* No, assign default */
        if (control->Io.iVerbose >= VERBUNITS)
          fprintf(stderr,"WARNING: %s not set in file %s, defaulting to %s.\n",options->cName,files->Infile[iFile].cIn,options->cDefault);
        control->Units[iFile].iTime = iAssignUnitTime(options->cDefault,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
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
  assert(0);
}

void ReadUnitAngle(CONTROL *control,FILES *files,OPTIONS *options,int iFile) {
  int iFileNow,lTmp=-1;
  char cTmp[OPTLEN];
  /* Angle Unit
     0=rad
     1=deg */

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (iFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->Io.iVerbose >= VERBINPUT)
        fprintf(stderr,"WARNING: %s set in %s, all bodies will use this unit.\n",options->cName,files->Infile[iFile].cIn);
      control->Units[iFile].iAngle = iAssignUnitAngle(cTmp,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);

      for (iFileNow=1;iFileNow<files->iNumInputs;iFileNow++) {
        control->Units[iFileNow].iAngle = control->Units[iFile].iAngle;
        /* UpdateFoundOption(&files->Infile[iFileNow],options,-lTmp,iFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
        if (control->Io.iVerbose >= VERBERR)
          fprintf(stderr,"ERROR: %s found in primary and body files!\n",options->cName);
        fprintf(stderr,"\t%s, Line: %d\n",options->cFile[0],options->iLine[0]);
        fprintf(stderr,"\t%s, Line: %d\n",files->Infile[iFile].cIn,lTmp);
        exit(EXIT_INPUT);
      } else {
        /* Wasn't assigned in primary */
        control->Units[iFile].iAngle = iAssignUnitAngle(cTmp,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
        UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
      }
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
        /* No, assign default */
        if (control->Io.iVerbose >= VERBUNITS)
          fprintf(stderr,"WARNING: %s not set in file %s, defaulting to %s.\n",options->cName,files->Infile[iFile].cIn,options->cDefault);
        control->Units[iFile].iAngle = iAssignUnitAngle(options->cDefault,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
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
  assert(0);
}

void ReadUnitLength(CONTROL *control,FILES *files,OPTIONS *options,int iFile) {
  int iFileNow,lTmp=-1;
  char cTmp[OPTLEN];
  /* Length Unit
     0=cm
     1=m
     2=km
     3=R_sun
     4=R_earth
     5=R_Jup
     6=AU */

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (iFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->Io.iVerbose >= VERBINPUT)
        fprintf(stderr,"WARNING: %s set in %s, all bodies will use this unit.\n",options->cName,files->Infile[iFile].cIn);
      control->Units[iFile].iLength = iAssignUnitLength(cTmp,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);

      options->iLine[iFile] = lTmp;
      for (iFileNow=1;iFileNow<files->iNumInputs;iFileNow++) {
        control->Units[iFileNow].iLength = control->Units[iFile].iLength;
        /* UpdateFoundOption(&files->Infile[iFileNow],options,lTmp,iFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
        if (control->Io.iVerbose >= VERBERR)
          fprintf(stderr,"ERROR: %s found in primary and body files!\n",options->cName);
        fprintf(stderr,"\t%s, Line: %d\n",options->cFile[0],options->iLine[0]);
        fprintf(stderr,"\t%s, Line: %d\n",files->Infile[iFile].cIn,lTmp);
        exit(EXIT_INPUT);
      } else {
        /* Wasn't assigned in primary */
        control->Units[iFile].iLength = iAssignUnitLength(cTmp,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
        UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
      }
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
        /* No, assign default */
        if (control->Io.iVerbose >= VERBUNITS)
          fprintf(stderr,"WARNING: %s not set in file %s, defaulting to %s.\n",options->cName,files->Infile[iFile].cIn,options->cDefault);
        control->Units[iFile].iLength = iAssignUnitLength(options->cDefault,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read */
    }
  }
}

int iAssignTempUnit(char cTmp[],int iVerbose,char cFile[],char cName[],int iLine) {
  if (memcmp(sLower(cTmp),"k",1) == 0)
    return 0;
  else if (memcmp(sLower(cTmp),"c",1) == 0)
    return 1;
  else if (memcmp(sLower(cTmp),"f",1) == 0)
    return 2;
  else {
    if (iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Unknown argument to %s: %s. Options are: K, C, F.\n",cName,cTmp);
    LineExit(cFile,iLine);
  }
  assert(0);
}

void ReadUnitTemp(CONTROL *control,FILES *files,OPTIONS *options,int iFile) {
  int iFileNow,lTmp=-1;
  char cTmp[OPTLEN];
  /* Temperature Units
     0=Kelvin
     1=Celsius
     2=Farenheit */

  //Copied from ReadUnitMass
  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (iFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->Io.iVerbose >= VERBINPUT)
        fprintf(stderr,"WARNING: %s set in %s, all bodies will use this unit.\n",options->cName,files->Infile[iFile].cIn);
      control->Units[iFile].iTemp = iAssignTempUnit(cTmp,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);

      for (iFileNow=1;iFileNow<files->iNumInputs;iFileNow++) {
        control->Units[iFileNow].iTemp = control->Units[iFile].iTemp;
        /* Negative sign for lTmp indicated that the parameter was found in another file */
        /* UpdateFoundOption(&files->Infile[iFileNow],options,-lTmp,iFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* Assigned in body file */
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
        /* Assigned in primary file */
        if (control->Io.iVerbose >= VERBERR)
          fprintf(stderr,"ERROR: %s found in primary and body files!\n",options->cName);
        fprintf(stderr,"\t%s, Line: %d\n",files->Infile[0].cIn,options->iLine[0]);
        fprintf(stderr,"\t%s, Line: %d\n",files->Infile[iFile].cIn,lTmp);
        exit(EXIT_INPUT);
      } else {
        /* Wasn't assigned in primary */
        control->Units[iFile].iTemp = iAssignTempUnit(cTmp,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
        UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
      }
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
        /* No, assign default */
        if (control->Io.iVerbose >= VERBUNITS)
          fprintf(stderr,"WARNING: %s not set in file %s, defaulting to %s.\n",options->cName,files->Infile[iFile].cIn,options->cDefault);
        control->Units[iFile].iTemp = iAssignTempUnit(options->cDefault,control->Io.iVerbose,files->Infile[iFile].cIn,options->cName,lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read */
    }
  }

}

void ReadSystemName(CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* System Name */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    strcpy(system->cName,cTmp);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  }
}

void ReadBodyFileNames(CONTROL *control,FILES *files,OPTIONS *options,INFILE *infile) {
  int iIndex,iNumIndices=0,iNumLines=0;
  int *lTmp;
  char saTmp[MAXARRAY][OPTLEN];

  lTmp=malloc(MAXLINES*sizeof(int));

  AddOptionStringArray(infile->cIn,options->cName,saTmp,&iNumIndices,&iNumLines,lTmp,control->Io.iVerbose);

  if (lTmp[0] >= 0) {
    if (iNumIndices == 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: No files supplied for option %s.\n",options->cName);
      LineExit(infile->cIn,lTmp[0]);
    }
    files->iNumInputs=iNumIndices+1;
  } else {
    if (control->Io.iVerbose >= VERBERR)
      fprintf(stderr,"ERROR: Option %s is required in file %s.\n",options->cName,infile->cIn);
    exit(EXIT_INPUT);
  }

  /* With body files identified, must allocate space */
  files->Infile = malloc(files->iNumInputs*sizeof(INFILE));
  files->Infile[0].bLineOK = malloc(infile->iNumLines*sizeof(int));

  InfileCopy(&files->Infile[0],infile);

  for (iIndex=0;iIndex<iNumIndices;iIndex++)

    strcpy(files->Infile[iIndex+1].cIn,saTmp[iIndex]);

  control->Evolve.iNumBodies=iNumIndices;
  files->Outfile = malloc(iNumIndices*sizeof(OUTFILE));

  UpdateFoundOptionMulti(&files->Infile[0],options,lTmp,iNumLines,0);

  free(lTmp);
}

/*
 *
 * Master subroutine for initial options
 *
 */

void ReadInitialOptions(BODY **body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,OUTPUT *output,SYSTEM *system,char infile[]) {
  int iFile,iBody,iModule;
  INFILE input;

  strcpy(input.cIn,infile);
  /* Initialize primary input file */
  InitializeInput(&input);

  /* First find input files */
  ReadBodyFileNames(control,files,&options[OPT_BODYFILES],&input);
  *body = malloc(control->Evolve.iNumBodies*sizeof(BODY));

  InitializeBodyModules(body,control->Evolve.iNumBodies);

  /* Is iVerbose set in primary input? */
  ReadVerbose(files,&options[OPT_VERBOSE],&control->Io.iVerbose,0);

  /* Now we can search through files for all options. First we scan the files for Verbosity */
  /* Initialize other input files */
  for (iFile=1;iFile<files->iNumInputs;iFile++) {
    InitializeInput(&files->Infile[iFile]);
    ReadVerbose(files,&options[OPT_VERBOSE],&control->Io.iVerbose,iFile);
  }

  /* Now initialize arrays */
  control->Units = malloc(files->iNumInputs*sizeof(UNITS));

  /* Initialize functions in the module struct */
  InitializeModule(module,control->Evolve.iNumBodies);
  control->Halt = malloc(control->Evolve.iNumBodies*sizeof(HALT));
  /* XXX Does this belong here? Need to know iNumHalts, so should this come in verify? -- I think so
     InitializeHalt(control,module); */

  /* Next we must find the units, modules, and system name */
  for (iFile=0;iFile<files->iNumInputs;iFile++) {
    ReadUnitMass(control,files,&options[OPT_UNITMASS],iFile);
    ReadUnitTime(control,files,&options[OPT_UNITTIME],iFile);
    ReadUnitAngle(control,files,&options[OPT_UNITANGLE],iFile);
    ReadUnitLength(control,files,&options[OPT_UNITLENGTH],iFile);
    ReadUnitTemp(control,files,&options[OPT_UNITTEMP],iFile);
    ReadSystemName(control,files,&options[OPT_SYSTEMNAME],system,iFile);
    /* Get Modules first as it helps with verification
       ReadModules is in module.c */
    ReadModules(*body,control,files,module,&options[OPT_MODULES],iFile);
  }

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++)
    FinalizeModule(*body,module,iBody);

  /* XXX Should check this file here */

  free(input.bLineOK);

}

void AssignDefaultDouble(OPTIONS *options,double *dOption,int iNumFiles) {
  int iFile;

  for (iFile=0;iFile<iNumFiles;iFile++)
    if (options->iLine[iFile] != -1)
      /* Options already input in other file */
      return;

  /* If made it here, not input already, so assign default */
  *dOption = options->dDefault;
}

void AssignDefaultInt(OPTIONS *options,int *iOption,int iNumFiles) {
  int iFile;

  for (iFile=0;iFile<iNumFiles;iFile++)
    if (options->iLine[iFile] != -1)
      /* Options already input in other file */
      return;

  /* If made it here, not input already, so assign default */
  *iOption = atoi(options->cDefault);
}

void AssignDefaultString(OPTIONS *options,char cOption[],int iNumFiles) {
  int iFile;

  for (iFile=0;iFile<iNumFiles;iFile++)
    if (options->iLine[iFile] != -1)
      /* Options already input in other file */
      return;

  /* If made it here, not input already, so assign default */
  strcpy(cOption,options->cDefault);
}

int bOptionAlreadyFound(int *iLine,int iNumFiles) {
  int iFile;

  for (iFile=0;iFile<iNumFiles;iFile++) {
    if (iLine[iFile] >= 0)
      return 1;
  }
  return 0;
}

/*
 * Here are the subroutines for options used by all modules
 */


/*
 *
 * A
 *
 */

/* Age */

void ReadAge(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary input file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile-1].dAge = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    } else
      body[iFile-1].dAge = dTmp*fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dAge,files->iNumInputs);
}

/* Albedo */

void ReadAlbedoGlobal(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary input file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      fprintf(stderr,"ERROR: %s cannot be negative.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    } else
      body[iFile-1].dAlbedoGlobal = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dAlbedoGlobal,files->iNumInputs);
}

/* Body Type */

void ReadBodyType(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
    /* This parameter cannot exist in primary file */
    int lTmp=-1;
    int iTmp;

    AddOptionInt(files->Infile[iFile].cIn,options->cName,&iTmp,&lTmp,control->Io.iVerbose);
    if (lTmp >= 0) {
      NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
      if (iTmp < 0) {
        if (control->Io.iVerbose >= VERBERR)
          fprintf(stderr,"ERROR: %s must be non-negative.\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
    } else
        body[iFile-1].iBodyType = iTmp;
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
    } else
      if (iFile > 0)
        AssignDefaultInt(options,&body[iFile-1].iBodyType,files->iNumInputs);
}

/*
 *
 * B
 *
 */

/* Do Backward Integration? */

void ReadDoBackward(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    control->Evolve.bDoBackward = atoi(options->cDefault);
}

/* Output File Name */

void ReadOutFile(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file -- Each body has an output file */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    strcpy(files->Outfile[iFile-1].cOut,cTmp);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      AssignDefaultString(options,files->Outfile[iFile-1].cOut,files->iNumInputs);
}

/* Backward Eta */

void ReadEta(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    control->Evolve.dEta = dTmp;
    if (control->Evolve.dEta > 1 && control->Io.iVerbose >= VERBALL)
      fprintf(stderr,"WARNING: %s > 1 is not advised (%s:%d).\n",options->cName,files->Infile[iFile].cIn,lTmp);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultDouble(options,&control->Evolve.dEta,files->iNumInputs);
}

/* Backward integration output interval */

void ReadOutputTime(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    /* Convert output time to cgs */
    control->Io.dOutputTime = dTmp*fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultDouble(options,&control->Io.dOutputTime,files->iNumInputs);
}

/* Backward integration stop time */

void ReadStopTime(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    /* Convert stop time to cgs */
    control->Evolve.dStopTime = dTmp*fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultDouble(options,&control->Evolve.dStopTime,files->iNumInputs);
}

/* Integration timestep */

void ReadTimeStep(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
          fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    /* Convert timestep to cgs */
    control->Evolve.dTimeStep = dTmp*fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultDouble(options,&control->Evolve.dTimeStep,files->iNumInputs);
}

/* Do variable timestep? */

void ReadVarDt(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Evolve.bVarDt = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&control->Evolve.bVarDt,files->iNumInputs);
}

/* Body Name */

void ReadBodyName(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file -- Each body has an output file */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    strcpy(body[iFile-1].cName,cTmp);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      sprintf(body[iFile-1].cName,"%d",iFile);
}

/* Body color (for plotting) */
void ReadColor(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    strcpy(body[iFile-1].cColor,cTmp);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      strcpy(body[iFile-1].cColor,options->cDefault);
}

/*
 *
 * D
 *
 */

/* Digits */

void ReadDigits(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn,options->cName,&iTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Io.iDigits = iTmp;
    if (control->Io.iDigits < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be non-negative.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,options->iLine[iFile]);
    }
    if (control->Io.iDigits > 16) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be less than 17.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,options->iLine[iFile]);
    }
    control->Io.iDigits = iTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    // XXX Don't we need to check if it was found in another file already??
    AssignDefaultInt(options,&control->Io.iDigits,files->iNumInputs);
}


/*
 *
 * E
 *
 */

/* Eccentricity */

void ReadEcc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* Cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0 || dTmp >= 1) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be in the range [0,1).\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    body[iFile-1].dEcc = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);

  } else
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dEcc,files->iNumInputs);
}

/*
 *
 * F
 *
 */


/* Forward integration? */

void ReadDoForward(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Evolve.bDoForward = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    /* Set to default */
    AssignDefaultInt(options,&control->Evolve.bDoForward,files->iNumInputs);
}

/*
 *
 *   HALT
 *
 */


/* Maximum Eccentricity */

void ReadHaltMaxEcc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */

  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be in the range (0,1).\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    control->Halt[iFile-1].dMaxEcc = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      control->Halt[iFile-1].dMaxEcc = options->dDefault;
//       AssignDefaultDouble(options,&control->Halt[iFile-1].dMaxEcc,files->iNumInputs);
  }
}

/* Merge */

void ReadHaltMerge(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Merging is not allowed for the central body */
    if (iFile == 1) {
      fprintf(stderr,"ERROR: Cannot set %s for systems with more than 2 bodies.\n",options[OPT_HALTMERGE].cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }

    control->Halt[iFile-1].bMerge = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile == 1)
      control->Halt[iFile-1].bMerge = 0;
    if (iFile > 1) {
      /* HaltMerge is unusual in that its default value depends on the body's
	 modules. These are set in ReadInitialOptions, so they are always
	 known by ReadOptionsGeneral. Therefore, we can assign it based on
	 the "bModule" members of the body struct. */
      // XXX Russell -- Include galhabit?
      if (body[iFile-1].bEqtide || body[iFile-1].bDistOrb || body[iFile-1].bBinary)
	control->Halt[iFile-1].bMerge = 1;
      else
	control->Halt[iFile-1].bMerge = 0;
    }
  }
}

/* Minimum Eccentricity */

void ReadHaltMinEcc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be in the range (0,1).\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    control->Halt[iFile-1].dMinEcc = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultDouble(options,&control->Halt[iFile-1].dMinEcc,files->iNumInputs);
  }
}


/* Need to add ReadHaltDblSync function, since halt.bDblSync is never initialized anywhere! */


/* Minimum obliquity */

void ReadHaltMinObl(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBINPUT)
          fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBINPUT)
          fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    control->Halt[iFile-1].dMinObl = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      control->Halt[iFile-1].dMinObl = options->dDefault;
  }
}

/* Minimum Semi-Major Axis */

void ReadHaltMinSemi(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be larger than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    control->Halt[iFile-1].dMinSemi = dTmp*fdUnitsLength(control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultDouble(options,&control->Halt[iFile-1].dMinSemi,files->iNumInputs);
  }
}

/* Positive de/dt */

void ReadHaltPosDeDt(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Halt[iFile-1].bPosDeDt = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultInt(options,&control->Halt[iFile-1].bPosDeDt,files->iNumInputs);
  }
}

/*
 *
 * H
 *
 */

/* Hecc -- currently this is not supported. XXX */
void ReadHecc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr,"ERROR: Option %s is not currently supported.\n",options->cName);
    exit(EXIT_INPUT);
  } else
    if (iFile > 0)
      body[iFile-1].dHecc = options->dDefault;
}

/*
 *
 * I
 *
 */

/* Integration Method */
void ReadIntegrationMethod(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int i,lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (memcmp(sLower(cTmp),"e",1) == 0)
      control->Evolve.iOneStep = EULER;
    else if (memcmp(sLower(cTmp),"r",1) == 0)
      control->Evolve.iOneStep = RUNGEKUTTA;
    else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,"ERROR: Unknown argument to %s: %s.\n",options->cName,cTmp);
        fprintf(stderr,"Options are Euler.\n");
      }
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  }
  /* If not input, VerifyIntegration assigns default */
}

/*
 *
 * K
 *
 */

/* Kecc -- currently unsupported. XXX */
void ReadKecc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr,"ERROR: Option %s is not currently supported.\n",options->cName);
    exit(EXIT_INPUT);
  } else
    if (iFile > 0)
      body[iFile-1].dKecc = options->dDefault;
}

/*
 *
 *   LOG
 *
 */

/* Do log file? */

void ReadDoLog(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    control->Io.bLog = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&control->Io.bLog,files->iNumInputs);
}

/* Log file name */

void ReadLogFile(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int i,lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    strcpy(files->cLog,cTmp);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    /* Assign Default */
    if (iFile == files->iNumInputs-1) {
      for (i=0;i<files->iNumInputs;i++)
        if (options->iLine[i] != -1)
          /* Was aaigned, return */
          return;
    }
    /* Wasn't entered, assign default */
    sprintf(files->cLog,"%s.log",system->cName);
  }
}

/* Longitude of pericenter */
void ReadLongP(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dLongP = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dLongP = options->dDefault;
}

/* Longitude of ascending node */

void ReadLongA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dLongA = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dLongA = options->dDefault;
}


/* Argument of pericenter */

void ReadArgP(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {

  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dArgP = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dArgP = options->dDefault;
}

/* Inclination */

void ReadInc(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dInc = dTmp;
    body[iFile-1].dSinc = sin(0.5*dTmp);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0) {
      body[iFile-1].dInc = options->dDefault;
      body[iFile-1].dSinc = sin(0.5*options->dDefault);
    }
}

/* LXUV -- currently unsupported */
void ReadLXUV(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr,"ERROR: Option %s is not currently supported.\n",options->cName);
    exit(EXIT_INPUT);
  } else
    if (iFile > 0)
      body[iFile-1].dLXUV = options->dDefault;
}

/*
 *
 *   M
 *
 */

/* Mass */

void ReadMass(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Density and MassRad */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dMass = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dMass = dTmp*fdUnitsMass(control->Units[iFile].iMass);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dMass = options->dDefault;
}

/* Mass-Radius relationship */

void ReadMassRad(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Density and Mass */
  int lTmp=-1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn,options->cName,cTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (memcmp(sLower(cTmp),"r",1) == 0) {
      /* Reid & Hawley 2000 */
      // XXX Should change number to #define'd variables!
      control->iMassRad[iFile-1]=0;
    } else if (memcmp(sLower(cTmp),"g",1) == 0) {
      /* Gorda and Svenchnikov 1999 */
      control->iMassRad[iFile-1]=1;
    } else if (memcmp(sLower(cTmp),"b",1) == 0) {
      /* Bayless & Orosz 2006 */
      control->iMassRad[iFile-1]=2;
    } else if (memcmp(sLower(cTmp),"s",1) == 0) {
      /* Sotin et al 2007 */
      control->iMassRad[iFile-1]=3;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,"ERROR: Unknown argument to %s: %s.\n",options->cName,cTmp);
        fprintf(stderr,"Options are GS99 (Gorda & Svechnikov 1999), BO06 (Bayless & Orosz 2006), Sotin07 (Sotin et al. 2007), or RH00 (Reid & Hawley 2000).\n");
      }
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    /* This one is weird, since the default is "none", but if this option
       is not set, then we need to set this variable to 0 for logging
       purposes. */
    if (iFile > 0)
      control->iMassRad[iFile-1] = 0;
  }
}

/* Mean Motion */

void ReadMeanMotion(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    body[iFile-1].dMeanMotion = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dMeanMotion,files->iNumInputs);
  }
}

/* Minimum Value */

void ReadMinValue(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be larger than 0.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    control->Evolve.dMinValue = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultDouble(options,&control->Evolve.dMinValue,files->iNumInputs);
}

/*
 *
 * O
 *
 */

/* Obliquity */

void ReadObliquity(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,180].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iFile-1].dObliquity = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dObliquity = options->dDefault;
}

/*
 * Output Order -- This one is special and takes different arguments
 */


void ReadOutputOrder(FILES *files,MODULE *module,OPTIONS *options,OUTPUT *output,int iFile,int iVerbose) {
  int i,j,count,iLen,iNumIndices=0,bNeg[MAXARRAY],ok=1,iNumGrid=0;
  int k,iOut,*lTmp;
  char saTmp[MAXARRAY][OPTLEN],cTmp[OPTLEN],cOption[MAXARRAY][OPTLEN],cOut[OPTLEN];
  int iLen1,iLen2;

  lTmp = malloc(MAXLINES*sizeof(int));

  AddOptionStringArray(files->Infile[iFile].cIn,options[OPT_OUTPUTORDER].cName,saTmp,&iNumIndices,&files->Infile[iFile].iNumLines,lTmp,iVerbose);

  if (lTmp[0] >= 0) {
    NotPrimaryInput(iFile,options[OPT_OUTPUTORDER].cName,files->Infile[iFile].cIn,lTmp[0],iVerbose);

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
      for (j=0;j<MODULEOUTEND;j++) {
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
          if (output[j].bGrid == 1)
	    // Exit!
            iNumGrid += 1;
          j = MODULEOUTEND; /* Poor man's break! */
        } else {
          if (iLen1 < iLen2)
            iLen=iLen1;
          else
            iLen=iLen2;

          if (memcmp(sLower(cTmp),sLower(cOut),iLen) == 0 && iLen1 > iLen2) {
            /* Output option found! */
            strcpy(cOption[count],output[j].cName);
            count++;
            iOut = j;
            if (output[j].bGrid == 1)
	      // Exit!
              iNumGrid += 1;
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
        LineExit(files->Infile[iFile].cIn,lTmp[0]);
      }

      if (!count) {
        /* Option not found */
        if (iVerbose >= VERBERR)
          fprintf(stderr,"ERROR: Unknown output option \"%s\".\n",saTmp[i]);
        LineExit(files->Infile[iFile].cIn,lTmp[0]);
      }

      if (count == 1) {
        /* Unique option */

        /* Verify and record negative options */
        if (bNeg[i]) {
          // Is the negative option allowed?
          if (!output[iOut].bNeg) { /* No */
            if (iVerbose >= VERBERR) {
              fprintf(stderr,"ERROR: Output option %s ",saTmp[i]);
              if (strlen(saTmp[i]) < strlen(output[iOut].cName))
                fprintf(stderr,"(= %s) ",output[iOut].cName);
              fprintf(stderr,"cannot be negative.\n");
            }
            LineExit(files->Infile[iFile].cIn,lTmp[0]);
          } else { // Yes, initialize bDoNeg to true
            output[iOut].bDoNeg[iFile-1] = 1;
          }
        } else { // Negative option not set, initialize bDoNeg to false
            output[iOut].bDoNeg[iFile-1] = 0;
        }
        if (output[iOut].bGrid == 0 || output[iOut].bGrid == 2) {
          memset(files->Outfile[iFile-1].caCol[i],'\0',OPTLEN);
          strcpy(files->Outfile[iFile-1].caCol[i],output[iOut].cName);
        } else {
          memset(files->Outfile[iFile-1].caGrid[iNumGrid-1],'\0',OPTLEN);
          strcpy(files->Outfile[iFile-1].caGrid[iNumGrid-1],output[iOut].cName);
        }
        // Is option part of selected modules?
        if (module->iBitSum[iFile-1] & output[iOut].iModuleBit) {
          // Parameter is part of selected modules
        } else {
          fprintf(stderr,"ERROR: Output parameter %s requires module(s): ",output[iOut].cName);
          PrintModuleList(stderr,output[iOut].iModuleBit);
          fprintf(stderr,"\n");
          ok=0;
        }
      }
    }

    if (!ok)
      DoubleLineExit(files->Infile[iFile].cIn,files->Infile[iFile].cIn,lTmp[0],options[OPT_MODULES].iLine[iFile]);

    files->Outfile[iFile-1].iNumCols = iNumIndices;
    /*
    files->Outfile[iFile-1].iNumCols = iNumIndices-iNumGrid;
    files->Outfile[iFile-1].iNumGrid = iNumGrid;
    */
    UpdateFoundOptionMulti(&files->Infile[iFile],&options[OPT_OUTPUTORDER],lTmp,files->Infile[iFile].iNumLines,iFile);
  } else {
    files->Outfile[iFile-1].iNumCols=0;
  }

  free(lTmp);
}

void ReadGridOutput(FILES *files,OPTIONS *options,OUTPUT *output,int iFile,int iVerbose) {
  int i,j,count,iLen,iNumIndices=0,bNeg[MAXARRAY],ok=0,iNumGrid=0;
  int k,iOut,*lTmp;
  char saTmp[MAXARRAY][OPTLEN],cTmp[OPTLEN],cOption[MAXARRAY][OPTLEN],cOut[OPTLEN];
  int iLen1,iLen2;

  lTmp = malloc(MAXLINES*sizeof(int));

  AddOptionStringArray(files->Infile[iFile].cIn,options[OPT_GRIDOUTPUT].cName,saTmp,&iNumIndices,&files->Infile[iFile].iNumLines,lTmp,iVerbose);

  if (lTmp[0] >= 0) {
    NotPrimaryInput(iFile,options[OPT_GRIDOUTPUT].cName,files->Infile[iFile].cIn,lTmp[0],iVerbose);

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
      for (j=0;j<MODULEOUTEND;j++) {
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
          if (output[j].bGrid == 1 || output[j].bGrid == 2)
            iNumGrid += 1;
          j = MODULEOUTEND; /* Poor man's break! */
        } else {
          if (iLen1 < iLen2)
            iLen=iLen1;
          else
            iLen=iLen2;

          if (memcmp(sLower(cTmp),sLower(cOut),iLen) == 0 && iLen1 > iLen2) {
            /* Output option found! */
            strcpy(cOption[count],output[j].cName);
            count++;
            iOut = j;
            if (output[j].bGrid == 1 || output[j].bGrid == 2)
              iNumGrid += 1;
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
        LineExit(files->Infile[iFile].cIn,lTmp[0]);
      }

      if (!count) {
        /* Option not found */
        if (iVerbose >= VERBERR)
          fprintf(stderr,"ERROR: Unknown output option \"%s\".\n",saTmp[i]);
        LineExit(files->Infile[iFile].cIn,lTmp[0]);
      }

      if (count == 1) {
        /* Unique option */

        /* Verify and record negative options */
        if (bNeg[i]) {
          // Is the negative option allowed?
          if (!output[iOut].bNeg) { /* No */
            if (iVerbose >= VERBERR) {
              fprintf(stderr,"ERROR: Output option %s ",saTmp[i]);
              if (strlen(saTmp[i]) < strlen(output[iOut].cName))
                fprintf(stderr,"(= %s) ",output[iOut].cName);
              fprintf(stderr,"cannot be negative.\n");
            }
            LineExit(files->Infile[iFile].cIn,lTmp[0]);
          } else { // Yes, initialize bDoNeg to true
            output[iOut].bDoNeg[iFile-1] = 1;
          }
        } else { // Negative option not set, initialize bDoNeg to false
            output[iOut].bDoNeg[iFile-1] = 0;
        }
        if (output[iOut].bGrid == 0) {
          memset(files->Outfile[iFile-1].caCol[i],'\0',OPTLEN);
          strcpy(files->Outfile[iFile-1].caCol[i],output[iOut].cName);
        } else {
          memset(files->Outfile[iFile-1].caGrid[iNumGrid-1],'\0',OPTLEN);
          strcpy(files->Outfile[iFile-1].caGrid[iNumGrid-1],output[iOut].cName);
        }
      }
    }

    //files->Outfile[iFile-1].iNumGrid = iNumGrid;
    UpdateFoundOptionMulti(&files->Infile[iFile],&options[OPT_GRIDOUTPUT],lTmp,files->Infile[iFile].iNumLines,iFile);
  }

  files->Outfile[iFile-1].iNumGrid = iNumGrid;

  free(lTmp);
}

void ReadOverwrite(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM  *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Io.bOverwrite > -1) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"WARNING: -f set at command line, and %s set (%s:%d). Overwrite authorized.\n",options->cName,files->Infile[iFile].cIn,lTmp);
    } else {
      control->Io.bOverwrite = bTmp;
      UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
    }
  } else
    if (control->Io.bOverwrite == -1)
      AssignDefaultInt(options,&control->Io.bOverwrite,files->iNumInputs);
}

/*
 *
 * P
 *
 */

/* Orbital Period */

void ReadOrbPeriod(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify with Semi and MeanMotion */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dOrbPeriod = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dOrbPeriod = dTmp*fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dOrbPeriod,files->iNumInputs);
}

/* Precession parameter */

void ReadPrecA(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2*PI) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,2*PI].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR)
            fprintf(stderr,"ERROR: %s must be in the range [0,360].\n",options->cName);
        LineExit(files->Infile[iFile].cIn,lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile-1].dPrecA = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      body[iFile-1].dPrecA = options->dDefault;
  }
}

void ReadDynEllip(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* Cannot exist in primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0 || dTmp >= 1) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be in the range [0,1).\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    body[iFile-1].dDynEllip = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);

  } else
    if (iFile > 0)
      body[iFile-1].dDynEllip = options->dDefault;

}

void ReadCalcDynEllip(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  int lTmp=-1,bTmp;
  AddOptionBool(files->Infile[iFile].cIn,options->cName,&bTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    /* Option was found */
    body[iFile-1].bCalcDynEllip = bTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].bCalcDynEllip = options->dDefault;
//     AssignDefaultInt(options,&body[iFile-1].bCalcDynEllip,files->iNumInputs);
}



/*
 *
 * R
 *
 */

/* Radius */

void ReadRadius(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Mass and MassRad */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dRadius = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dRadius = dTmp*fdUnitsLength(control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dRadius = options->dDefault;
}

/* Radius of Gyration */

void ReadRadiusGyration(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be greater than zero.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    body[iFile-1].dRadGyra = dTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      body[iFile-1].dRadGyra = options->dDefault;
}

/* Rotation Period */

void ReadRotPeriod(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify in conjunction with RotRate, RotVel and ForceEqSpin */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dRotPer = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      if (iFile > 0)
        body[iFile-1].dRotPer = dTmp*fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  }
}

/* Rotational Frequency */

void ReadRotRate(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify in conjunction with RotPer, RotVel and ForceEqSpin */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be non-negative.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    if (iFile > 0)
      body[iFile-1].dRotRate = dTmp/fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  }
}

/* Rotational Velocity */

void ReadRotVel(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify in conjunction with RotPer, RotRate and ForceEqSpin */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dRotVel = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dRotVel = dTmp*fdUnitsLength(control->Units[iFile].iLength)/fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  }
}

/*
 *
 * S
 *
 */

/* Scientific Notation */

void ReadSciNot(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn,options->cName,&iTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files,options,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (iTmp < 0) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be non-negative.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    if (iTmp > 16) {
      if (control->Io.iVerbose >= VERBERR)
        fprintf(stderr,"ERROR: %s must be less than 16.\n",options->cName);
      LineExit(files->Infile[iFile].cIn,lTmp);
    }
    control->Io.iSciNot = iTmp;
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    AssignDefaultInt(options,&control->Io.iSciNot,files->iNumInputs);
}

/* Semi-Major Axis */

void ReadSemiMajorAxis(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dSemi = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dSemi = dTmp*fdUnitsLength(control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dSemi,files->iNumInputs);
}

void ReadOptionsGeneral(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[]) {
  /* Now get all other options, if not in MODULE mode */
  int iOpt,iFile;

  /* For each option, search the file for its occurence */
  /* is iLine necessary? */

  for (iFile=0;iFile<files->iNumInputs;iFile++) {
    /* Start at 100 because 0-99 are reserved for initial options */
    for (iOpt=100;iOpt<NUMOPT;iOpt++)
      /* OutputOrder is special */
      if (options[iOpt].iType != -1 && iOpt != OPT_OUTPUTORDER && iOpt != OPT_GRIDOUTPUT) {
        fnRead[iOpt](body,control,files,&options[iOpt],system,iFile);
      }
  }
}

void ReadViscUMan(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify modules: this is used when distrot+eqtide are called without thermint */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile,options->cName,files->Infile[iFile].cIn,lTmp,control->Io.iVerbose);
    if (dTmp < 0)
      body[iFile-1].dViscUMan = dTmp*dNegativeDouble(*options,files->Infile[iFile].cIn,control->Io.iVerbose);
    else
      body[iFile-1].dViscUMan = dTmp*fdUnitsLength(control->Units[iFile].iLength)/fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile],options,lTmp,iFile);
  } else {
    if (iFile > 0)
      AssignDefaultDouble(options,&body[iFile-1].dViscUMan,files->iNumInputs);
  }
}

void ReadOptionsModules(BODY *body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,SYSTEM *system,fnReadOption fnRead[]) {
  int iBody,iModule;

  for (iBody=0;iBody<control->Evolve.iNumBodies;iBody++) {
    for (iModule=0;iModule<module->iNumModules[iBody];iModule++)
      module->fnReadOptions[iBody][iModule](body,control,files,options,system,fnRead,iBody);
  }
}

/*
 *
 * X
 *
 */

/* Xobl -- currently this is not supported. XXX */
void ReadXobl(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr,"ERROR: Option %s is not currently supported.\n",options->cName);
    exit(EXIT_INPUT);
  } else
    if (iFile > 0)
      body[iFile-1].dXobl = options->dDefault;
}

/*
 *
 * Y
 *
 */

/* Yobl -- currently this is not supported. XXX */
void ReadYobl(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr,"ERROR: Option %s is not currently supported.\n",options->cName);
    exit(EXIT_INPUT);
  } else
    if (iFile > 0)
      body[iFile-1].dYobl = options->dDefault;
}

/*
 *
 * Z
 *
 */

/* Zobl -- currently this is not supported. XXX */
void ReadZobl(BODY *body,CONTROL *control,FILES *files,OPTIONS *options,SYSTEM *system,int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp=-1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn,options->cName,&dTmp,&lTmp,control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr,"ERROR: Option %s is not currently supported.\n",options->cName);
    exit(EXIT_INPUT);
  } else
    if (iFile > 0)
      body[iFile-1].dZobl = options->dDefault;
}



/*
 *
 * Master Read Options Subroutine
 *
 */

void ReadOptions(BODY **body,CONTROL *control,FILES *files,MODULE *module,OPTIONS *options,OUTPUT *output,SYSTEM *system,UPDATE **update,fnReadOption fnRead[],char infile[]) {
  int iFile,iModule;

  /* Read options for files, units, verbosity, and system name. */
  ReadInitialOptions(body,control,files,module,options,output,system,infile);

  /* Now that we know how many bodies there are, initialize more features */
  *update = malloc(control->Evolve.iNumBodies*sizeof(UPDATE));

  /* Initialize module control */
  InitializeControl(control,module);

  /* Initialize halts XXX Done in verify
     InitializeHalt(control,module); */

  /* Now read in remaining options */
  ReadOptionsGeneral(*body,control,files,options,system,fnRead);

  /* Read in module options */
  ReadOptionsModules(*body,control,files,module,options,system,fnRead);

  /* Read in output order -- merge into ReadGeneralOptions? */
  for (iFile=1;iFile<files->iNumInputs;iFile++) {
    ReadOutputOrder(files,module,options,output,iFile,control->Io.iVerbose);
    if ((*body)[iFile-1].bPoise) {
      ReadGridOutput(files,options,output,iFile,control->Io.iVerbose);
    } else
      // Initialize iNumGrid to 0 so no memory issues
      files->Outfile[iFile-1].iNumGrid = 0;
  }

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
  options[OPT_AGE].dDefault = 0;
  options[OPT_AGE].iType = 2;
  options[OPT_AGE].dNeg = 1e9*YEARSEC;
  sprintf(options[OPT_AGE].cNeg,"Gyr");
  fnRead[OPT_AGE] = &ReadAge;

  sprintf(options[OPT_ALBEDOGLOBAL].cName,"dAlbedoGlobal");
  sprintf(options[OPT_ALBEDOGLOBAL].cDescr,"Globally averaged albedo");
  sprintf(options[OPT_ALBEDOGLOBAL].cDefault,"0.3");
  options[OPT_ALBEDOGLOBAL].dDefault = 0;
  options[OPT_ALBEDOGLOBAL].iType = 2;
  fnRead[OPT_ALBEDOGLOBAL] = &ReadAlbedoGlobal;

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
   *   C
   *
   */

  sprintf(options[OPT_COLOR].cName,"cColor");
  sprintf(options[OPT_COLOR].cDescr,"Body Color");
  sprintf(options[OPT_COLOR].cDefault,"000000");
  options[OPT_COLOR].iType = 2;
  fnRead[OPT_COLOR] = &ReadColor;

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
  sprintf(options[OPT_HALTMERGE].cDefault,"If eqtide or distorb called 1, else 0");
  options[OPT_HALTMERGE].iType = 0;
  fnRead[OPT_HALTMERGE] = &ReadHaltMerge;

  sprintf(options[OPT_HALTMINECC].cName,"dHaltMinEcc");
  sprintf(options[OPT_HALTMINECC].cDescr,"Minimum Eccentricity Value that Halts Integration");
  sprintf(options[OPT_HALTMINECC].cDefault,"-1");
  options[OPT_HALTMINECC].dDefault = -1;
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
   * H
   *
   */

  sprintf(options[OPT_HECC].cName,"dHecc");
  sprintf(options[OPT_HECC].cDescr,"Poincare's h -- Unsuppoted!");
  sprintf(options[OPT_HECC].cDefault,"-1");
  options[OPT_HECC].dDefault = -1;
  options[OPT_HECC].iType = 2;
  fnRead[OPT_HECC] = &ReadHecc;

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
   * K
   *
   */

  sprintf(options[OPT_KECC].cName,"dKecc");
  sprintf(options[OPT_KECC].cDescr,"Poincare's k -- Unsuppoted!");
  sprintf(options[OPT_KECC].cDefault,"-1");
  options[OPT_KECC].dDefault = -1;
  options[OPT_KECC].iType = 2;
  fnRead[OPT_KECC] = &ReadKecc;

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

  sprintf(options[OPT_LONGP].cName,"dLongP");
  sprintf(options[OPT_LONGP].cDescr,"Longitude of pericenter of planet's orbit");
  sprintf(options[OPT_LONGP].cDefault,"0");
  options[OPT_LONGP].dDefault = 0.0;
  options[OPT_LONGP].iType = 2;
  options[OPT_LONGP].iMultiFile = 1;
//   options[OPT_LONGP].dNeg = DEGRAD;
//   sprintf(options[OPT_LONGP].cNeg,"Degrees");
  fnRead[OPT_LONGP] = &ReadLongP;

  sprintf(options[OPT_LXUV].cName,"dLXUV");
  sprintf(options[OPT_LXUV].cDescr,"Total XUV Luminosity -- Unsuppoted!");
  sprintf(options[OPT_LXUV].cDefault,"-1");
  options[OPT_LXUV].dDefault = -1;
  options[OPT_LXUV].iType = 2;
  fnRead[OPT_LXUV] = &ReadLXUV;

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

  sprintf(options[OPT_MODULES].cName,"saModules");
  sprintf(options[OPT_MODULES].cDescr,"Body's Module List");
  sprintf(options[OPT_MODULES].cDefault,"none");
  options[OPT_MODULES].dDefault = -1;
  options[OPT_MODULES].iType = 4;

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

  sprintf(options[OPT_GRIDOUTPUT].cName,"saGridOutput");
  sprintf(options[OPT_GRIDOUTPUT].cDescr,"Gridded Output Parameter(s)");
  sprintf(options[OPT_GRIDOUTPUT].cDefault,"None");
  options[OPT_GRIDOUTPUT].iType = 14;
  options[OPT_GRIDOUTPUT].iMultiFile = 1;

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

  sprintf(options[OPT_INC].cName,"dInc");
  sprintf(options[OPT_INC].cDescr,"Inclination of planet's orbital plane");
  sprintf(options[OPT_INC].cDefault,"0");
  options[OPT_INC].dDefault = 0.0;
  options[OPT_INC].iType = 2;
  options[OPT_INC].iMultiFile = 1;
//   options[OPT_INC].dNeg = DEGRAD;
//   sprintf(options[OPT_INC].cNeg,"Degrees");
  fnRead[OPT_INC] = &ReadInc;

  sprintf(options[OPT_ARGP].cName,"dArgP");
  sprintf(options[OPT_ARGP].cDescr,"Argument of pericenter of planet's orbit");
  sprintf(options[OPT_ARGP].cDefault,"0");
  options[OPT_ARGP].dDefault = 0.0;
  options[OPT_ARGP].iType = 2;
  options[OPT_ARGP].iMultiFile = 1;
//   options[OPT_ARGP].dNeg = DEGRAD;
//   sprintf(options[OPT_ARGP].cNeg,"Degrees");
  fnRead[OPT_ARGP] = &ReadArgP;


  /*
   * P
   */

  sprintf(options[OPT_PRECA].cName,"dPrecA");
  sprintf(options[OPT_PRECA].cDescr,"Planet's precession angle");
  sprintf(options[OPT_PRECA].cDefault,"0");
  options[OPT_PRECA].dDefault = 0.0;
  options[OPT_PRECA].iType = 2;
  options[OPT_PRECA].iMultiFile = 1;
//   options[OPT_LONGA].dNeg = DEGRAD;
//   sprintf(options[OPT_LONGA].cNeg,"Degrees");
  fnRead[OPT_PRECA] = &ReadPrecA;

  sprintf(options[OPT_LONGA].cName,"dLongA");
  sprintf(options[OPT_LONGA].cDescr,"Longitude of ascending node of planet's orbital plane");
  sprintf(options[OPT_LONGA].cDefault,"0");
  options[OPT_LONGA].dDefault = 0.0;
  options[OPT_LONGA].iType = 2;
  options[OPT_LONGA].iMultiFile = 1;
//   options[OPT_LONGA].dNeg = DEGRAD;
//   sprintf(options[OPT_LONGA].cNeg,"Degrees");
  fnRead[OPT_LONGA] = &ReadLongA;

  sprintf(options[OPT_DYNELLIP].cName,"dDynEllip");
  sprintf(options[OPT_DYNELLIP].cDescr,"Planet's dynamical ellipticity");
  sprintf(options[OPT_DYNELLIP].cDefault,"0.00328");
  options[OPT_DYNELLIP].dDefault = 0.00328;
  options[OPT_DYNELLIP].iType = 2;
  options[OPT_DYNELLIP].iMultiFile = 1;
  fnRead[OPT_DYNELLIP] = &ReadDynEllip;

  sprintf(options[OPT_CALCDYNELLIP].cName,"bCalcDynEllip");
  sprintf(options[OPT_CALCDYNELLIP].cDescr,"Calculate dynamical ellipticity from RotRate");
  sprintf(options[OPT_CALCDYNELLIP].cDefault,"0");
  options[OPT_CALCDYNELLIP].dDefault = 0;
  options[OPT_CALCDYNELLIP].iType = 0;
  options[OPT_CALCDYNELLIP].iMultiFile = 1;
  fnRead[OPT_CALCDYNELLIP] = &ReadCalcDynEllip;



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

  sprintf(options[OPT_BODYTYPE].cName,"iBodyType");
  sprintf(options[OPT_BODYTYPE].cDescr,"BodyType");
  sprintf(options[OPT_BODYTYPE].cDefault,"0 Planet");
  options[OPT_BODYTYPE].dDefault = 0;
  options[OPT_BODYTYPE].iType = 1;
  options[OPT_BODYTYPE].iMultiFile = 1;
  fnRead[OPT_BODYTYPE] = &ReadBodyType;

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
  sprintf(options[OPT_ROTRATE].cDefault,"2*pi/day");
  options[OPT_ROTRATE].dDefault = 2*PI/DAYSEC;
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

  sprintf(options[OPT_UNITTEMP].cName,"sUnitTemp");
  sprintf(options[OPT_UNITTEMP].cDescr,"Temperature Units: Kelvin Celsius Farenheit");
  sprintf(options[OPT_UNITTEMP].cDefault,"Kelvin");
  options[OPT_UNITTEMP].iType = 3;

  /*
   *
   *   V
   *
   */

  sprintf(options[OPT_VERBOSE].cName,"iVerbose");
  sprintf(options[OPT_VERBOSE].cDescr,"Verbosity Level: 1-5");
  sprintf(options[OPT_VERBOSE].cDefault,"3");
  options[OPT_VERBOSE].iType = 1;

  sprintf(options[OPT_VISCUMAN].cName,"dViscUMan");
  sprintf(options[OPT_VISCUMAN].cDescr,"Upper mantle viscosity");
  sprintf(options[OPT_VISCUMAN].cDefault,"0");
  options[OPT_VISCUMAN].dDefault = 0;
  options[OPT_VISCUMAN].iType = 2;
  options[OPT_VISCUMAN].iMultiFile = 1;
//   options[OPT_VISCUMAN].bNeg = 0;
  fnRead[OPT_VISCUMAN] = &ReadViscUMan;

  /*
   *
   * X
   *
   */

  sprintf(options[OPT_XOBL].cName,"dXobl");
  sprintf(options[OPT_XOBL].cDescr,"Deitrick's X -- Unsuppoted!");
  sprintf(options[OPT_XOBL].cDefault,"-1");
  options[OPT_XOBL].dDefault = -1;
  options[OPT_XOBL].iType = 2;
  fnRead[OPT_XOBL] = &ReadXobl;

  /*
   *
   * Y
   *
   */

  sprintf(options[OPT_YOBL].cName,"dYobl");
  sprintf(options[OPT_YOBL].cDescr,"Deitrick's Y -- Unsuppoted!");
  sprintf(options[OPT_YOBL].cDefault,"-1");
  options[OPT_YOBL].dDefault = -1;
  options[OPT_YOBL].iType = 2;
  fnRead[OPT_YOBL] = &ReadYobl;

  /*
   *
   * Z
   *
   */

  sprintf(options[OPT_ZOBL].cName,"dZobl");
  sprintf(options[OPT_ZOBL].cDescr,"Deitrick's Z -- Unsuppoted!");
  sprintf(options[OPT_ZOBL].cDefault,"-1");
  options[OPT_ZOBL].dDefault = -1;
  options[OPT_ZOBL].iType = 2;
  fnRead[OPT_ZOBL] = &ReadZobl;

}

void InitializeOptions(OPTIONS *options,fnReadOption *fnRead) {
  int iBody,iOpt,iFile,iModule;

  /* Initialize all parameters describing the option's location */
  for (iOpt=0;iOpt<MODULEOPTEND;iOpt++) {
    memset(options[iOpt].cName,'\0',OPTLEN);
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

  /* Now populate entries for general options. */
  InitializeOptionsGeneral(options,fnRead);

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  InitializeOptionsEqtide(options,fnRead);
  InitializeOptionsRadheat(options,fnRead);
  InitializeOptionsDistOrb(options,fnRead);
  InitializeOptionsDistRot(options,fnRead);
  InitializeOptionsThermint(options,fnRead);
  InitializeOptionsAtmEsc(options,fnRead);
  InitializeOptionsStellar(options,fnRead);
  InitializeOptionsPoise(options,fnRead);
  InitializeOptionsBinary(options,fnRead);
  InitializeOptionsFlare(options,fnRead);
  InitializeOptionsGalHabit(options,fnRead);
  InitializeOptionsSpiNBody(options,fnRead);

}
