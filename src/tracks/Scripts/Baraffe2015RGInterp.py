# coding: utf-8
"""
David P. Fleming, Aug. 20th, 2018

In this notebook, I linearly interpolate Baraffe+2015 stellar evolution grids
(see http://perso.ens-lyon.fr/isabelle.baraffe/BHAC15dir/) to compute stellar
luminosity, effective temperature, radius, and radius of gyration as a
function of age for each given stellar mass.  I also linearly extrapolate to make
tracks just outside the Baraffe+2015 grid to ensure subsequent bicubic interpolations
are numerically stable.

This script outputs the numbers that form the arrays, like DATA_RG[STELLAR_BAR_MLEN][STELLAR_BAR_ALEN]
in body.h, and produces diagnostic plots to show that the interpolation is working.

Note, you need to unzip  tracks/Data/BHAC15_tracks+structure.txt.gz in ../Data
prior to running this script.
"""

import os

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from scipy.interpolate import UnivariateSpline, interp1d

# Typical plot parameters that make for pretty plots
mpl.rcParams["figure.figsize"] = (9, 8)
mpl.rcParams["font.size"] = 25.0

## for Palatino and other serif fonts use:
mpl.rc("font", **{"family": "serif", "serif": ["Computer Modern"]})
mpl.rc("text", usetex=True)

# Old Baraffe+2015 mass grid points [Msun]
masses = [
    0.0698,
    0.07,
    0.072,
    0.075,
    0.08,
    0.09,
    0.1,
    0.2,
    0.3,
    0.4,
    0.5,
    0.6,
    0.7,
    0.8,
    0.9,
    1.0,
    1.1,
    1.2,
    1.3,
    1.4,
    1.41,
]

