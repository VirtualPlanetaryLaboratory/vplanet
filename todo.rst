TODO
----

*. Just made Radius a primary variable. Verify.
*. Replace stellar files with a .h file and static consts
0. Verify that RungeKutta is working wiht iaType = 0
1. Adapt Euler to iaType = 0 variables
2. Add code in ForceBehavior to update dLXUV
3. Add other escape regimes
4. Log stuff

NOTES
-----

- To add an evolving variable, add to atmesc.c, atmesc.h, module.c, update.c, vplanet.h, options.c