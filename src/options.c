/**
  @file options.c
  @brief All subroutines necessary to read in all options. Also
         monitor input files for mistakes and log all option data.
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date May 7 2014
*/

#include "vplanet.h"

/*
 *
 * Utility Subroutines
 *
 */

/* Is the current input file the primary, i.e. the one on the command
   line */

void NotPrimaryInput(int iFile, char cName[], char cFile[], int iLine,
                     int iVerbose) {
  if (iLine > 0 && iFile == 0) {
    if (iVerbose >= VERBINPUT) {
      fprintf(stderr, "ERROR: %s is not allowed in file %s.\n", cName, cFile);
    }
    LineExit(cFile, iLine);
  }
}

/* Commented lines start with a # */
int fbCommentedLine(char cLine[], int iLen) {
  int iPos;

  for (iPos = 0; iPos < iLen; iPos++) {
    if (!isspace(cLine[iPos])) {
      if (cLine[iPos] == 35) { // # is ASCII code 35
        return 1;
      } else {
        return 0;
      }
    }
  }

  return 0;
}

void GetLine(char *cFile, char *cOption, char **cLine, int *iLine,
             int iVerbose) {
  int iLen, bDone = 0, iLineTmp = 0;
  char cWord[LINE], cTmp[LINE];
  FILE *fp;

  *cLine = NULL;

  iLen = strlen(cOption);

  fp = fopen(cFile, "r");
  memset(cTmp, '\0', LINE);
  memset(cWord, '\0', OPTLEN);

  while (fgets(cTmp, LINE, fp) != NULL) {
    if (!fbCommentedLine(cTmp, LINE)) {
      sscanf(cTmp, "%s", cWord);
      if (memcmp(cWord, cOption, iLen + 1) == 0) {
        /* Parameter Found! */
        if (bDone) {
          if (iVerbose > VERBINPUT) {
            fprintf(stderr, "Multiple occurences of parameter %s found.\n",
                    cOption);
          }
          fprintf(stderr, "\t%s, lines: %d and %d\n", cFile, (*iLine + 1),
                  iLineTmp + 1);
          exit(1);
        }
        fvFormattedString(cLine, cTmp);
        *iLine = iLineTmp;
        bDone  = 1;
      }
    }
    iLineTmp++;
    memset(cTmp, '\0', LINE);
    memset(cWord, '\0', OPTLEN);
  }
  fclose(fp);
}

/* If the previous line ended in $, must find the next valid line
   (the next lines could be a # or blank). The search is made
   recursively, and cLine and *iLine are the line and line number,
   respectively. */

void GetNextValidLine(char cFile[], int iStart, char **cLine, int *iLine) {
  FILE *fp;
  int iPos, iLineTmp, ok = 1;
  char cTmp[LINE];

  fp     = fopen(cFile, "r");
  *iLine = 0;
  *cLine = NULL;

  for (iLineTmp = 0; iLineTmp < iStart; iLineTmp++) {
    if (fgets(cTmp, LINE, fp) == NULL) {
      fprintf(stderr, "ERROR: Unable to read next valid line.");
      LineExit(cFile, iStart);
    }
    fvFormattedString(cLine, cTmp);
    (*iLine)++;
  }

  /* If EOF, return */
  if (fgets(cTmp, LINE, fp) == NULL) {
    fvFormattedString(cLine, "null");
    fclose(fp);
    return;
  }

  /* Now check for blank line, comment (# = 35), continue ($ = 36)
     or blank line (line feed = 10). */

  for (iPos = 0; iPos < LINE; iPos++) {
    if (cTmp[iPos] == 36 || cTmp[iPos] == 35 || cTmp[iPos] == 10) {
      /* First character is a $, # or \n: continue */
      GetNextValidLine(cFile, iStart + 1, cLine, iLine);
      fclose(fp);
      return;
    }
    if (!isspace(cTmp[iPos])) {
      /* Found next valid line */
      fclose(fp);
      fvFormattedString(cLine, cTmp);
      return;
    }
  }
  /* If made it here, line was blank */
  GetNextValidLine(cFile, iStart + 1, cLine, iLine);
  fclose(fp);
}

/* Where is the first non-white-space character in a line? */

int GetPos(char cLine[]) {
  int iPos;

  for (iPos = 0; iPos < strlen(cLine); iPos++) {
    if (!isspace(cLine[iPos])) {
      return iPos;
    }
  }

  /* Shouldn't be possible to get here */
  return 0;
}

/* Separate a line into words. cInput is an array of strings, each
   containing one word. This routine also checks if the final word has
   a trailing $, if so, it is an array that continues to the next
   line. */

void GetWords(char *cLine, char cInput[MAXARRAY][OPTLEN], int *iNumWords,
              int *bContinue) {
  int iPos, iPosStart, iWord;
  char cTmp[OPTLEN];

  if (cLine == NULL) {
    *iNumWords = 0;
    *bContinue = 0;
    return;
  }

  // iPos0=GetPos(cLine);
  iWord = 0;
  /* Use GetPos to avoid white space */
  for (iPos = GetPos(cLine); iPos < strlen(cLine); iPos++) {
    // for (iPos=GetPos(cLine);iPos<strlen(cLine)-GetPos(cLine);iPos++) {
    /* MEM: Something is wrong here, but it is intermittent. Sometimes a call
       here produces a memory error with valgrind. On 12/14/16 a run without the
       next print statements produced an error, but with them in, the error
       disappeared. After commenting out again, the problem was still gone. */
    /* DEBUG
    printf("%s\n",cLine);
    printf("%d %d %d\n",(int)strlen(cLine),GetPos(cLine),iPos);
    fflush(stdout);
    */
    iPosStart = 0;
    while (cLine[iPos] && !isspace(cLine[iPos])) {
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
        if (iPosStart == 0) {
          iWord--;
        }

        iPos = strlen(cLine);
        break;
      }
    }
    /* Now advance to next word */
    while (cLine[iPos] && isspace(cLine[iPos])) {
      iPos++;
    }

    iPos--;
    iWord++;
  }
  /* Is the last character a $? If so, remove it and adjust iNumWords */
  if (cInput[iWord - 1][strlen(cInput[iWord - 1]) - 1] == 36) {
    *bContinue = 1;
    if (strlen(cInput[iWord - 1]) == 1) {
      *iNumWords = iWord - 1;
    } else {
      *iNumWords = iWord;
    }
    cInput[iWord - 1][strlen(cInput[iWord - 1]) - 1] = '\0';
  } else {
    *bContinue = 0;
    *iNumWords = iWord;
  }
}

/* If a parameter wasn't found, print a warning if iVerbose is large
   enough. */

void PrintDefault(char cName[], char cDefault[], char cFile[], int iVerbose,
                  int iVerbThreshold) {
  if (iVerbose >= iVerbThreshold) {
    fprintf(stderr, "INFO: %s not set in file %s, defaulting to %s.\n", cName,
            cFile, cDefault);
  }
}

double dNegativeDouble(OPTIONS options, char cFile[], int iVerbose) {
  if (iVerbose >= VERBUNITS) {
    fprintf(stderr, "INFO: %s < 0 in file %s, units assumed to be %s.\n",
            options.cName, cFile, options.cNeg);
  }
  return -options.dNeg;
}

/* Get all fields in a string array. The fields are stored in saInput,
   and the lines which were read are in iNumLines. If a parameter is
   not found, saInput is an array of empty strings, and iLine is
   unchanged. */

void AddOptionStringArray(char *cFile, char *cOption, char ***saInput,
                          int *iNumIndices, int *iNumLines, int *iLine,
                          int iVerbose) {
  char *cLine, cTmp[MAXARRAY][OPTLEN], **saInputCopy;
  int iPos, iWord, bContinue, iNumWords;
  FILE *fp;

  iLine[0] = -1;

  /* Fencepost problem. If saInput continues across multiple lines,
     then we must keep getting more lines. For the first line, we
     remove the first word, as it is cOption. iLine must come
     preassigned and set to 0. */

  /* Initialize the array to have nothing in it. */
  for (iPos = 0; iPos < MAXARRAY; iPos++) {
    memset(cTmp[iPos], '\0', OPTLEN);
  }

  GetLine(cFile, cOption, &cLine, &iLine[0], iVerbose);
  GetWords(cLine, cTmp, &iNumWords, &bContinue);
  *iNumLines = 1;
  saInputCopy = (char **)malloc(MAXARRAY * sizeof(char *));

  for (iWord = 0; iWord < MAXARRAY; iWord++) {
    saInputCopy[iWord] = NULL;
  }

  for (iWord = 0; iWord < iNumWords - 1; iWord++) {

    fvFormattedString(&saInputCopy[iWord], cTmp[iWord + 1]);
    /* Reset cTmp string: If the next time cTmp is filled, the
       new string is longer than the old, then vestigial characters
       can remain after a trailing $. */
    memset(cTmp[iWord + 1], '\0', OPTLEN);
  }
  /* Now subtract off OptionName */
  *iNumIndices = iNumWords - 1;
  /* Reset first cTmp string */
  memset(cTmp[0], '\0', OPTLEN);

  /* Now keep getting lines until done */
  while (bContinue) {
    GetNextValidLine(cFile, iLine[*iNumLines - 1] + 1, &cLine,
                     &iLine[*iNumLines]);
    if (memcmp(cLine, "null", 4)) {
      GetWords(cLine, cTmp, &iNumWords, &bContinue);
      if (*iNumIndices + iNumWords > MAXARRAY) {
        fprintf(stderr,
                "ERROR: Too many arguments to %s. Either remove options, or "
                "increase value of MAXARRAY",
                cOption);
      }
      for (iWord = 0; iWord < iNumWords; iWord++) {
        fvFormattedString(&saInputCopy[*iNumIndices + iWord], cTmp[iWord]);
        memset(cTmp[iWord], '\0', OPTLEN);
      }
      *iNumIndices += iNumWords;
      (*iNumLines)++;
    } else {
      if (iVerbose >= VERBINPUT) {
        fprintf(stderr,
                "WARNING: Trailing $ found without a subsequent valid line for "
                "option %s in file %s.\n",
                cOption, cFile);
        bContinue = 0;
      }
    }
  }
  *saInput = saInputCopy;
}

/* Get all fields in a double array. The fields are stored in daInput,
   and the lines which were read are in iNumLines. If a parameter is
   not found, daInput is empty, and iLine is unchanged. */

void AddOptionDoubleArray(char *cFile, char *cOption, double *daInput,
                          int *iNumIndices, int *iNumLines, int *iLine,
                          int iVerbose) {
  int iIndex;
  char **saTmp;

  AddOptionStringArray(cFile, cOption, &saTmp, iNumIndices, iNumLines, iLine,
                       iVerbose);
  for (iIndex = 0; iIndex < *iNumIndices; iIndex++) {
    daInput[iIndex] = atof(saTmp[iIndex]);
  }
}

void AddOptionDouble(char *cFile, char *cOption, double *dInput, int *iLine,
                     int iVerbose) {
  char cTmp[OPTLEN], *cLine;

  GetLine(cFile, cOption, &cLine, iLine, iVerbose);
  if (*iLine >= 0) {
    sscanf(cLine, "%s %lf", cTmp, dInput);
  }
  free(cLine);
}

void AddOptionInt(char *cFile, char *cOption, int *iInput, int *iLine,
                  int iVerbose) {
  char cTmp[OPTLEN], *cLine;

  GetLine(cFile, cOption, &cLine, iLine, iVerbose);
  if (*iLine >= 0) {
    sscanf(cLine, "%s %d", cTmp, iInput);
  }
  free(cLine);
}

void AddOptionBool(char *cFile, char *cOption, int *iInput, int *iLine,
                   int iVerbose) {

  AddOptionInt(cFile, cOption, iInput, iLine, iVerbose);
  if (*iLine == -1) {
    return;
  }
  if (*iInput == 0 || *iInput == 1) {
    return;
  } else {
    if (iVerbose >= VERBERR) {
      fprintf(stderr, "ERROR: %s must be either 0 or 1.\n", cOption);
      LineExit(cFile, *iLine);
    }
  }
}

void AddOptionString(char *cFile, char *cOption, char cInput[], int *iLine,
                     int iVerbose) {
  char cTmp[OPTLEN], *cLine;

  memset(cTmp, '\0', OPTLEN);

  GetLine(cFile, cOption, &cLine, iLine, iVerbose);
  if (*iLine >= 0) {
    sscanf(cLine, "%s %s", cTmp, cInput);
  }
  free(cLine);
}

int iGetNumLines(char *cFile) {
  int iNumLines = 0, iChar, bFileOK = 1;
  int bComment, bReturn;
  FILE *fp;
  char cLine[LINE];

  fp = fopen(cFile, "r");
  if (fp == NULL) {
    fprintf(stderr, "Unable to open %s.\n", cFile);
    exit(EXIT_INPUT);
  }

  memset(cLine, '\0', LINE);
  while (fgets(cLine, LINE, fp) != NULL) {
    iNumLines++;

    /* Check to see if line is too long. The maximum length of a line is set
       by LINE. If a carriage return is not found in the first LINE
       characters *and* is not preceded by a comment, the line is too long. */
    bComment = 0;
    bReturn  = 0;
    for (iChar = 0; iChar < LINE && cLine[iChar] != '\0'; iChar++) {
      if (cLine[iChar] == 35) { // 35 is ASCII code for #
        bComment = 1;
      }
      // Maybe unnecessary with the second conditional in the loop
      // initialization?
      if (cLine[iChar] == 10) { // 10 is ASCII code for line feed
        bReturn = 1;
      }
    }

    if (!bReturn && !bComment) {
      if (iChar >= LINE) {
        fprintf(stderr,
                "ERROR: Line %s:%d is longer than allowed (%d characters).\n",
                cFile, iNumLines, LINE);
        bFileOK = 0;
      }
    }
    memset(cLine, '\0', LINE);
  }

  if (!bFileOK) {
    exit(EXIT_INPUT);
  }

  return iNumLines;
}

void CheckFileExists(char *cFile) {
  FILE *fp;

  fp = fopen(cFile, "r");
  if (fp == NULL) {
    fprintf(stderr, "ERROR: Unable to open %s for reading.\n", cFile);
    exit(EXIT_INPUT);
  }
  fclose(fp);
}

int fbBlankLine(char *cLine,int iLineLength) {
  int iPos,bBlank = 1;
  for (iPos = 0; iPos < LINE; iPos++) {
    if (!isspace(cLine[iPos]) && cLine[iPos] != '\0') {
      bBlank = 0;
    }
  }
  return bBlank;
}

void RecordCommentsAndWhiteSpace(INFILE *infile) {
  int iLine, iPos, bBlank;
  char cLine[LINE];
  FILE *fp;

  fp = fopen(infile->cIn, "r");
  infile->bLineOK   = malloc(infile->iNumLines * sizeof(int));

  for (iLine = 0; iLine < infile->iNumLines; iLine++) {
    infile->bLineOK[iLine] = 0;
    memset(cLine, '\0', LINE);

    // XXX Isn't this the same as CheckFileExists?
    if (fgets(cLine, LINE, fp) == NULL) {
      fprintf(stderr, "ERROR: Unable to open %s.\n", infile->cIn);
      exit(EXIT_INPUT);
    }
    if (fbCommentedLine(cLine, LINE) || fbBlankLine(cLine,LINE)) {
      infile->bLineOK[iLine] = 1;
    }
  }
}

void Unrecognized(FILES files) {
  FILE *fp;
  char cLine[LINE], cWord[NAMELEN];
  int iFile, iLine, bExit = 0; /* Assume don't exit */

  for (iFile = 0; iFile < files.iNumInputs; iFile++) {
    fp = fopen(files.Infile[iFile].cIn, "r");

    iLine = 0;
    while (fgets(cLine, LINE, fp) != NULL) {
      if (!files.Infile[iFile].bLineOK[iLine]) {
        /* Bad line */
        sscanf(cLine, "%s", cWord);
        fprintf(stderr, "ERROR: Unrecognized option \"%s\" in %s, line %d.\n",
                cWord, files.Infile[iFile].cIn, iLine + 1);
        bExit = 1;
      }
      memset(cLine, '\0', LINE);
      iLine++;
    }
  }
  if (bExit) {
    exit(EXIT_INPUT);
  }
}

void UpdateFoundOption(INFILE *input, OPTIONS *options, int iLine, int iFile) {
  input->bLineOK[iLine] = 1;
  options->iLine[iFile] = iLine;
  fvFormattedString(&options->cFile[iFile], input->cIn);
}

void UpdateFoundOptionMulti(INFILE *input, OPTIONS *options, int *iLine,
                            int iNumLines, int iFile) {
  int iLineNow;

  /*
    For now options->iLine is a scalar, so just use the first line.
    The user should be able to figure it out from there.
  */
  options->iLine[iFile] = iLine[0];
  for (iLineNow = 0; iLineNow < iNumLines; iLineNow++) {
    input->bLineOK[iLine[iLineNow]] = 1;
  }

  fvFormattedString(&options->cFile[iFile], input->cIn);
}

