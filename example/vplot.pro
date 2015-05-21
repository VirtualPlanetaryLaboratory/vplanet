pro vplot

                                ;### vplot.pro PED 3/24/15 ###
                                ;### Plotting routine to plot the output of vplanet.
  star='gl581'
  planet1='c'
  planet2='d'

  outfile_star=star+'.star.forward'
  outfile_planet1=star+'.'+planet1+'.forward'
  outfile_planet2=star+'.'+planet2+'.forward'

                                ;# Define all possible arrays.
  n=1e4
  time=dblarr(n)
  semim=dblarr(n)
  ecce=dblarr(n)
  obli=dblarr(n)
  surfenfluxeqtide=dblarr(n)
  surfenfluxradheat=dblarr(n)
  k40pow=dblarr(n)
  th232pow=dblarr(n)
  u238pow=dblarr(n)
  radpow=dblarr(n)
  surfenfluxtot=dblarr(n)

;# Output file formatting.
  print,'Default output order: Time Semim Ecce Obli -SurfEnFluxEqtide -SurfEnFluxRadheat '+$
  '-40kpow -232thpow -238upow -radpow -SurfEnFluxT'
  ch_outputorder=0.
  read,'Change output order? (n=0,y=1) :',ch_outputorder
  if ch_outputorder then begin
     print,'UNDER CONSTRUCTION!'
     stop
  endif

                                ;# Read in planet1.
  openr,1,outfile_planet1       ;open file to read.
  i=0.  ;index of line in file (also time step number)
  while not EOF(1) do begin
     tmp=dblarr(11)
     readf,1,tmp
     time(i)=tmp(0)
     semim(i)=tmp(1)
     ecce(i)=tmp(2)
     obli(i)=tmp(3)
     surfenfluxeqtide(i)=tmp(4)
     surfenfluxradheat(i)=tmp(5)
     k40pow(i)=tmp(6)
     th232pow(i)=tmp(7)
     u238pow(i)=tmp(8)
     radpow(i)=tmp(9)
     surfenfluxtot(i)=tmp(10)
;     print,i,time(i),semim(i),ecce(i) &      stop
     i=i+1.
  endwhile
  close,1  ;close file.

                                ;# Shorten arrays to just the length of the output.
  time=time(0:i-1)
  semim=semim(0:i-1)
  ecce=ecce(0:i-1)  
  obli=obli(0:i-1)
  surfenfluxeqtide=surfenfluxeqtide(0:i-1)
  surfenfluxradheat=surfenfluxradheat(0:i-1)  
  k40pow=k40pow(0:i-1)
  th232pow=th232pow(0:i-1)
  u238pow=u238pow(0:i-1)
  radpow=radpow(0:i-1)
  surfenfluxtot=surfenfluxtot(0:i-1)  

;#####  DONE READING FILES #######

;###### PRINT FINAL STATES  ######
  print,'Final state at t='+stringf(time(i-1))
  print,'a='+stringf(semim(i-1))+', e='+stringf(ecce(i-1))+', obli='+stringf(obli(i-1))
  print,'surfenfluxeqtide='+stringf(surfenfluxeqtide(i-1))+', surfenfluxradheat='+stringf(surfenfluxradheat(i-1))+$
        ', surfenfluxtot='+stringf(surfenfluxtot(i-1))
  print,'k40pow='+stringf(k40pow(i-1))+', th232pow='+stringf(th232pow(i-1))+', u238pow='+stringf(u238pow(i-1))+$
        ', radpow='+stringf(radpow(i-1))
;#### END PRINT STATE ####
  
;##### BEGIN PLOTTING #######
;# Plotting options.
  loadct,39
  black=0 & blue=50 & green=150. & red=250. & white=255.
  !p.background=255             ;white
  !p.color=0                    ;black
  colors=[black,blue,green,red]
  !p.font=-1.
  !p.charsize=1.5
  LABEL_OPTIONS:
  opt_plot=-1.
  read,'Choose plotting option: 1=Orbit ; 2=Heat Fluxes ; 3=Radiogenic Powers :',opt_plot
  case opt_plot of
     -1:goto,LABEL_END
     1:goto,LABEL1
     2:goto,LABEL2
     3:goto,LABEL3
  endcase

                                ;111111111111111111111111
                                ;### PLOT: Orbit Evolution a,e vs t.
  LABEL1:
  !p.multi=[0,2,2]
  xtitle='yr'
  ytitle='a [AU]'
  plot,time,semim,xtitle=xtitle,ytitle=ytitle
  ytitle='eccentricity'
  plot,time,ecce,xtitle=xtitle,ytitle=ytitle
  goto,LABEL_OPTIONS
                                ;111111111111111111111111

                                ;22222222222222222222222222222
                                ;### PLOT: Surface Heat Fluxes
  LABEL2:
  !p.multi=[0,1,1]
  xtitle='yr'
  ytitle='W m!U-2!N'
  ylog=1.
  plot,time,surfenfluxtot,xtitle=xtitle,ytitle=ytitle,ylog=ylog
  oplot,time,surfenfluxeqtide,color=colors(1)
  oplot,time,surfenfluxradheat,color=colors(2)
  legend,['Total','EqTide','Rad'],color=colors(0:2),/right,linestyle=intarr(3)
  goto,LABEL_OPTIONS
                                ;22222222222222222222222222222

                                ;333333333333333333
                                ;### PLOT: Rad Power
  LABEL3:
  !p.multi=[0,1,1]
  xtitle='yr'
  ytitle='TW'
  ylog=1
  yra=[min([radpow,k40pow,th232pow,u238pow]),max([radpow,k40pow,th232pow,u238pow])]
  plot,time,radpow,xtitle=xtitle,ytitle=ytitle,ylog=ylog,yra=yra
  oplot,time,k40pow,color=colors(1)
  oplot,time,th232pow,color=colors(2)
  oplot,time,u238pow,color=colors(3)
  legend,['Total','K40','Th232','U238'],color=colors(0:3),linestyle=intarr(4),/right
  goto,LABEL_OPTIONS
                                ;333333333333333333

  
  LABEL_END:
  print,'END of vplot.pro'
  stop
end
