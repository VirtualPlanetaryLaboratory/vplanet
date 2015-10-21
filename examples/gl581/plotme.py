import matplotlib.pyplot as pl
import vplot

fig, ax = vplot.Plot()

# Change the title of the plot
fig.suptitle('Whoa! VPLOT is AWESOME', fontsize = 24)

# Increase horizontal (width) padding between subplots
fig.subplots_adjust(wspace = 0.75)

# Change an axis label
ax[2].set_ylabel('Flux (pirate-ninjas)')
ax[6].set_ylabel('Luminosity (crazy units)')

# Change the x axis scale of one of the subplots
ax[1].set_xscale('Linear')

# Make the tick label font size smaller for that subplot
for tick in ax[1].xaxis.get_major_ticks():
  tick.label.set_fontsize(8) 

# Add a horizontal line to one of the subplots
ax[14].axhline(5980, color = 'r', ls = '--')

# Change the line style of one of the curves
ax[15].lines[0].set_color('k')
ax[15].lines[0].set_linestyle('--')

# Update it in the legend!
leg = ax[15].legend()
leg.legendHandles[0].set_color('k')
leg.legendHandles[0].set_linestyle('--')

# Scale the data in one of the subplots (i.e., a unit change)
# We're converting TO to kg here.
x, y = ax[13].lines[0].get_data()
y *= 1.39e21
ax[13].lines[0].set_data(x,y)
ax[13].set_ylim(0,1.5e21)
ax[13].set_ylabel('Surface Water (kg)')

# This is a little hack to prevent plotting an offset at the top left corner of the plot
ax[13].set_yticklabels(ax[13].get_yticks())

pl.show()