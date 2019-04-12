% Logicky projekt - rubikova kostka
% Jan Pawlus, xpawlu00

main :-
	prompt(_, ''),
	read_lines(P),
	parse_input_into_cube(P, X1, X2, X3, X4, X5, X6),
	iter_deep(X1:X2:X3:X4:X5:X6, [['1','1','1'],['1','1','1'],['1','1','1']]:[['2','2','2'],['2','2','2'],['2','2','2']]:[['3','3','3'],['3','3','3'],['3','3','3']]:[['4','4','4'],['4','4','4'],['4','4','4']]:[['5','5','5'],['5','5','5'],['5','5','5']]:[['6','6','6'],['6','6','6'],['6','6','6']], Path, 1),
	print_path(Path).

parse_input_into_cube(P, X1, X2, X3, X4, X5, X6) :-
	take(3, P, X5),
	nth0(3,P,L1),
	nth0(4,P,L2),
	nth0(5,P,L3),	
	get_in_cols(L1,L2,L3,1,LL),
	get_in_cols(L1,L2,L3,2,LL1),
	get_in_cols(L1,L2,L3,3,LL2),
	get_matrix(LL,LL1,LL2,I),
	transpose(I,X1),
	get_in_cols(L1,L2,L3,5,RR),
	get_in_cols(L1,L2,L3,6,RR1),
	get_in_cols(L1,L2,L3,7,RR2),
	get_matrix(RR,RR1,RR2,II),
	transpose(II,X2),
	get_in_cols(L1,L2,L3,9,GG),
	get_in_cols(L1,L2,L3,10,GG1),
	get_in_cols(L1,L2,L3,11,GG2),
	get_matrix(GG,GG1,GG2,III),
	transpose(III,X3),
	get_in_cols(L1,L2,L3,13,MM),
	get_in_cols(L1,L2,L3,14,MM1),
	get_in_cols(L1,L2,L3,15,MM2),
	get_matrix(MM,MM1,MM2,IIII),
	transpose(IIII,X4),
	nth0(6,P,R1),
	nth0(7,P,R2),
	nth0(8,P,R3),
	get_matrix(R1,R2,R3,X6).

%prints the final path
print_path([X1|[]]) :-
	print_cube(X1).

print_path([X1|X]) :-
	print_cube(X1),
	write('\n'),
	print_path(X).


	
% THIS IS A DARK SECTION. IM NOT PROUD OF THIS CODE (MIGHT BE THE WORST CODE ANYONE HAS EVER WRITTEN). ONLY WAY I COULD GET THOSE CUBE TURNS WAS ACTUALLY PLAYING WITH A CUBE AND SOMEHOW PUT IT LIKE THIS. SKIP THIS PLEASE

%rotate_left_column_up
makeTurn(X1:X2:X3:X4:X5:X6,Y1:X2:Y3:Y4:Y5:Y6,1) :-		
	rotate_clockwise(X1,1,[X11|_]),
	reverse(X11,X111),
	replace_in_list(X111,X5,Y5,1),
	rotate_clockwise(X5,1,[X55|_]),
	replace_in_list(X55,X3, Y3,3),
	rotate_clockwise(X3,3,[X33|_]),
	reverse(X33,X333),
	replace_in_list(X333,X6,Y6,1),
	transpose(X6,[X66|_]),
	%rotate_clockwise(X6,1,[X66|_]),
	replace_in_list(X66,X1,Y1,1),
	rotate_clockwise(X4, 3, Y4).
	

%rotate_left_column_down
makeTurn(X1:X2:X3:X4:X5:X6,Y1:X2:Y3:Y4:Y5:Y6,1) :-
	transpose(X1,[X11|_]),
	replace_in_list(X11,X6,Y6,1),
	rotate_clockwise(X6,1,[X66|_]),
	replace_in_list(X66,X3, Y3,3),
	rotate_clockwise(X3,3,[X33|_]),
	reverse(X33,X333),
	replace_in_list(X333,X5,Y5,1),
	transpose(X5,[X55|_]),
	replace_in_list(X55,X1,Y1,1),
	rotate_clockwise(X4, 1, Y4).
	


%rotate_right_column_up
makeTurn(X1:X2:X3:X4:X5:X6,Y1:Y2:Y3:X4:Y5:Y6,1) :-
	rotate_clockwise(X1,3,[X11|_]),
	replace_in_list(X11,X5,Y5,3),
	rotate_clockwise(X5,3,[X55|_]),
	reverse(X55, X555),
	replace_in_list(X555,X3,Y3,1),
	rotate_clockwise(X3,1,[X33|_]),
	replace_in_list(X33,X6,Y6,3),
	rotate_clockwise(X6,3,[X66|_]),
	replace_in_list(X66,X1,Y1,3),
	rotate_clockwise(X2,1,Y2).

