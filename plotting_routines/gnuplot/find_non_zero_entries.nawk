
# Add absolute value function

function abs(v) {v += 0; return v < 0 ? -v : v}

BEGIN {FS = ","; OFS = ","; number_non_zero_entries = 0; EPSILON = 1e-24;}
{
# Variables
# column_number
if (abs($column_number + 0.0) > EPSILON)
   number_non_zero_entries = number_non_zero_entries + 1;
}
END {printf("%d\n",number_non_zero_entries);}
