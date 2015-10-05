TODO
----

-  Make interpolation order a user option in `STELLAR`
-  Negative option for ``dRotRate`` not working.
-  Add angular momentum conservation when radius changes. Compare to 
   http://adsabs.harvard.edu/abs/1989A%26A...223..112Z and 
   http://adsabs.harvard.edu/abs/2010MNRAS.401..257K
-  Baraffe routines returned NaN at age = 10 Gyr. Check why. Perhaps make it a halting condition.
-  Adapt Euler to iaType = 0 variables
-  Add other escape regimes
-  Log stuff
-  What's the deal with ``dRotDt`` in `vplanet.h`?

NOTES
-----

- To add an evolving variable, add to atmesc.c, atmesc.h, module.c, update.c, vplanet.h, options.c