%rotate_right_column_down
makeTurn(X1:X2:X3:X4:X5:X6,Y1:Y2:Y3:X4:Y5:Y6,1) :-
	rotate_clockwise(X1,3,[X11|_]),
	replace_in_list(X11,X6,Y6,3),
	rotate_clockwise(X6,3,[X66|_]),
	reverse(X66, X666),
	replace_in_list(X666,X3,Y3,1),
	rotate_clockwise(X3,1,[X33|_]),
	replace_in_list(X33,X5,Y5,3),
	rotate_clockwise(X5,3,[X55|_]),
	replace_in_list(X55,X1,Y1,3),
	rotate_clockwise(X2,3,Y2).

%rotate_top_row_right
makeTurn([X11|X1]:[X22|X2]:[X33|X3]:[X44|X4]:X5:X6,[X44|X1]:[X11|X2]:[X22|X3]:[X33|X4]:Y5:X6,1) :-
	rotate_clockwise(X5, 3, Y5).

%rotate_top_row_left
makeTurn([X11|X1]:[X22|X2]:[X33|X3]:[X44|X4]:X5:X6,[X22|X1]:[X33|X2]:[X44|X3]:[X11|X4]:Y5:X6,1) :-
	rotate_clockwise(X5, 1, Y5).

%rotate_bottom_row_right
makeTurn(X1:X2:X3:X4:X5:X6,Y1:Y2:Y3:Y4:X5:Y6,1) :-	
	reverse(X1,[X11|Y11]),
	reverse(X2,[X22|Y22]),
	reverse(X3,[X33|Y33]),
	reverse(X4,[X44|Y44]),
	reverse([X11|Y22],Y2),
	reverse([X22|Y33],Y3),
	reverse([X33|Y44],Y4),
	reverse([X44|Y11],Y1),
	rotate_clockwise(X6,1,Y6).

%rotate_bottom_row_left
makeTurn(X1:X2:X3:X4:X5:X6,Y1:Y2:Y3:Y4:X5:Y6,1) :-
	reverse(X1,[X11|Y11]),
	reverse(X2,[X22|Y22]),
	reverse(X3,[X33|Y33]),
	reverse(X4,[X44|Y44]),
	reverse([X33|Y22],Y2),
	reverse([X44|Y33],Y3),
	reverse([X11|Y44],Y4),
	reverse([X22|Y11],Y1),
	rotate_clockwise(X6,3,Y6).

%rotate_front_left
makeTurn(X1:X2:X3:X4:X5:[X66|X6],Y1:Y2:X3:Y4:Y5:[X44|X6],1) :-
	rotate_clockwise(X2, 1, [X22|Y22]),
	reverse(X5, [X55|Y55]),
	rotate_clockwise(X4, 3, [X44|Y44]),
	rotate_clockwise([X66|Y22],3,Y2),
	reverse(X22, X222),
	reverse([X222|Y55],Y5),
	reverse(X55,X555),
	rotate_clockwise([X555|Y44],1,Y4),
	rotate_clockwise(X1,3,Y1).
	

%rotate_front_right
makeTurn(X1:X2:X3:X4:X5:[X66|X6],Y1:Y2:X3:Y4:Y5:[X22|X6],1) :-
	rotate_clockwise(X2, 1, [X22|Y22]),
	reverse(X5, [X55|Y55]),
	rotate_clockwise(X4, 3, [X44|Y44]),
	reverse(X55,X555),
	rotate_clockwise([X555|Y22],3,Y2),
	reverse(X44,X444),
	reverse([X444|Y55],Y5),
	rotate_clockwise([X66|Y44],1,Y4),
	rotate_clockwise(X1,1,Y1).

%rotate_back_left
makeTurn(X1:X2:X3:X4:[X55|X5]:X6,X1:Y2:Y3:Y4:[X22|X5]:Y6,1) :-
	rotate_clockwise(X2, 3, [X22|Y22]),
	reverse(X6, [X66|Y66]),
	rotate_clockwise(X4, 1, [X44|Y44]),
	reverse(X66,X666),
	rotate_clockwise([X666|Y22],1,Y2),
	reverse(X44,X444),
	reverse([X444|Y66],Y6),
	rotate_clockwise([X55|Y44],3,Y4),
	rotate_clockwise(X3,1,Y3).

%rotate_back_right
makeTurn(X1:X2:X3:X4:[X55|X5]:X6,X1:Y2:Y3:Y4:[X44|X5]:Y6,1) :-
	rotate_clockwise(X2, 3, [X22|Y22]),
	reverse(X6, [X66|Y66]),
	rotate_clockwise(X4, 1, [X44|Y44]),
	rotate_clockwise([X55|Y22],1,Y2),
	reverse(X22,X222),
	reverse([X222|Y66],Y6),
	reverse(X66,X666),
	rotate_clockwise([X666|Y44],3,Y4),
	rotate_clockwise(X3,3,Y3).

