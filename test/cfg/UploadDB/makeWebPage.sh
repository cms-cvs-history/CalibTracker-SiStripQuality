#!/bin/sh

function makeHtml(){
    thewidth=300
    modulo=3
    resize=400x400
#Header
    echo "<html><head><title>Summary Page Quality</title></head>" 
    echo '<table cellpadding="2" cellspacing="2" border="1" width="100%" align="center">'
    echo '<tbody>'
  
    i=0
    for file in `ls $webpath | grep xml | cut -d "." -f 1`
      do
      
      Run=`echo $file | cut -d "_" -f 3 | cut -d "_" -f 1` 

      [ ! -e $webpath/$file.th.png ] &&  convert -size $resize $webpath/$file.png -resize $resize $webpath/$file.th.png

      htmlfile=${htmlpath}/$file

      [ $i -eq 0 ] && echo '<tr>'
      echo '<td valign="middle" align="center">'
      echo '<a name="'$Run'">'Run $Run'</a>&nbsp;&nbsp;<a href="'${htmlfile}.pdf'">pdf</a>  <a href="'${htmlfile}.xml'">svg</a><br>'
      echo '<a href="'${htmlfile}.png'"><img src="'${htmlfile}.th.png'" style="border: 0px solid ; width: '$thewidth'px; "></a>'

      let i++
      [ $i -eq $modulo ] && i=0
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

cp -vu TkMap*_Run_*.*  $webpath/.

cd $webpathBase
makeHtml > ${webfile}
cd -
