#!/bin/sh
doCmsRun=0
[ "c$1" == "c1" ] && doCmsRun=1

tag=TBuffer_VenturiList_v2

eval `scramv1 runtime -sh`

workarea=/tmp/giordano/v2

mkdir $workarea


rm dbfile.db
cmscond_bootstrap_detector.pl --offline_connect sqlite_file:dbfile.db --auth /afs/cern.ch/cms/DB/conddb/authentication.xml STRIP

echo 1 >list.tmp
cat big_defect_list.txt | grep "[a-Z]" | awk -F "|" '{print strtonum($3)"\n" strtonum($4)+1}' | sort -n | uniq >> list.tmp


for run in `cat list.tmp | sort -n`
  do
  echo -e "\n\n Uploading Run $run"
  
#Modules
  Tmodules=`cat big_defect_list.txt |  grep "[a-Z]" | awk -F "|" '{detid=$1;start=strtonum($3);stop=strtonum($4);badC=$2;if(start<=Rstart && stop>Rstart && $0~/all/) {print detid;}}' Rstart=$run | sort  | uniq | awk 'BEGIN{BadString=""}{detid=$1; BadString=sprintf("%s , %s",BadString,detid);} END{print BadString;}' | sed -e "s@,@@"` 

 #APV
  Tapvs=`cat big_defect_list.txt |  grep "[a-Z]" | awk -F "|" '{detid=$1;start=strtonum($3);stop=strtonum($4);badC=$2;if(start<=Rstart && stop>Rstart && $0!~/all/){print detid" | "badC;}}' Rstart=$run | sort | uniq | awk -F "|" 'BEGIN{BadString="";}{detid=$1;badC=$2;BadString=sprintf("%s, { uint32 BadModule = %s vuint32 BadApvList = {%s} } ",BadString,detid,badC);} END{print BadString;}' | sed -e "s@,@@"`

  echo Module $Tmodules
  echo 
  echo Apv $Tapvs

  cat template_SiStripBadFiberBuilder.cfg | sed -e "s@ insertRun@$run@g" -e "s@insertBadList@$Tapvs@" -e "s@insertTag@$tag@g" > $workarea/Fibers_$run.cfg
  rm -f  $workarea/Fibers_$run.out
  [ $doCmsRun -eq 1 ] && cmsRun  $workarea/Fibers_$run.cfg | tee $workarea/Fibers_$run.out

  cat template_SiStripBadModuleBuilder.cfg | sed -e "s@ insertRun@$run@g" -e "s@insertBadList@$Tmodules@"  -e "s@insertTag@$tag@g" > $workarea/Modules_$run.cfg
  rm -f $workarea/Modules_$run.out
  [ $doCmsRun -eq 1 ] && cmsRun $workarea/Modules_$run.cfg | tee $workarea/Modules_$run.out

done

rm -f out
mkdir /tmp/giordano/TkMap
cat SiStripQualityStatistics.cfg |  sed -e "s@insertTag@$tag@g" > $workarea/Quality.cfg
[ $doCmsRun -eq 1 ] && cmsRun  $workarea/Quality.cfg | grep -v "MSG" | awk '{if ( $0~/New IOV/ ) {flag=1 ; print "";}if ( $0~/created palette/){flag=0 ; }if ( flag ) print $0}' | tee out