# Standardized Baraffe+2015 age grid points in Gyr (note that the Baraffe+2015 time units are log10 years)
ages = np.array(
    [
        0.00099813710097553117,
        0.001,
        0.0010186289902446876,
        0.0010376050197669117,
        0.0010569345535579883,
        0.0010766241770454933,
        0.0010966805983368686,
        0.0011171106505048242,
        0.0011379212939153151,
        0.0011591196185988858,
        0.0011807128466661903,
        0.001202708334768512,
        0.0012251135766041189,
        0.0012479362054713115,
        0.0012711839968690289,
        0.0012948648711459049,
        0.0013189868961986705,
        0.0013435582902208262,
        0.0013685874245025192,
        0.0013940828262825786,
        0.0014200531816536831,
        0.0014465073385216472,
        0.0014734543096198358,
        0.0015009032755797366,
        0.0015288635880587311,
        0.0015573447729261352,
        0.0015863565335085897,
        0.0016159087538959191,
        0.0016460115023085497,
        0.0016766750345276991,
        0.0017079097973894269,
        0.0017397264323438008,
        0.0017721357790803584,
        0.0018051488792211082,
        0.0018387769800823268,
        0.0018730315385064365,
        0.0019079242247652651,
        0.0019434669265360202,
        0.0019796717529513325,
        0.0020165510387247466,
        0.0020541173483530641,
        0.0020923834803969771,
        0.0021313624718414378,
        0.0021710676025372649,
        0.0022115123997254889,
        0.0022527106426459805,
        0.0022946763672319361,
        0.0023374238708918148,
        0.0023809677173803581,
        0.0024253227417603529,
        0.0024705040554568254,
        0.0025165270514053918,
        0.0025634074092965148,
        0.0026111611009174596,
        0.0026598043955937583,
        0.0027093538657320514,
        0.0027598263924661806,
        0.0028112391714084643,
        0.0028636097185081159,
        0.0029169558760187959,
        0.0029712958185773341,
        0.0030266480593956922,
        0.0030830314565682771,
        0.0031404652194967522,
        0.0031989689154345376,
        0.0032585624761532262,
        0.0033192662047331896,
        0.0033811007824806854,
        0.0034440872759738234,
        0.0035082471442397923,
        0.0035736022460657883,
        0.0036401748474461414,
        0.0037079876291681727,
        0.0037770636945393646,
        0.0038474265772585059,
        0.0039191002494334019,
        0.0039921091297480532,
        0.0040664780917818539,
        0.0041422324724838975,
        0.0042193980808050242,
        0.0042980012064907985,
        0.0043780686290381668,
        0.0044596276268190956,
        0.0045427059863740425,
        0.004627332011878693,
        0.0047135345347869067,
        0.0048013429236534498,
        0.00489078709413959,
        0.0049818975192051597,
        0.0050747052394904662,
        0.0051692418738915986,
        0.0052655396303327552,
        0.0053636313167392397,
        0.0054635503522148755,
        0.0055653307784276464,
        0.0056690072712074343,
        0.0057746151523598195,
        0.0058821904016999561,
        0.0059917696693106192,
        0.0061033902880286211,
        0.0062170902861638268,
        0.0063329084004551143,
        0.0064508840892676927,
        0.0065710575460362708,
        0.0066934697129586605,
        0.0068181622949444794,
        0.0069451777738236964,
        0.0070745594228198783,
        0.0072063513212930527,
        0.0073405983697572121,
        0.0074773463051775882,
        0.0076166417165528921,
        0.007758532060787836,
        0.0079030656788613481,
        0.0080502918122959816,
        0.008200260619934131,
        0.0083530231950267788,
        0.0085086315826405825,
        0.0086671387973892341,
        0.0088285988414951496,
        0.0089930667231876236,
        0.0091605984754437094,
        0.0093312511750782492,
        0.0095050829621895119,
        0.0096821530599670753,
        0.0098625217948687845,
        0.010046250617173407,
        0.010233402121916417,
        0.010424040070215563,
        0.010618229410993841,
        0.0108160363031071,
        0.011017528137883868,
        0.011222773562085077,
        0.011431842501291497,
        0.011644806183726861,
        0.011861737164524785,
        0.012082709350447765,
        0.012307798025066652,
        0.012537079874409203,
        0.01277063301308644,
        0.013008537010905713,
        0.013250872919979532,
        0.013497723302339417,
        0.01374917225806419,
        0.014005305453932197,
        0.014266210152607369,
        0.014531975242368954,
        0.014802691267395087,
        0.015078450458610511,
        0.01535934676510897,
        0.01564547588616096,
        0.015936935303817745,
        0.016233824316122782,
        0.016536244070941803,
        0.016844297600423153,
        0.01715808985610005,
        0.017477727744646808,
        0.01780332016430114,
        0.018134978041964952,
        0.018472814370996342,
        0.018816944249705557,
        0.019167484920568151,
        0.019524555810168608,
        0.019888278569888097,
        0.02025877711735017,
        0.020636177678638588,
        0.021020608831301583,
        0.021412201548157293,
        0.021811089241915188,
        0.022217407810628836,
        0.022631295683995283,
        0.023052893870517073,
        0.023482346005542751,
        0.023919798400202384,
        0.024365400091254649,
        0.02481930289186254,
        0.025281661443314993,
        0.025752633267711999,
        0.026232378821631222,
        0.026721061550794338,
        0.027218847945751783,
        0.027725907598604826,
        0.028242413260784343,
        0.028768540901905927,
        0.029304469769721429,
        0.029850382451187308,
        0.030406464934670672,
        0.030972906673314083,
        0.031549900649580867,
        0.032137643441002769,
        0.03273633528715246,
        0.033346180157863642,
        0.033967385822722081,
        0.034600163921851085,
        0.035244730038015842,
        0.035901303770070683,
        0.036570108807774893,
        0.037251373008002096,
        0.03794532847236938,
        0.038652211626312619,
        0.039372263299634794,
        0.040105728808554959,
        0.040852858039285615,
        0.041613905533167073,
        0.042389130573387786,
        0.043178797273320212,
        0.043983174666502235,
        0.044802536798294899,
        0.045637162819247588,
        0.046487337080202563,
        0.047353349229171164,
        0.04823549431001467,
        0.049134072862963618,
        0.050049391027009532,
        0.050981760644204246,
        0.05193149936590212,
        0.052898930760981511,
        0.053884384426082239,
        0.054888196097896719,
        0.0559107077675529,
        0.056952267797128218,
        0.058013231038333751,
        0.05909395895340968,
        0.06019481973827271,
        0.06131618844795772,
        0.062458447124396146,
        0.063621984926574851,
        0.064807198263119675,
        0.066014490927348859,
        0.067244274234842458,
        0.068496967163574438,
        0.069772996496655346,
        0.07107279696773415,
        0.072396811409108738,
        0.073745490902595495,
        0.075119294933209652,
        0.076518691545708215,
        0.077944157504049469,
        0.079396178453822797,
        0.080875249087704534,
        0.08238187331399606,
        0.083916564428301574,
        0.085479845288404091,
        0.087072248492399168,
        0.088694316560147091,
        0.090346602118105246,
        0.0920296680876041,
        0.093744087876629911,
        0.095490445575180771,
        0.097269336154261685,
        0.099081365668586663,
        0.10092715146305709,
        0.10280732238308647,
        0.10472251898884344,
        0.10667339377348571,
        0.10866061138545968,
        0.1106848488549412,
        0.11274679582449461,
        0.1148471547840289,
        0.11698664131013069,
        0.11916598430985588,
        0.12138592626906278,
        0.12364722350537151,
        0.12595064642583575,
        0.12829697978941476,
        0.13068702297433463,
        0.13312159025043077,
        0.13560151105656335,
        0.13812763028320094,
        0.14070080856026851,
        0.14332192255035739,
        0.14599186524739785,
        0.14871154628089536,
        0.15148189222583458,
        0.15430384691835647,
        0.15717837177731628,
        0.16010644613183178,
        0.16308906755493327,
        0.1661272522034293,
        0.16922203516410372,
        0.17237447080636198,
        0.17558563314144687,
        0.1788566161883462,
        0.18218853434651652,
        0.18558252277555171,
        0.18903973778192196,
        0.19256135721291967,
        0.19614858085794293,
        0.19980263085725486,
        0.20352475211835758,
        0.20731621274012291,
        0.21117830444482422,
        0.2151123430182165,
        0.21911966875781475,
        0.22320164692952324,
        0.22735966823277151,
        0.23159514927431521,
        0.23590953305086337,
        0.24030428944069671,
        0.24478091570444402,
        0.24934093699518778,
        0.25398590687807238,
        0.25871740785959213,
        0.26353705192673932,
        0.26844648109619623,
        0.27344736797375796,
        0.27854141632417662,
        0.28373036165162113,
        0.28901597179095084,
        0.29440004751000337,
        0.29988442312310276,
        0.30547096711599681,
        0.311161582782436,
        0.3169582088726115,
        0.32286282025367302,
        0.32887742858255103,
        0.33500408299131329,
        0.34124487078528892,
        0.34760191815419778,
        0.35407739089652696,
        0.36067349515740282,
        0.36739247818020743,
        0.37423662907219807,
        0.3812082795843888,
        0.38830980490596051,
        0.39554362447347013,
        0.40291220279513479,
        0.4104180502904709,
        0.4180637241455758,
        0.42585182918434145,
        0.43378501875589892,
        0.44186599563859413,
        0.45009751296080464,
        0.4584823751389096,
        0.46702343883273306,
        0.47572361391878848,
        0.48458586448164909,
        0.4936132098237912,
        0.50280872549424749,
        0.51217554433642365,
        0.5217168575554344,
        0.53143591580532357,
        0.54133603029653754,
        0.55142057392402966,
        0.56169298241638044,
        0.57215675550632461,
        0.582815458123084,
        0.59367272160691198,
        0.60473224494626432,
        0.61599779603801619,
        0.62747321297115743,
        0.63916240533439994,
        0.65106935554814549,
        0.6631981202212669,
        0.67555283153316392,
        0.68813769864156626,
        0.70095700911656178,
        0.71401513040133957,
        0.72731651130014541,
        0.74086568349395598,
        0.7546672630843887,
        0.76872595216637285,
        0.78304654043011834,
        0.79763390679292734,
        0.81249302106140475,
        0.8276289456246344,
        0.84304683717889672,
        0.85875194848451708,
        0.87474963015544149,
        0.89104533248215134,
        0.90764460728853569,
        0.92455310982335703,
        0.9417766006869519,
        0.95932094779382415,
        0.9771921283717997,
        0.99539623099842334,
        1.0139394576752918,
        1.0328281259410286,
        1.0520686710236229,
        1.0716676480328633,
        1.0916317341936148,
        1.111967731120699,
        1.1326825671361538,
        1.1537832996296609,
        1.1752771174629453,
        1.1971713434189657,
        1.219473436696737,
        1.2421909954526162,
        1.2653317593889417,
        1.2889036123908915,
        1.312914585212464,
        1.3373728582124951,
        1.3622867641416454,
        1.3876647909813071,
        1.4135155848353942,
        1.4398479528760066,
        1.4666708663439672,
        1.4939934636052563,
        1.5218250532643856,
        1.5501751173357687,
        1.5790533144741743,
        1.6084694832653552,
        1.6384336455779831,
        1.6689560099780232,
        1.7000469752067164,
        1.7317171337233528,
        1.7639772753140435,
        1.7968383907677192,
        1.8303116756206113,
        1.8644085339704852,
        1.8991405823619336,
        1.9345196537440443,
        1.9705578015017984,
        2.0072673035625681,
        2.0446606665791158,
        2.0827506301905143,
        2.1215501713624501,
        2.1610725088083766,
        2.2013311074930302,
        2.242339683219845,
        2.2841122073038234,
        2.3266629113314581,
        2.3700062920093279,
        2.4141571161030178,
        2.4591304254680439,
        2.5049415421745023,
        2.551606073727184,
        2.5991399183829329,
        2.6475592705670667,
        2.6968806263906924,
        2.7471207892708116,
        2.7982968756551156,
        2.8504263208534346,
        2.903526884977814,
        2.9576166589932535,
        3.0127140708811644,
        3.0688378919176422,
        3.1260072430687003,
        3.1842416015046497,
        3.243560807235808,
        3.3039850698718549,
        3.3655349755070909,
        3.4282314937339673,
        3.4920959847872677,
        3.5571502068213823,
        3.6234163233231462,
        3.6909169106627746,
        3.7596749657854636,
        3.8297139140462773,
        3.9010576171909892,
        3.9737303814856042,
        4.0477569659973183,
        4.1231625910297485,
        4.1999729467153015,
        4.2782142017676126,
        4.3579130123970256,
        4.4390965313921669,
        4.5217924173706976,
        4.606028844202398,
        4.6918345106077943,
        4.7792386499355954,
        4.8682710401222788,
        4.9589620138372119,
        5.0513424688167614,
        5.1454438783909247,
        5.2412983022060562,
        5.3389383971473503,
        5.4383974284647962,
        5.5397092811064006,
        5.6429084712625368,
        5.748030158125351,
        5.855110155867238,
        5.9641849458424598,
        6.0752916890160709,
        6.1884682386243828,
        6.3037531530712752,
        6.4211857090647584,
        6.5408059149982529,
        6.6626545245811499,
        6.7867730507232951,
        6.9132037796781276,
        7.0419897854492888,
        7.1731749444656128,
        7.3068039505294999,
        7.4429223300437579,
        7.5815764575221101,
        7.7228135713886426,
        7.8666817900715822,
        8.013230128396879,
        8.1625085142872198,
        8.3145678057720556,
        8.4694598083145767,
        8.6272372924614409,
        8.7879540118213111,
        8.9516647213782932,
        9.1184251961465623,
        9.2882922501724892,
        9.4613237558907599,
        9.6375786638410794,
        9.8171170227521838,
        10.0,
        10.018288297724782,
    ]
)


