TMP=$(mktemp /tmp/xpawlu00.XXXXXX)
function rmtemp {
	rm -rf "$TMP"
}
trap rmtemp 1 2 15 EXIT

while getopts 'gr:d:' opt; do
	case "$opt" in
		g)
			garg=1
			echo "digraph GSYM {";;
		r)
			rarg=$OPTARG;;
		d)
			darg=$OPTARG;;
		:)	echo "Nezadan parametr"
			exit 0;;
			
	esac
done

pozice=1

if [[ -n "$darg" ]] && [[ -n "$rarg" ]];
then
        echo "Nelze zadat prepinace -d a -r zároven!"
        exit 0
fi

# Vystřihnutí jiných parametrů, než názvů souborů

for argument in $@
do
	
	if [[ $argument == -d ]] || [[ $argument == -r ]]
	then	
		set -- "${$:1:$pozice-1}" "${@:$pozice+2:$#}";
	elif [[ $1 == -* ]]
	then
		shift 1
		((pozice--))	
	elif [[ $argument == -* ]]
	then
		set -- "${@:1:$pozice-1}" "${@:$pozice+1:$#}";
		((pozice--))
	fi
	((pozice++))	 
done

if [[ -n "$garg" ]] && [[ -n "$darg" ]]
then
	
	darg=${darg/\-/_}
	darg=${darg/\./D}
	darg=${darg/\+/P}
fi

if [[ -n "$garg" ]] && [[ -n "$rarg" ]]
then
	rarg=${rarg/\-/_}
	rarg=${rarg/\./D}
	rarg=${rarg/\+/P}
fi

listU=$(nm $@ | grep -v '@' | sed 's/  */;/' | cut -f2 -d";" | sed 's/[U] /:/' | grep -E '[:]' | sed 's/://')
listOthers=$(nm $@ | grep -v '@' | sed 's/  */;/' | cut -f2 -d";" | sed 's/[TBGCD] /:/' | grep -E '[:]' | sed 's/://')
# První cyklus - hledá symboly U 
for varname in $listU
do
	
	if [[ $garg == 1 ]] && [[ $varname == *.o* ]]
	then
		filename=${varname/\-/_}
		filename=${filename/\./D}
		filename=${filename/\+/P}
	elif [[ $varname == *.o* ]]
	then
		filename=$varname	
	else
# Druhý cyklus - hledá symboly k uloženému U
		for varrname in $listOthers
		do
			if [[ $varrname == *.o* ]] && [[ $garg == 1 ]] ;
			then
				sourcefilename=${varrname/\-/_}
				sourcefilename=${sourcefilename/\./D}
				sourcefilename=${sourcefilename/\+/P}
				
			elif [[ $varrname == *.o* ]]
			then
				sourcefilename=$varrname						
			fi
			if [ -n "$darg" ] 
			then
	
				if [[ "$darg" == "$filename" ]] && [[ "$varname" == "$varrname" ]] ; then
					if [[ -n "$garg" ]]
					then					
						echo "$filename -> $sourcefilename [label=\"$varrname\"];" >> $TMP
					else
						echo "$filename -> $sourcefilename ($varrname)" >> $TMP
					fi
					
				fi			
			elif  [ -n "$rarg" ]
			then
				if [[ "$rarg" == "$sourcefilename" ]] && [[ "$varname" == "$varrname" ]] ; then
					if [ -n "$garg" ]
					then
						echo "$filename -> $sourcefilename [label=\"$varrname\"];" >> $TMP
					else
						echo "$filename -> $sourcefilename ($varrname)" >> $TMP
					fi
				fi
			elif [ "$varname" == "$varrname" ]
			then
				if [ -n "$garg" ]
				then
					echo "$filename -> $sourcefilename [label=\"$varrname\"];"
				else
					echo "$filename -> $sourcefilename ($varrname)" >> $TMP
				fi
			fi		
		done
	fi
done

sort -u $TMP

# Pokud je zadan argument -g, dopisuje se na konec výstupu label zadaných souborů
if [ -n "$garg" ]
then
	if [[ $@ == "*.o" ]]
	then	
		for f in *.o
		do
			echo "${f/\.o/Do} [label=\"$f\"];"
		done
	elif [[ $@ != "*.o" ]]
	then
		
		for g in $@
		do
			echo "${g/\.o/Do} [label=\"$g\"];"
		done 
	fi
	echo "}"
fi