%END OF DARK SECTION


%iterative deepening DFS
iter_deep(X,Y,Path, StartingPos) :-
	X1 = X,
	(dfs(X1,Y,Path1,StartingPos,0,Result), Result == 1 -> 
		Path = Path1;(Depth = StartingPos+1, iter_deep(X,Y,Path,Depth))
	).

%DFS
dfs(X,X,[X], _, _, Result) :- 
	Result = 1, !.
dfs(X,Y,[X|T], MaxDepth, Depth, Result) :-
  MaxDepth > Depth,
  makeTurn(X,X1,_),
  Depth1 is Depth + 1,
  dfs(X1, Y, T, MaxDepth, Depth1, Result).
    
%prints one list
print_list([X1|[]]) :-
	write(X1).
print_list([X1|X]) :-
	write(X1),
	write(''),
	print_list(X).

%prints all the lists
print_lists([X1|[]]) :-
	print_list(X1),
	write('\n').
print_lists([X1|X]) :-
	print_list(X1),
	write(' '),
	print_lists(X).

%prints the cube
print_cube(X1:X2:X3:X4:X5:X6) :-
	print_matrix(X5),
	print_cloak(X1,X2,X3,X4),
	print_matrix(X6).

%prints the cube cloak
print_cloak([X1|[]], [X2|[]], [X3|[]], [X4|[]]) :-
	print_lists([X1,X2,X3,X4]).

print_cloak([X1|X11], [X2|X22], [X3|X33], [X4|X44]) :-
	print_lists([X1,X2,X3,X4]),
	print_cloak(X11,X22,X33,X44).

%prints a matrix
print_matrix([X1|[]]) :-
	print_list(X1),
	write('\n').
print_matrix([X1|X]) :-
	print_list(X1),
	write('\n'),
	print_matrix(X).


%replaces column in matrix (first or last column)
replace_in_list(S, D, R, 1) :- 
	transpose(D,[_|F1]),
	transpose([S|F1], R).

replace_in_list(S, D, R, 3) :-
	transpose(D,DD),
	reverse(DD,[_|DD2]),
	reverse([S|DD2], DD3),
	transpose(DD3, R).


get_matrix(X1, X2, X3, [X1,X2,X3]).
get_cube(X1, X2, X3, X4, X5, X6, [X1,X2,X3,X4,X5,X6]).


get_in_cols([H1|_], [H2|_], [H3|_], 1, [H1,H2,H3]).
get_in_cols([_|X1], [_|X2], [_|X3], X, Y) :- 
	X4 is X - 1,
	get_in_cols(X1, X2, X3, X4, Y).


%https://stackoverflow.com/questions/35594027/rotate-a-matrix-in-prolog
rotate_clockwise(Matrix, N, Rotated) :-
    N_mod_4 is mod(N, 4),
    rotate_clockwise_(N_mod_4, Matrix, Rotated).

rotate_clockwise_(0, M, M).
rotate_clockwise_(1, M, R) :-
    transpose(M, R0),
    maplist(reverse, R0, R).
rotate_clockwise_(2, M, R) :-
    reverse(M, R0),
    maplist(reverse, R0, R).
rotate_clockwise_(3, M, R) :-
    transpose(M, R0),
    reverse(R0, R).

take(N, _, Xs) :- N =< 0, !, N =:= 0, Xs = [].
take(_, [], []).
take(N, [X|Xs], [X|Ys]) :- M is N-1, take(M, Xs, Ys).



%Reads line from stdin, terminates on LF or EOF.
read_line(L,C) :-
	get_char(C),
	(isEOFEOL(C), L = [], !;
		read_line(LL,_),% atom_codes(C,[Cd]),
		[C|LL] = L).

%Tests if character is EOF or LF.
isEOFEOL(C) :-
	C == end_of_file;
	(char_code(C,Code), Code==10).

read_lines(Ls) :-
	read_line(L,C),
	%write(L),
	( C == end_of_file, Ls = [] ;
	  read_lines(LLs), Ls = [L|LLs]
	).

% from clpfd library - https://github.com/SWI-Prolog/swipl-devel/blob/master/library/clp/clpfd.pl#L6371
transpose(Ls, Ts) :-
        must_be(list(list), Ls),
        lists_transpose(Ls, Ts).

lists_transpose([], []).
lists_transpose([L|Ls], Ts) :-
        maplist(same_length(L), Ls),
        foldl(transpose_, L, Ts, [L|Ls], _).

transpose_(_, Fs, Lists0, Lists) :-
        maplist(list_first_rest, Lists0, Fs, Lists).

list_first_rest([L|Ls], L, Ls).