void CheckDuplication(FILES *files, OPTIONS *options, char cFile[], int iLine,
                      int iVerbose) {
  int iFile;

  if (options->bMultiFile) {
    fprintf(stderr,
            "ERROR: CheckDuplication called, but options.bMultiFile = %d\n",
            options->bMultiFile);
    exit(EXIT_INPUT);
  }

  for (iFile = 0; iFile < files->iNumInputs; iFile++) {
    if (options->iLine[iFile] >= 0 &&
        memcmp(files->Infile[iFile].cIn, cFile, strlen(cFile)) != 0) {
      /* Found previously set location */
      if (iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Option %s found in multiple files\n",
                options->cName);
      }
      fprintf(stderr, "\t%s, Line: %d\n", files->Infile[iFile].cIn,
              options->iLine[iFile]);
      fprintf(stderr, "\t%s, Line: %d\n", cFile, iLine);
      exit(EXIT_INPUT);
    }
  }
}

/*
 *
 * Initial Option Subroutines
 *
 */

void ReadVerbose(FILES *files, OPTIONS *options, int *iVerbose, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp, VERBALL);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp, VERBALL);
    if (iTmp < 0 || iTmp > VERBALL) {
      fprintf(stderr, "ERROR: %s must be in the range [0,%d]\n", options->cName,
              VERBALL);
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    if (*iVerbose == VERBALL) {
      fprintf(stderr, "INFO: -v set at command line, but %s option set.\n",
              options->cName);
      fprintf(stderr, "\tiVerbose is set to %d.\n", VERBALL);
    } else if (*iVerbose == 0) {
      /*
      These lines defeat the point of setting the ``-q`` flag!
      fprintf(stderr,"WARNING: -q set at command line, but %s option
      set.\n",options->cName); fprintf(stderr,"\tiVerbose is set to 0.\n");
      */
    } else {
      *iVerbose = iTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (*iVerbose ==
             -1) { // Was not set at command line, so set to default
    *iVerbose = atoi(options->cDefault);
  }
}

int iAssignMassUnit(char cTmp[], int iVerbose, char cFile[], char cName[],
                    int iLine) {
  if (memcmp(sLower(cTmp), "g", 1) == 0) {
    return U_GRAM;
  } else if (memcmp(sLower(cTmp), "k", 1) == 0) {
    return U_KILOGRAM;
  } else if (memcmp(sLower(cTmp), "s", 1) == 0) {
    return U_SOLARMASS;
  } else if (memcmp(sLower(cTmp), "e", 1) == 0) {
    return U_EARTHMASS;
  } else if (memcmp(sLower(cTmp), "j", 1) == 0) {
    return U_JUPITERMASS;
  } else if (memcmp(sLower(cTmp), "n", 1) == 0) {
    return U_NEPTUNEMASS;
  } else {
    if (iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Unknown argument to %s: %s. Options are: gm, kg, solar, "
              "Earth, Jupiter, Neptune.\n",
              cName, cTmp);
    }
    LineExit(cFile, iLine);
  }

  /* Whoops! */
  assert(0);

  return 0; // To avoid compiler warnings
}

void ReadUnitMass(CONTROL *control, FILES *files, OPTIONS *options, int iFile) {
  int iFileNow, lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (iFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr, "INFO: %s set in %s, all bodies will use this unit.\n",
                options->cName, files->Infile[iFile].cIn);
      }
      control->Units[iFile].iMass =
            iAssignMassUnit(cTmp, control->Io.iVerbose,
                            files->Infile[iFile].cIn, options->cName, lTmp);
      UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);

      for (iFileNow = 1; iFileNow < files->iNumInputs; iFileNow++) {
        control->Units[iFileNow].iMass = control->Units[iFile].iMass;
        /* Negative sign for lTmp indicated that the parameter was found in
         * another file */
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
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s found in primary and body files!\n",
                  options->cName);
        }
        fprintf(stderr, "\t%s, Line: %d\n", files->Infile[0].cIn,
                options->iLine[0]);
        fprintf(stderr, "\t%s, Line: %d\n", files->Infile[iFile].cIn, lTmp);
        exit(EXIT_INPUT);
      } else {
        /* Wasn't assigned in primary */
        control->Units[iFile].iMass =
              iAssignMassUnit(cTmp, control->Io.iVerbose,
                              files->Infile[iFile].cIn, options->cName, lTmp);
        UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
      }
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
        /* No, assign default */
        if (control->Io.iVerbose >= VERBUNITS) {
          fprintf(stderr, "INFO: %s not set in file %s, defaulting to %s.\n",
                  options->cName, files->Infile[iFile].cIn, options->cDefault);
        }
        control->Units[iFile].iMass =
              iAssignMassUnit(options->cDefault, control->Io.iVerbose,
                              files->Infile[iFile].cIn, options->cName, lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read
       */
    }
  }
}

int iAssignUnitTime(char cTmp[], int iVerbose, char cFile[], char cName[],
                    int iLine) {
  if (memcmp(sLower(cTmp), "s", 1) == 0) {
    return U_SECOND;
  } else if (memcmp(sLower(cTmp), "d", 1) == 0) {
    return U_DAY;
  } else if (memcmp(sLower(cTmp), "y", 1) == 0) {
    return U_YEAR;
  } else if (memcmp(sLower(cTmp), "m", 1) == 0) {
    return U_MYR;
  } else if (memcmp(sLower(cTmp), "g", 1) == 0) {
    return U_GYR;
  } else {
    if (iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Unknown argument to %s: %s. Options are seconds, days, "
              "years, Myr, or Gyr.\n",
              cName, cTmp);
    }
    LineExit(cFile, iLine);
  }

  /* Whoops! */
  assert(0);

  return 0; // To avoid compiler warnings
}


void ReadUnitTime(CONTROL *control, FILES *files, OPTIONS *options, int iFile) {
  int iFileNow, lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (iFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr, "INFO: %s set in %s, all bodies will use this unit.\n",
                options->cName, files->Infile[iFile].cIn);
      }
      control->Units[iFile].iTime =
            iAssignUnitTime(cTmp, control->Io.iVerbose,
                            files->Infile[iFile].cIn, options->cName, lTmp);
      UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);

      for (iFileNow = 1; iFileNow < files->iNumInputs; iFileNow++) {
        control->Units[iFileNow].iTime = control->Units[iFile].iTime;
        /* UpdateFoundOption(&files->Infile[iFileNow],options,-lTmp,iFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s found in primary and body files!\n",
                  options->cName);
        }
        fprintf(stderr, "\t%s, Line: %d\n", options->cFile[0],
                options->iLine[0]);
        fprintf(stderr, "\t%s, Line: %d\n", files->Infile[iFile].cIn, lTmp);
        exit(EXIT_INPUT);
      } else {
        /* Wasn't assigned in primary */
        control->Units[iFile].iTime =
              iAssignUnitTime(cTmp, control->Io.iVerbose,
                              files->Infile[iFile].cIn, options->cName, lTmp);
        UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
      }
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
        /* No, assign default */
        if (control->Io.iVerbose >= VERBUNITS) {
          fprintf(stderr, "INFO: %s not set in file %s, defaulting to %s.\n",
                  options->cName, files->Infile[iFile].cIn, options->cDefault);
        }
        control->Units[iFile].iTime =
              iAssignUnitTime(options->cDefault, control->Io.iVerbose,
                              files->Infile[iFile].cIn, options->cName, lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read
       */
    }
  }
}

int iAssignUnitAngle(char cTmp[], int iVerbose, char cFile[], char cName[],
                     int iLine) {
  if (memcmp(sLower(cTmp), "r", 1) == 0) {
    return U_RADIANS;
  } else if (memcmp(sLower(cTmp), "d", 1) == 0) {
    return U_DEGREES;
  } else {
    if (iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Unknown argument to %s: %s. Options are radians or "
              "degrees.\n",
              cName, cTmp);
    }
    LineExit(cFile, iLine);
  }

  /* Whoops! */
  assert(0);

  return 0; // To avoid compiler warnings
}

void ReadUnitAngle(CONTROL *control, FILES *files, OPTIONS *options,
                   int iFile) {
  int iFileNow, lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (iFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr, "INFO: %s set in %s, all bodies will use this unit.\n",
                options->cName, files->Infile[iFile].cIn);
      }
      control->Units[iFile].iAngle =
            iAssignUnitAngle(cTmp, control->Io.iVerbose,
                             files->Infile[iFile].cIn, options->cName, lTmp);
      UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);

      for (iFileNow = 1; iFileNow < files->iNumInputs; iFileNow++) {
        control->Units[iFileNow].iAngle = control->Units[iFile].iAngle;
        /* UpdateFoundOption(&files->Infile[iFileNow],options,-lTmp,iFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s found in primary and body files!\n",
                  options->cName);
        }
        fprintf(stderr, "\t%s, Line: %d\n", options->cFile[0],
                options->iLine[0]);
        fprintf(stderr, "\t%s, Line: %d\n", files->Infile[iFile].cIn, lTmp);
        exit(EXIT_INPUT);
      } else {
        /* Wasn't assigned in primary */
        control->Units[iFile].iAngle =
              iAssignUnitAngle(cTmp, control->Io.iVerbose,
                               files->Infile[iFile].cIn, options->cName, lTmp);
        UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
      }
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
        /* No, assign default */
        if (control->Io.iVerbose >= VERBUNITS) {
          fprintf(stderr, "INFO: %s not set in file %s, defaulting to %s.\n",
                  options->cName, files->Infile[iFile].cIn, options->cDefault);
        }
        control->Units[iFile].iAngle =
              iAssignUnitAngle(options->cDefault, control->Io.iVerbose,
                               files->Infile[iFile].cIn, options->cName, lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read
       */
    }
  }
}

int iAssignUnitLength(char cTmp[], int iVerbose, char cFile[], char cName[],
                      int iLine) {
  if (memcmp(sLower(cTmp), "c", 1) == 0) {
    return U_CENTIMETER;
  } else if (memcmp(sLower(cTmp), "m", 1) == 0) {
    return U_METER;
  } else if (memcmp(sLower(cTmp), "k", 1) == 0) {
    return U_KILOMETER;
  } else if (memcmp(sLower(cTmp), "s", 1) == 0) {
    return U_SOLARRADIUS;
  } else if (memcmp(sLower(cTmp), "e", 1) == 0) {
    return U_EARTHRADIUS;
  } else if (memcmp(sLower(cTmp), "j", 1) == 0) {
    return U_JUPRADIUS;
  } else if (memcmp(sLower(cTmp), "a", 1) == 0) {
    return U_AU;
  } else {
    if (iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Unknown argument to %s: %s. Options are cm, m, km, "
              "solar, Earth, Jupiter, AU.\n",
              cName, cTmp);
    }
    LineExit(cFile, iLine);
  }

  /* Whoops! */
  assert(0);

  return 0; // To avoid compiler warnings
}

void ReadUnitLength(CONTROL *control, FILES *files, OPTIONS *options,
                    int iFile) {
  int iFileNow, lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (iFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr, "INFO: %s set in %s, all bodies will use this unit.\n",
                options->cName, files->Infile[iFile].cIn);
      }
      control->Units[iFile].iLength =
            iAssignUnitLength(cTmp, control->Io.iVerbose,
                              files->Infile[iFile].cIn, options->cName, lTmp);
      UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);

      options->iLine[iFile] = lTmp;
      for (iFileNow = 1; iFileNow < files->iNumInputs; iFileNow++) {
        control->Units[iFileNow].iLength = control->Units[iFile].iLength;
        /* UpdateFoundOption(&files->Infile[iFileNow],options,lTmp,iFile); */
      }
    } /* If not set in primary file, do nothing */
  } else {
    /* Not in primary file */
    if (lTmp >= 0) {
      /* First check, was it set in primary file? */
      if (options->iLine[0] != -1) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s found in primary and body files!\n",
                  options->cName);
        }
        fprintf(stderr, "\t%s, Line: %d\n", options->cFile[0],
                options->iLine[0]);
        fprintf(stderr, "\t%s, Line: %d\n", files->Infile[iFile].cIn, lTmp);
        exit(EXIT_INPUT);
      } else {
        /* Wasn't assigned in primary */
        control->Units[iFile].iLength =
              iAssignUnitLength(cTmp, control->Io.iVerbose,
                                files->Infile[iFile].cIn, options->cName, lTmp);
        UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
      }
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
        /* No, assign default */
        if (control->Io.iVerbose >= VERBUNITS) {
          fprintf(stderr, "INFO: %s not set in file %s, defaulting to %s.\n",
                  options->cName, files->Infile[iFile].cIn, options->cDefault);
        }
        control->Units[iFile].iLength =
              iAssignUnitLength(options->cDefault, control->Io.iVerbose,
                                files->Infile[iFile].cIn, options->cName, lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read
       */
    }
  }
}

int iAssignTempUnit(char cTmp[], int iVerbose, char cFile[], char cName[],
                    int iLine) {
  if (memcmp(sLower(cTmp), "k", 1) == 0) {
    return U_KELVIN;
  } else if (memcmp(sLower(cTmp), "c", 1) == 0) {
    return U_CELSIUS;
  } else if (memcmp(sLower(cTmp), "f", 1) == 0) {
    return U_FARENHEIT;
  } else {
    if (iVerbose >= VERBERR) {
      fprintf(stderr,
              "ERROR: Unknown argument to %s: %s. Options are: Kelvin, "
              "Celsius, Farenheit.\n",
              cName, cTmp);
    }
    LineExit(cFile, iLine);
  }
  assert(0);

  return 0; // To avoid compiler warnings
}

void ReadUnitTemp(CONTROL *control, FILES *files, OPTIONS *options, int iFile) {
  int iFileNow, lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (iFile == 0) {
    if (lTmp >= 0) {
      /* This unit is propagated to all other files */
      /* Now assign the integer value */
      if (control->Io.iVerbose >= VERBINPUT) {
        fprintf(stderr, "INFO: %s set in %s, all bodies will use this unit.\n",
                options->cName, files->Infile[iFile].cIn);
      }
      control->Units[iFile].iTemp =
            iAssignTempUnit(cTmp, control->Io.iVerbose,
                            files->Infile[iFile].cIn, options->cName, lTmp);
      UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);

      for (iFileNow = 1; iFileNow < files->iNumInputs; iFileNow++) {
        control->Units[iFileNow].iTemp = control->Units[iFile].iTemp;
        /* Negative sign for lTmp indicated that the parameter was found in
         * another file */
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
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s found in primary and body files!\n",
                  options->cName);
        }
        fprintf(stderr, "\t%s, Line: %d\n", files->Infile[0].cIn,
                options->iLine[0]);
        fprintf(stderr, "\t%s, Line: %d\n", files->Infile[iFile].cIn, lTmp);
        exit(EXIT_INPUT);
      } else {
        /* Wasn't assigned in primary */
        control->Units[iFile].iTemp =
              iAssignTempUnit(cTmp, control->Io.iVerbose,
                              files->Infile[iFile].cIn, options->cName, lTmp);
        UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
      }
    } else {
      /* Not assigned in this file */
      /* Was it assigned in primary? */
      if (options->iLine[0] == -1) {
        /* No, assign default */
        if (control->Io.iVerbose >= VERBUNITS) {
          fprintf(stderr, "INFO: %s not set in file %s, defaulting to %s.\n",
                  options->cName, files->Infile[iFile].cIn, options->cDefault);
        }
        control->Units[iFile].iTemp =
              iAssignTempUnit(options->cDefault, control->Io.iVerbose,
                              files->Infile[iFile].cIn, options->cName, lTmp);
      }
      /* If assigned in primary, nothing to do, as assigned during primary read
       */
    }
  }
}

void ReadSystemName(CONTROL *control, FILES *files, OPTIONS *options,
                    SYSTEM *system, int iFile) {
  /* System Name */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    fvFormattedString(&system->cName, cTmp);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  }
}

void ReadBodyFileNames(BODY **body,CONTROL *control, FILES *files, OPTIONS *options,
                       char *cFile, char ***saBodyFiles, int *iNumLines, int *iaLines) {
  int iNumIndices;

  AddOptionStringArray(cFile, options->cName, saBodyFiles, &iNumIndices,
                       iNumLines, iaLines, control->Io.iVerbose);

  if (iaLines[0] >= 0) {
    if (iNumIndices == 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: No files supplied for option %s.\n",
                options->cName);
      }
      LineExit(cFile, iaLines[0]);
    }
    if (iNumIndices >= MAXFILES) {
      fprintf(stderr,
              "ERROR: Number of input files (%d) exceeds MAXFILES (%d).\n",
              iNumIndices, MAXFILES);
      fprintf(
            stderr,
            "Either use less body files, or increase MAXFILES in vplanet.h.\n");
      LineExit(cFile, iaLines[0]);
    }
  } else {
    fprintf(stderr, "ERROR: Option %s is required in file %s.\n",
            options->cName, cFile);
    exit(EXIT_INPUT);
  }

  control->Evolve.iNumBodies = iNumIndices;
  *body = malloc(control->Evolve.iNumBodies * sizeof(BODY));
}

/*
 *
 * Master subroutine for initial options
 *
 */

