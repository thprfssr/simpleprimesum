#!/bin/bash
# TODO: So I wanted to make the python script include the host name. But I just
# realized that what I did was to include the hostname of the machine running
# the ANALYSIS, not the machine that actually collected the data. So I need to
# fix that.



# All the data from collect.sh will be written here
datafile=data.tsv

# If the datafile doesn't exist, we want to create it.
# If it does exist, then we leave it alone.
touch $datafile

# The datafile needs to be formatted correctly in order to work with it
# in Python. In particular, the header needs to hold the names of the
# columns in the table.
header=$(head -n 1 $datafile)
if [ "$header" != "$(printf "N\tT\n")" ]; then
	# Add the correct header to the first line
	sed -i "1s/^/N\tT\n/" $datafile
fi

# Finally, we run this short python script.
python <<EOF
import pandas as pd
import matplotlib.pyplot as plt

# Read the .tsv file into a pandas dataframe
dataframe = pd.read_csv("$datafile", sep = '\\t')

# Select the appropriate columns in the dataframe
x = dataframe["N"]
y = dataframe["T"] / 1e9 # To convert it from nanoseconds to seconds

# Plot the data, label the axes, and save the graph to file.
plt.scatter(x, y)
plt.xlabel("N")
plt.ylabel("T (s)")
plt.title("Efficiency of Albert's primesum algorithm in $HOSTNAME")
plt.grid()
# Uncomment the line below to also display the plot on-screen.
#plt.show()
plt.savefig("plot.png")
plt.savefig("plot.pdf")
EOF