# Load in data
data = pd.read_table(
    "../Data/BHAC15_tracks+structure.txt",
    comment="!",
    delim_whitespace=True,
    header=-1,
    names=[
        "M/Ms",
        "log t(yr)",
        "Teff",
        "L/Ls",
        "g",
        "R/Rs",
        "Log(Li/Li0)",
        "log Tc",
        "log ROc",
        "Mrad",
        "Rrad",
        "k2conv",
        "k2rad",
    ],
)
# Examine data
print(data.head())

# Group by masses to extract parameters
groups = data.groupby("M/Ms").groups

# Dicts to hold values, times
barrafe_times = {}
baraffe_rg = {}
baraffe_rad = {}
baraffe_lum = {}
baraffe_teff = {}

for group in groups:
    # Find rows corresponding to each stellar mass
    inds = np.array(groups[group])

    # Extract data (note, we'll fit log10 lum and teff, linear rg and rad)
    barrafe_times[str(group)] = data.values[inds, 1]
    baraffe_rg[str(group)] = np.sqrt(
        data.values[inds, 11] ** 2 + data.values[inds, 12] ** 2
    )
    baraffe_rad[str(group)] = data.values[inds, 5]
    baraffe_lum[str(group)] = data.values[inds, 3]
    baraffe_teff[str(group)] = np.log10(data.values[inds, 2])