void ReadInitialOptions(BODY **body, CONTROL *control, FILES *files,
                        MODULE *module, OPTIONS *options, OUTPUT *output,
                        SYSTEM *system, char *sPrimaryFile) {
  int iFile, iBody, iModule,iNumBodyFileLines,*iaLines;
  char **saBodyFiles;

  iaLines=malloc(MAXLINES*sizeof(int));

  ReadBodyFileNames(body, control, files, &options[OPT_BODYFILES], sPrimaryFile, &saBodyFiles, &iNumBodyFileLines, iaLines);

  InitializeFiles(files, options, sPrimaryFile, saBodyFiles, control->Evolve.iNumBodies);

  UpdateFoundOptionMulti(&files->Infile[0], &options[OPT_BODYFILES], iaLines, iNumBodyFileLines,  0);

  /* Initialize functions in the module struct */
  InitializeModule(*body, control, module);

  /* Is iVerbose set ? */
  ReadVerbose(files, &options[OPT_VERBOSE], &control->Io.iVerbose, 0);

  /* We have to initialize other input files first */
  for (iFile = 1; iFile < files->iNumInputs; iFile++) {
    ReadVerbose(files, options, &control->Io.iVerbose, iFile);
  }

  /* Need units prior to any parameter read */
  control->Units = malloc(files->iNumInputs * sizeof(UNITS));

  /* Next we must find the units, modules, and system name */
  for (iFile = 0; iFile < files->iNumInputs; iFile++) {
    ReadUnitMass(control, files, &options[OPT_UNITMASS], iFile);
    ReadUnitTime(control, files, &options[OPT_UNITTIME], iFile);
    ReadUnitAngle(control, files, &options[OPT_UNITANGLE], iFile);
    ReadUnitLength(control, files, &options[OPT_UNITLENGTH], iFile);
    ReadUnitTemp(control, files, &options[OPT_UNITTEMP], iFile);
    ReadSystemName(control, files, &options[OPT_SYSTEMNAME], system, iFile);
    /* Get Modules first as it helps with verification
       ReadModules is in module.c */
    ReadModules(*body, control, files, module, &options[OPT_MODULES], iFile);
  }

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    FinalizeModule(*body, control, module, iBody);
  }

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    VerifyModuleCompatability(*body, control, files, module, options, iBody);
  }
  free(saBodyFiles);
  free(iaLines);
}

void AssignDefaultDouble(OPTIONS *options, double *dOption, int iNumFiles) {
  int iFile;

  for (iFile = 0; iFile < iNumFiles; iFile++) {
    if (options->iLine[iFile] != -1 && !options->bMultiFile) {
      /* Options already input in other file and can only be set once */
      return;
    }
  }

  /* If made it here, not input already, so assign default */
  *dOption = options->dDefault;
}

void AssignDefaultInt(OPTIONS *options, int *iOption, int iNumFiles) {
  int iFile;

  for (iFile = 0; iFile < iNumFiles; iFile++) {
    if (options->iLine[iFile] != -1 && !options->bMultiFile) {
      /* Options already input in other file and can only be set once */
      return;
    }
  }

  /* If made it here, not input already, so assign default */
  *iOption = atoi(options->cDefault);
}

void AssignDefaultString(OPTIONS *options, char cOption[], int iNumFiles) {
  int iFile;

  for (iFile = 0; iFile < iNumFiles; iFile++) {
    if (options->iLine[iFile] != -1 && !options->bMultiFile) {
      /* Options already input in other file and can only be set once */
      return;
    }
  }

  /* If made it here, not input already, so assign default */
  fvFormattedString(&cOption, options->cDefault);
}

int bOptionAlreadyFound(int *iLine, int iNumFiles) {
  int iFile;

  for (iFile = 0; iFile < iNumFiles; iFile++) {
    if (iLine[iFile] >= 0) {
      return 1;
    }
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

void ReadAge(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
             SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary input file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dAge =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dAge = dTmp * fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultDouble(options, &body[iFile - 1].dAge, files->iNumInputs);
  }
}

/* Albedo */

void ReadAlbedoGlobal(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary input file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      fprintf(stderr, "ERROR: %s cannot be negative.\n", options->cName);
      LineExit(files->Infile[iFile].cIn, lTmp);
    } else {
      body[iFile - 1].dAlbedoGlobal = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultDouble(options, &body[iFile - 1].dAlbedoGlobal,
                        files->iNumInputs);
  }
}

/* Body Type */

void ReadBodyType(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (iTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be non-negative.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    } else {
      body[iFile - 1].iBodyType = iTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultInt(options, &body[iFile - 1].iBodyType, files->iNumInputs);
  }
}

/*
 *
 * B
 *
 */

/* Do Backward Integration? */

void ReadDoBackward(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
    control->Evolve.bDoBackward = bTmp;
    if (control->Evolve.bDoBackward) {
      fprintf(stderr,
              "\nWARNING: Backward integrations have not been validated "
              "and may be unstable!\n");
      fprintf(stderr, "Use at your own risk.\n\n");
    }
  } else {
    AssignDefaultInt(options, &control->Evolve.bDoBackward, files->iNumInputs);
  }
}

/* Output File Name */

void ReadOutFile(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  int lTmp = -1;
  char cTmp[NAMELEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file -- Each body has an output file */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    fvFormattedString(&files->Outfile[iFile - 1].cOut, cTmp);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultString(options, files->Outfile[iFile - 1].cOut,
                        files->iNumInputs);
  }
}

/* Backward Eta */

void ReadEta(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
             SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    control->Evolve.dEta = dTmp;
    if (control->Evolve.dEta > 1 && control->Io.iVerbose >= VERBALL) {
      fprintf(stderr, "WARNING: %s > 1 is not advised (%s:%d).\n",
              options->cName, files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &control->Evolve.dEta, files->iNumInputs);
  }
}

/* Backward integration output interval */

void ReadOutputTime(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    /* Convert output time to cgs */
    control->Io.dOutputTime = dTmp * fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &control->Io.dOutputTime, files->iNumInputs);
  }
}

/* Backward integration stop time */

void ReadStopTime(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    /* Convert stop time to cgs */
    control->Evolve.dStopTime = dTmp * fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &control->Evolve.dStopTime, files->iNumInputs);
  }
}

/* Integration timestep */

void ReadTimeStep(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    /* Convert timestep to cgs */
    control->Evolve.dTimeStep = dTmp * fdUnitsTime(control->Units[iFile].iTime);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &control->Evolve.dTimeStep, files->iNumInputs);
  }
}

/* Do variable timestep? */

void ReadVarDt(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    control->Evolve.bVarDt = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &control->Evolve.bVarDt, files->iNumInputs);
  }
}

/* Body Name */

void ReadBodyName(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file -- Each body has an output file */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile-1].cName=NULL;
    if (strlen(cTmp) > 0) {
      fvFormattedString(&body[iFile - 1].cName, cTmp);
    } else {
      fvFormattedString(&body[iFile - 1].cName, "%d", iFile);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile-1].cName=NULL;
    fvFormattedString(&body[iFile - 1].cName, "%d", iFile);
  }
}

/* Body color (for plotting) */
void ReadColor(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile-1].sColor = NULL;
    fvFormattedString(&body[iFile - 1].sColor, cTmp);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile-1].sColor = NULL;
    fvFormattedString(&body[iFile - 1].sColor, options->cDefault);
  }
}

/*
 *
 * D
 *
 */

/* Digits */

void ReadDigits(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    control->Io.iDigits = iTmp;
    if (control->Io.iDigits < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be non-negative.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, options->iLine[iFile]);
    }
    if (control->Io.iDigits > 16) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be less than 17.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, options->iLine[iFile]);
    }
    control->Io.iDigits = iTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &control->Io.iDigits, files->iNumInputs);
  }
}


/*
 *
 * E
 *
 */

/* Eccentricity */

void ReadEcc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
             SYSTEM *system, int iFile) {
  /* Cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0 || dTmp >= 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be in the range [0,1).\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dEcc = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);

  } else if (iFile > 0) {
    AssignDefaultDouble(options, &body[iFile - 1].dEcc, files->iNumInputs);
  }
}

/**
Read the planet's initial gaseous envelope mass.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadEnvelopeMass(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dEnvelopeMass =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dEnvelopeMass = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dEnvelopeMass = options->dDefault;
  }
}


/*
 *
 * F
 *
 */


/* Forward integration? */

void ReadDoForward(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    control->Evolve.bDoForward = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    /* Set to default */
    AssignDefaultInt(options, &control->Evolve.bDoForward, files->iNumInputs);
  }
}

void ReadGRCorr(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].bGRCorr = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].bGRCorr = atoi(options->cDefault);
    }
  }
}

/*
 *
 *   HALT
 *
 */


/* Maximum Eccentricity */

void ReadHaltMaxEcc(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be in the range (0,1).\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    control->Halt[iFile - 1].dMaxEcc = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      control->Halt[iFile - 1].dMaxEcc = options->dDefault;
    }
    //       AssignDefaultDouble(options,&control->Halt[iFile-1].dMaxEcc,files->iNumInputs);
  }
}

/* Merge */

void ReadHaltMerge(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Merging is not allowed for the central body */
    if (iFile == 1) {
      fprintf(stderr,
              "ERROR: Cannot set %s for systems with more than 2 bodies.\n",
              options[OPT_HALTMERGE].cName);
      LineExit(files->Infile[iFile].cIn, lTmp);
    }

    control->Halt[iFile - 1].bMerge = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile == 1) {
      control->Halt[iFile - 1].bMerge = 0;
    }
    if (iFile > 1) {
      /* HaltMerge is unusual in that its default value depends on the body's
   modules. These are set in ReadInitialOptions, so they are always
   known by ReadOptionsGeneral. Therefore, we can assign it based on
   the "bModule" members of the body struct. */
      if (body[iFile - 1].bEqtide || body[iFile - 1].bDistOrb ||
          body[iFile - 1].bBinary) {
        control->Halt[iFile - 1].bMerge = 1;
      } else {
        control->Halt[iFile - 1].bMerge = 0;
      }
    }
  }
}

/**
  Read in the maximum allowed mutual inclination. This parameter applies to both
  SpiNBbody and DistOrb. If set to 0, then the mutual inclination will not be
  calculated every timestep in HaltMaxMutualIncSpiNBody or
  HaltMaxMutualIncDistorb. This parameter can exist in any file, but only once.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/

void ReadHaltMaxMutualInc(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {

  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    // System-wide halt, so stored in body 0
    control->Halt[0].dMaxMutualInc =
          dTmp * fdUnitsAngle(control->Units[iFile].iAngle);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    /*
    if (iFile > 0) {
      control->Halt[iFile+1].dMaxMutualInc = options->dDefault;
    }
    */
  }
}

/* Minimum Eccentricity */

void ReadHaltMinEcc(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be in the range (0,1).\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    control->Halt[iFile - 1].dMinEcc = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultDouble(options, &control->Halt[iFile - 1].dMinEcc,
                          files->iNumInputs);
    }
  }
}


/* Need to add ReadHaltDblSync function, since halt.bDblSync is never
 * initialized anywhere! */


/* Minimum obliquity */

void ReadHaltMinObl(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Cannot exist in primary input file */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr, "ERROR: %s must be in the range [0,PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBINPUT) {
          fprintf(stderr, "ERROR: %s must be in the range [0,180].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    control->Halt[iFile - 1].dMinObl = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      control->Halt[iFile - 1].dMinObl = options->dDefault;
    }
  }
}

/* Minimum Semi-Major Axis */

void ReadHaltMinSemi(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be larger than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    control->Halt[iFile - 1].dMinSemi =
          dTmp * fdUnitsLength(control->Units[iFile].iLength);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultDouble(options, &control->Halt[iFile - 1].dMinSemi,
                          files->iNumInputs);
    }
  }
}

/* Positive de/dt */

void ReadHaltPosDeDt(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    control->Halt[iFile - 1].bPosDeDt = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultInt(options, &control->Halt[iFile - 1].bPosDeDt,
                       files->iNumInputs);
    }
  }
}

/*
 *
 * H
 *
 */

/* Hecc -- currently this is not supported. */
void ReadHecc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr, "ERROR: Option %s is not currently supported.\n",
            options->cName);
    exit(EXIT_INPUT);
  } else if (iFile > 0) {
    body[iFile - 1].dHecc = options->dDefault;
  }
}

/*
 *
 * I
 *
 */

/* Integration Method */
void ReadIntegrationMethod(BODY *body, CONTROL *control, FILES *files,
                           OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int i, lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (memcmp(sLower(cTmp), "e", 1) == 0) {
      control->Evolve.iOneStep = EULER;
    } else if (memcmp(sLower(cTmp), "r", 1) == 0) {
      control->Evolve.iOneStep = RUNGEKUTTA;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Unknown argument to %s: %s.\n", options->cName,
                cTmp);
        fprintf(stderr, "Options are Euler.\n");
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  }
  /* If not input, VerifyIntegration assigns default */
}

/*
 *
 * K
 *
 */

/* Kecc -- currently unsupported. */
void ReadKecc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr, "ERROR: Option %s is not currently supported.\n",
            options->cName);
    exit(EXIT_INPUT);
  } else if (iFile > 0) {
    body[iFile - 1].dKecc = options->dDefault;
  }
}

/*
 *
 *   LOG
 *
 */

/* Do log file? */

void ReadDoLog(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    control->Io.bLog = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &control->Io.bLog, files->iNumInputs);
  }
}

/* Log file name */

void ReadLogFile(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int i, lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    fvFormattedString(&files->cLog, cTmp);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    /* Assign Default */
    if (iFile == files->iNumInputs - 1) {
      for (i = 0; i < files->iNumInputs; i++) {
        if (options->iLine[i] != -1) {
          /* Was aaigned, return */
          return;
        }
      }
    }
    /* Wasn't entered, assign default */
    fvFormattedString(&files->cLog, "%s.log", system->cName);
  }
}

/* Longitude of pericenter */
void ReadLongP(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2 * PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,2*PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,360].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dLongP = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dLongP = options->dDefault;
  }
}

/* Longitude of ascending node */

void ReadLongA(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2 * PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,2*PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,360].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dLongA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dLongA = options->dDefault;
  }
}


/* Argument of pericenter */

void ReadArgP(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {

  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2 * PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,2*PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,360].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dArgP = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dArgP = options->dDefault;
  }
}

/* Inclination */

void ReadInc(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
             SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,180].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dInc  = dTmp;
    body[iFile - 1].dSinc = sin(0.5 * dTmp);
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dInc  = options->dDefault;
    body[iFile - 1].dSinc = sin(0.5 * options->dDefault);
  }
}

void ReadLuminosity(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dLuminosity =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dLuminosity =
            dTmp * fdUnitsPower(control->Units[iFile - 1].iTime,
                                control->Units[iFile - 1].iMass,
                                control->Units[iFile - 1].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dLuminosity = options->dDefault;
    }
  }
}

/* LXUV -- currently unsupported */
void ReadLXUV(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr, "ERROR: Option %s is not currently supported.\n",
            options->cName);
    exit(EXIT_INPUT);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dLXUV = options->dDefault;
    }
  }
}

/*
 *
 *   M
 *
 */

/* Mass */

void ReadMass(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Density and MassRad */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dMass =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dMass = dTmp * fdUnitsMass(control->Units[iFile].iMass);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dMass = options->dDefault;
  }
}

/* Mass-Radius relationship */
void ReadMassRad(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Density and Mass */
  int lTmp = -1;
  char cTmp[OPTLEN];

  AddOptionString(files->Infile[iFile].cIn, options->cName, cTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (memcmp(sLower(cTmp), "r", 1) == 0) {
      /* Reid & Hawley 2000 */
      control->iMassRad[iFile - 1] = REIDHAWLEY;
    } else if (memcmp(sLower(cTmp), "g", 1) == 0) {
      /* Gorda and Svenchnikov 1999 */
      control->iMassRad[iFile - 1] = GORDASVECH99;
    } else if (memcmp(sLower(cTmp), "b", 1) == 0) {
      /* Bayless & Orosz 2006 */
      control->iMassRad[iFile - 1] = BAYLESSOROSZ06;
    } else if (memcmp(sLower(cTmp), "s", 1) == 0) {
      /* Sotin et al 2007 */
      control->iMassRad[iFile - 1] = SOTIN07;
    } else {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: Unknown argument to %s: %s.\n", options->cName,
                cTmp);
        fprintf(stderr, "Options are GS99 (Gorda & Svechnikov 1999), BO06 "
                        "(Bayless & Orosz 2006), Sotin07 (Sotin et al. 2007), "
                        "or RH00 (Reid & Hawley 2000).\n");
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    /* This one is weird, since the default is "none", but if this option
       is not set, then we need to set this variable to 0 for logging
       purposes. */
    if (iFile > 0) {
      control->iMassRad[iFile - 1] = 0;
    }
  }
}

/* Mean Motion */

void ReadMeanMotion(BODY *body, CONTROL *control, FILES *files,
                    OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dMeanMotion = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultDouble(options, &body[iFile - 1].dMeanMotion,
                          files->iNumInputs);
    }
  }
}

