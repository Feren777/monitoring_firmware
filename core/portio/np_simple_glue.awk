# 
# np_simple_glue.awk
#
BEGIN {
  print "dnl"
  print "dnl This file has been automatically generated by np_simple_glue.awk"
  print "dnl"
  print "divert(0)"
}
/^[\s#]*$/ {next;}
{
  if ($3 == 1 || $3 == "HIGH") {
    if ($2 == 1 || $2 == "INPUT") {
      # The pin is a normal (i.e. non-inverted) input pin.
      print "np_simple_normal_in("$1","$4")"
    } 
    else {
      # Non-inverted output pin.
      print "np_simple_normal_out("$1","$4")"
    }
  }
  else {
    if ($2 == 1 || $2 == "INPUT") {
      # The pin is an inverted input pin.
      print "np_simple_inverted_in("$1","$4")"
    } 
    else {
      # Inverted output pin.
      print "np_simple_inverted_out("$1","$4")"
    }
  }
}