# Build linear interpolation functions for L, Teff, R, rg
baraffe_rg_interp_funcs = {}
baraffe_rad_interp_funcs = {}
baraffe_lum_interp_funcs = {}
baraffe_teff_interp_funcs = {}

for group in groups:
    baraffe_rg_interp_funcs[str(group)] = interp1d(
        barrafe_times[str(group)],
        baraffe_rg[str(group)],
        kind="linear",
        fill_value=np.nan,
        assume_sorted=True,
        bounds_error=False,
    )

    baraffe_rad_interp_funcs[str(group)] = interp1d(
        barrafe_times[str(group)],
        baraffe_rad[str(group)],
        kind="linear",
        fill_value=np.nan,
        assume_sorted=True,
        bounds_error=False,
    )

    baraffe_lum_interp_funcs[str(group)] = interp1d(
        barrafe_times[str(group)],
        baraffe_lum[str(group)],
        kind="linear",
        fill_value=np.nan,
        assume_sorted=True,
        bounds_error=False,
    )

    baraffe_teff_interp_funcs[str(group)] = interp1d(
        barrafe_times[str(group)],
        baraffe_teff[str(group)],
        kind="linear",
        fill_value=np.nan,
        assume_sorted=True,
        bounds_error=False,
    )

# Interpolate tracks to standardized grid points
baraffe_rg_interp = {}
baraffe_lum_interp = {}
baraffe_rad_interp = {}
baraffe_teff_interp = {}

