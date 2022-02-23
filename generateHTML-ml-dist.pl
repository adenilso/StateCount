#!/usr/bin/perl -w

use strict;
use IO::Handle;

autoflush STDOUT 1; 

my $fsmfile = shift();
my $numfsms = shift();
my $dir   = shift();
my $htmldir   = shift();
my $verboseLevel;
$verboseLevel = 3 unless defined $verboseLevel;

my  $seed = time() % 123443;

my $sc = "/home/adenilso/development/StateCount-C++/fsm-k-ml-sc";

open(FD, "$sc $numfsms < $fsmfile | tee /tmp/gensc |");

open(NULL, ">/dev/null");
  
if ($verboseLevel > 2) {
  *STD1 = *STDOUT;
} else {
  *STD1 = *NULL;
}

if ($verboseLevel > 1) {
  *STD2 = *STDOUT;
} else {
  *STD2 = *NULL;
}

*STD3 = *STDOUT;

print STD1 "<H2> Original FSM </H2>\n";
my $count = 0;
while(<FD>) {
#  print;
  if (m/^INFO>BEGIN FSM/) {
    print STD1 "<HR>\n";
    print STD1 "<H2> FSM $count </H2>\n";
  } elsif (m/^INFO>END FSM/){
    $count++;
  } elsif (m/^DOT>(digraph FSM {)/) {
    open(DOT, "| dot -Tjpg > $dir/rg-fsm-$seed-$count.jpg");
    print DOT "digraph FSM {\n";
    while(<FD>) {
      if (m/^DOT>}/) {
        print DOT "}\n";
        close DOT;
        print STD1 "<TABLE><TR>\n";
        if ($count > 0) {
          print STD1 "<TD><IMG SRC=$htmldir/rg-fsm-$seed-0.jpg></TD>\n";
        }
        print STD1 "<TD><IMG SRC=$htmldir/rg-fsm-$seed-$count.jpg></TD>\n";
        print STD1 "</TR></TABLE>\n";
        last;
      } elsif (m/^DOT>(.*)/) {
        print DOT "$1\n";
      }
    } 
  } elsif (m/FSM>(.*)/) {
    print STD1 "<CODE>$1</CODE><BR>\n";
  } elsif (m/>>>(.*)/) {
    print STD1 "$1<BR>\n";
  }
}

close FD;
