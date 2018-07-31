/**
  @file update.h
  @brief Subroutines controlling the integration
  @author Rory Barnes ([RoryBarnes](https://github.com/RoryBarnes/))
  @date A long, long time ago
*/

/* @cond DOXYGEN_OVERRIDE */

void InitializeUpdateBodyPerts(CONTROL*,UPDATE*,int);
void InitializeUpdateTmpBody(BODY*,CONTROL*,MODULE*,UPDATE*,int);
void UpdateCopy(UPDATE*,UPDATE*,int);
void InitializeUpdate(BODY*,CONTROL*,MODULE*,UPDATE*,fnUpdateVariable****);

/* @endcond */
