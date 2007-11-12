#!/bin/sh

function makeHtml(){
    thewidth=600

#Header
    echo "<html><head><title>Summary Page Quality</title></head>" 
    echo '<table cellpadding="2" cellspacing="2" border="0" width="100%" align="center">'
    echo '<tbody>'
  
    for file in `ls $webpath | grep png | cut -d "." -f 1`
      do
      Run=`echo $file | cut -d "_" -f 3 | cut -d "_" -f 1` 

      htmlfile=${htmlpath}/$file
      echo '<tr><td valign="middle" align="center">'
      echo '<a name="'$Run'">'Run $Run'</a>&nbsp;&nbsp;<a href="'${htmlfile}.pdf'">pdf</a>  <a href="'${htmlfile}.xml'">svg</a><br>'
      echo '<a href="'${htmlfile}.png'"><img src="'${htmlfile}.png'" style="border: 0px solid ; width: '$thewidth'px; "></a>'

    done

    echo '  </tbody>'
    echo '</table>'
}

tag=""
[ "c$1" == "c" ] && echo -e "\nplease specify tag" && exit
tag=$1

webpathBase=/data1/MonitorQuality
webpath=${webpathBase}/$tag
webadd="http://cmstac11.cern.ch:8080"
export htmlpath=`echo $webpath | sed -e "s@/data1@$webadd@"`

webfile=$webpathBase/MonitorQuality_$tag.html

[ ! -e $webpathBase ] && mkdir $webpathBase
[ ! -e $webpath ] && mkdir $webpath

cp TkMap*_Run_*.*  $webpath/.

cd $webpathBase
makeHtml > ${webfile}
cd -