/**
Read the minimum surface water mass.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadMinSurfaceWaterMass(BODY *body, CONTROL *control, FILES *files,
                             OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dMinSurfaceWaterMass =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dMinSurfaceWaterMass = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dMinSurfaceWaterMass = options->dDefault;
  }
}

/**
Read the minimum envelope mass.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadMinEnvelopeMass(BODY *body, CONTROL *control, FILES *files,
                         OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dMinEnvelopeMass =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dMinEnvelopeMass = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dMinEnvelopeMass = options->dDefault;
  }
}


/* Minimum Value */

void ReadMinValue(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be larger than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    control->Evolve.dMinValue = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultDouble(options, &control->Evolve.dMinValue, files->iNumInputs);
  }
}

/*
 *
 * O
 *
 */

/* Obliquity */

void ReadObliquity(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 180) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,180].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }
    body[iFile - 1].dObliquity = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dObliquity = options->dDefault;
  }
}

/* cos(obliquity) */

void ReadCosObl(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < -1 || dTmp > 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be in the range [-1,1].\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dCosObl = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dCosObl = options->dDefault;
    }
  }
}

/*
 * Output Order -- This one is special and takes different arguments
 */


void ReadOutputOrder(FILES *files, MODULE *module, OPTIONS *options,
                     OUTPUT *output, int iFile, int iVerbose) {
  int i, j, count, iLen, iNumIndices = 0, bNeg[MAXARRAY], ok = 1, iNumGrid = 0,iOption;
  int k, iOut         = -1, *lTmp, iCol, jCol;
  char **saTmp, *cTmp = NULL, **cOption, *cOut = NULL;
  int iLen1, iLen2;

  lTmp    = malloc(MAXLINES * sizeof(int));
  cOption = malloc(MAXARRAY * sizeof(char *));
  for (iOption=0;iOption<MAXARRAY;iOption++) {
    cOption[iOption]=NULL;
  }

  AddOptionStringArray(files->Infile[iFile].cIn, options[OPT_OUTPUTORDER].cName,
                       &saTmp, &iNumIndices, &files->Infile[iFile].iNumLines,
                       lTmp, iVerbose);

  if (lTmp[0] >= 0) {
    NotPrimaryInput(iFile, options[OPT_OUTPUTORDER].cName,
                    files->Infile[iFile].cIn, lTmp[0], iVerbose);

    if (iNumIndices >= MAXARRAY) {
      if (iVerbose >= VERBERR) {
        fprintf(stderr,
                "ERROR: Too many output options in file %s. Either reduce, or "
                "increase MAXARRAY in vplanet.h.\n",
                files->Infile[iFile].cIn);
      }
      exit(EXIT_INPUT);
    }

    /* First remove and record negative signs */
    for (i = 0; i < iNumIndices; i++) {
      if (saTmp[i][0] == 45) {
        /* Option is negative */
        bNeg[i] = 1;
        /* Now remove negative sign */
        for (j = 0; j < strlen(saTmp[i]); j++) {
          saTmp[i][j] = saTmp[i][j + 1];
        }
        saTmp[i][strlen(saTmp[i])] = 0;
      } else {
        bNeg[i] = 0;
      }
    }

    /* Check for ambiguity */
    for (i = 0; i < iNumIndices; i++) {
      count = 0; /* Number of possibilities */
      // for (j = 0; j < OPTLEN; j++) {
      //   cTmp[j] = 0;
      // }
      fvFormattedString(&cTmp, saTmp[i]);
      for (j = 0; j < MODULEOUTEND; j++) {
        // for (k = 0; k < OPTLEN; k++) {
        //   cOut[k] = 0;
        // }
        fvFormattedString(&cOut, output[j].cName);
        iLen1 = strlen(cOut);
        iLen2 = strlen(cTmp);
        /* Check for perfect match */
        if ((iLen1 == iLen2) &&
            (memcmp(sLower(cTmp), sLower(cOut), strlen(cOut)) == 0)) {
          /* Output option found! */
          fvFormattedString(&cOption[count], output[j].cName);
          count = 1;
          iOut  = j;
          if (output[j].bGrid == 1) {
            // Exit!
            iNumGrid += 1;
          }
          j = MODULEOUTEND; /* Poor man's break! */
        } else {
          if (iLen1 < iLen2) {
            iLen = iLen1;
          } else {
            iLen = iLen2;
          }

          if (memcmp(sLower(cTmp), sLower(cOut), iLen) == 0 && iLen1 > iLen2) {
            /* Output option found! */
            fvFormattedString(&cOption[count], output[j].cName);
            count++;
            iOut = j;
            if (output[j].bGrid == 1) {
              // Exit!
              iNumGrid += 1;
            }
          }
        }
      }

      if (count > 1) {
        /* More than one possibility */
        if (iVerbose >= VERBERR) {
          fprintf(stderr,
                  "ERROR: Output option \"%s\" is ambiguous. Options are ",
                  saTmp[i]);
          for (j = 0; j < count; j++) {
            fprintf(stderr, "%s", cOption[j]);
            if (j < count - 1) {
              fprintf(stderr, ", ");
            }
          }
          fprintf(stderr, ".\n");
        }
        LineExit(files->Infile[iFile].cIn, lTmp[0]);
      }

      if (!count) {
        /* Option not found */
        if (iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: Unknown output option \"%s\".\n", saTmp[i]);
        }
        LineExit(files->Infile[iFile].cIn, lTmp[0]);
      }

      if (count == 1) {
        /* Unique option */

        /* Verify and record negative options */
        if (bNeg[i]) {
          // Is the negative option allowed?
          if (!output[iOut].bNeg) { /* No */
            if (iVerbose >= VERBERR) {
              fprintf(stderr, "ERROR: Output option %s ", saTmp[i]);
              if (strlen(saTmp[i]) < strlen(output[iOut].cName)) {
                fprintf(stderr, "(= %s) ", output[iOut].cName);
              }
              fprintf(stderr, "cannot be negative.\n");
            }
            LineExit(files->Infile[iFile].cIn, lTmp[0]);
          } else { // Yes, initialize bDoNeg to true
            output[iOut].bDoNeg[iFile - 1] = 1;
          }
        } else { // Negative option not set, initialize bDoNeg to false
          output[iOut].bDoNeg[iFile - 1] = 0;
        }
        if (output[iOut].bGrid == 0 || output[iOut].bGrid == 2) {
          fvFormattedString(&files->Outfile[iFile - 1].caCol[i],
                            output[iOut].cName);
        } else {
          memset(files->Outfile[iFile - 1].caGrid[iNumGrid - 1], '\0', OPTLEN);
          fvFormattedString(&files->Outfile[iFile - 1].caGrid[iNumGrid - 1],
                            output[iOut].cName);
        }
        // Is option part of selected modules?
        if (module->iBitSum[iFile - 1] & output[iOut].iModuleBit) {
          // Parameter is part of selected modules
        } else {
          fprintf(stderr, "ERROR: Output parameter %s requires module(s): ",
                  output[iOut].cName);
          PrintModuleList(stderr, output[iOut].iModuleBit, 0);
          fprintf(stderr, "\n");
          ok = 0;
        }
      }
    }

    if (!ok) {
      DoubleLineExit(files->Infile[iFile].cIn, files->Infile[iFile].cIn,
                     lTmp[0], options[OPT_MODULES].iLine[iFile]);
    }

    /* Check for duplicate columns, which is not allowed becaue it would be
       incompatible with BigPlanet's employment of Pandas data frames. */
    if (iNumIndices > 0) {
      ok = 1;
      for (iCol = 0; iCol < iNumIndices; iCol++) {
        for (jCol = iCol + 1; jCol < iNumIndices; jCol++) {
          iLen1 = strlen(files->Outfile[iFile - 1].caCol[iCol]);
          iLen2 = strlen(files->Outfile[iFile - 1].caCol[jCol]);
          /* Check for perfect match */
          if ((iLen1 == iLen2) &&
              (memcmp(files->Outfile[iFile - 1].caCol[iCol],
                      files->Outfile[iFile - 1].caCol[jCol],
                      strlen(files->Outfile[iFile - 1].caCol[iCol])) == 0)) {
            fprintf(stderr,
                    "ERROR: Output option %s selected twice, which is "
                    "not allowed.\n",
                    files->Outfile[iFile - 1].caCol[iCol]);
            ok = 0;
          }
        }
      }

      if (!ok) {
        LineExit(files->Infile[iFile].cIn, lTmp[0]);
      }
    }

    // All checks pass, update structs
    files->Outfile[iFile - 1].iNumCols = iNumIndices;
    /*
    files->Outfile[iFile-1].iNumCols = iNumIndices-iNumGrid;
    files->Outfile[iFile-1].iNumGrid = iNumGrid;
    */
    UpdateFoundOptionMulti(&files->Infile[iFile], &options[OPT_OUTPUTORDER],
                           lTmp, files->Infile[iFile].iNumLines, iFile);
  } else {
    files->Outfile[iFile - 1].iNumCols = 0;
  }

  free(lTmp);
  free(cTmp);
  free(cOption);
  free(saTmp);
  free(cOut);
}

void ReadGridOutput(FILES *files, OPTIONS *options, OUTPUT *output, int iFile,
                    int iVerbose) {
  int i, j, count, iLen, iNumIndices = 0, bNeg[MAXARRAY], ok = 0, iNumGrid = 0,iOption;
  int k, iOut = -1, *lTmp;
  char **saTmp, *cTmp=NULL, **cOption, *cOut=NULL;
  int iLen1, iLen2;

  lTmp    = malloc(MAXLINES * sizeof(int));
  cOption = malloc(MAXARRAY * sizeof(char *));
  for (iOption=0;iOption<MAXARRAY;iOption++) {
    cOption[iOption]=NULL;
  }

  AddOptionStringArray(files->Infile[iFile].cIn, options[OPT_GRIDOUTPUT].cName,
                       &saTmp, &iNumIndices, &files->Infile[iFile].iNumLines,
                       lTmp, iVerbose);

  if (lTmp[0] >= 0) {
    NotPrimaryInput(iFile, options[OPT_GRIDOUTPUT].cName,
                    files->Infile[iFile].cIn, lTmp[0], iVerbose);

    /* First remove and record negative signs */
    for (i = 0; i < iNumIndices; i++) {
      if (saTmp[i][0] == 45) {
        /* Option is negative */
        bNeg[i] = 1;
        /* Now remove negative sign */
        for (j = 0; j < strlen(saTmp[i]); j++) {
          saTmp[i][j] = saTmp[i][j + 1];
        }
        saTmp[i][strlen(saTmp[i])] = 0;
      } else {
        bNeg[i] = 0;
      }
    }

    /* Check for ambiguity */
    for (i = 0; i < iNumIndices; i++) {
      count = 0; /* Number of possibilities */
      // for (j = 0; j < OPTLEN; j++) {
      //   cTmp[j] = 0;
      // }
      fvFormattedString(&cTmp, saTmp[i]);
      for (j = 0; j < MODULEOUTEND; j++) {
        // for (k = 0; k < OPTLEN; k++) {
        //   cOut[k] = 0;
        // }
        fvFormattedString(&cOut, output[j].cName);
        iLen1 = strlen(cOut);
        iLen2 = strlen(cTmp);
        /* Check for perfect match */
        if ((iLen1 == iLen2) &&
            (memcmp(sLower(cTmp), sLower(cOut), strlen(cOut)) == 0)) {
          /* Output option found! */
          fvFormattedString(&cOption[count], output[j].cName);
          count = 1;
          iOut  = j;
          if (output[j].bGrid == 1 || output[j].bGrid == 2) {
            iNumGrid += 1;
          }
          j = MODULEOUTEND; /* Break! */
        } else {
          if (iLen1 < iLen2) {
            iLen = iLen1;
          } else {
            iLen = iLen2;
          }

          if (memcmp(sLower(cTmp), sLower(cOut), iLen) == 0 && iLen1 > iLen2) {
            /* Output option found! */
            fvFormattedString(&cOption[count], output[j].cName);
            count++;
            iOut = j;
            if (output[j].bGrid == 1 || output[j].bGrid == 2) {
              iNumGrid += 1;
            }
          }
        }
      }

      if (count > 1) {
        /* More than one possibility */
        if (iVerbose >= VERBERR) {
          fprintf(stderr,
                  "ERROR: Output option \"%s\" is ambiguous. Options are ",
                  saTmp[i]);
          for (j = 0; j < count; j++) {
            fprintf(stderr, "%s", cOption[j]);
            if (j < count - 1) {
              fprintf(stderr, ", ");
            }
          }
          fprintf(stderr, ".\n");
        }
        LineExit(files->Infile[iFile].cIn, lTmp[0]);
      }

      if (!count) {
        /* Option not found */
        if (iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: Unknown output option \"%s\".\n", saTmp[i]);
        }
        LineExit(files->Infile[iFile].cIn, lTmp[0]);
      }

      if (count == 1) {
        /* Unique option */

        /* Verify and record negative options */
        if (bNeg[i]) {
          // Is the negative option allowed?
          if (!output[iOut].bNeg) { /* No */
            if (iVerbose >= VERBERR) {
              fprintf(stderr, "ERROR: Output option %s ", saTmp[i]);
              if (strlen(saTmp[i]) < strlen(output[iOut].cName)) {
                fprintf(stderr, "(= %s) ", output[iOut].cName);
              }
              fprintf(stderr, "cannot be negative.\n");
            }
            LineExit(files->Infile[iFile].cIn, lTmp[0]);
          } else { // Yes, initialize bDoNeg to true
            output[iOut].bDoNeg[iFile - 1] = 1;
          }
        } else { // Negative option not set, initialize bDoNeg to false
          output[iOut].bDoNeg[iFile - 1] = 0;
        }
        if (output[iOut].bGrid == 0) {
          memset(files->Outfile[iFile - 1].caCol[i], '\0', OPTLEN);
          fvFormattedString(&files->Outfile[iFile - 1].caCol[i],
                            output[iOut].cName);
        } else {
          fvFormattedString(&files->Outfile[iFile - 1].caGrid[iNumGrid - 1],
                            output[iOut].cName);
        }
      }
    }

    UpdateFoundOptionMulti(&files->Infile[iFile], &options[OPT_GRIDOUTPUT],
                           lTmp, files->Infile[iFile].iNumLines, iFile);
  }

  files->Outfile[iFile - 1].iNumGrid = iNumGrid;

  free(lTmp);
  free(cTmp);
  free(cOption);
}

void ReadOverwrite(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (control->Io.bOverwrite > -1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr,
                "INFO: -f set at command line, and %s set (%s:%d). Overwrite "
                "authorized.\n",
                options->cName, files->Infile[iFile].cIn, lTmp);
      }
    } else {
      control->Io.bOverwrite = bTmp;
      UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
    }
  } else if (control->Io.bOverwrite == -1) {
    AssignDefaultInt(options, &control->Io.bOverwrite, files->iNumInputs);
  }
}

/*
 *
 * P
 *
 */

/* Orbital Period */

void ReadOrbPeriod(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify with Semi and MeanMotion */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dOrbPeriod =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dOrbPeriod =
            dTmp * fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultDouble(options, &body[iFile - 1].dOrbPeriod,
                        files->iNumInputs);
  }
}

/* Precession parameter */

void ReadPrecA(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
               SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (control->Units[iFile].iAngle == 0) {
      if (dTmp < 0 || dTmp > 2 * PI) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,2*PI].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
    } else {
      if (dTmp < 0 || dTmp > 360) {
        if (control->Io.iVerbose >= VERBERR) {
          fprintf(stderr, "ERROR: %s must be in the range [0,360].\n",
                  options->cName);
        }
        LineExit(files->Infile[iFile].cIn, lTmp);
      }
      /* Change to radians */
      dTmp *= DEGRAD;
    }

    body[iFile - 1].dPrecA = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dPrecA = options->dDefault;
    }
  }
}

void ReadDynEllip(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* Cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    /* Option was found */
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0 || dTmp >= 1) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be in the range [0,1).\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dDynEllip = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);

  } else if (iFile > 0) {
    body[iFile - 1].dDynEllip = options->dDefault;
  }
}

void ReadCalcDynEllip(BODY *body, CONTROL *control, FILES *files,
                      OPTIONS *options, SYSTEM *system, int iFile) {
  int lTmp = -1, bTmp;
  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    /* Option was found */
    body[iFile - 1].bCalcDynEllip = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].bCalcDynEllip = options->dDefault;
  }
  //     AssignDefaultInt(options,&body[iFile-1].bCalcDynEllip,files->iNumInputs);
}


/*
 *
 * R
 *
 */

/* Radius */

void ReadRadius(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  /* Must verify in conjuction with Radius, Mass and MassRad */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dRadius =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dRadius =
            dTmp * fdUnitsLength(control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRadius = options->dDefault;
  }
}

/* Radius of Gyration */

void ReadRadiusGyration(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp <= 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than zero.\n",
                options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    body[iFile - 1].dRadGyra = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dRadGyra = options->dDefault;
  }
}

/* Rotation Period */

void ReadRotPeriod(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                   SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify in conjunction with RotRate, RotVel and ForceEqSpin */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dRotPer =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else if (iFile > 0) {
      body[iFile - 1].dRotPer = dTmp * fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  }
}

