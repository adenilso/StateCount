#!/usr/bin/perl -w

use strict;
use IO::Handle;

autoflush STDOUT 1; 

my $fsmfile = shift();
my $tmstmp = shift();
my $distType = shift();
my $stopfile = "/tmp/ex3-stopfile$tmstmp.txt";
my $dir   = shift();
my $htmldir   = shift();
my $verboseLevel;
$verboseLevel = 1 unless defined $verboseLevel;

my $sc = "/home/adenilso/development/StateCount-C++/fsm-all-sc";

open(FD, "$sc $stopfile $distType < $fsmfile | tee /tmp/gensc |");

open(NULL, ">/dev/null");

*STD1 = *STDOUT;
*STD2 = *STDOUT;
*STD3 = *STDOUT;

print STD3 "This process can take a long time. This page will be updated with the most recent information. Please, reload it after some time.<BR>\n";
print STD3 "Click <A HREF=\"../interruptExp3.php?code=$tmstmp\">here</A> to interrupt.<BR>\n";
print STD3 "Click <A HREF=\"../exp3.php\">here</A> to run another sample.<BR>\n";

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

my $ctc = 0;
my %tcseq = ();
my $crsc = 0;
my %rscseq = ();
my $csc = 0;
my %scseq = ();

while(<FD>) {
  if (m/^INFO>BEGIN FSM/ or m/^INFO>BEGIN MUTANT FSM/) {
    print STD1 "<HR>\n";
    print STD1 "<H2> FSM $count </H2>\n";
    %tmpinfos = ();
    $tmpinfos{"FSM Number"} = $count;
  } elsif (m/^INFO>END FSM/ or m/^INFO>END MUTANT FSM/){
    $count++;
  } elsif (m/FSM>(.*)/) {
    print STD1 "<CODE>$1</CODE><BR>\n";
  } elsif (m/INFO>BEGIN DTC/) {
    print "81<BR>\n";
    my $dtc = "";
    while (<FD>) {
      if (m/INFO>END DTC/) {
        open(GP, "| tee /tmp/gnuplot-input | gnuplot");
        print GP <<EOF;
set terminal png
set xlabel "output distance"
set ylabel "FSMs"
set output '$dir/dtc-1-$tmstmp.png'
plot '-' using (\$1):(\$2) title "not killed FSM x outDist" with linespoints
$dtc
e
set output '$dir/dtc-2-$tmstmp.png'
plot '-' using (\$1):(\$5) title "FSM x outDist" with linespoints
$dtc
e
set yrange [0.0:1.0]
set output '$dir/dtc-3-$tmstmp.png'
plot '-' using (\$1):(\$2/\$4) title "\% of not killed FSM x outDist" with linespoints, '-' using (\$1):(\$5/\$7) title "\% of FSM x outDist" with linespoints
$dtc
e
$dtc
e
set key right bottom
set output '$dir/dtc-4-$tmstmp.png'
plot '-' using (\$1):(\$3/\$4) title "sum of \% of not killed FSM x outDist" with linespoints, '-' using (\$1):(\$6/\$7) title "sum of \% FSM x outDist" with linespoints
$dtc
e
$dtc
e
EOF
        print STD1 "<h1>TC</h1>\n";
        print STD1 "<h3>Number of not killed FSMs <i>versus</i> output distance</h3>\n";
        print STD1 "<IMG SRC=\"$htmldir/dtc-1-$tmstmp.png\"><BR>\n";
        print STD1 "<h3>Number of FSMs <i>versus</i> output distance</h3>\n";
        print STD1 "<IMG SRC=\"$htmldir/dtc-2-$tmstmp.png\"><BR>\n";
        print STD1 "<h3>Both previous graphics together</h3>\n";
        print STD1 "<IMG SRC=\"$htmldir/dtc-3-$tmstmp.png\"><BR>\n";
        print STD1 "<h3>Accumulative</h3>\n";
        print STD1 "Counts the percentage of FSMs (not killed and total, respectively) that is up to distance k";
        print STD1 "<IMG SRC=\"$htmldir/dtc-4-$tmstmp.png\"><BR>\n";
        last;
      } elsif (m/DTC>(.*)/) {
        $dtc .= "$1\n";
      }
    }
  } elsif (m/INFO>BEGIN DRSC/) {
    my $drsc = "";
    while (<FD>) {
      if (m/INFO>END DRSC/) {
        open(GP, "| gnuplot");
        print GP <<EOF;
set terminal png
set xlabel "output distance"
set ylabel "FSMs"
set output '$dir/drsc-1-$tmstmp.png'
plot '-' using (\$1):(\$2) title "not killed FSM x outDist" with linespoints
$drsc
e
set output '$dir/drsc-2-$tmstmp.png'
plot '-' using (\$1):(\$5) title "FSM x outDist" with linespoints
$drsc
e
set yrange [0.0:1.0]
set output '$dir/drsc-3-$tmstmp.png'
plot '-' using (\$1):(\$2/\$4) title "\% of not killed FSM x outDist" with linespoints, '-' using (\$1):(\$5/\$7) title "\% of FSM x outDist" with linespoints
$drsc
e
$drsc
e
set key right bottom
set output '$dir/drsc-4-$tmstmp.png'
plot '-' using (\$1):(\$3/\$4) title "sum of \% of not killed FSM x outDist" with linespoints, '-' using (\$1):(\$6/\$7) title "sum of \% FSM x outDist" with linespoints
$drsc
e
$drsc
e
EOF
close GP;
        print STD1 "<h1>RSC</h1>\n";
        print STD1 "<h3>Number of not killed FSMs <i>versus</i> output distance</h3>\n";
        print STD1 "<IMG SRC=\"$htmldir/drsc-1-$tmstmp.png\"><BR>\n";
        print STD1 "<h3>Number of FSMs <i>versus</i> output distance</h3>\n";
        print STD1 "<IMG SRC=\"$htmldir/drsc-2-$tmstmp.png\"><BR>\n";
        print STD1 "<h3>Both previous graphics together</h3>\n";
        print STD1 "<IMG SRC=\"$htmldir/drsc-3-$tmstmp.png\"><BR>\n";
        print STD1 "<h3>Accumulative</h3>\n";
        print STD1 "Counts the percentage of FSMs (not killed and total, respectively) that is up to distance k";
        print STD1 "<IMG SRC=\"$htmldir/drsc-4-$tmstmp.png\"><BR>\n";
        last;
      } elsif (m/DRSC>(.*)/) {
        $drsc .= "$1\n";
      }
    }
  } elsif (m/^INFO>(.*)/) {
    my $info = $1;
    print STD1 "$info<BR>\n";
  } elsif (m/seq tc:(.*)/) {
    print STD1 "<B>tc$ctc</B>: $1<BR>\n";
    $tcseq{$ctc} = $1;
    $ctc ++;
  } elsif (m/seq rsc:(.*)/) {
    print STD1 "<B>rsc$crsc</B>: $1<BR>\n";
    $rscseq{$crsc} = $1;
    $crsc ++;
  } elsif (m/seq sc:(.*)/) {
    print STD1 "<B>sc$csc</B>: $1<BR>\n";
    $scseq{$csc} = $1;
    $csc ++;
  } elsif (m/>>>(.*)/) {
    print STD1 "$1<BR>\n";
  }
}

close FD;
