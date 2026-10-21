//g++ -O2 GUI.cpp V1.cpp V2.cpp -o GUI.exe -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
#include <bits/stdc++.h>
#include "raylib.h"
#include "engines.h"
using namespace std;
typedef long long ll;
pair<ll,ll> squareToCoordinate(ll sq){
	ll row=sq/5,col=sq%5;
	if(sq%5){
		row++;
	}
	if(col==0){
		col=5;
	}
	col*=2;
	if(row%2==0){
		col--;
	}
	return {row,col};
}
ll coordinateToSquare(pair<ll,ll> coordinates){
	ll row=coordinates.first,col=coordinates.second;
	ll coord=((5*(row-1))+ ((col+1)/2));
	return coord;
}
bool isEmpty(ll sq,ll &white_men,ll &white_kings,ll &black_men,ll &black_kings){
	return (((white_men&(1LL<<sq))==0) && ((white_kings&(1LL<<sq))==0) && ((black_men&(1LL<<sq))==0) && ((black_kings&(1LL<<sq))==0));
}
void playMove(string move,ll &white_men,ll &white_kings,ll &black_men,ll &black_kings,ll &kingStreak){
	vector<ll> squares;
	if(count(move.begin(),move.end(),'-')==1){
		ll start=0,end=0;
		if(move[1]=='-'){
			start=stoi(move.substr(0,1));
			end=stoi(move.substr(2));
		}
		else{
			start=stoi(move.substr(0,2));
			end=stoi(move.substr(3));
		}
		squares={start,end};
		if((white_kings&(1LL<<start)) || (black_kings&(1LL<<start))){
			kingStreak++;
		}
		else{
			kingStreak=0;
		}
	}
	else{
		kingStreak=0;
		ll num=0;
		for(size_t i=0;i<move.size();i++){
			if(move[i]=='x'){
				squares.push_back(num);
				num=0;
			}
			else{
				num*=10;
				num+=(move[i]-'0');
			}
		}
		squares.push_back(num);
	}
	if((white_men&(1LL<<squares[0]))){
		white_men^=(1LL<<squares[0]);
		white_men|=(1LL<<squares[squares.size()-1]);
	}
	if((white_kings&(1LL<<squares[0]))){
		white_kings^=(1LL<<squares[0]);
		white_kings|=(1LL<<squares[squares.size()-1]);
	}
	if((black_men&(1LL<<squares[0]))){
		black_men^=(1LL<<squares[0]);
		black_men|=(1LL<<squares[squares.size()-1]);
	}
	if((black_kings&(1LL<<squares[0]))){
		black_kings^=(1LL<<squares[0]);
		black_kings|=(1LL<<squares[squares.size()-1]);
	}
	for(size_t i=1;i<squares.size();i++){
		pair<ll,ll> p1,p2;
		p1=squareToCoordinate(squares[i-1]);
		p2=squareToCoordinate(squares[i]);
		while(p1!=p2){
			ll curr_sq=coordinateToSquare(p1);
			white_men&=(~(1LL<<curr_sq));
			white_kings&=(~(1LL<<curr_sq));
			black_men&=(~(1LL<<curr_sq));
			black_kings&=(~(1LL<<curr_sq));
			if(p1.first<p2.first){
				p1.first++;
			}
			else{
				p1.first--;
			}
			if(p1.second<p2.second){
				p1.second++;
			}
			else{
				p1.second--;
			}
		}
	}
	for(ll sq=1;sq<=5;sq++){
		if(white_men&(1LL<<sq)){
			white_men^=(1LL<<sq);
			white_kings|=(1LL<<sq);
		}
	}
	for(ll sq=46;sq<=50;sq++){
		if(black_men&(1LL<<sq)){
			black_men^=(1LL<<sq);
			black_kings|=(1LL<<sq);
		}
	}
}
void dfs(ll node,vector<vector<ll>> &adj,vector<bool> capt,string path,vector<string> &mencaptures){
	mencaptures.push_back(path);
	for(size_t i=0;i<adj[node].size();i++){
		pair<ll,ll> p1,p2;
		p1=squareToCoordinate(node);
		p2=squareToCoordinate(adj[node][i]);
		ll x,y;
		x=(p1.first+p2.first)/2;
		y=(p1.second+p2.second)/2;
		ll sq=coordinateToSquare({x,y});
		if(!capt[sq]){
			path+=('x'+to_string(adj[node][i]));
			capt[sq]=true;
			dfs(adj[node][i],adj,capt,path,mencaptures);
			ll idx;
			for(ll j=((ll)path.size())-1;j>=0;j--){
				if(path[j]=='x'){
					idx=j;
					break;
				}
			}
			path=path.substr(0,idx);
			capt[sq]=false;
		}
	}
}
void dfs2(ll node,vector<vector<pair<ll,ll>>> &adj2,vector<bool> capt,string path,vector<string> &kingcaptures){
	kingcaptures.push_back(path);
	for(size_t i=0;i<adj2[node].size();i++){
		pair<ll,ll> p1,p2;
		p1=squareToCoordinate(node);
		p2=squareToCoordinate(adj2[node][i].first);
		if(!capt[adj2[node][i].second]){
			path+=('x'+to_string(adj2[node][i].first));
			capt[adj2[node][i].second]=true;
			dfs2(adj2[node][i].first,adj2,capt,path,kingcaptures);
			ll idx;
			for(ll j=((ll)path.size())-1;j>=0;j--){
				if(path[j]=='x'){
					idx=j;
					break;
				}
			}
			path=path.substr(0,idx);
			capt[adj2[node][i].second]=false;
		}
	}
}
vector<string> generateValidMoves(ll white_men,ll white_kings,ll black_men,ll black_kings,string turn){
	vector<string> valid;
	ll longestCaptureSequence=0;
	vector<vector<ll>> adj(51);
	vector<vector<pair<ll,ll>>> adj2(51);
	vector<string> mencaptures,kingcaptures;
	if(turn=="w"){
		for(ll sq=1;sq<=50;sq++){
			if((black_men&(1LL<<sq)) || (black_kings&(1LL<<sq))){
				ll i=squareToCoordinate(sq).first,j=squareToCoordinate(sq).second;
				if((i-1)>=1 && (j-1)>=1 && (i+1)<=10 && (j+1)<=10){
					ll sq2=coordinateToSquare({i-1,j-1});
					ll sq3=coordinateToSquare({i+1,j+1});
					if(isEmpty(sq2,white_men,white_kings,black_men,black_kings) && isEmpty(sq3,white_men,white_kings,black_men,black_kings)){
						adj[sq2].push_back(sq3);
						adj[sq3].push_back(sq2);
					}
				}
				if((i-1)>=1 && (j+1)<=10 && (i+1)<=10 && (j-1)>=1){
					ll sq2=coordinateToSquare({i-1,j+1});
					ll sq3=coordinateToSquare({i+1,j-1});
					if(isEmpty(sq2,white_men,white_kings,black_men,black_kings) && isEmpty(sq3,white_men,white_kings,black_men,black_kings)){
						adj[sq2].push_back(sq3);
						adj[sq3].push_back(sq2);
					}
				}
			}
			if((black_men&(1LL<<sq)) || (black_kings&(1LL<<sq))){
				vector<ll> l1,r1,l2,r2;
				ll i=squareToCoordinate(sq).first,j=squareToCoordinate(sq).second;
				ll curr_i=i,curr_j=j;
				while(curr_i>=2 && curr_j>=2){
					curr_i--;
					curr_j--;
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings)){
						l1.push_back(coordinateToSquare({curr_i,curr_j}));
					}
					else{
						break;
					}
				}
				curr_i=i;
				curr_j=j;
				while(curr_i<=9 && curr_j<=9){
					curr_i++;
					curr_j++;
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings)){
						r1.push_back(coordinateToSquare({curr_i,curr_j}));
					}
					else{
						break;
					}
				}
				curr_i=i;
				curr_j=j;
				while(curr_i>=2 && curr_j<=9){
					curr_i--;
					curr_j++;
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings)){
						l2.push_back(coordinateToSquare({curr_i,curr_j}));
					}
					else{
						break;
					}
				}
				curr_i=i;
				curr_j=j;
				while(curr_i<=9 && curr_j>=2){
					curr_i++;
					curr_j--;
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings)){
						r2.push_back(coordinateToSquare({curr_i,curr_j}));
					}
					else{
						break;
					}
				}
				for(size_t i=0;i<l1.size();i++){
					for(size_t j=0;j<r1.size();j++){
						adj2[l1[i]].push_back({r1[j],sq});
						adj2[r1[j]].push_back({l1[i],sq});
					}
				}
				for(size_t i=0;i<l2.size();i++){
					for(size_t j=0;j<r2.size();j++){
						adj2[l2[i]].push_back({r2[j],sq});
						adj2[r2[j]].push_back({l2[i],sq});
					}
				}
			}
		}
		for(ll sq=1;sq<=50;sq++){
			vector<bool> capt(51,false);
			ll i=squareToCoordinate(sq).first,j=squareToCoordinate(sq).second;
			if(white_men&(1LL<<sq)){
				ll to_sq;
				if(1<=(i-2) && (i-2)<=10 && 1<=(j-2) && (j-2)<=10){
					ll sq1=coordinateToSquare({i-1,j-1});
					if((black_men&(1LL<<sq1)) || (black_kings&(1LL<<sq1))){
						to_sq=coordinateToSquare({i-2,j-2});
						if(isEmpty(to_sq,white_men,white_kings,black_men,black_kings)){
							string path=to_string(sq)+"x"+to_string(to_sq);
							capt[sq1]=true;
							dfs(to_sq,adj,capt,path,mencaptures);
						}
					}
				}
				for(ll i=1;i<=50;i++){
					capt[i]=false;
				}
				if(1<=(i-2) && (i-2)<=10 && 1<=(j+2) && (j+2)<=10){
					ll sq2=coordinateToSquare({i-1,j+1});
					if((black_men&(1LL<<sq2)) || (black_kings&(1LL<<sq2))){
						to_sq=coordinateToSquare({i-2,j+2});
						if(isEmpty(to_sq,white_men,white_kings,black_men,black_kings)){
							string path=to_string(sq)+"x"+to_string(to_sq);
							capt[sq2]=true;
							dfs(to_sq,adj,capt,path,mencaptures);
						}
					}
				}
				for(ll i=1;i<=50;i++){
					capt[i]=false;
				}
				if(1<=(i+2) && (i+2)<=10 && 1<=(j-2) && (j-2)<=10){
					ll sq3=coordinateToSquare({i+1,j-1});
					if((black_men&(1LL<<sq3)) || (black_kings&(1LL<<sq3))){
						to_sq=coordinateToSquare({i+2,j-2});
						if(isEmpty(to_sq,white_men,white_kings,black_men,black_kings)){
							string path=to_string(sq)+"x"+to_string(to_sq);
							capt[sq3]=true;
							dfs(to_sq,adj,capt,path,mencaptures);
						}
					}
				}
				for(ll i=1;i<=50;i++){
					capt[i]=false;
				}
				if(1<=(i+2) && (i+2)<=10 && 1<=(j+2) && (j+2)<=10){
					ll sq4=coordinateToSquare({i+1,j+1});
					if((black_men&(1LL<<sq4)) || (black_kings&(1LL<<sq4))){
						to_sq=coordinateToSquare({i+2,j+2});
						if(isEmpty(to_sq,white_men,white_kings,black_men,black_kings)){
							string path=to_string(sq)+"x"+to_string(to_sq);
							capt[sq4]=true;
							dfs(to_sq,adj,capt,path,mencaptures);
						}
					}
				}
			}
			if(white_kings&(1LL<<sq)){
				bool blackPiece=false;
				ll p_sq=0;
				ll curr_i=i,curr_j=j;
				while(curr_i>=2 && curr_j>=2){
					curr_i--;
					curr_j--;
					if((black_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (black_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						if(blackPiece){
							break;
						}
						blackPiece=true;
						p_sq=coordinateToSquare({curr_i,curr_j});
					}
					if((white_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (white_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						break;
					}
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings) && blackPiece){
						string path;
						path=to_string(sq)+"x"+to_string(coordinateToSquare({curr_i,curr_j}));
						capt[p_sq]=true;
						dfs2(coordinateToSquare({curr_i,curr_j}),adj2,capt,path,kingcaptures);
						for(ll i=1;i<=50;i++){
							capt[i]=false;
						}
					}
				}
				blackPiece=false;
				curr_i=i;
				curr_j=j;
				while(curr_i<=9 && curr_j<=9){
					curr_i++;
					curr_j++;
					if((black_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (black_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						if(blackPiece){
							break;
						}
						blackPiece=true;
						p_sq=coordinateToSquare({curr_i,curr_j});
					}
					if((white_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (white_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						break;
					}
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings) && blackPiece){
						string path;
						path=to_string(sq)+"x"+to_string(coordinateToSquare({curr_i,curr_j}));
						capt[p_sq]=true;
						dfs2(coordinateToSquare({curr_i,curr_j}),adj2,capt,path,kingcaptures);
						for(ll i=1;i<=50;i++){
							capt[i]=false;
						}
					}
				}
				blackPiece=false;
				curr_i=i;
				curr_j=j;
				while(curr_i<=9 && curr_j>=2){
					curr_i++;
					curr_j--;
					if((black_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (black_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						if(blackPiece){
							break;
						}
						blackPiece=true;
						p_sq=coordinateToSquare({curr_i,curr_j});
					}
					if((white_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (white_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						break;
					}
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings) && blackPiece){
						string path;
						path=to_string(sq)+"x"+to_string(coordinateToSquare({curr_i,curr_j}));
						capt[p_sq]=true;
						dfs2(coordinateToSquare({curr_i,curr_j}),adj2,capt,path,kingcaptures);
						for(ll i=1;i<=50;i++){
							capt[i]=false;
						}
					}
				}
				blackPiece=false;
				curr_i=i;
				curr_j=j;
				while(curr_i>=2 && curr_j<=9){
					curr_i--;
					curr_j++;
					if((black_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (black_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						if(blackPiece){
							break;
						}
						blackPiece=true;
						p_sq=coordinateToSquare({curr_i,curr_j});
					}
					if((white_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (white_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						break;
					}
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings) && blackPiece){
						string path;
						path=to_string(sq)+"x"+to_string(coordinateToSquare({curr_i,curr_j}));
						capt[p_sq]=true;
						dfs2(coordinateToSquare({curr_i,curr_j}),adj2,capt,path,kingcaptures);
						for(ll i=1;i<=50;i++){
							capt[i]=false;
						}
					}
				}
			}
		}
		for(size_t i=0;i<mencaptures.size();i++){
			longestCaptureSequence=max(longestCaptureSequence,static_cast<ll>(count(mencaptures[i].begin(),mencaptures[i].end(),'x')));
		}
		for(size_t i=0;i<kingcaptures.size();i++){
			longestCaptureSequence=max(longestCaptureSequence,static_cast<ll>(count(kingcaptures[i].begin(),kingcaptures[i].end(),'x')));
		}
		if(longestCaptureSequence==0){
			for(ll sq=1;sq<=50;sq++){
				ll i=squareToCoordinate(sq).first,j=squareToCoordinate(sq).second;
				if(white_men&(1LL<<sq)){
					if(j>=2){
						ll sq2=coordinateToSquare({i-1,j-1});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
					}
					if(j<=9){
						ll sq2=coordinateToSquare({i-1,j+1});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
					}
				}
				if(white_kings&(1LL<<sq)){
					ll curr_i=i,curr_j=j;
					while(curr_i>=2 && curr_j>=2){
						curr_i--;
						curr_j--;
						ll sq2=coordinateToSquare({curr_i,curr_j});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
						else{
							break;
						}
					}
					curr_i=i;
					curr_j=j;
					while(curr_i>=2 && curr_j<=9){
						curr_i--;
						curr_j++;
						ll sq2=coordinateToSquare({curr_i,curr_j});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
						else{
							break;
						}
					}
					curr_i=i;
					curr_j=j;
					while(curr_i<=9 && curr_j>=2){
						curr_i++;
						curr_j--;
						ll sq2=coordinateToSquare({curr_i,curr_j});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
						else{
							break;
						}
					}
					curr_i=i;
					curr_j=j;
					while(curr_i<=9 && curr_j<=9){
						curr_i++;
						curr_j++;
						ll sq2=coordinateToSquare({curr_i,curr_j});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
						else{
							break;
						}
					}
				}
			}
		}
		else{
			for(size_t i=0;i<mencaptures.size();i++){
				if(count(mencaptures[i].begin(),mencaptures[i].end(),'x')==longestCaptureSequence){
					valid.push_back(mencaptures[i]);
				}
			}
			for(size_t i=0;i<kingcaptures.size();i++){
				if(count(kingcaptures[i].begin(),kingcaptures[i].end(),'x')==longestCaptureSequence){
					valid.push_back(kingcaptures[i]);
				}
			}
		}
	}
	else{
		for(ll sq=1;sq<=50;sq++){
			if((white_men&(1LL<<sq)) || (white_kings&(1LL<<sq))){
				ll i=squareToCoordinate(sq).first,j=squareToCoordinate(sq).second;
				if((i-1)>=1 && (j-1)>=1 && (i+1)<=10 && (j+1)<=10){
					ll sq2=coordinateToSquare({i-1,j-1});
					ll sq3=coordinateToSquare({i+1,j+1});
					if(isEmpty(sq2,white_men,white_kings,black_men,black_kings) && isEmpty(sq3,white_men,white_kings,black_men,black_kings)){
						adj[sq2].push_back(sq3);
						adj[sq3].push_back(sq2);
					}
				}
				if((i-1)>=1 && (j+1)<=10 && (i+1)<=10 && (j-1)>=1){
					ll sq2=coordinateToSquare({i-1,j+1});
					ll sq3=coordinateToSquare({i+1,j-1});
					if(isEmpty(sq2,white_men,white_kings,black_men,black_kings) && isEmpty(sq3,white_men,white_kings,black_men,black_kings)){
						adj[sq2].push_back(sq3);
						adj[sq3].push_back(sq2);
					}
				}
			}
			if((white_men&(1LL<<sq)) || (white_kings&(1LL<<sq))){
				vector<ll> l1,r1,l2,r2;
				ll i=squareToCoordinate(sq).first,j=squareToCoordinate(sq).second;
				ll curr_i=i,curr_j=j;
				while(curr_i>=2 && curr_j>=2){
					curr_i--;
					curr_j--;
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings)){
						l1.push_back(coordinateToSquare({curr_i,curr_j}));
					}
					else{
						break;
					}
				}
				curr_i=i;
				curr_j=j;
				while(curr_i<=9 && curr_j<=9){
					curr_i++;
					curr_j++;
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings)){
						r1.push_back(coordinateToSquare({curr_i,curr_j}));
					}
					else{
						break;
					}
				}
				curr_i=i;
				curr_j=j;
				while(curr_i>=2 && curr_j<=9){
					curr_i--;
					curr_j++;
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings)){
						l2.push_back(coordinateToSquare({curr_i,curr_j}));
					}
					else{
						break;
					}
				}
				curr_i=i;
				curr_j=j;
				while(curr_i<=9 && curr_j>=2){
					curr_i++;
					curr_j--;
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings)){
						r2.push_back(coordinateToSquare({curr_i,curr_j}));
					}
					else{
						break;
					}
				}
				for(size_t i=0;i<l1.size();i++){
					for(size_t j=0;j<r1.size();j++){
						adj2[l1[i]].push_back({r1[j],sq});
						adj2[r1[j]].push_back({l1[i],sq});
					}
				}
				for(size_t i=0;i<l2.size();i++){
					for(size_t j=0;j<r2.size();j++){
						adj2[l2[i]].push_back({r2[j],sq});
						adj2[r2[j]].push_back({l2[i],sq});
					}
				}
			}
		}
		for(ll sq=1;sq<=50;sq++){
			vector<bool> capt(51,false);
			ll i=squareToCoordinate(sq).first,j=squareToCoordinate(sq).second;
			if(black_men&(1LL<<sq)){
				ll to_sq;
				if(1<=(i-2) && (i-2)<=10 && 1<=(j-2) && (j-2)<=10){
					ll sq1=coordinateToSquare({i-1,j-1});
					if((white_men&(1LL<<sq1)) || (white_kings&(1LL<<sq1))){
						to_sq=coordinateToSquare({i-2,j-2});
						if(isEmpty(to_sq,white_men,white_kings,black_men,black_kings)){
							string path=to_string(sq)+"x"+to_string(to_sq);
							capt[sq1]=true;
							dfs(to_sq,adj,capt,path,mencaptures);
						}
					}
				}
				for(ll i=1;i<=50;i++){
					capt[i]=false;
				}
				if(1<=(i-2) && (i-2)<=10 && 1<=(j+2) && (j+2)<=10){
					ll sq2=coordinateToSquare({i-1,j+1});
					if((white_men&(1LL<<sq2)) || (white_kings&(1LL<<sq2))){
						to_sq=coordinateToSquare({i-2,j+2});
						if(isEmpty(to_sq,white_men,white_kings,black_men,black_kings)){
							string path=to_string(sq)+"x"+to_string(to_sq);
							capt[sq2]=true;
							dfs(to_sq,adj,capt,path,mencaptures);
						}
					}
				}
				for(ll i=1;i<=50;i++){
					capt[i]=false;
				}
				if(1<=(i+2) && (i+2)<=10 && 1<=(j-2) && (j-2)<=10){
					ll sq3=coordinateToSquare({i+1,j-1});
					if((white_men&(1LL<<sq3)) || (white_kings&(1LL<<sq3))){
						to_sq=coordinateToSquare({i+2,j-2});
						if(isEmpty(to_sq,white_men,white_kings,black_men,black_kings)){
							string path=to_string(sq)+"x"+to_string(to_sq);
							capt[sq3]=true;
							dfs(to_sq,adj,capt,path,mencaptures);
						}
					}
				}
				for(ll i=1;i<=50;i++){
					capt[i]=false;
				}
				if(1<=(i+2) && (i+2)<=10 && 1<=(j+2) && (j+2)<=10){
					ll sq4=coordinateToSquare({i+1,j+1});
					if((white_men&(1LL<<sq4)) || (white_kings&(1LL<<sq4))){
						to_sq=coordinateToSquare({i+2,j+2});
						if(isEmpty(to_sq,white_men,white_kings,black_men,black_kings)){
							string path=to_string(sq)+"x"+to_string(to_sq);
							capt[sq4]=true;
							dfs(to_sq,adj,capt,path,mencaptures);
						}
					}
				}
			}
			if(black_kings&(1LL<<sq)){
				bool whitePiece=false;
				ll p_sq=0;
				ll curr_i=i,curr_j=j;
				while(curr_i>=2 && curr_j>=2){
					curr_i--;
					curr_j--;
					if((white_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (white_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						if(whitePiece){
							break;
						}
						whitePiece=true;
						p_sq=coordinateToSquare({curr_i,curr_j});
					}
					if((black_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (black_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						break;
					}
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings) && whitePiece){
						string path;
						path=to_string(sq)+"x"+to_string(coordinateToSquare({curr_i,curr_j}));
						capt[p_sq]=true;
						dfs2(coordinateToSquare({curr_i,curr_j}),adj2,capt,path,kingcaptures);
						for(ll i=1;i<=50;i++){
							capt[i]=false;
						}
					}
				}
				whitePiece=false;
				curr_i=i;
				curr_j=j;
				while(curr_i<=9 && curr_j<=9){
					curr_i++;
					curr_j++;
					if((white_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (white_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						if(whitePiece){
							break;
						}
						whitePiece=true;
						p_sq=coordinateToSquare({curr_i,curr_j});
					}
					if((black_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (black_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						break;
					}
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings) && whitePiece){
						string path;
						path=to_string(sq)+"x"+to_string(coordinateToSquare({curr_i,curr_j}));
						capt[p_sq]=true;
						dfs2(coordinateToSquare({curr_i,curr_j}),adj2,capt,path,kingcaptures);
						for(ll i=1;i<=50;i++){
							capt[i]=false;
						}
					}
				}
				whitePiece=false;
				curr_i=i;
				curr_j=j;
				while(curr_i<=9 && curr_j>=2){
					curr_i++;
					curr_j--;
					if((white_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (white_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						if(whitePiece){
							break;
						}
						whitePiece=true;
						p_sq=coordinateToSquare({curr_i,curr_j});
					}
					if((black_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (black_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						break;
					}
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings) && whitePiece){
						string path;
						path=to_string(sq)+"x"+to_string(coordinateToSquare({curr_i,curr_j}));
						capt[p_sq]=true;
						dfs2(coordinateToSquare({curr_i,curr_j}),adj2,capt,path,kingcaptures);
						for(ll i=1;i<=50;i++){
							capt[i]=false;
						}
					}
				}
				whitePiece=false;
				curr_i=i;
				curr_j=j;
				while(curr_i>=2 && curr_j<=9){
					curr_i--;
					curr_j++;
					if((white_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (white_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						if(whitePiece){
							break;
						}
						whitePiece=true;
						p_sq=coordinateToSquare({curr_i,curr_j});
					}
					if((black_men&(1LL<<coordinateToSquare({curr_i,curr_j}))) || (black_kings&(1LL<<coordinateToSquare({curr_i,curr_j})))){
						break;
					}
					if(isEmpty(coordinateToSquare({curr_i,curr_j}),white_men,white_kings,black_men,black_kings) && whitePiece){
						string path;
						path=to_string(sq)+"x"+to_string(coordinateToSquare({curr_i,curr_j}));
						capt[p_sq]=true;
						dfs2(coordinateToSquare({curr_i,curr_j}),adj2,capt,path,kingcaptures);
						for(ll i=1;i<=50;i++){
							capt[i]=false;
						}
					}
				}
			}
		}
		for(size_t i=0;i<mencaptures.size();i++){
			longestCaptureSequence=max(longestCaptureSequence,static_cast<ll>(count(mencaptures[i].begin(),mencaptures[i].end(),'x')));
		}
		for(size_t i=0;i<kingcaptures.size();i++){
			longestCaptureSequence=max(longestCaptureSequence,static_cast<ll>(count(kingcaptures[i].begin(),kingcaptures[i].end(),'x')));
		}
		if(longestCaptureSequence==0){
			for(ll sq=1;sq<=50;sq++){
				ll i=squareToCoordinate(sq).first,j=squareToCoordinate(sq).second;
				if(black_men&(1LL<<sq)){
					if(j>=2){
						ll sq2=coordinateToSquare({i+1,j-1});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
					}
					if(j<=9){
						ll sq2=coordinateToSquare({i+1,j+1});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
					}
				}
				if(black_kings&(1LL<<sq)){
					ll curr_i=i,curr_j=j;
					while(curr_i>=2 && curr_j>=2){
						curr_i--;
						curr_j--;
						ll sq2=coordinateToSquare({curr_i,curr_j});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
						else{
							break;
						}
					}
					curr_i=i;
					curr_j=j;
					while(curr_i>=2 && curr_j<=9){
						curr_i--;
						curr_j++;
						ll sq2=coordinateToSquare({curr_i,curr_j});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
						else{
							break;
						}
					}
					curr_i=i;
					curr_j=j;
					while(curr_i<=9 && curr_j>=2){
						curr_i++;
						curr_j--;
						ll sq2=coordinateToSquare({curr_i,curr_j});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
						else{
							break;
						}
					}
					curr_i=i;
					curr_j=j;
					while(curr_i<=9 && curr_j<=9){
						curr_i++;
						curr_j++;
						ll sq2=coordinateToSquare({curr_i,curr_j});
						if(isEmpty(sq2,white_men,white_kings,black_men,black_kings)){
							valid.push_back(to_string(sq)+"-"+to_string(sq2));
						}
						else{
							break;
						}
					}
				}
			}
		}
		else{
			for(size_t i=0;i<mencaptures.size();i++){
				if(count(mencaptures[i].begin(),mencaptures[i].end(),'x')==longestCaptureSequence){
					valid.push_back(mencaptures[i]);
				}
			}
			for(size_t i=0;i<kingcaptures.size();i++){
				if(count(kingcaptures[i].begin(),kingcaptures[i].end(),'x')==longestCaptureSequence){
					valid.push_back(kingcaptures[i]);
				}
			}
		}
	}
	return valid;
}
void drawCheck(ll &white_men,ll &white_kings,ll &black_men,ll &black_kings,ll &two_pc_draw,ll &three_pc_draw,vector<vector<ll>> &history,bool &repetition){
	ll wkcnt=0,wmcnt=0,bmcnt=0,bkcnt=0;
	for(ll sq=1;sq<=50;sq++){
		if(white_men&(1LL<<sq)){
			wmcnt++;
		}
		if(white_kings&(1LL<<sq)){
			wkcnt++;
		}
		if(black_men&(1LL<<sq)){
			bmcnt++;
		}
		if(black_kings&(1LL<<sq)){
			bkcnt++;
		}
	}
	if((wmcnt==0 && wkcnt==1 && bkcnt>=1 && ((bmcnt+bkcnt)<=2))||(bmcnt==0 && bkcnt==1 && wkcnt>=1 && ((wmcnt+wkcnt)<=2))){
		two_pc_draw++;
	}
	else{
		two_pc_draw=0;
	}
	if((wmcnt==0 && wkcnt==1 && bkcnt>=1 && ((bmcnt+bkcnt)<=3))||(bmcnt==0 && bkcnt==1 && wkcnt>=1 && ((wmcnt+wkcnt)<=3))){
		three_pc_draw++;
	}
	else{
		three_pc_draw=0;
	}
	ll cnt=0;
	for(size_t i=0;i<history.size();i++){
		if(history[i]==history[((ll)history.size())-1]){
			cnt++;
		}
	}
	if(cnt==3){
		repetition=true;
	}
}
void update(ll &white_men,ll &white_kings,ll &black_men,ll &black_kings,Texture2D &board,Texture2D &whiteMan,Texture2D &whiteKing,Texture2D &blackMan,Texture2D &blackKing){
	BeginDrawing();
	ClearBackground(BLACK);
	DrawTexture(board,0,0,WHITE);
	for(ll sq=1;sq<=50;sq++){
		ll x=squareToCoordinate(sq).first,y=squareToCoordinate(sq).second;
		if(white_men&(1LL<<sq)){
			DrawTexture(whiteMan,(y-1)*60,(x-1)*60,WHITE);
		}
		if(white_kings&(1LL<<sq)){
			DrawTexture(whiteKing,(y-1)*60,(x-1)*60,WHITE);
		}
		if(black_men&(1LL<<sq)){
			DrawTexture(blackMan,(y-1)*60,(x-1)*60,WHITE);
		}
		if(black_kings&(1LL<<sq)){
			DrawTexture(blackKing,(y-1)*60,(x-1)*60,WHITE);
		}
	}
	EndDrawing();
}
int main(){
	ll white_men=2251797666201600,white_kings=0,black_men=2097151,black_kings=0;
	string engine1Color="w",engine2Color="b";
	ll kingStreak=0,two_pc_draw=0,three_pc_draw=0;
	bool repetition=false;
	vector<vector<ll>> history;
	InitWindow(600,600,"Draughts");
	SetTargetFPS(60);
	Texture2D board=LoadTexture("Assets/Board.png");
    Texture2D whiteMan=LoadTexture("Assets/wM.png");
    Texture2D whiteKing=LoadTexture("Assets/wK.png");
	Texture2D blackMan=LoadTexture("Assets/bM.png");
    Texture2D blackKing=LoadTexture("Assets/bK.png");
	Image icon=LoadImage("Assets/bk.png");
	SetWindowIcon(icon);
	UnloadImage(icon);
	update(white_men,white_kings,black_men,black_kings,board,whiteMan,whiteKing,blackMan,blackKing);
	while(true){
		vector<string> validMoves;
		validMoves=generateValidMoves(white_men,white_kings,black_men,black_kings,engine1Color);
		if(validMoves.size()==0){
			cout<<"Engine 2 won!"<<endl;
			break;
		}
		string engine1Move=engine1(white_men,white_kings,black_men,black_kings,engine1Color,validMoves);	
		cout<<engine1Move<<endl;
		playMove(engine1Move,white_men,white_kings,black_men,black_kings,kingStreak);
		history.push_back({white_men,white_kings,black_men,black_kings,1});
		drawCheck(white_men,white_kings,black_men,black_kings,two_pc_draw,three_pc_draw,history,repetition);
		if((kingStreak==50) || (two_pc_draw==10) || (three_pc_draw==32) || repetition){
			cout<<"Draw!"<<endl;
			break;
		}
		update(white_men,white_kings,black_men,black_kings,board,whiteMan,whiteKing,blackMan,blackKing);
		validMoves=generateValidMoves(white_men,white_kings,black_men,black_kings,engine2Color);
		if(validMoves.size()==0){
			cout<<"Engine 1 won!"<<endl;
			break;
		}
		string engine2Move=engine2(white_men,white_kings,black_men,black_kings,engine2Color,validMoves);	
		cout<<engine2Move<<endl;
		playMove(engine2Move,white_men,white_kings,black_men,black_kings,kingStreak);
		history.push_back({white_men,white_kings,black_men,black_kings,0});
		drawCheck(white_men,white_kings,black_men,black_kings,two_pc_draw,three_pc_draw,history,repetition);
		if((kingStreak==50) || (two_pc_draw==10) || (three_pc_draw==32) || repetition){
			cout<<"Draw!"<<endl;
			break;
		}
		update(white_men,white_kings,black_men,black_kings,board,whiteMan,whiteKing,blackMan,blackKing);
	}
	UnloadTexture(board);
	UnloadTexture(whiteMan);
	UnloadTexture(whiteKing);
	UnloadTexture(blackMan);
	UnloadTexture(blackKing);
	CloseWindow();
}