/* Rotational Frequency */

void ReadRotRate(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                 SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify in conjunction with RotPer, RotVel and ForceEqSpin */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be non-negative.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    if (iFile > 0) {
      body[iFile - 1].dRotRate =
            dTmp / fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  }
}

/* Rotational Velocity */

void ReadRotVel(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify in conjunction with RotPer, RotRate and ForceEqSpin */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dRotVel =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dRotVel = dTmp *
                                fdUnitsLength(control->Units[iFile].iLength) /
                                fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  }
}

/*
 *
 * S
 *
 */

/* Scientific Notation */

void ReadSciNot(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  int iTmp;

  AddOptionInt(files->Infile[iFile].cIn, options->cName, &iTmp, &lTmp,
               control->Io.iVerbose);
  if (lTmp >= 0) {
    CheckDuplication(files, options, files->Infile[iFile].cIn, lTmp,
                     control->Io.iVerbose);
    if (iTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be non-negative.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    if (iTmp > 16) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be less than 16.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }
    control->Io.iSciNot = iTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    AssignDefaultInt(options, &control->Io.iSciNot, files->iNumInputs);
  }
}

/* Semi-Major Axis */

void ReadSemiMajorAxis(BODY *body, CONTROL *control, FILES *files,
                       OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter can exist in any file, but only once */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dSemi =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dSemi =
            dTmp * fdUnitsLength(control->Units[iFile].iLength);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    AssignDefaultDouble(options, &body[iFile - 1].dSemi, files->iNumInputs);
  }
}

/**
Read the planet's initial surface water mass.

@param body A pointer to the current BODY instance
@param control A pointer to the integration CONTROL instance
@param files A pointer to the array of input FILES
@param options A pointer to the OPTIONS instance
@param system A pointer to the SYSTEM instance
@param iFile The current file number
*/
void ReadSurfaceWaterMass(BODY *body, CONTROL *control, FILES *files,
                          OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dSurfaceWaterMass =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dSurfaceWaterMass = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dSurfaceWaterMass = options->dDefault;
  }
}

void ReadTemperature(BODY *body, CONTROL *control, FILES *files,
                     OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dTemperature =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dTemperature = dTmp;
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else if (iFile > 0) {
    body[iFile - 1].dTemperature = options->dDefault;
  }
}

/* Tidal Q -- must be in options because it is relevant for thermint */

void ReadTidalQ(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      if (control->Io.iVerbose >= VERBERR) {
        fprintf(stderr, "ERROR: %s must be greater than 0.\n", options->cName);
      }
      LineExit(files->Infile[iFile].cIn, lTmp);
    }

    body[iFile - 1].dTidalQ = dTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].dTidalQ = options->dDefault;
    }
  }
}

// Use outer layer's tidal Q for the body's Q?
void ReadUseOuterTidalQ(BODY *body, CONTROL *control, FILES *files,
                        OPTIONS *options, SYSTEM *system, int iFile) {
  /* This parameter cannot exist in primary file */
  int lTmp = -1;
  int bTmp;

  AddOptionBool(files->Infile[iFile].cIn, options->cName, &bTmp, &lTmp,
                control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    body[iFile - 1].bUseOuterTidalQ = bTmp;
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      body[iFile - 1].bUseOuterTidalQ = 0; // Default to no
    }
  }
}

void ReadOptionsGeneral(BODY *body, CONTROL *control, FILES *files,
                        MODULE *module, OPTIONS *options, OUTPUT *output,
                        SYSTEM *system, fnReadOption fnRead[]) {
  /* Now get all other options, if not in MODULE mode */
  int iOpt, iFile;

  /* For each option, search the file for its occurence */
  /* is iLine necessary? */

  for (iFile = 0; iFile < files->iNumInputs; iFile++) {
    /* Start at 100 because 0-99 are reserved for initial options */
    for (iOpt = 100; iOpt < NUMOPT; iOpt++) {
      /* OutputOrder is special */
      if (options[iOpt].iType != -1 && iOpt != OPT_OUTPUTORDER &&
          iOpt != OPT_GRIDOUTPUT) {
        // printf("%d\n",iOpt);
        // fflush(stdout);
        fnRead[iOpt](body, control, files, &options[iOpt], system, iFile);
      }
    }
  }

  /* Read in output order */
  for (iFile = 1; iFile < files->iNumInputs; iFile++) {
    ReadOutputOrder(files, module, options, output, iFile,
                    control->Io.iVerbose);
    if (body[iFile - 1].bPoise) {
      ReadGridOutput(files, options, output, iFile, control->Io.iVerbose);
    } else {
      // Initialize iNumGrid to 0 so no memory issues
      files->Outfile[iFile - 1].iNumGrid = 0;
    }
  }
}

void ReadViscUMan(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
                  SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  /* Must verify modules: this is used when distrot+eqtide are called without
   * thermint */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    NotPrimaryInput(iFile, options->cName, files->Infile[iFile].cIn, lTmp,
                    control->Io.iVerbose);
    if (dTmp < 0) {
      body[iFile - 1].dViscUMan =
            dTmp * dNegativeDouble(*options, files->Infile[iFile].cIn,
                                   control->Io.iVerbose);
    } else {
      body[iFile - 1].dViscUMan = dTmp *
                                  fdUnitsLength(control->Units[iFile].iLength) /
                                  fdUnitsTime(control->Units[iFile].iTime);
    }
    UpdateFoundOption(&files->Infile[iFile], options, lTmp, iFile);
  } else {
    if (iFile > 0) {
      AssignDefaultDouble(options, &body[iFile - 1].dViscUMan,
                          files->iNumInputs);
    }
  }
}

void ReadOptionsModules(BODY *body, CONTROL *control, FILES *files,
                        MODULE *module, OPTIONS *options, SYSTEM *system,
                        fnReadOption fnRead[]) {
  int iBody, iModule;

  for (iBody = 0; iBody < control->Evolve.iNumBodies; iBody++) {
    for (iModule = 0; iModule < module->iNumModules[iBody]; iModule++) {
      module->fnReadOptions[iBody][iModule](body, control, files, options,
                                            system, fnRead, iBody);
    }
  }
}

/*
 *
 * X
 *
 */

/* Xobl -- currently this is not supported. */
void ReadXobl(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr, "ERROR: Option %s is not currently supported.\n",
            options->cName);
    exit(EXIT_INPUT);
  } else if (iFile > 0) {
    body[iFile - 1].dXobl = options->dDefault;
  }
}

/*
 *
 * Y
 *
 */

/* Yobl -- currently this is not supported.  */
void ReadYobl(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr, "ERROR: Option %s is not currently supported.\n",
            options->cName);
    exit(EXIT_INPUT);
  } else if (iFile > 0) {
    body[iFile - 1].dYobl = options->dDefault;
  }
}

/*
 *
 * Z
 *
 */

/* Zobl -- currently this is not supported.  */
void ReadZobl(BODY *body, CONTROL *control, FILES *files, OPTIONS *options,
              SYSTEM *system, int iFile) {
  /* This parameter cannot exist in the primary file */
  int lTmp = -1;
  double dTmp;

  AddOptionDouble(files->Infile[iFile].cIn, options->cName, &dTmp, &lTmp,
                  control->Io.iVerbose);
  if (lTmp >= 0) {
    fprintf(stderr, "ERROR: Option %s is not currently supported.\n",
            options->cName);
    exit(EXIT_INPUT);
  } else if (iFile > 0) {
    body[iFile - 1].dZobl = options->dDefault;
  }
}


/*
 *
 * Master Read Options Subroutine
 *
 */

void ReadOptions(BODY **body, CONTROL *control, FILES *files, MODULE *module,
                 OPTIONS *options, OUTPUT *output, SYSTEM *system,
                 UPDATE **update, fnReadOption fnRead[], char infile[]) {
  int iBody;

  /* Read options for files, units, verbosity, and system name. */
  ReadInitialOptions(body, control, files, module, options, output, system,
                     infile);

  InitializeSystem(*body, control,system);

  /* Now that we know how many bodies there are, initialize more features */
  *update = malloc(control->Evolve.iNumBodies * sizeof(UPDATE));

  // Assign MODULE pointers for each selected module
  AddModules(*body, control, module);

  /* Initialize module control */
  InitializeControl(control, module);

  /* Now read in multi-module options */
  ReadOptionsGeneral(*body, control, files, module, options, output, system,
                     fnRead);

  /* Read in module options */
  ReadOptionsModules(*body, control, files, module, options, system, fnRead);

  /* Any unrecognized options? */
  Unrecognized(*files);
}

/*
 *
 * Initialize Options
 *
 */

