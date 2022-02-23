#!/usr/bin/perl -w

use strict;
use IO::Handle;

autoflush STDOUT 1; 

my $nouts = shift();
my $ninps = shift();
my $nstas = shift();
my $variants = shift();
my $seed = shift();
my $dir   = shift();
my $htmldir   = shift();
my $watchfile   = shift();
my $verboseLevel = shift();
$verboseLevel = 1 unless defined $verboseLevel;

my $sc = "/home/adenilso/development/StateCount-C++/sc";

if ($seed == -1) {
  $seed = time() % 123443;
}

open(FD, "$sc $nouts $ninps $nstas $variants $seed | tee /tmp/gensc |");

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

print STD3 "This process can take a long time. This page will be updated with the most recent information. Please, reload it after some time.<BR>\n";
print STD3 "Click <A HREF=\"../interruptSML.php?seed=$seed\">here</A> to interrupt.<BR>\n";
print STD3 "Click <A HREF=\"../exp1.php?seed=$seed\">here</A> to run another sample.<BR>\n";
my $count = 0;
my %tmpinfos = ();
my %infos = ();
my $numTrans;
my @numTrans = ();
my %minNT = ();
my %maxNT = ();
my %sumNT = ();
my %sqSumNT = ();
my %numNT = ();

my %minNTSP = ();
my %maxNTSP = ();
my %sumNTSP = ();
my %sqSumNTSP = ();
my %numNTSP = ();
my $numTransSeparablePairs;
my %numTransSeparablePairs;
my @numTransSeparablePairs;

my %minSP = ();
my %maxSP = ();
my %sumSP = ();
my %sqSumSP = ();
my %numSP = ();
my $numSeparablePairs;
my %numSeparablePairs;
my @numSeparablePairs;
my @fields = ("Total Length for TC", "Total Length for RSC", "Total Length for SC", "Number of Separable Pairs");

