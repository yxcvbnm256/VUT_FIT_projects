TMP=$(mktemp /tmp/xpawlu00.XXXXXX)
function rmtemp {
	rm -rf "$TMP"
}
trap rmtemp 1 2 15 EXIT

while getopts 'gpr:d:' opt; do
	case "$opt" in
		g)
		garg=1
		echo "digraph CG {";;
		p)
		parg=1;;
		r)
		rarg="$OPTARG";;		
		d)
		darg="$OPTARG";;		
	esac
done

if [[ -n "$darg" ]] && [[ -n "$rarg" ]];
then
	echo "Nelze zadat prepinace -d a -r zaroven!"
	exit 0
fi
 
number=$(($#-1))

shift $number

list=$(objdump -d -j .text $@ | cut -f 3,4 | grep -E '^callq|>:' | sed 's/callq  //')

for arg in $list
do
	if [[ $arg == *: ]]
	then
		caller=${arg%":"}
		caller=${caller/\</}
		caller=${caller/\>/}
		
		if [ -n "$garg" ]
		then
			caller=${caller/\@plt/_PLT}
		fi
		continue			
	elif [[ $arg == \<*\> ]]	
	then	
		
		callee=${arg/\</}
		callee=${callee/\>/}
		if [ -n "$garg" ]
		then
			callee=${callee/\@plt/_PLT}
		fi
	else
		continue		
	fi
	if [[ "$callee" == *+* ]]
	then
		continue
	fi

	if [[ -n "$garg" ]] && [[ -z "$parg" ]] && [[ "$callee" == *_PLT* ]]
	then
		continue
	elif [[ -n "$garg" ]]
	then
		echo "$caller -> $callee;" >> $TMP
	elif [[ -z "$garg" ]] && [[ -z "$parg" ]] && [[ "$callee" == *@plt* ]]
	then
		continue
	elif [[ -z "$garg" ]]
	then
		echo "$caller -> $callee" >> $TMP
	fi
done

if [[ -n "$darg" ]]
then
	sort "$TMP" | uniq | grep -E "^$darg" 
elif [[ -n "$rarg" ]]
then
	sort "$TMP" | uniq | grep -E "$rarg$"
else
	sort "$TMP" | uniq
fi

if [ -n "$garg" ]
then
	echo "}"
fi