void InitializeOptionsGeneral(OPTIONS *options, fnReadOption fnRead[]) {

  /* Do System Name first as it is used by other options. */

  fvFormattedString(&options[OPT_SYSTEMNAME].cName, "sSystemName");
  fvFormattedString(&options[OPT_SYSTEMNAME].cDescr, "System Name");
  fvFormattedString(&options[OPT_SYSTEMNAME].cDefault,
                    "None - must be supplied");
  options[OPT_SYSTEMNAME].iModuleBit = 0;
  options[OPT_SYSTEMNAME].iType      = 3;
  options[OPT_SYSTEMNAME].iFileType  = 0;
  options[OPT_SYSTEMNAME].bNeg       = 0;

  /*
   *
   *   A
   *
   */

  fvFormattedString(&options[OPT_AGE].cName, "dAge");
  fvFormattedString(&options[OPT_AGE].cDescr, "System Age");
  fvFormattedString(&options[OPT_AGE].cDefault, "0");
  fvFormattedString(&options[OPT_AGE].cNeg, "Gyr");
  fvFormattedString(&options[OPT_AGE].cDimension, "time");
  options[OPT_AGE].dDefault   = 0;
  options[OPT_AGE].iType      = 2;
  options[OPT_AGE].iModuleBit = 0;
  options[OPT_AGE].bNeg       = 1;
  options[OPT_AGE].dNeg       = 1e9 * YEARSEC;
  options[OPT_AGE].iFileType  = 2;
  fnRead[OPT_AGE]             = &ReadAge;

  fvFormattedString(&options[OPT_ALBEDOGLOBAL].cName, "dAlbedoGlobal");
  fvFormattedString(&options[OPT_ALBEDOGLOBAL].cDescr,
                    "Globally averaged albedo");
  fvFormattedString(&options[OPT_ALBEDOGLOBAL].cDefault, "0.3");
  fvFormattedString(&options[OPT_ALBEDOGLOBAL].cDimension, "nd");
  options[OPT_ALBEDOGLOBAL].dDefault   = 0;
  options[OPT_ALBEDOGLOBAL].bMultiFile = 1;
  options[OPT_ALBEDOGLOBAL].iType      = 2;
  options[OPT_ALBEDOGLOBAL].iModuleBit = 0;
  options[OPT_ALBEDOGLOBAL].bNeg       = 0;
  options[OPT_ALBEDOGLOBAL].iFileType  = 1;
  fnRead[OPT_ALBEDOGLOBAL]             = &ReadAlbedoGlobal;

  /*
   *
   *   B
   *
   */

  fvFormattedString(&options[OPT_BACK].cName, "bDoBackward");
  fvFormattedString(&options[OPT_BACK].cDescr, "Do Backward Integration?");
  fvFormattedString(&options[OPT_BACK].cDefault, "No");
  options[OPT_BACK].iType      = 0;
  options[OPT_BACK].iModuleBit = 0;
  options[OPT_BACK].bNeg       = 0;
  options[OPT_BACK].iFileType  = 2;
  fnRead[OPT_BACK]             = &ReadDoBackward;

  fvFormattedString(&options[OPT_OUTFILE].cName, "sOutFile");
  fvFormattedString(&options[OPT_OUTFILE].cDescr, "Name of Output File");
  fvFormattedString(&options[OPT_OUTFILE].cDefault, "cSystemName.backward");
  options[OPT_OUTFILE].iType      = 3;
  options[OPT_OUTFILE].bMultiFile = 1;
  options[OPT_OUTFILE].iModuleBit = 0;
  options[OPT_OUTFILE].bNeg       = 0;
  options[OPT_OUTFILE].iFileType  = 2;
  fnRead[OPT_OUTFILE]             = &ReadOutFile;

  fvFormattedString(&options[OPT_ETA].cName, "dEta");
  fvFormattedString(&options[OPT_ETA].cDescr, "Variable Time Step Coefficient");
  fvFormattedString(&options[OPT_ETA].cDefault, "1");
  fvFormattedString(&options[OPT_ETA].cDimension, "nd");
  options[OPT_ETA].dDefault   = 1;
  options[OPT_ETA].iType      = 2;
  options[OPT_ETA].iModuleBit = 0;
  options[OPT_ETA].bNeg       = 0;
  options[OPT_ETA].iFileType  = 2;
  fnRead[OPT_ETA]             = &ReadEta;
  fvFormattedString(
        &options[OPT_ETA].cLongDescr,
        "The timestep will be set to %s times the smallest instantaneous "
        "timescale, \n"
        "i.e. min(x/(dx/dt) where x represents the primary variables.",
        options[OPT_ETA].cName);

  fvFormattedString(&options[OPT_OUTPUTTIME].cName, "dOutputTime");
  fvFormattedString(&options[OPT_OUTPUTTIME].cDescr, "Output Interval");
  fvFormattedString(&options[OPT_OUTPUTTIME].cDefault, "1 year");
  fvFormattedString(&options[OPT_OUTPUTTIME].cNeg, "Years");
  fvFormattedString(&options[OPT_OUTPUTTIME].cDimension, "time");
  options[OPT_OUTPUTTIME].dDefault   = YEARSEC;
  options[OPT_OUTPUTTIME].iType      = 2;
  options[OPT_OUTPUTTIME].iModuleBit = 0;
  options[OPT_OUTPUTTIME].bNeg       = 1;
  options[OPT_OUTPUTTIME].dNeg       = YEARSEC;
  options[OPT_OUTPUTTIME].iFileType  = 2;
  fnRead[OPT_OUTPUTTIME]             = &ReadOutputTime;

  fvFormattedString(&options[OPT_STOPTIME].cName, "dStopTime");
  fvFormattedString(&options[OPT_STOPTIME].cDescr, "Integration Stop Time");
  fvFormattedString(&options[OPT_STOPTIME].cDefault, "10 years");
  fvFormattedString(&options[OPT_STOPTIME].cNeg, "Years");
  fvFormattedString(&options[OPT_STOPTIME].cDimension, "time");
  options[OPT_STOPTIME].dDefault   = 10 * YEARSEC;
  options[OPT_STOPTIME].iType      = 2;
  options[OPT_STOPTIME].iModuleBit = 0;
  options[OPT_STOPTIME].bNeg       = 1;
  options[OPT_STOPTIME].dNeg       = YEARSEC;
  options[OPT_STOPTIME].iFileType  = 2;
  fnRead[OPT_STOPTIME]             = &ReadStopTime;

  fvFormattedString(&options[OPT_TIMESTEP].cName, "dTimeStep");
  fvFormattedString(&options[OPT_TIMESTEP].cDescr, "Integration Timestep");
  fvFormattedString(&options[OPT_TIMESTEP].cDefault, "1 year");
  fvFormattedString(&options[OPT_TIMESTEP].cNeg, "Years");
  fvFormattedString(&options[OPT_TIMESTEP].cDimension, "time");
  options[OPT_TIMESTEP].dDefault   = YEARSEC;
  options[OPT_TIMESTEP].iType      = 2;
  options[OPT_TIMESTEP].iModuleBit = 0;
  options[OPT_TIMESTEP].bNeg       = 0;
  options[OPT_TIMESTEP].dNeg       = YEARSEC;
  options[OPT_TIMESTEP].iFileType  = 2;
  fnRead[OPT_TIMESTEP]             = &ReadTimeStep;

  fvFormattedString(&options[OPT_VARDT].cName, "bVarDt");
  fvFormattedString(&options[OPT_VARDT].cDescr, "Use variable timestep?");
  fvFormattedString(&options[OPT_VARDT].cDefault, "None");
  options[OPT_VARDT].iType      = 0;
  options[OPT_VARDT].iModuleBit = 0;
  options[OPT_VARDT].bNeg       = 0;
  options[OPT_VARDT].iFileType  = 2;
  fnRead[OPT_VARDT]             = &ReadVarDt;

  fvFormattedString(&options[OPT_BODYFILES].cName, "saBodyFiles");
  fvFormattedString(&options[OPT_BODYFILES].cDescr,
                    "Input files for each body");
  fvFormattedString(&options[OPT_BODYFILES].cDefault, "None");
  options[OPT_BODYFILES].iModuleBit = 0;
  options[OPT_BODYFILES].bNeg       = 0;
  options[OPT_BODYFILES].iFileType  = 0;
  options[OPT_BODYFILES].iType      = 13;

  fvFormattedString(&options[OPT_BODYNAME].cName, "sName");
  fvFormattedString(&options[OPT_BODYNAME].cDescr, "Body's Name");
  fvFormattedString(&options[OPT_BODYNAME].cDefault,
                    "Integer of Input Order, i.e. 1");
  options[OPT_BODYNAME].iType      = 3;
  options[OPT_BODYNAME].iModuleBit = 0;
  options[OPT_BODYNAME].bNeg       = 0;
  options[OPT_BODYNAME].iFileType  = 1;
  fnRead[OPT_BODYNAME]             = &ReadBodyName;

  /*
   *
   *   C
   *
   */

  fvFormattedString(&options[OPT_COLOR].cName, "sColor");
  fvFormattedString(&options[OPT_COLOR].cDescr,
                    "Hexadecimal color code for the body to be used in vplot");
  fvFormattedString(&options[OPT_COLOR].cDefault, "000000");
  options[OPT_COLOR].iType      = 3;
  options[OPT_COLOR].iModuleBit = 0;
  options[OPT_COLOR].bNeg       = 0;
  options[OPT_COLOR].iFileType  = 1;
  fnRead[OPT_COLOR]             = &ReadColor;

  /*
   *
   *   E
   *
   */

  /*
   *   FORWARD
   *
   */

  fvFormattedString(&options[OPT_FORW].cName, "bDoForward");
  fvFormattedString(&options[OPT_FORW].cDescr, "Do Forward Integration?");
  fvFormattedString(&options[OPT_FORW].cDefault, "0");
  options[OPT_FORW].iType      = 0;
  options[OPT_FORW].iModuleBit = 0;
  options[OPT_FORW].bNeg       = 0;
  options[OPT_FORW].iFileType  = 2;
  fnRead[OPT_FORW]             = &ReadDoForward;

  fvFormattedString(&options[OPT_GRCORR].cName, "bGRCorr");
  fvFormattedString(&options[OPT_GRCORR].cDescr,
                    "Use general relativity correction");
  fvFormattedString(&options[OPT_GRCORR].cDefault, "0");
  options[OPT_GRCORR].dDefault   = 0;
  options[OPT_GRCORR].iType      = 0;
  options[OPT_GRCORR].bMultiFile = 1;
  options[OPT_GRCORR].iModuleBit = DISTORB + DISTROT + SPINBODY;
  options[OPT_GRCORR].bNeg       = 0;
  options[OPT_GRCORR].iFileType  = 1;
  fnRead[OPT_GRCORR]             = &ReadGRCorr;

  /*
   *
   *   HALT
   *
   */

  fvFormattedString(&options[OPT_HALTMAXECC].cName, "dHaltMaxEcc");
  fvFormattedString(&options[OPT_HALTMAXECC].cDescr,
                    "Maximum eccentricity value that halts ntegration");
  fvFormattedString(&options[OPT_HALTMAXECC].cDefault, "1");
  fvFormattedString(&options[OPT_HALTMAXECC].cDimension, "nd");
  options[OPT_HALTMAXECC].dDefault   = 1;
  options[OPT_HALTMAXECC].iType      = 2;
  options[OPT_HALTMAXECC].iModuleBit = EQTIDE + DISTORB;
  options[OPT_HALTMAXECC].bNeg       = 0;
  options[OPT_HALTMAXECC].iFileType  = 2;
  fnRead[OPT_HALTMAXECC]             = &ReadHaltMaxEcc;

  fvFormattedString(&options[OPT_HALTMAXMUTUALINC].cName, "dHaltMaxMutualInc");
  fvFormattedString(&options[OPT_HALTMAXMUTUALINC].cDescr,
                    "Maximum mutual inclination value that halts integration");
  fvFormattedString(&options[OPT_HALTMAXMUTUALINC].cDefault, "0 [not checked]");
  fvFormattedString(&options[OPT_HALTMAXMUTUALINC].cDimension, "angle");
  options[OPT_HALTMAXMUTUALINC].dDefault   = 0;
  options[OPT_HALTMAXMUTUALINC].iType      = 2;
  options[OPT_HALTMAXMUTUALINC].iModuleBit = SPINBODY + DISTORB;
  options[OPT_HALTMAXMUTUALINC].bNeg       = 0;
  options[OPT_HALTMAXMUTUALINC].iFileType  = 2;
  fnRead[OPT_HALTMAXMUTUALINC]             = &ReadHaltMaxMutualInc;
  fvFormattedString(
        &options[OPT_HALTMAXMUTUALINC].cLongDescr,
        "The execution halts when dHaltMaxMutualInc is reached. The mutual "
        "(or\n"
        "relative) inclination is the value of the angle between the orbital\n"
        "angular momentum vector of two bodies. If set to 0, then the mutual\n"
        "inclination will not be checked every timestep for halts, i.e. the "
        "code\n"
        "should run faster. For DistOrb calculations with the RD4 %s model,\n"
        "mutual inclinations above %.2lf degrees, especially if the "
        "eccentricities\n"
        "are significant, should be interpreted cautiously. For the "
        "DistOrb-%s\n"
        "model, values above %.2lf degrees are suspect. SpiNBody is accurate "
        "for\n"
        "any value.",
        options[OPT_ORBITMODEL].cName, ((double)MAXMUTUALINCRD4),
        options[OPT_ORBITMODEL].cName, ((double)MAXMUTUALINCLL2));

  fvFormattedString(&options[OPT_HALTMERGE].cName, "bHaltMerge");
  fvFormattedString(&options[OPT_HALTMERGE].cDescr, "Halt at Merge");
  fvFormattedString(&options[OPT_HALTMERGE].cDefault,
                    "If eqtide or distorb called 1, else 0");
  options[OPT_HALTMERGE].iType      = 0;
  options[OPT_HALTMERGE].iModuleBit = EQTIDE + DISTORB + SPINBODY + STELLAR;
  options[OPT_HALTMERGE].bNeg       = 0;
  options[OPT_HALTMERGE].iFileType  = 2;
  fnRead[OPT_HALTMERGE]             = &ReadHaltMerge;

  fvFormattedString(&options[OPT_HALTMINECC].cName, "dHaltMinEcc");
  fvFormattedString(&options[OPT_HALTMINECC].cDescr,
                    "Minimum Eccentricity Value that Halts Integration");
  fvFormattedString(&options[OPT_HALTMINECC].cDefault, "-1");
  fvFormattedString(&options[OPT_HALTMINECC].cDimension, "nd");
  options[OPT_HALTMINECC].dDefault   = -1;
  options[OPT_HALTMINECC].iType      = 2;
  options[OPT_HALTMINECC].iModuleBit = EQTIDE + SPINBODY + DISTORB;
  options[OPT_HALTMINECC].bNeg       = 0;
  options[OPT_HALTMINECC].iFileType  = 2;
  fnRead[OPT_HALTMINECC]             = &ReadHaltMinEcc;

  fvFormattedString(&options[OPT_HALTMINOBL].cName, "dHaltMinObl");
  fvFormattedString(&options[OPT_HALTMINOBL].cDescr,
                    "Minimum Obliquity Value that Halts Integration");
  fvFormattedString(&options[OPT_HALTMINOBL].cDefault, "-1 degrees");
  fvFormattedString(&options[OPT_HALTMINOBL].cDimension, "angle");
  options[OPT_HALTMINOBL].dDefault   = -DEGRAD;
  options[OPT_HALTMINOBL].iType      = 2;
  options[OPT_HALTMINOBL].bMultiFile = 1;
  options[OPT_HALTMINOBL].iModuleBit = EQTIDE + SPINBODY + DISTORB;
  options[OPT_HALTMINOBL].bNeg       = 0;
  options[OPT_HALTMINOBL].iFileType  = 2;
  fnRead[OPT_HALTMINOBL]             = &ReadHaltMinObl;

  fvFormattedString(&options[OPT_HALTMINSEMI].cName, "dHaltMinSemi");
  fvFormattedString(&options[OPT_HALTMINSEMI].cDescr,
                    "Minimum Semi-Major Axis Value that Halts Integration");
  fvFormattedString(&options[OPT_HALTMINSEMI].cDefault, "0");
  fvFormattedString(&options[OPT_HALTMINSEMI].cNeg, "au");
  fvFormattedString(&options[OPT_HALTMINSEMI].cDimension, "length");
  options[OPT_HALTMINSEMI].dDefault   = 0;
  options[OPT_HALTMINSEMI].iType      = 2;
  options[OPT_HALTMINSEMI].dNeg       = AUM;
  options[OPT_HALTMINSEMI].iModuleBit = EQTIDE + SPINBODY;
  options[OPT_HALTMINSEMI].bNeg       = 1;
  options[OPT_HALTMINSEMI].iFileType  = 2;
  fnRead[OPT_HALTMINSEMI]             = &ReadHaltMinSemi;

  fvFormattedString(&options[OPT_HALTPOSDEDT].cName, "bHaltPosDeDt");
  fvFormattedString(&options[OPT_HALTPOSDEDT].cDescr, "Halt if de/dt > 0?");
  fvFormattedString(&options[OPT_HALTPOSDEDT].cDefault, "0");
  fvFormattedString(&options[OPT_HALTPOSDEDT].cDimension, "time^-1");
  options[OPT_HALTPOSDEDT].iType      = 0;
  options[OPT_HALTPOSDEDT].iModuleBit = EQTIDE + SPINBODY + DISTORB;
  options[OPT_HALTPOSDEDT].bNeg       = 0;
  options[OPT_HALTPOSDEDT].iFileType  = 2;
  fnRead[OPT_HALTPOSDEDT]             = &ReadHaltPosDeDt;

  /*
   *
   * H
   *
   */

  fvFormattedString(&options[OPT_HECC].cName, "dHecc");
  fvFormattedString(&options[OPT_HECC].cDescr, "Poincare's h -- Unsupported!");
  fvFormattedString(&options[OPT_HECC].cDefault, "-1");
  fvFormattedString(&options[OPT_HECC].cDimension, "nd");
  options[OPT_HECC].dDefault   = -1;
  options[OPT_HECC].iType      = 2;
  options[OPT_HECC].iModuleBit = DISTORB + SPINBODY;
  options[OPT_HECC].bNeg       = 0;
  options[OPT_HECC].iFileType  = 1;
  fnRead[OPT_HECC]             = &ReadHecc;

  /*
   *
   *   I
   *
   */

  fvFormattedString(&options[OPT_INTEGRATIONMETHOD].cName,
                    "sIntegrationMethod");
  fvFormattedString(
        &options[OPT_INTEGRATIONMETHOD].cDescr,
        "Integration Method: Euler, Runge-Kutta4 (Default = Runge-Kutta4)");
  fvFormattedString(&options[OPT_INTEGRATIONMETHOD].cDefault, "Runge-Kutta4");
  options[OPT_INTEGRATIONMETHOD].iType      = 3;
  options[OPT_INTEGRATIONMETHOD].iModuleBit = 0;
  options[OPT_INTEGRATIONMETHOD].bNeg       = 0;
  options[OPT_INTEGRATIONMETHOD].iFileType  = 2;
  fnRead[OPT_INTEGRATIONMETHOD]             = &ReadIntegrationMethod;

  /*
   *
   * K
   *
   */

  /* Someday...

  fvFormattedString(options[OPT_KECC].cName,"dKecc");
  fvFormattedString(options[OPT_KECC].cDescr,"Poincare's k -- Unsuppoted!");
  fvFormattedString(options[OPT_KECC].cDefault,"-1");
  options[OPT_KECC].dDefault = -1;
  options[OPT_KECC].iType = 2;
  options[OPT_KECC].iModuleBit = DISTORB + SPINBODY;
  options[OPT_KECC].bNeg = 0;
  options[OPT_KECC].iFileType = 1;
  fnRead[OPT_KECC] = &ReadKecc;

*/

  /*
   *
   *   L
   *
   */

  fvFormattedString(&options[OPT_LOG].cName, "bDoLog");
  fvFormattedString(&options[OPT_LOG].cDescr, "Write Log File?");
  fvFormattedString(&options[OPT_LOG].cDefault, "0");
  options[OPT_LOG].iType      = 0;
  options[OPT_LOG].iModuleBit = 0;
  options[OPT_LOG].bNeg       = 0;
  options[OPT_LOG].iFileType  = 2;
  fnRead[OPT_LOG]             = &ReadDoLog;

  fvFormattedString(&options[OPT_LOGFILE].cName, "sLogFile");
  fvFormattedString(&options[OPT_LOGFILE].cDescr, "Log File Name");
  options[OPT_LOGFILE].iType      = 3;
  options[OPT_LOGFILE].iModuleBit = 0;
  options[OPT_LOGFILE].bNeg       = 0;
  options[OPT_LOGFILE].iFileType  = 2;
  fnRead[OPT_LOGFILE]             = &ReadLogFile;

  fvFormattedString(&options[OPT_LONGP].cName, "dLongP");
  fvFormattedString(&options[OPT_LONGP].cDescr,
                    "Longitude of pericenter of planet's orbit");
  fvFormattedString(&options[OPT_LONGP].cDefault, "0");
  fvFormattedString(&options[OPT_LONGP].cDimension, "angle");
  options[OPT_LONGP].dDefault   = 0.0;
  options[OPT_LONGP].iType      = 2;
  options[OPT_LONGP].bMultiFile = 1;
  options[OPT_LONGP].iModuleBit = DISTORB + SPINBODY;
  options[OPT_LONGP].bNeg       = 0;
  options[OPT_LONGP].iFileType  = 1;
  fnRead[OPT_LONGP]             = &ReadLongP;

  fvFormattedString(&options[OPT_LUMINOSITY].cName, "dLuminosity");
  fvFormattedString(&options[OPT_LUMINOSITY].cDescr, "Initial Luminosity");
  fvFormattedString(&options[OPT_LUMINOSITY].cDefault, "0");
  fvFormattedString(&options[OPT_LUMINOSITY].cNeg, "LSUN");
  fvFormattedString(&options[OPT_LUMINOSITY].cDimension, "energy/time");
  options[OPT_LUMINOSITY].dDefault   = 0;
  options[OPT_LUMINOSITY].iType      = 2;
  options[OPT_LUMINOSITY].bMultiFile = 1;
  options[OPT_LUMINOSITY].dNeg       = LSUN;
  fnRead[OPT_LUMINOSITY]             = &ReadLuminosity;

  fvFormattedString(&options[OPT_LXUV].cName, "dLXUV");
  fvFormattedString(&options[OPT_LXUV].cDescr,
                    "Total XUV Luminosity -- Unsupported!");
  fvFormattedString(&options[OPT_LXUV].cDefault, "-1");
  fvFormattedString(&options[OPT_LXUV].cDimension, "energy/time");
  options[OPT_LXUV].dDefault   = -1;
  options[OPT_LXUV].iType      = 2;
  options[OPT_LXUV].iModuleBit = STELLAR;
  options[OPT_LXUV].bNeg       = 0;
  options[OPT_LXUV].iFileType  = 1;
  fnRead[OPT_LXUV]             = &ReadLXUV;

  /*
   *
   *   M
   *
   */

  fvFormattedString(&options[OPT_MINVALUE].cName, "dMinValue");
  fvFormattedString(&options[OPT_MINVALUE].cDescr,
                    "Minimum Non-Zero Value of Eccentricity and Obliquities");
  fvFormattedString(&options[OPT_MINVALUE].cDefault, "0");
  fvFormattedString(&options[OPT_MINVALUE].cDimension, "nd");
  options[OPT_MINVALUE].dDefault = 0;
  options[OPT_MINVALUE].iType    = 2;
  options[OPT_MINVALUE].iModuleBit =
        EQTIDE + SPINBODY + DISTORB + POISE + DISTROT;
  options[OPT_MINVALUE].bNeg      = 0;
  options[OPT_MINVALUE].iFileType = 2;
  fnRead[OPT_MINVALUE]            = &ReadMinValue;

  fvFormattedString(&options[OPT_MODULES].cName, "saModules");
  fvFormattedString(&options[OPT_MODULES].cDescr, "Body's Module List");
  fvFormattedString(&options[OPT_MODULES].cDefault, "none");
  options[OPT_MODULES].dDefault   = -1;
  options[OPT_MODULES].iModuleBit = 0;
  options[OPT_MODULES].bNeg       = 0;
  options[OPT_MODULES].iFileType  = 0;
  options[OPT_MODULES].iType      = 13;
  fvFormattedString(
        &options[OPT_MODULES].cLongDescr,
        "List of names of modules to be applied to the body. Spelling must be "
        "exact, but any capitalization works");

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

  fvFormattedString(&options[OPT_OUTDIGITS].cName, "iDigits");
  fvFormattedString(&options[OPT_OUTDIGITS].cDescr,
                    "Number of Digits After Decimal Point");
  fvFormattedString(&options[OPT_OUTDIGITS].cDefault, "4");
  options[OPT_OUTDIGITS].iType      = 1;
  options[OPT_OUTDIGITS].iModuleBit = 0;
  options[OPT_OUTDIGITS].bNeg       = 0;
  options[OPT_OUTDIGITS].iFileType  = 2;
  fnRead[OPT_OUTDIGITS]             = &ReadDigits;
  fvFormattedString(
        &options[OPT_OUTDIGITS].cLongDescr,
        "For all floating point output, print this many number of digits after "
        "the decimal point");

  fvFormattedString(&options[OPT_OUTPUTORDER].cName, "saOutputOrder");
  fvFormattedString(&options[OPT_OUTPUTORDER].cDescr, "Output Parameter(s)");
  fvFormattedString(&options[OPT_OUTPUTORDER].cDefault, "None");
  options[OPT_OUTPUTORDER].iType      = 13;
  options[OPT_OUTPUTORDER].iModuleBit = 0;
  options[OPT_OUTPUTORDER].bNeg       = 0;
  options[OPT_OUTPUTORDER].iFileType  = 1;
  options[OPT_OUTPUTORDER].bMultiFile = 1;

  fvFormattedString(&options[OPT_GRIDOUTPUT].cName, "saGridOutput");
  fvFormattedString(&options[OPT_GRIDOUTPUT].cDescr,
                    "Gridded Output Parameter(s)");
  fvFormattedString(&options[OPT_GRIDOUTPUT].cDefault, "None");
  options[OPT_GRIDOUTPUT].iType      = 13;
  options[OPT_GRIDOUTPUT].iModuleBit = POISE;
  options[OPT_GRIDOUTPUT].bNeg       = 0;
  options[OPT_GRIDOUTPUT].bMultiFile = 1;
  options[OPT_GRIDOUTPUT].iFileType  = 1;

  fvFormattedString(&options[OPT_OUTSCINOT].cName, "iSciNot");
  fvFormattedString(&options[OPT_OUTSCINOT].cDescr,
                    "Logarithm to Change from Standard to Scientific Notation");
  fvFormattedString(&options[OPT_OUTSCINOT].cDefault, "4");
  options[OPT_OUTSCINOT].iType      = 1;
  options[OPT_OUTSCINOT].iModuleBit = 0;
  options[OPT_OUTSCINOT].bNeg       = 0;
  options[OPT_OUTSCINOT].iFileType  = 2;
  fnRead[OPT_OUTSCINOT]             = &ReadSciNot;

  fvFormattedString(&options[OPT_OVERWRITE].cName, "bOverwrite");
  fvFormattedString(&options[OPT_OVERWRITE].cDescr, "Permit file overwrite?");
  fvFormattedString(&options[OPT_OVERWRITE].cDefault, "0");
  options[OPT_OVERWRITE].iType      = 0;
  options[OPT_OVERWRITE].iModuleBit = 0;
  options[OPT_OVERWRITE].bNeg       = 0;
  options[OPT_OVERWRITE].iFileType  = 2;
  fnRead[OPT_OVERWRITE]             = &ReadOverwrite;

  /*
   *
   *   ORBIT
   *
   */

  fvFormattedString(&options[OPT_ORBECC].cName, "dEcc");
  fvFormattedString(&options[OPT_ORBECC].cDescr, "Orbital Eccentricity");
  fvFormattedString(&options[OPT_ORBECC].cDefault, "0");
  fvFormattedString(&options[OPT_ORBECC].cDimension, "nd");
  options[OPT_ORBECC].dDefault   = 0;
  options[OPT_ORBECC].iType      = 2;
  options[OPT_ORBECC].iModuleBit = 0;
  options[OPT_ORBECC].bNeg       = 0;
  options[OPT_ORBECC].iFileType  = 1;
  fnRead[OPT_ORBECC]             = &ReadEcc;

  fvFormattedString(&options[OPT_ORBMEANMOTION].cName, "dMeanMotion");
  fvFormattedString(&options[OPT_ORBMEANMOTION].cDescr, "Orbital Mean Motion");
  fvFormattedString(&options[OPT_ORBMEANMOTION].cDefault, "1 /yr");
  fvFormattedString(&options[OPT_ORBMEANMOTION].cNeg, "/Year");
  fvFormattedString(&options[OPT_ORBMEANMOTION].cDimension, "time^-1");
  options[OPT_ORBMEANMOTION].dDefault   = 1. / YEARSEC;
  options[OPT_ORBMEANMOTION].iType      = 2;
  options[OPT_ORBMEANMOTION].dNeg       = 1. / YEARSEC;
  options[OPT_ORBMEANMOTION].iModuleBit = 0;
  options[OPT_ORBMEANMOTION].bNeg       = 1;
  options[OPT_ORBMEANMOTION].iFileType  = 1;
  fnRead[OPT_ORBMEANMOTION]             = &ReadMeanMotion;

  fvFormattedString(&options[OPT_ORBPER].cName, "dOrbPeriod");
  fvFormattedString(&options[OPT_ORBPER].cDescr, "Orbital Period");
  fvFormattedString(&options[OPT_ORBPER].cDefault, "1 year");
  fvFormattedString(&options[OPT_ORBPER].cNeg, "Days");
  fvFormattedString(&options[OPT_ORBPER].cDimension, "time");
  options[OPT_ORBPER].dDefault   = YEARSEC;
  options[OPT_ORBPER].iType      = 2;
  options[OPT_ORBPER].dNeg       = DAYSEC;
  options[OPT_ORBPER].iModuleBit = 0;
  options[OPT_ORBPER].bNeg       = 1;
  options[OPT_ORBPER].iFileType  = 1;
  fnRead[OPT_ORBPER]             = &ReadOrbPeriod;

  fvFormattedString(&options[OPT_ORBSEMI].cName, "dSemi");
  fvFormattedString(&options[OPT_ORBSEMI].cDescr, "Semi-Major Axis");
  fvFormattedString(&options[OPT_ORBSEMI].cDefault, "1 AU");
  fvFormattedString(&options[OPT_ORBSEMI].cNeg, "AU");
  fvFormattedString(&options[OPT_ORBSEMI].cDimension, "length");
  options[OPT_ORBSEMI].dDefault   = AUM;
  options[OPT_ORBSEMI].iType      = 2;
  options[OPT_ORBSEMI].dNeg       = AUM;
  options[OPT_ORBSEMI].bMultiFile = 1;
  options[OPT_ORBSEMI].iModuleBit = 0;
  options[OPT_ORBSEMI].bNeg       = 1;
  options[OPT_ORBSEMI].iFileType  = 1;
  fnRead[OPT_ORBSEMI]             = &ReadSemiMajorAxis;

  fvFormattedString(&options[OPT_INC].cName, "dInc");
  fvFormattedString(&options[OPT_INC].cDescr,
                    "Inclination of planet's orbital plane");
  fvFormattedString(&options[OPT_INC].cDefault, "0");
  fvFormattedString(&options[OPT_INC].cNeg, "Degrees");
  fvFormattedString(&options[OPT_INC].cDimension, "angle");
  options[OPT_INC].dDefault   = 0.0;
  options[OPT_INC].iType      = 2;
  options[OPT_INC].bMultiFile = 1;
  options[OPT_INC].dNeg       = DEGRAD;
  options[OPT_INC].iModuleBit = DISTORB + SPINBODY;
  options[OPT_INC].bNeg       = 0;
  options[OPT_INC].iFileType  = 1;
  fnRead[OPT_INC]             = &ReadInc;

  fvFormattedString(&options[OPT_ARGP].cName, "dArgP");
  fvFormattedString(&options[OPT_ARGP].cDescr,
                    "Argument of pericenter of planet's orbit");
  fvFormattedString(&options[OPT_ARGP].cDefault, "0");
  fvFormattedString(&options[OPT_ARGP].cNeg, "Degrees");
  fvFormattedString(&options[OPT_ARGP].cDimension, "angle");
  options[OPT_ARGP].dDefault   = 0.0;
  options[OPT_ARGP].iType      = 2;
  options[OPT_ARGP].bMultiFile = 1;
  options[OPT_ARGP].dNeg       = DEGRAD;
  options[OPT_ARGP].iModuleBit = DISTORB + SPINBODY + POISE;
  options[OPT_ARGP].bNeg       = 0;
  options[OPT_ARGP].iFileType  = 1;
  fnRead[OPT_ARGP]             = &ReadArgP;

  /*
   * P
   */

  fvFormattedString(&options[OPT_PRECA].cName, "dPrecA");
  fvFormattedString(&options[OPT_PRECA].cDescr, "Planet's precession angle");
  fvFormattedString(&options[OPT_PRECA].cDefault, "0");
  fvFormattedString(&options[OPT_PRECA].cNeg, "Degrees");
  fvFormattedString(&options[OPT_PRECA].cDimension, "angle");
  options[OPT_PRECA].dDefault   = 0.0;
  options[OPT_PRECA].iType      = 2;
  options[OPT_PRECA].bMultiFile = 1;
  options[OPT_PRECA].dNeg       = DEGRAD;
  options[OPT_PRECA].iModuleBit = SPINBODY + DISTROT + POISE;
  options[OPT_PRECA].bNeg       = 0;
  options[OPT_PRECA].iFileType  = 1;
  fnRead[OPT_PRECA]             = &ReadPrecA;

  fvFormattedString(&options[OPT_LONGA].cName, "dLongA");
  fvFormattedString(&options[OPT_LONGA].cDescr,
                    "Longitude of ascending node of planet's orbital plane");
  fvFormattedString(&options[OPT_LONGA].cDefault, "0");
  fvFormattedString(&options[OPT_LONGA].cNeg, "Degrees");
  fvFormattedString(&options[OPT_LONGA].cDimension, "angle");
  options[OPT_LONGA].dDefault   = 0.0;
  options[OPT_LONGA].iType      = 2;
  options[OPT_LONGA].bMultiFile = 1;
  options[OPT_LONGA].dNeg       = DEGRAD;
  options[OPT_LONGA].iModuleBit = SPINBODY + DISTROT + POISE;
  options[OPT_LONGA].bNeg       = 0;
  options[OPT_LONGA].iFileType  = 1;
  fnRead[OPT_LONGA]             = &ReadLongA;

  fvFormattedString(&options[OPT_DYNELLIP].cName, "dDynEllip");
  fvFormattedString(&options[OPT_DYNELLIP].cDescr,
                    "Planet's dynamical ellipticity");
  fvFormattedString(&options[OPT_DYNELLIP].cDefault, "0.00328");
  fvFormattedString(&options[OPT_DYNELLIP].cDimension, "nd");
  options[OPT_DYNELLIP].dDefault   = 0.00328;
  options[OPT_DYNELLIP].iType      = 2;
  options[OPT_DYNELLIP].bMultiFile = 1;
  options[OPT_DYNELLIP].iModuleBit = SPINBODY + DISTROT + THERMINT;
  options[OPT_DYNELLIP].bNeg       = 0;
  options[OPT_DYNELLIP].iFileType  = 1;
  fnRead[OPT_DYNELLIP]             = &ReadDynEllip;

  fvFormattedString(&options[OPT_CALCDYNELLIP].cName, "bCalcDynEllip");
  fvFormattedString(&options[OPT_CALCDYNELLIP].cDescr,
                    "Calculate dynamical ellipticity from RotRate");
  fvFormattedString(&options[OPT_CALCDYNELLIP].cDefault, "0");
  options[OPT_CALCDYNELLIP].dDefault   = 0;
  options[OPT_CALCDYNELLIP].iType      = 0;
  options[OPT_CALCDYNELLIP].bMultiFile = 1;
  options[OPT_CALCDYNELLIP].iModuleBit = SPINBODY + DISTROT + THERMINT;
  options[OPT_CALCDYNELLIP].bNeg       = 0;
  options[OPT_CALCDYNELLIP].iFileType  = 1;
  fnRead[OPT_CALCDYNELLIP]             = &ReadCalcDynEllip;

  fvFormattedString(&options[OPT_SURFACEWATERMASS].cName, "dSurfWaterMass");
  fvFormattedString(&options[OPT_SURFACEWATERMASS].cDescr,
                    "Initial Surface Water Mass");
  fvFormattedString(&options[OPT_SURFACEWATERMASS].cDefault, "0");
  fvFormattedString(&options[OPT_SURFACEWATERMASS].cNeg,
                    "Terrestrial Oceans (TO)");
  fvFormattedString(&options[OPT_SURFACEWATERMASS].cDimension, "mass");
  options[OPT_SURFACEWATERMASS].dDefault   = 0;
  options[OPT_SURFACEWATERMASS].iType      = 2;
  options[OPT_SURFACEWATERMASS].bMultiFile = 1;
  options[OPT_SURFACEWATERMASS].dNeg       = TOMASS;
  fnRead[OPT_SURFACEWATERMASS]             = &ReadSurfaceWaterMass;

  fvFormattedString(&options[OPT_MINSURFACEWATERMASS].cName,
                    "dMinSurfWaterMass");
  fvFormattedString(&options[OPT_MINSURFACEWATERMASS].cDescr,
                    "Minimum Surface Water Mass");
  fvFormattedString(&options[OPT_MINSURFACEWATERMASS].cDefault, "1.e-5 TO");
  fvFormattedString(&options[OPT_MINSURFACEWATERMASS].cNeg,
                    "Terrestrial Oceans (TO)");
  fvFormattedString(&options[OPT_MINSURFACEWATERMASS].cDimension, "mass");
  options[OPT_MINSURFACEWATERMASS].dDefault   = 1.e-5 * TOMASS;
  options[OPT_MINSURFACEWATERMASS].iType      = 2;
  options[OPT_MINSURFACEWATERMASS].dNeg       = TOMASS;
  options[OPT_MINSURFACEWATERMASS].bMultiFile = 1;
  fnRead[OPT_MINSURFACEWATERMASS]             = &ReadMinSurfaceWaterMass;

  fvFormattedString(&options[OPT_ENVELOPEMASS].cName, "dEnvelopeMass");
  fvFormattedString(&options[OPT_ENVELOPEMASS].cDescr, "Initial Envelope Mass");
  fvFormattedString(&options[OPT_ENVELOPEMASS].cDefault, "0");
  fvFormattedString(&options[OPT_ENVELOPEMASS].cNeg, "Mearth");
  fvFormattedString(&options[OPT_ENVELOPEMASS].cDimension, "mass");
  options[OPT_ENVELOPEMASS].dDefault   = 0;
  options[OPT_ENVELOPEMASS].iType      = 2;
  options[OPT_ENVELOPEMASS].bMultiFile = 1;
  options[OPT_ENVELOPEMASS].dNeg       = MEARTH;
  fnRead[OPT_ENVELOPEMASS]             = &ReadEnvelopeMass;

  fvFormattedString(&options[OPT_MINENVELOPEMASS].cName, "dMinEnvelopeMass");
  fvFormattedString(&options[OPT_MINENVELOPEMASS].cDescr,
                    "Minimum Envelope Mass");
  fvFormattedString(&options[OPT_MINENVELOPEMASS].cDefault, "1.e-8 Earth");
  fvFormattedString(&options[OPT_MINENVELOPEMASS].cNeg, "Mearth");
  fvFormattedString(&options[OPT_MINENVELOPEMASS].cDimension, "mass");
  options[OPT_MINENVELOPEMASS].dDefault = 1.e-8 * MEARTH;
  options[OPT_MINENVELOPEMASS].iType    = 2;
  options[OPT_MINENVELOPEMASS].dNeg     = MEARTH;
  fnRead[OPT_MINENVELOPEMASS]           = &ReadMinEnvelopeMass;

  /*
   *
   *   BODY PROPERTIES
   *
   */

  fvFormattedString(&options[OPT_MASS].cName, "dMass");
  fvFormattedString(&options[OPT_MASS].cDescr, "Mass");
  fvFormattedString(&options[OPT_MASS].cDefault, "1 Earth Mass");
  fvFormattedString(&options[OPT_MASS].cNeg, "Mearth");
  fvFormattedString(&options[OPT_MASS].cDimension, "mass");
  options[OPT_MASS].dDefault   = MEARTH;
  options[OPT_MASS].iType      = 2;
  options[OPT_MASS].bMultiFile = 1;
  options[OPT_MASS].dNeg       = MEARTH;
  options[OPT_MASS].iModuleBit = 0;
  options[OPT_MASS].bNeg       = 1;
  options[OPT_MASS].iFileType  = 1;
  fnRead[OPT_MASS]             = &ReadMass;

  fvFormattedString(&options[OPT_BODYTYPE].cName, "iBodyType");
  fvFormattedString(&options[OPT_BODYTYPE].cDescr, "BodyType");
  fvFormattedString(&options[OPT_BODYTYPE].cDefault, "0 Planet");
  options[OPT_BODYTYPE].dDefault   = 0;
  options[OPT_BODYTYPE].iType      = 1;
  options[OPT_BODYTYPE].bMultiFile = 1;
  options[OPT_BODYTYPE].iModuleBit = 0;
  options[OPT_BODYTYPE].bNeg       = 0;
  options[OPT_BODYTYPE].iFileType  = 1;
  fnRead[OPT_BODYTYPE]             = &ReadBodyType;

  fvFormattedString(&options[OPT_MASSRAD].cName, "sMassRad");
  fvFormattedString(
        &options[OPT_MASSRAD].cDescr,
        "Mass-Radius Relationship for Central Body: GS99 RH00 BO06 Sotin07 ");
  fvFormattedString(&options[OPT_MASSRAD].cDefault, "None");
  options[OPT_MASSRAD].iType      = 3;
  options[OPT_MASSRAD].bMultiFile = 1;
  options[OPT_MASSRAD].iModuleBit =
        SPINBODY + DISTROT + POISE + STELLAR + BINARY + EQTIDE;
  options[OPT_MASSRAD].bNeg      = 0;
  options[OPT_MASSRAD].iFileType = 1;
  fnRead[OPT_MASSRAD]            = &ReadMassRad;

  fvFormattedString(&options[OPT_OBL].cName, "dObliquity");
  fvFormattedString(&options[OPT_OBL].cDescr, "Obliquity");
  fvFormattedString(&options[OPT_OBL].cDefault, "0");
  fvFormattedString(&options[OPT_OBL].cDimension, "angle");
  options[OPT_OBL].dDefault   = 0;
  options[OPT_OBL].iType      = 2;
  options[OPT_OBL].bMultiFile = 1;
  options[OPT_OBL].iModuleBit = SPINBODY + DISTROT + POISE + BINARY + EQTIDE;
  options[OPT_OBL].bNeg       = 0;
  options[OPT_OBL].iFileType  = 1;
  fnRead[OPT_OBL]             = &ReadObliquity;

  fvFormattedString(&options[OPT_COSOBL].cName, "dCosObl");
  fvFormattedString(&options[OPT_COSOBL].cDescr, "Coine of the Obliquity");
  fvFormattedString(&options[OPT_COSOBL].cDefault, "0.5");
  fvFormattedString(&options[OPT_COSOBL].cDimension, "nd");
  options[OPT_COSOBL].dDefault   = 0.5;
  options[OPT_COSOBL].iType      = 2;
  options[OPT_COSOBL].bMultiFile = 1;
  options[OPT_COSOBL].iModuleBit = SPINBODY + DISTROT + POISE + BINARY + EQTIDE;
  options[OPT_COSOBL].bNeg       = 0;
  options[OPT_COSOBL].iFileType  = 1;
  fnRead[OPT_COSOBL]             = &ReadCosObl;
  fvFormattedString(
        &options[OPT_COSOBL].cLongDescr,
        "Planet formation simulations predict that an isotropic distribution "
        "of\n"
        "rotational angular momentum vectors is a typical outcome. This "
        "result is\n"
        "identical to a uniform distribution in cosine obliquity. Use this "
        "option\n"
        "to sample a realistic distribution of initial obliquities.\n");

  fvFormattedString(&options[OPT_RADIUS].cName, "dRadius");
  fvFormattedString(&options[OPT_RADIUS].cDescr, "Radius");
  fvFormattedString(&options[OPT_RADIUS].cDefault, "1 Earth Radius");
  fvFormattedString(&options[OPT_RADIUS].cNeg, "Rearth");
  fvFormattedString(&options[OPT_RADIUS].cDimension, "length");
  options[OPT_RADIUS].dDefault   = REARTH;
  options[OPT_RADIUS].iType      = 2;
  options[OPT_RADIUS].bMultiFile = 1;
  options[OPT_RADIUS].dNeg       = REARTH;
  options[OPT_RADIUS].iModuleBit = 0;
  options[OPT_RADIUS].bNeg       = 1;
  options[OPT_RADIUS].iFileType  = 1;
  fnRead[OPT_RADIUS]             = &ReadRadius;

  fvFormattedString(&options[OPT_RG].cName, "dRadGyra");
  fvFormattedString(&options[OPT_RG].cDescr,
                    "Radius of Gyration; moment of inertia constant");
  fvFormattedString(&options[OPT_RG].cDefault, "0.5");
  fvFormattedString(&options[OPT_RG].cDimension, "nd");
  options[OPT_RG].dDefault   = 0.5;
  options[OPT_RG].iType      = 2;
  options[OPT_RG].bMultiFile = 1;
  options[OPT_RG].iModuleBit = 0;
  options[OPT_RG].bNeg       = 0;
  options[OPT_RG].iFileType  = 1;
  fnRead[OPT_RG]             = &ReadRadiusGyration;

  fvFormattedString(&options[OPT_ROTPER].cName, "dRotPeriod");
  fvFormattedString(&options[OPT_ROTPER].cDescr, "Rotation Period");
  fvFormattedString(&options[OPT_ROTPER].cDefault, "1 Day");
  fvFormattedString(&options[OPT_ROTPER].cNeg, "Days");
  fvFormattedString(&options[OPT_ROTPER].cDimension, "time");
  options[OPT_ROTPER].dDefault   = DAYSEC;
  options[OPT_ROTPER].iType      = 2;
  options[OPT_ROTPER].bMultiFile = 1;
  options[OPT_ROTPER].dNeg       = DAYSEC;
  options[OPT_ROTPER].iModuleBit = 0;
  options[OPT_ROTPER].bNeg       = 1;
  options[OPT_ROTPER].iFileType  = 1;
  fnRead[OPT_ROTPER]             = &ReadRotPeriod;

  fvFormattedString(&options[OPT_ROTRATE].cName, "dRotRate");
  fvFormattedString(&options[OPT_ROTRATE].cDescr,
                    "Rotational Angular Frequency");
  fvFormattedString(&options[OPT_ROTRATE].cDefault, "2*pi/day");
  fvFormattedString(&options[OPT_ROTRATE].cNeg, "/Day");
  fvFormattedString(&options[OPT_ROTRATE].cDimension, "time^-1");
  options[OPT_ROTRATE].dDefault   = 2 * PI / DAYSEC;
  options[OPT_ROTRATE].iType      = 2;
  options[OPT_ROTRATE].bMultiFile = 1;
  options[OPT_ROTRATE].dNeg       = 1. / DAYSEC;
  options[OPT_ROTRATE].iModuleBit = 0;
  options[OPT_ROTRATE].bNeg       = 1;
  options[OPT_ROTRATE].iFileType  = 1;
  fnRead[OPT_ROTRATE]             = &ReadRotRate;

  fvFormattedString(&options[OPT_ROTVEL].cName, "dRotVel");
  fvFormattedString(&options[OPT_ROTVEL].cDescr, "Rotational Velocity");
  fvFormattedString(&options[OPT_ROTVEL].cDefault, "0");
  fvFormattedString(&options[OPT_ROTVEL].cNeg, "km/s");
  fvFormattedString(&options[OPT_ROTVEL].cDimension, "mass/time");
  options[OPT_ROTVEL].dDefault   = 0;
  options[OPT_ROTVEL].iType      = 2;
  options[OPT_ROTVEL].bMultiFile = 1;
  options[OPT_ROTVEL].dNeg       = 1e5;
  options[OPT_ROTVEL].iModuleBit = 0;
  options[OPT_ROTVEL].bNeg       = 1;
  options[OPT_ROTVEL].iFileType  = 1;
  fnRead[OPT_ROTVEL]             = &ReadRotVel;

  /*
   *
   *   UNITS
   *
   */

  fvFormattedString(&options[OPT_UNITANGLE].cName, "sUnitAngle");
  fvFormattedString(&options[OPT_UNITANGLE].cDescr,
                    "Angle Units: Degrees Radians");
  fvFormattedString(&options[OPT_UNITANGLE].cDefault, "Radians");
  options[OPT_UNITANGLE].iModuleBit = 0;
  options[OPT_UNITANGLE].bNeg       = 0;
  options[OPT_UNITANGLE].iType      = 3;
  options[OPT_UNITANGLE].iFileType  = 2;

  fvFormattedString(&options[OPT_UNITLENGTH].cName, "sUnitLength");
  fvFormattedString(&options[OPT_UNITLENGTH].cDescr,
                    "Length Units: cm m km Solar Earth Jupiter AU");
  fvFormattedString(&options[OPT_UNITLENGTH].cDefault, "cm");
  options[OPT_UNITLENGTH].iModuleBit = 0;
  options[OPT_UNITLENGTH].bNeg       = 0;
  options[OPT_UNITLENGTH].iType      = 3;
  options[OPT_UNITLENGTH].iFileType  = 2;

  fvFormattedString(&options[OPT_UNITMASS].cName, "sUnitMass");
  fvFormattedString(&options[OPT_UNITMASS].cDescr,
                    "Mass Units: Grams Kilograms Solar Earth Jupiter Saturn");
  fvFormattedString(&options[OPT_UNITMASS].cDefault, "grams");
  options[OPT_UNITMASS].iModuleBit = 0;
  options[OPT_UNITMASS].bNeg       = 0;
  options[OPT_UNITMASS].iType      = 3;
  options[OPT_UNITMASS].iFileType  = 2;

  fvFormattedString(&options[OPT_UNITTIME].cName, "sUnitTime");
  fvFormattedString(&options[OPT_UNITTIME].cDescr,
                    "Time Units: Seconds, Days Years Myr Gyr");
  fvFormattedString(&options[OPT_UNITTIME].cDefault, "Seconds");
  options[OPT_UNITTIME].iModuleBit = 0;
  options[OPT_UNITTIME].bNeg       = 0;
  options[OPT_UNITTIME].iType      = 3;
  options[OPT_UNITTIME].iFileType  = 2;

  fvFormattedString(&options[OPT_UNITTEMP].cName, "sUnitTemp");
  fvFormattedString(&options[OPT_UNITTEMP].cDescr,
                    "Temperature Units: Kelvin Celsius Farenheit");
  fvFormattedString(&options[OPT_UNITTEMP].cDefault, "Kelvin");
  options[OPT_UNITTEMP].iModuleBit = 0;
  options[OPT_UNITTEMP].bNeg       = 0;
  options[OPT_UNITTEMP].iType      = 3;
  options[OPT_UNITTEMP].iFileType  = 2;

  fvFormattedString(&options[OPT_TEMPERATURE].cName, "dTemperature");
  fvFormattedString(&options[OPT_TEMPERATURE].cDescr,
                    "Initial effective temperature");
  fvFormattedString(&options[OPT_TEMPERATURE].cDefault, "TSUN");
  fvFormattedString(&options[OPT_TEMPERATURE].cDimension, "temperature");
  options[OPT_TEMPERATURE].dDefault   = TSUN;
  options[OPT_TEMPERATURE].iType      = 2;
  options[OPT_TEMPERATURE].bMultiFile = 1;
  fnRead[OPT_TEMPERATURE]             = &ReadTemperature;

  fvFormattedString(&options[OPT_USEOUTERTIDALQ].cName, "bUseOuterTidalQ");
  fvFormattedString(&options[OPT_USEOUTERTIDALQ].cDescr,
                    "User outermost layer's tidal Q as body's total tidal Q?");
  fvFormattedString(&options[OPT_USEOUTERTIDALQ].cDefault, "0");
  options[OPT_USEOUTERTIDALQ].iType      = 0;
  options[OPT_USEOUTERTIDALQ].bMultiFile = 1;
  fnRead[OPT_USEOUTERTIDALQ]             = &ReadUseOuterTidalQ;
  fvFormattedString(
        &options[OPT_USEOUTERTIDALQ].cLongDescr,
        "The total tidal Q of a body can be computed either as the sum of\n"
        "contributions of all layers (mantle, ocean, envelope), or as the "
        "tidal Q\n"
        "of the outer most layer. When %s is set to 0, the tidal Q is the "
        "sum,\n"
        "when set to 1, it is the outer layer's (envelope, then ocean, then\n"
        "mantle) value.\n",
        options[OPT_USEOUTERTIDALQ].cName);

  /*
   *
   *   V
   *
   */

  fvFormattedString(&options[OPT_VERBOSE].cName, "iVerbose");
  fvFormattedString(&options[OPT_VERBOSE].cDescr, "Verbosity Level: 0-5");
  fvFormattedString(&options[OPT_VERBOSE].cDefault, "3");
  options[OPT_VERBOSE].iModuleBit = 0;
  options[OPT_VERBOSE].bNeg       = 0;
  options[OPT_VERBOSE].iType      = 1;
  options[OPT_VERBOSE].iFileType  = 2;
  fvFormattedString(
        &options[OPT_VERBOSE].cLongDescr,
        "Set how much text is written to the screen. 0 = no output, 1 = only\n"
        "errors, 2 = progress updates, 3 = statements about input choices,  4 "
        "=\n"
        "information about unit choices, 5 = all possible output. Note that "
        "levels\n"
        "0 and 5 can be set at execution with the -q and -v options, "
        "respectively.");

  fvFormattedString(&options[OPT_VISCUMAN].cName, "dViscUMan");
  fvFormattedString(&options[OPT_VISCUMAN].cDescr, "Upper mantle viscosity");
  fvFormattedString(&options[OPT_VISCUMAN].cDefault, "0");
  fvFormattedString(&options[OPT_VISCUMAN].cDimension, "length^2/time");
  options[OPT_VISCUMAN].dDefault   = 0;
  options[OPT_VISCUMAN].iType      = 2;
  options[OPT_VISCUMAN].bMultiFile = 1;
  options[OPT_VISCUMAN].bNeg       = 0;
  options[OPT_VISCUMAN].iModuleBit = THERMINT + DISTROT;
  fnRead[OPT_VISCUMAN]             = &ReadViscUMan;
  options[OPT_VISCUMAN].iFileType  = 1;

  /*
   *
   * X
   *
   */

  /* Someday...

  fvFormattedString(options[OPT_XOBL].cName,"dXobl");
  fvFormattedString(options[OPT_XOBL].cDescr,"Deitrick's X -- Unsupported!");
  fvFormattedString(options[OPT_XOBL].cDefault,"-1");
  options[OPT_XOBL].dDefault = -1;
  options[OPT_XOBL].iType = 2;
  options[OPT_XOBL].iModuleBit = SPINBODY + DISTROT;
  options[OPT_XOBL].bNeg = 0;
  options[OPT_XOBL].iFileType = 1;
  fnRead[OPT_XOBL] = &ReadXobl;

  / *
   *
   * Y
   *
   * /

  fvFormattedString(options[OPT_YOBL].cName,"dYobl");
  fvFormattedString(options[OPT_YOBL].cDescr,"Deitrick's Y -- Unsupported!");
  fvFormattedString(options[OPT_YOBL].cDefault,"-1");
  options[OPT_YOBL].dDefault = -1;
  options[OPT_YOBL].iType = 2;
  options[OPT_YOBL].iModuleBit = SPINBODY + DISTROT;
  options[OPT_YOBL].bNeg = 0;
  options[OPT_YOBL].iFileType = 1;
  fnRead[OPT_YOBL] = &ReadYobl;

  / *
   *
   * Z
   *
   * /

  fvFormattedString(options[OPT_ZOBL].cName,"dZobl");
  fvFormattedString(options[OPT_ZOBL].cDescr,"Deitrick's Z -- Unsupported!");
  fvFormattedString(options[OPT_ZOBL].cDefault,"-1");
  options[OPT_ZOBL].dDefault = -1;
  options[OPT_ZOBL].iType = 2;
  options[OPT_ZOBL].iModuleBit = SPINBODY + DISTROT;
  options[OPT_ZOBL].bNeg = 0;
  options[OPT_ZOBL].iFileType = 1;
  fnRead[OPT_ZOBL] = &ReadZobl;
*/
}