for group in groups:
    baraffe_rg_interp[str(group)] = baraffe_rg_interp_funcs[str(group)](
        np.log10(ages * 1.0e9)
    )
    baraffe_rad_interp[str(group)] = baraffe_rad_interp_funcs[str(group)](
        np.log10(ages * 1.0e9)
    )
    baraffe_lum_interp[str(group)] = baraffe_lum_interp_funcs[str(group)](
        np.log10(ages * 1.0e9)
    )
    baraffe_teff_interp[str(group)] = baraffe_teff_interp_funcs[str(group)](
        np.log10(ages * 1.0e9)
    )

### Plot tracks and true Baraffe+2015 points for sanity check

# Plot them!
fig, ax = plt.subplots()

for ii, group in enumerate(groups):

    if group >= np.min(masses):
        ax.plot(
            ages * 1.0e9,
            baraffe_rg_interp[str(group)],
            label="%s" % str(group),
            color="C%d" % (ii % 9),
            lw=2,
        )
        ax.scatter(
            10 ** barrafe_times[str(group)],
            baraffe_rg[str(group)],
            color="C%d" % (ii % 9),
            s=20,
        )

ax.set_xlabel("Age [yr]")
ax.set_xscale("log")
ax.set_ylabel("$r_g$")
ax.set_title("Radius of Gyration Interpolation")
ax.legend(loc="best", fontsize=10)
fig.savefig("rg_interp.png", bbox_inches="tight")

