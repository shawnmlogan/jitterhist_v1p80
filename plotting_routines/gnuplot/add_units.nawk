
# Add absolute value function
  
function abs(v) {v += 0; return v < 0 ? -v : v}

BEGIN {FS = ","; OFS = ",";}
{
# num_digits = 4;
# suffix = "Hz";
width = 1;

if (abs(($1 + 0.0)) < 1e-9) {
	if (($1 + 0.0) == 0.0)
		value_label = sprintf("0.0 %s",suffix);
	else
		value_label = sprintf("%*.*f p%s",width,num_digits,($1 + 0.0)/1e-12,suffix);
	} else {
	   if (abs(($1 + 0.0)) < 1e-6) {
	      value_label = sprintf("%*.*f n%s",width,num_digits,($1 + 0.0)/1e-09,suffix);
	   } else {
	      if (abs(($1 + 0.0)) < 1e-3) {
	         value_label = sprintf("%*.*f u%s",width,num_digits,($1 + 0.0)/1e-06,suffix);
	      } else {
	         if (abs(($1 + 0.0)) < 1.0) {
	            value_label = sprintf("%*.*f m%s",width,num_digits,($1 + 0.0)/1e-03,suffix);
	         } else {
	            if (abs(($1 + 0.0)) < 1e3) {
	               value_label = sprintf("%*.*f %s",width,num_digits,($1 + 0.0),suffix);
	            } else {  
	               if (abs(($1 + 0.0)) < 1e6) {
	                  value_label = sprintf("%*.*f k%s",width,num_digits,($1 + 0.0)/1e3,suffix);
	               } else {
	                  if (abs(($1 + 0.0)) < 1e9) {
	                      value_label = sprintf("%*.*f M%s",width,num_digits,($1 + 0.0)/1e6,suffix);
	                  } else {
	                      value_label = sprintf("%*.*f G%s",width,num_digits,($1 + 0.0)/1e9,suffix);
	                  }
	               }
	            }
	         }
	      }
	   }
	}
}

END { printf("%s\n",value_label);}
