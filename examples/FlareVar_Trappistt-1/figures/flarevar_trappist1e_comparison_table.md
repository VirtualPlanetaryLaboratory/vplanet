# TRAPPIST-1e flarevar comparison

All three runs below use the same time-averaged stellar XUV energy. Differences therefore isolate sensitivity to temporal structure in the forcing.

| Run | Final water [TO] | Total water lost [TO] | Final O2 [bar] | Diffusion switch age [Myr] | Fraction time $F_{\mathrm{XUV}} > F_{\mathrm{XUV,crit}}$ |
|---|---:|---:|---:|---:|---:|
| Smooth baseline | 8.347848 | 1.652152 | 163.992741 | NA | 1.000000 |
| Flarevar periodic | 8.369823 | 1.630177 | 164.213992 | NA | 1.000000 |
| Flarevar random-bin | 8.380418 | 1.619582 | 164.214842 | NA | 1.000000 |

## Resolution test (periodic vs periodic dt/5)

| Metric | Periodic | Periodic dt/5 | Absolute difference |
|---|---:|---:|---:|
| Final water [TO] | 8.369823 | 8.370058 | 2.358500e-04 |
| Final O2 [bar] | 164.213992 | 164.214883 | 8.909200e-04 |
| Diffusion switch age [Myr] | NA | NA | NA |
