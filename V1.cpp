#include <bits/stdc++.h>
#include "engines.h"
using namespace std;
typedef long long ll;
vector<string> generateValidMoves(ll white_men,ll white_kings,ll black_men,ll black_kings,string turn);
void playMove(string move,ll &white_men,ll &white_kings,ll &black_men,ll &black_kings,ll &kingStreak);
float eval1(ll white_men,ll white_kings,ll black_men,ll black_kings,string turn,ll depth,bool endgame){
	float posValue=0;
	ll maxDepth=7;
	if(endgame){
		maxDepth--;
	}
	if(depth==maxDepth){
		for(ll sq=1;sq<=50;sq++){
			if(white_men&(1LL<<sq)){
				posValue+=1;
			}
			if(white_kings&(1LL<<sq)){
				posValue+=3.5;
			}
			if(black_men&(1LL<<sq)){
				posValue-=1;
			}
			if(black_kings&(1LL<<sq)){
				posValue-=3.5;
			}
		}
		return posValue;
	}
	float ans;
	if(turn=="w"){
		ans=LLONG_MIN;
	}
	else{
		ans=LLONG_MAX;
	}
	vector<string> validMoves=generateValidMoves(white_men,white_kings,black_men,black_kings,turn);
	for(size_t i=0;i<validMoves.size();i++){
		ll wm=white_men,wk=white_kings,bm=black_men,bk=black_kings,fakeKingStreak=0;
		playMove(validMoves[i],wm,wk,bm,bk,fakeKingStreak);
		if(turn=="w"){
			ans=max(ans,eval1(wm,wk,bm,bk,"b",depth+1,endgame));
		}
		else{
			ans=min(ans,eval1(wm,wk,bm,bk,"w",depth+1,endgame));
		}
	}
	return ans;
}
string engine1(ll white_men,ll white_kings,ll black_men,ll black_kings,string turn,vector<string> validMoves){
	if(validMoves.size()==1){
		return validMoves[0];
	}
	float best;
	ll bestMoveIndex=0;
	if(turn=="w"){
		best=LLONG_MIN;
	}
	else{
		best=LLONG_MAX;
	}
	for(size_t i=0;i<validMoves.size();i++){
		ll wm=white_men,wk=white_kings,bm=black_men,bk=black_kings,fakeKingStreak=0,cnt=0;
		playMove(validMoves[i],wm,wk,bm,bk,fakeKingStreak);
		for(ll sq=1;sq<=50;sq++){
			if(((1LL<<sq)&white_men) || ((1LL<<sq)&white_kings) || ((1LL<<sq)&black_men) || ((1LL<<sq)&black_kings)){
				cnt++;
			}
		}
		bool endgame=(cnt<=15);
		float posEval;
		if(turn=="w"){
			posEval=eval1(wm,wk,bm,bk,"b",1,endgame);
		}
		else{
			posEval=eval1(wm,wk,bm,bk,"w",1,endgame);
		}
		if(turn=="w"){
			if(posEval>best){
				best=posEval;
				bestMoveIndex=i;
			}
		}
		else{
			if(posEval<best){
				best=posEval;
				bestMoveIndex=i;
			}
		}
	}
	return validMoves[bestMoveIndex];
}