# Plot them!
fig, ax = plt.subplots()

for ii, group in enumerate(groups):

    if group >= np.min(masses):
        ax.plot(
            ages * 1.0e9,
            baraffe_rad_interp[str(group)],
            label="%s" % str(group),
            color="C%d" % (ii % 9),
            lw=2,
        )
        ax.scatter(
            10 ** barrafe_times[str(group)],
            baraffe_rad[str(group)],
            color="C%d" % (ii % 9),
            s=20,
        )

ax.set_xlabel("Age [yr]")
ax.set_xscale("log")
ax.set_ylabel("Radius [R$_{\odot}$]")
ax.set_title("Radius Interpolation")
fig.savefig("rad_interp.png", bbox_inches="tight")

# Plot them!
fig, ax = plt.subplots()

for ii, group in enumerate(groups):

    if group >= np.min(masses):
        ax.plot(
            ages * 1.0e9,
            10 ** baraffe_teff_interp[str(group)],
            label="%s" % str(group),
            color="C%d" % (ii % 9),
            lw=2,
        )
        ax.scatter(
            10 ** barrafe_times[str(group)],
            10 ** baraffe_teff[str(group)],
            color="C%d" % (ii % 9),
            s=20,
        )

ax.set_xlabel("Age [yr]")
ax.set_xscale("log")
ax.set_ylabel("Effective Temperature [K]")
ax.set_title("T$_{\mathrm{eff}}$ Interpolation")
fig.savefig("teff_interp.png", bbox_inches="tight")

# Plot them!
fig, ax = plt.subplots()

for ii, group in enumerate(groups):

    if group >= np.min(masses):
        ax.plot(
            ages * 1.0e9,
            10 ** baraffe_lum_interp[str(group)],
            label="%s" % str(group),
            color="C%d" % (ii % 9),
            lw=2,
        )
        ax.scatter(
            10 ** barrafe_times[str(group)],
            10 ** baraffe_lum[str(group)],
            color="C%d" % (ii % 9),
            s=10,
        )

ax.set_xlabel("Age [yr]")
ax.set_xscale("log")
ax.set_yscale("log")
ax.set_ylabel("Luminosity [L$_{\odot}$]")
ax.set_title("Luminosity Interpolation")
fig.savefig("lum_interp.png", bbox_inches="tight")

# Use linear interpolation to find tracks for points just outside Baraffe+15 grid
# These tracks will only be used as end cases to keep the bilinear and bicubic
# methods numerically stable

# Luminosity

# Upper edge
fst = baraffe_lum_interp["1.3"]
snd = baraffe_lum_interp["1.4"]

baraffe_lum_interp["1.41"] = snd + (snd - fst) / (1.4 - 1.3) * (1.41 - 1.4)

# Lower edge
fst = baraffe_lum_interp["0.07200000000000001"]
snd = baraffe_lum_interp["0.07"]

baraffe_lum_interp["0.0698"] = snd + (snd - fst) / (0.07 - 0.072) * (0.0698 - 0.07)

# Radius

# Upper edge
fst = baraffe_rad_interp["1.3"]
snd = baraffe_rad_interp["1.4"]

baraffe_rad_interp["1.41"] = snd + (snd - fst) / (1.4 - 1.3) * (1.41 - 1.4)

# Lower edge
fst = baraffe_rad_interp["0.07200000000000001"]
snd = baraffe_rad_interp["0.07"]

