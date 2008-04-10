#!/bin/sh
doCmsRun=0
[ "c$1" == "c1" ] && doCmsRun=1

workarea=/tmp/$USER/v2

connectstringA="sqlite_file:dbfile.db"
connectstringB="oracle://cms_orcoff_int2r/CMS_COND_STRIP"
connectstringC="oracle://orcon/CMS_COND_STRIP"

connectstring=${connectstringC}
USER=CMS_COND_STRIP
PASSWD=xxxxx
#authPath=/afs/cern.ch/cms/DB/conddb
authPath=/afs/cern.ch/user/x/xiezhen/auth/orconowner

tag=VenturiList_TIF_20X

echo $workarea

eval `scramv1 runtime -sh`

#-----------------------------------------------

mkdir $workarea

scriptDir=`dirname $0`
cd $scriptDir

eval `scramv1 runtime -sh`

export TNS_ADMIN=/afs/cern.ch/project/oracle/admin


rm dbfile.db
#cmscond_bootstrap_detector.pl --offline_connect sqlite_file:dbfile.db --auth /afs/cern.ch/cms/DB/conddb/authentication.xml STRIP
workdir=`pwd`
if [ ! -e $CMSSW_BASE/src/CondFormats/SiStripObjects/xml ]; then
    cd $CMSSW_BASE/src 
    cvs co CondFormats/SiStripObjects/xml 
    cd $workdir 
fi

if [ `echo ${connectstring} | grep -c sqlite` -ne 0 ] ; then
    USER=CMS_COND_STRIP
    PASSWD=xxxxx
    echo -e "\n-----------\nCreating tables for db ${connectstring} \n-----------\n"
    
    IsSqlite=1
    rm `echo ${connectstring} | sed -e "s@sqlite_file:@@"`

    #cmscond_bootstrap_detector.pl --offline_connect ${connectstring} --auth /afs/cern.ch/cms/DB/conddb/authentication.xml STRIP
    for obj in `ls $CMSSW_BASE/src/CondFormats/SiStripObjects/xml/*xml`
      do
      echo -e  "\npool_build_object_relational_mapping -f $obj   -d CondFormatsSiStripObjects -c ${connectstring}\n"
      pool_build_object_relational_mapping -f $obj   -d CondFormatsSiStripObjects -c ${connectstring} -u $USER -p $PASSWD

    done
fi


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

  cat template_SiStripBadFiberBuilder.cfg | sed -e "s@ insertRun@$run@g" -e "s@insertBadList@$Tapvs@" -e "s@insertTag@$tag@g"  -e "s@insert_connect_string@${connectstring}@g" -e "s@insert_authPath@${authPath}@g" > $workarea/Fibers_$run.cfg
  rm -f  $workarea/Fibers_$run.out
  [ $doCmsRun -eq 1 ] && cmsRun  $workarea/Fibers_$run.cfg | tee $workarea/Fibers_$run.out

  cat template_SiStripBadModuleBuilder.cfg | sed -e "s@ insertRun@$run@g" -e "s@insertBadList@$Tmodules@"  -e "s@insertTag@$tag@g"  -e "s@insert_connect_string@${connectstring}@g" -e "s@insert_authPath@${authPath}@g" > $workarea/Modules_$run.cfg
  rm -f $workarea/Modules_$run.out
  [ $doCmsRun -eq 1 ] && cmsRun $workarea/Modules_$run.cfg | tee $workarea/Modules_$run.out
done

rm -f out
mkdir /tmp/giordano/TkMap
cat SiStripQualityStatistics.cfg |  sed -e "s@insertTag@$tag@g" > $workarea/Quality.cfg
[ $doCmsRun -eq 1 ] && cmsRun  $workarea/Quality.cfg | grep -v "MSG" | awk '{if ( $0~/New IOV/ ) {flag=1 ; print "";}if ( $0~/created palette/){flag=0 ; }if ( flag ) print $0}' | tee out