while(<FD>) {
#  print;
  if (m/^INFO>BEGIN FSM/) {
    print STD1 "<HR>\n";
    print STD1 "<H2> FSM $count </H2>\n";
    %tmpinfos = ();
    $tmpinfos{"FSM Number"} = $count;
  } elsif (m/^INFO>END FSM/){
    foreach my $f (@fields) {
      if ($minNT{$numTrans}{$f} > $tmpinfos{$f}) {
        $minNT{$numTrans}{$f} = $tmpinfos{$f};
      }
      if ($maxNT{$numTrans}{$f} < $tmpinfos{$f}) {
        $maxNT{$numTrans}{$f} = $tmpinfos{$f};
      }
      $sumNT{$numTrans}{$f} += $tmpinfos{$f};
      $sqSumNT{$numTrans}{$f} += $tmpinfos{$f} * $tmpinfos{$f};
      $numNT{$numTrans}{$f} ++;

      if ($minSP{$numSeparablePairs}{$f} > $tmpinfos{$f}) {
        $minSP{$numSeparablePairs}{$f} = $tmpinfos{$f};
      }
      if ($maxSP{$numSeparablePairs}{$f} < $tmpinfos{$f}) {
        $maxSP{$numSeparablePairs}{$f} = $tmpinfos{$f};
      }
      $sumSP{$numSeparablePairs}{$f} += $tmpinfos{$f};
      $sqSumSP{$numSeparablePairs}{$f} += $tmpinfos{$f} * $tmpinfos{$f};
      $numSP{$numSeparablePairs}{$f} ++;

      $numTransSeparablePairs = "$numTrans & $numSeparablePairs";
      if (not defined($numTransSeparablePairs{$numTransSeparablePairs})) {
          $numTransSeparablePairs{$numTransSeparablePairs} = 1;
          foreach my $f (@fields) {
            $minNTSP{$numTransSeparablePairs}{$f} = 1.0e20;
            $maxNTSP{$numTransSeparablePairs}{$f} = -1.0e20;
            $sumNTSP{$numTransSeparablePairs}{$f} = 0.0;
            $sqSumNTSP{$numTransSeparablePairs}{$f} = 0.0;
            $numNTSP{$numTransSeparablePairs}{$f} = 0;
          }
          push(@numTransSeparablePairs, $numTransSeparablePairs);
      }
      if ($minNTSP{$numTransSeparablePairs}{$f} > $tmpinfos{$f}) {
        $minNTSP{$numTransSeparablePairs}{$f} = $tmpinfos{$f};
      }
      if ($maxNTSP{$numTransSeparablePairs}{$f} < $tmpinfos{$f}) {
        $maxNTSP{$numTransSeparablePairs}{$f} = $tmpinfos{$f};
      }
      $sumNTSP{$numTransSeparablePairs}{$f} += $tmpinfos{$f};
      $sqSumNTSP{$numTransSeparablePairs}{$f} += $tmpinfos{$f} * $tmpinfos{$f};
      $numNTSP{$numTransSeparablePairs}{$f} ++;
    }
    push(@{$infos{$numTrans}}, [%tmpinfos]);
    $count++;
  } elsif (m/^INFO>(.*)/) {
    my $info = $1;
    if ($info =~ m/^([^:]*):(.*)$/) {
      $tmpinfos{$1} = $2;
      if ($1 eq "Number of Transitions") {
        $numTrans = $2;
        if (not defined($infos{$numTrans})) {
          push(@numTrans, $numTrans);
          foreach my $f (@fields) {
            $minNT{$numTrans}{$f} = 1.0e20;
            $maxNT{$numTrans}{$f} = -1.0e20;
            $sumNT{$numTrans}{$f} = 0.0;
            $sqSumNT{$numTrans}{$f} = 0.0;
            $numNT{$numTrans}{$f} = 0;
          }
        }
     } elsif ($1 eq "Number of Separable Pairs") {
        $numSeparablePairs = $2;
        if (not defined($numSeparablePairs{$numSeparablePairs})) {
          $numSeparablePairs{$numSeparablePairs} = 1;
          foreach my $f (@fields) {
            $minSP{$numSeparablePairs}{$f} = 1.0e20;
            $maxSP{$numSeparablePairs}{$f} = -1.0e20;
            $sumSP{$numSeparablePairs}{$f} = 0.0;
            $sqSumSP{$numSeparablePairs}{$f} = 0.0;
            $numSP{$numSeparablePairs}{$f} = 0;
          }
          push(@numSeparablePairs, $numSeparablePairs);
        }
      }
    }
    print STD1 "$info<BR>\n";
  } elsif (m/^DOT>(digraph FSM {)/) {
    if ($verboseLevel > 2) {
      open(DOT, "| dot -Tjpg > $dir/rg-fsm-$seed-$count.jpg");
    } else {
      open(DOT, "| cat > /dev/null");
    }
    print DOT "digraph FSM {\n";
    while(<FD>) {
      if (m/^DOT>}/) {
        print DOT "}\n";
        close DOT;
        print STD1 "<IMG SRC=$htmldir/rg-fsm-$seed-$count.jpg><BR>\n";
        last;
      } elsif (m/^DOT>(.*)/) {
        print DOT "$1\n";
      }
    } 
  } elsif (m/FSM>(.*)/) {
    print STD1 "<CODE>$1</CODE><BR>\n";
  }
}

close FD;
print STD2 "Click <A HREF=\"../exp1.php?seed=$seed\">here</A> to run another sample.<BR>\n";
my @headers = ("FSM Number", "Number of Transitions", "Total Length for TC", "Total Length for RSC", "Number of Separable Pairs", "Total Length for SC");
print STD2 "<TABLE BORDER=\"1\" WIDTH=\"100%\">\n<TR>";
foreach my $h (@headers) {
  print STD2 "<TH>$h</TH>";
}
print STD2 "</TR>\n";

foreach my $numTrans (@numTrans) {
  foreach my $info (@{$infos{$numTrans}}) {
    my %tmpinfos = @{$info};
    print STD2 "<TR>";
    foreach my $h (@headers) {
      print STD2 "<TD ALIGN=\"CENTER\">$tmpinfos{$h}</TD>\n";
    }
    print STD2 "</TR>\n";
  }
}
print STD2 "</TABLE>";

print STD3 "<h2>By Specifiedness</h2>\n";
print STD3 "(min/max/avg/stddev/num)<BR>\n";

print STD3 "<TABLE BORDER=\"1\" WIDTH=\"100%\">\n<TR>";
print STD3 "<TH>Number of Transitions</TH>\n";
foreach my $f (@fields) {
  print STD3 "<TH>$f</TH>";
}
print STD3 "</TR>\n";

