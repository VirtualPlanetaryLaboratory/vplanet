from benchmark import Benchmark, benchmark 
import astropy.units as u 
 
@benchmark( 
   { 
       "log.initial.system.Age": {"value": 6.311520e+13, "unit": u.sec}, 
       "log.initial.system.Time": {"value": 0.000000, "unit": u.sec}, 
       "log.initial.system.TotAngMom": {"value": 1.115191e+42, "unit": (u.kg * u.m ** 2) / u.sec}, 
       "log.initial.system.TotEnergy": {"value": -2.515652e+39, "unit": u.erg}, 
       "log.initial.system.PotEnergy": {"value": -2.556201e+39, "unit": u.Joule}, 
       "log.initial.system.KinEnergy": {"value": 4.054947e+37, "unit": u.Joule}, 
       "log.initial.system.DeltaTime": {"value": 3.155760e+07, "unit": u.sec}, 
       "log.initial.a.Mass": {"value": 1.988416e+29, "unit": u.kg}, 
       "log.initial.a.Radius": {"value": 97.114438, "unit": u.Rearth}, 
       "log.initial.a.RadGyra": {"value": 0.448345}, 
       "log.initial.a.RotAngMom": {"value": 1.115191e+42, "unit": (u.kg * u.m ** 2) / u.sec}, 
       "log.initial.a.RotVel": {"value": 4.504445e+04, "unit": u.m / u.sec}, 
       "log.initial.a.BodyType": {"value": 0.000000}, 
       "log.initial.a.RotRate": {"value": 7.272205e-05, "unit": 1 / u.sec}, 
       "log.initial.a.RotPer": {"value": 1.000000, "unit": u.day}, 
       "log.initial.a.Density": {"value": 199.752981, "unit": u.kg / u.m ** 3}, 
       "log.initial.a.HZLimitDryRunaway": {"value": 3.200490e+10, "unit": u.m}, 
       "log.initial.a.HZLimRecVenus": {"value": 2.879712e+10, "unit": u.m}, 
       "log.initial.a.HZLimRunaway": {"value": 3.790332e+10, "unit": u.m}, 
       "log.initial.a.HZLimMoistGreenhouse": {"value": 3.810337e+10, "unit": u.m}, 
       "log.initial.a.HZLimMaxGreenhouse": {"value": 7.307316e+10, "unit": u.m}, 
       "log.initial.a.HZLimEarlyMars": {"value": 7.969376e+10, "unit": u.m}, 
       "log.initial.a.Instellation": {"value": -1.000000, "unit": u.kg / u.sec ** 3}, 
       "log.initial.a.CriticalSemiMajorAxis": {"value": -1.000000, "unit": u.m}, 
       "log.initial.a.LXUVTot": {"value": 2.136736e+22, "unit": u.kg / u.sec ** 3}, 
       "log.initial.a.LostEnergy": {"value": 5.562685e-309, "unit": u.Joule}, 
       "log.initial.a.LostAngMom": {"value": 5.562685e-309, "unit": (u.kg * u.m ** 2) / u.sec}, 
       "log.initial.a.Luminosity": {"value": 0.055557, "unit": u.LSUN}, 
       "log.initial.a.LXUVStellar": {"value": 2.136736e+22, "unit": u.W}, 
       "log.initial.a.Temperature": {"value": 2971.232396, "unit": u.K}, 
       "log.initial.a.LXUVFrac": {"value": 0.001000}, 
       "log.initial.a.RossbyNumber": {"value": 0.014575}, 
       "log.initial.a.DRotPerDtStellar": {"value": 4.420158e-10}, 
   } 
)
class Test_HZSingle(Benchmark): 
   pass 