baraffe_rad_interp["0.0698"] = snd + (snd - fst) / (0.07 - 0.072) * (0.0698 - 0.07)

# Teff

# Upper edge
fst = baraffe_teff_interp["1.3"]
snd = baraffe_teff_interp["1.4"]

baraffe_teff_interp["1.41"] = snd + (snd - fst) / (1.4 - 1.3) * (1.41 - 1.4)

# Lower edge
fst = baraffe_teff_interp["0.07200000000000001"]
snd = baraffe_teff_interp["0.07"]

baraffe_teff_interp["0.0698"] = snd + (snd - fst) / (0.07 - 0.072) * (0.0698 - 0.07)

# rg

# Upper edge
fst = baraffe_rg_interp["1.3"]
snd = baraffe_rg_interp["1.4"]

baraffe_rg_interp["1.41"] = snd + (snd - fst) / (1.4 - 1.3) * (1.41 - 1.4)

# Lower edge
fst = baraffe_rg_interp["0.07200000000000001"]
snd = baraffe_rg_interp["0.07"]

baraffe_rg_interp["0.0698"] = snd + (snd - fst) / (0.07 - 0.072) * (0.0698 - 0.07)

# Now output fits to vplanet-readable format

# rg
print("\n Radius of gyration \n")
print("{", end="")
for val in baraffe_rg_interp["0.0698"]:
    if np.isnan(val):
        print("NAN, ", end="")
    else:
        print("%.6lf, " % val, end="")
print()

for group in groups:
    if group >= np.min(masses):
        for val in baraffe_rg_interp[str(group)]:
            if np.isnan(val):
                print("NAN, ", end="")
            else:
                print("%.6lf, " % val, end="")
        print()
for val in baraffe_rg_interp["1.41"]:
    if np.isnan(val):
        print("NAN, ", end="")
    else:
        print("%.6lf, " % val, end="")
print()
print("}", end="")

# rad
print("\n Radius \n")
print("{", end="")
for val in baraffe_rad_interp["0.0698"]:
    if np.isnan(val):
        print("NAN, ", end="")
    else:
        print("%.6lf, " % val, end="")
print()

for group in groups:
    if group >= np.min(masses):
        for val in baraffe_rad_interp[str(group)]:
            if np.isnan(val):
                print("NAN, ", end="")
            else:
                print("%.6lf, " % val, end="")
        print()
for val in baraffe_rad_interp["1.41"]:
    if np.isnan(val):
        print("NAN, ", end="")
    else:
        print("%.6lf, " % val, end="")
print()
print("}", end="")

# teff
print("\n Effective Temperature \n")
print("{", end="")
for val in baraffe_teff_interp["0.0698"]:
    if np.isnan(val):
        print("NAN, ", end="")
    else:
        print("%.6lf, " % val, end="")
print()

for group in groups:
    if group >= np.min(masses):
        for val in baraffe_teff_interp[str(group)]:
            if np.isnan(val):
                print("NAN, ", end="")
            else:
                print("%.6lf, " % val, end="")
        print()
for val in baraffe_teff_interp["1.41"]:
    if np.isnan(val):
        print("NAN, ", end="")
    else:
        print("%.6lf, " % val, end="")
print()
print("}", end="")

# lum
print("\n Luminosity \n")
print("{", end="")
for val in baraffe_lum_interp["0.0698"]:
    if np.isnan(val):
        print("NAN, ", end="")
    else:
        print("%.6lf, " % val, end="")
print()

for group in groups:
    if group >= np.min(masses):
        for val in baraffe_lum_interp[str(group)]:
            if np.isnan(val):
                print("NAN, ", end="")
            else:
                print("%.6lf, " % val, end="")
        print()
for val in baraffe_lum_interp["1.41"]:
    if np.isnan(val):
        print("NAN, ", end="")
    else:
        print("%.6lf, " % val, end="")
print()
print("}", end="")
