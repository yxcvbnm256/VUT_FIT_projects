RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
PASSED=0

port=$(shuf -i 10000-20000 -n 1)
./ipkhttpserver -p ${port} -c 100 -t 5 &
sleep 0.5 
PID=$!
echo "----------------------------------------------------"
echo "Test 1: maly testovaci soubor - mensi nez chunk size"
./ipkhttpclient http://localhost:${port}/test.html

file=$(ls -t *.payload | head -1)
DIFF=$(diff www/test.html $file) 
if [ "$DIFF" != "" ] 
then
    echo "${RED}Chyba:${NC} www/test.html a ${file} se lisi."
else
	echo "${GREEN}Test 1 prosel.${NC}"
	PASSED=$((PASSED+1))
fi
kill -INT $PID
port=$(shuf -i 10000-20000 -n 1)
./ipkhttpserver -p ${port} -c 100 -t 5 &
sleep 0.5
PID=$!
echo "--------------------------------------------------------------"
echo "Test 2 - vetsi testovaci soubor - vetsi nez chunk size"
./ipkhttpclient http://localhost:${port}
file=$(ls -t *.payload | head -1)
DIFF=$(diff www/index.html $file) 
if [ "$DIFF" != "" ] 
then
    echo "${RED}Chyba:${NC} www/index.html a ${file} se lisi."
else
	echo "${GREEN}Test 2 prosel.${NC}"
	PASSED=$((PASSED+1))
fi
kill -INT $PID

port=$(shuf -i 10000-20000 -n 1)
./ipkhttpserver -p ${port} &
sleep 1
PID=$!
echo "--------------------------------------------------------------"
echo "Test 3 - vetsi testovaci soubor - nezadana chunk size ani time"
./ipkhttpclient http://localhost:${port}/
file=$(ls -t *.payload | head -1)
DIFF=$(diff www/index.html $file) 
if [ "$DIFF" != "" ] 
then
    echo "${RED}Chyba:${NC} www/index.html a ${file} se lisi."
else
	echo "${GREEN}Test 3 prosel.${NC}"
	PASSED=$((PASSED+1))
fi
kill -INT $PID

port=$(shuf -i 10000-20000 -n 1)
./ipkhttpserver -p ${port} &
sleep 1
PID=$!
echo "--------------------------------------------------------------"
echo "Test 4 - vetsi testovaci soubor s diakritikou"
./ipkhttpclient http://localhost:${port}/fit.html
file=$(ls -t *.payload | head -1)
DIFF=$(diff www/fit.html $file) 
if [ "$DIFF" != "" ] 
then
    echo "${RED}Chyba:${NC} www/fit.html a ${file} se lisi."
else
	echo "${GREEN}Test 4 prosel.${NC}"
	PASSED=$((PASSED+1))
fi
kill -INT $PID

port=$(shuf -i 10000-20000 -n 1)
./ipkhttpserver -p ${port} &
sleep 0.5
PID=$!
echo "--------------------------------------------------------------"
echo "Test 5 - pozadavek na neexistujici soubor"
./ipkhttpclient http://localhost:${port}/nope.html
file=$(ls -t *.payload | head -1)
DIFF=$(diff www/404.html $file) 
if [ "$DIFF" != "" ] 
then
    echo "${RED}Chyba:${NC} www/404.html a ${file} se lisi."
else
	echo "${GREEN}Test 5 prosel.${NC}"
	PASSED=$((PASSED+1))
fi
kill -INT $PID

echo "--------------------------------------------------------------"
echo "Test 6 - zadan zabrany port"
./ipkhttpserver -p 1 2> test6.err & 
sleep 0.5

DIFF=$(cat test6.err) 
if [ "$DIFF" != "" ] 
then
	echo "${GREEN}Test 6 prosel.${NC}"   
	PASSED=$((PASSED+1)) 
else
	echo "${RED}Chyba: zadny vypis na STDERR${NC}"
fi

echo "--------------------------------------------------------------"
echo "Test 7 - zadan neplatny host"
./ipkhttpserver -p 10000 & 
PID=$!
sleep 0.5
./ipkhttpclient http://localhosttt:10000/ 2> test7.err
DIFF=$(cat test7.err) 
if [ "$DIFF" != "" ] 
then
	echo "${GREEN}Test 6 prosel.${NC}" 
	PASSED=$((PASSED+1))   
else
	echo "${RED}Chyba: zadny vypis na STDERR${NC}"
fi
kill -INT $PID

echo "--------------------------------------------------------------"
echo "Test 8 - zadan neplatny argument"
./ipkhttpclient http://localhosttt:10000/ -l 1000 2> test8.err
DIFF=$(cat test8.err) 
if [ "$DIFF" != "" ] 
then
	echo "${GREEN}Test 8 prosel.${NC}" 
	PASSED=$((PASSED+1))   
else
	echo "${RED}Chyba: zadny vypis na STDERR${NC}"
fi


echo "--------------------------------------------------------------"
echo "Test 9 - napoveda server"
FILE="helpinServer.log"
./ipkhttpserver -p 1 -h > ${FILE} & 
sleep 0.1
if [ -s "$FILE" ] 
then
	echo "${GREEN}Test 9 prosel.${NC}"  
	PASSED=$((PASSED+1))  
else
	echo "${RED}Chyba: zadna napoveda na STDOUT${NC}"
fi

echo "--------------------------------------------------------------"
echo "Test 10 - napoveda client"
FILE="helpinClient.log"
./ipkhttpclient http://localhost:1/nope.html -h > ${FILE} & 
sleep 0.1
if [ -s "$FILE" ] 
then
	echo "${GREEN}Test 10 prosel.${NC}"  
	PASSED=$((PASSED+1))  
else
	echo "${RED}Chyba: zadna napoveda na STDOUT${NC}"
fi
echo "--------------------------------------------------------------"
if [ "$PASSED" = 10 ]
then
	echo "${GREEN}10/10 testu proslo.${NC}"
else
	echo "${RED}${PASSED}/10 testu proslo.${NC}"
fi