foreach my $numTrans (@numTrans) {
  print STD3 "<TR>";
  print STD3 "<TD>$numTrans</TD>\n";
  foreach my $f (@fields) {
    printf(STD3 "<TD>(%.2f/%.2f/%.2f/%.2f/%d)</TD>",
      $minNT{$numTrans}{$f},
      $maxNT{$numTrans}{$f},
      ($numNT{$numTrans}{$f} > 0)?$sumNT{$numTrans}{$f}/$numNT{$numTrans}{$f}:0.0,
      ($numNT{$numTrans}{$f} > 1)?sqrt((($numNT{$numTrans}{$f}*$sqSumNT{$numTrans}{$f}) - $sumNT{$numTrans}{$f}**2)/($numNT{$numTrans}{$f}*($numNT{$numTrans}{$f} - 1))):0.0,
      $numNT{$numTrans}{$f}
    );
  }
  print STD3 "</TR>\n";
}
print STD3 "</TABLE>";

print STD3 "<h2>By Separable Pairs</h2>\n";
print STD3 "(min/max/avg/stddev/num)<BR>\n";

print STD3 "<TABLE BORDER=\"1\" WIDTH=\"100%\">\n<TR>";
print STD3 "<TH>Number of Separable Pairs</TH>\n";
foreach my $f (@fields) {
  print STD3 "<TH>$f</TH>";
}
print STD3 "</TR>\n";

foreach my $numSeparablePairs (sort {$a <=> $b} @numSeparablePairs) {
  print STD3 "<TR>";
  print STD3 "<TD>$numSeparablePairs</TD>\n";
  foreach my $f (@fields) {
    printf(STD3 "<TD>(%.2f/%.2f/%.2f/%.2f/%d)</TD>",
      $minSP{$numSeparablePairs}{$f},
      $maxSP{$numSeparablePairs}{$f},
      ($numSP{$numSeparablePairs}{$f} > 0)?$sumSP{$numSeparablePairs}{$f}/$numSP{$numSeparablePairs}{$f}:0.0,
      ($numSP{$numSeparablePairs}{$f} > 1)?sqrt((($numSP{$numSeparablePairs}{$f}*$sqSumSP{$numSeparablePairs}{$f}) - $sumSP{$numSeparablePairs}{$f}**2)/($numSP{$numSeparablePairs}{$f}*($numSP{$numSeparablePairs}{$f} - 1))):0.0,
      $numSP{$numSeparablePairs}{$f}
    );
  }
  print STD3 "</TR>\n";
}
print STD3 "</TABLE>";

print STD3 "<h2>By Specifiedness AND Separable Pairs</h2>\n";
print STD3 "(min/max/avg/stddev/num)<BR>\n";

print STD3 "<TABLE BORDER=\"1\" WIDTH=\"100%\">\n<TR>";
print STD3 "<TH>Number of Separable Pairs</TH>\n";
foreach my $f (@fields) {
  print STD3 "<TH>$f</TH>";
}
print STD3 "</TR>\n";

foreach my $numTransSeparablePairs (sort {$a cmp $b} @numTransSeparablePairs) {
  print STD3 "<TR>";
  print STD3 "<TD>$numTransSeparablePairs</TD>\n";
  foreach my $f (@fields) {
    printf(STD3 "<TD>(%.2f/%.2f/%.2f/%.2f/%d)</TD>",
      $minNTSP{$numTransSeparablePairs}{$f},
      $maxNTSP{$numTransSeparablePairs}{$f},
      ($numNTSP{$numTransSeparablePairs}{$f} > 0)?$sumNTSP{$numTransSeparablePairs}{$f}/$numNTSP{$numTransSeparablePairs}{$f}:0.0,
      ($numNTSP{$numTransSeparablePairs}{$f} > 1)?sqrt((($numNTSP{$numTransSeparablePairs}{$f}*$sqSumNTSP{$numTransSeparablePairs}{$f}) - $sumNTSP{$numTransSeparablePairs}{$f}**2)/($numNTSP{$numTransSeparablePairs}{$f}*($numNTSP{$numTransSeparablePairs}{$f} - 1))):0.0,
      $numNTSP{$numTransSeparablePairs}{$f}
    );
  }
  print STD3 "</TR>\n";
}
print STD3 "</TABLE>";
