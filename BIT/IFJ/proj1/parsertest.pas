{ Test funkce copy }
var
	str1: string;
	str2: string;
	a: integer;

begin
	str1 := 'This project is working';
	str2 := copy(str1, 1, 4);
	write(str2, ''#10'');

	write(str2 = 'This', ''#10'');


	str2 := copy(str1, 17, 1000);
	write(str2, ''#10'');

	write(str2 = 'working', ''#10'');


	str2 := copy(str1, 17, 0-1000);
	write(str2, ''#10'');

	write(str2 = '', ''#10'');

	str2 := copy(str1, 0-17, 1000);
	write(str2, ''#10'');
	write('str2',str2 = '', ''#10'');

	write(str2 = '', ''#10'');

	a:=256;
	str2:='';
	str1:='';
	while a > 0 do
	begin
		str2 := str1 + 'a';
		str1 := str2 + 'a';
		a := a -1
	end;

	write(str1 = copy(str1, 1, 255), ''#10'');
	write(str1 = copy(str1, 1, 1000), ''#10'')
end.