void InitializeOptions(OPTIONS *options, fnReadOption *fnRead) {
  int iBody, iOpt, iFile, iModule;

  /* Initialize all parameters describing the option's location */
  for (iOpt = 0; iOpt < MODULEOPTEND; iOpt++) {

    options[iOpt].cName = NULL;
    options[iOpt].cDescr = NULL;
    options[iOpt].cLongDescr = NULL;
    options[iOpt].cDefault = NULL;
    options[iOpt].cValues = NULL;
    options[iOpt].cNeg = NULL;
    options[iOpt].cDimension = NULL;

    fvFormattedString(&options[iOpt].cName, "null");
    options[iOpt].iLine      = malloc(MAXFILES * sizeof(int));
    options[iOpt].bMultiFile = 0;
    options[iOpt].iMultiIn   = 0;
    options[iOpt].iType      = -1;
    fvFormattedString(&options[iOpt].cDescr, "null");
    fvFormattedString(&options[iOpt].cLongDescr, "null");
    fvFormattedString(&options[iOpt].cDefault, "null");
    fvFormattedString(&options[iOpt].cValues, "null");
    fvFormattedString(&options[iOpt].cNeg, "null");
    options[iOpt].dDefault   = NAN;
    options[iOpt].iModuleBit = 0;
    options[iOpt].bNeg       = 0;
    options[iOpt].iFileType  = 2;
    options[iOpt].dNeg       = 0;
  }

  /* Now populate entries for general options. */
  InitializeOptionsGeneral(options, fnRead);

  /************************
   * ADD NEW MODULES HERE *
   ************************/

  InitializeOptionsEqtide(options, fnRead);
  fvInitializeOptionsRadheat(options, fnRead);
  InitializeOptionsDistOrb(options, fnRead);
  InitializeOptionsDistRot(options, fnRead);
  fvInitializeOptionsThermint(options, fnRead);
  InitializeOptionsAtmEsc(options, fnRead);
  InitializeOptionsStellar(options, fnRead);
  InitializeOptionsPoise(options, fnRead);
  InitializeOptionsBinary(options, fnRead);
  InitializeOptionsGalHabit(options, fnRead);
  InitializeOptionsSpiNBody(options, fnRead);
  InitializeOptionsMagmOc(options, fnRead);
  InitializeOptionsFlare(options, fnRead);
}
