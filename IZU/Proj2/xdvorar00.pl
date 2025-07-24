% Zadani c. 30:
% Napiste program resici ukol dany predikatem u30(LIN,VOUT), kde LIN je  
% vstupni ciselny seznam s nejmene jednim prvkem a VOUT je promenna, ve 
% ktere se vraci prumerna hodnota absolutnich hodnot cisel v seznamu LIN. 

% Testovaci predikaty:                                 		% VOUT 
u30_1:- u30([5,3,-18,2,-9,-13,17],VOUT),write(VOUT).		% 9.57...
u30_2:- u30([5,2],VOUT),write(VOUT).				% 3.5
u30_3:- u30([-18.3,-9.2,-13.1],VOUT),write(VOUT).		% 13.53...
u30_r:- write('Zadej LIN: '),read(LIN),
	u30(LIN,VOUT),write(VOUT).

% Reseni:
u30(LIN,VOUT):-abs_list(LIN,LABS),!,
sum_lenght(LABS,SUM,LEN),
VOUT is SUM / LEN.

abs_list([], []).
abs_list([H|T], [AbsH|AbsT]) :-
    H >= 0, AbsH = H,
    abs_list(T, AbsT).
abs_list([H|T], [AbsH|AbsT]) :-
    H < 0, AbsH is -H,
    abs_list(T, AbsT).

sum_lenght([], 0, 0).
sum_lenght([H|T], Sum, Len) :-
    sum_lenght(T, RestSum,RestLen),
    Sum is H + RestSum,
    Len is RestLen + 1.