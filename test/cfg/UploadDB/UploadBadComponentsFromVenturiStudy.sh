#!/bin/sh

R1="1 14 15 16 17 20"
R6095="1 14 15 16 17 20"
R6835="1 2 3 4 5 14 15 16 17 20"
R6931="1 3 14 15 16 17 20"
R7233="1 3 7 14 15 16 17 20"
R8427="1 3 7 14 15 16 17 19 20"
R8433="1 3 7 11 14 15 16 17 19 20"
R8555="1 3 7 14 15 16 17 19 20"
R9149="1 3 6 7 8 13 14 15 16 17 19 20"
R9163="1 3 7 13 14 15 16 17 19 20"
R9342="1 3 7 14 15 16 17 19 20"
R10213="1 3 7 9 10 12 14 15 16 17 19 20"
R10685="1 7 9 10 14 15 16 17 19 20"
R11097="1 7 14 15 16 17 19 20"
R11270="1 7 14 15 16 17 18 19 20"
R12043="1 7 14 15 16 17 18 19 20 21"


A1="0x1600e444, 0x1600e448, 0x1600e44c"

A2="0x1600e414,0x1600e418,0x1600e41c,0x1600e424,0x1600e428,0x1600e42c,0x1600e434,0x1600e438,0x1600e43c"

A3="0x16006495,0x16006496,0x16006499,0x1600649a,0x1600649d,0x1600649e"

A4="0x1600a455,0x1600a456,0x1600a459,0x1600a45a,0x1600a45d,0x1600a45e"

A5="0x1600a815,0x1600a816,0x1600a819,0x1600a81a,0x1600a81d,0x1600a81e"

A6="0x16006845,0x16006846, 0x16006849, 0x1600684a, 0x1600684d, 0x1600684e"

A7="0x1a00a105, 0x1a00a106, 0x1a00a109, 0x1a00a10a, 0x1a00a10d, 0x1a00a10e, 0x1a00a111, 0x1a00a112, 0x1a00a115, 0x1a00a116, 0x1a00a119, 0x1a00a11a"

A8="0x1a0160a4, 0x1a0160a8, 0x1a0160ac, 0x1a0160b0, 0x1a0160b4, 0x1a0160b8"

A9="0x1a00e044, 0x1a00e048, 0x1a00e04c, 0x1a00e050, 0x1a00e054, 0x1a00e058"

A10="0x1a00e1c4, 0x1a00e1c8, 0x1a00e1cc, 0x1a00e1d0, 0x1a00e1d4, 0x1a00e1d8"

A11="0x1a016144, 0x1a016148, 0x1a01614c, 0x1a016150, 0x1a016154, 0x1a016158"

A12="0x1c09d3a5, 0x1c09d3a6, 0x1c09d3a9, 0x1c09d3aa, 0x1c09d3ad, 0x1c09d3ae, 0x1c09d3e4, 0x1c09d3e8, 0x1c09d3ec, 0x1c09d3f0, 0x1c09d3f4"

A13="0x1c0913a5, 0x1c0913a6, 0x1c0913a9, 0x1c0913aa, 0x1c0913ad, 0x1c0913ae, 0x1c0913e4, 0x1c0913e8, 0x1c0913ec, 0x1c0913f0, 0x1c0913f4"

A14="0x1600e8e4"


#--------------

A15="0x1600e848"   F15="2, 3" 

A16="0x180056b0"   F16="0, 1"

A17="0x180056ac"   F17="2, 3"

A18="0x18005cb2"   F18="2, 3"

A19="0x1c091346"   F19="0, 1"

A20="0x1a006086"   F20="0, 1"

A21="0x1c0892f4"   F21="0, 1"

#{ uint32 BadModule = insert_bad_module vuint32 BadChannelList = {insert_fiber} } 


eval `scramv1 runtime -sh`

workarea=tmp

mkdir $workarea

rm dbfile.db
cmscond_bootstrap_detector.pl --offline_connect sqlite_file:dbfile.db --auth /afs/cern.ch/cms/DB/conddb/authentication.xml STRIP

for run in 6095  6835  6931  7233  8427  8433  8555  9149  9163  9342  10213    10685    11097    11270    12043
  do
  echo -e "\nRun $run"
  eval blocklist=\${R$run}
  Tmodules=""
  Tapvs=""
  for block in $blocklist
    do
#    echo block $block
    eval modules=\${A$block}
    
    if [ $block -lt 15 ];
	then
	Tmodules=`echo $Tmodules , $modules `
    else
	eval apvs=\${F$block}
	Tapvs=`echo $Tapvs , { uint32 BadModule = $modules vuint32 BadApvList = {$apvs} }  `
    fi
  done
  Tmodules=`echo $Tmodules | sed -e "s#,##"`
  Tapvs=`echo $Tapvs | sed -e "s#,##"`
  

  cat template_SiStripBadFiberBuilder.cfg | sed -e "s@ insertRun@$run@g" -e "s@insertBadList@$Tapvs@" > $workarea/Fibers_$run.cfg
  rm -f  $workarea/Fibers_$run.out
  cmsRun  $workarea/Fibers_$run.cfg | tee $workarea/Fibers_$run.out

  cat template_SiStripBadModuleBuilder.cfg | sed -e "s@ insertRun@$run@g" -e "s@insertBadList@$Tmodules@" > $workarea/Modules_$run.cfg
  rm -f $workarea/Modules_$run.out
  cmsRun $workarea/Modules_$run.cfg | tee $workarea/Modules_$run.out

done

rm -f out
mkdir TkMap
rm -f TkMap/*
cmsRun SiStripQualityStatistics.cfg | tee out
