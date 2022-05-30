#include<bits/stdc++.h>
using namespace std;
#define LL long long
#define INF ((1LL<<61)+(1LL<<30)-1)
#define LD long double
#define endl '\n'
#define print(x) "[ " << #x << ": " << x << " ]" << flush
#define printW(x,y) "[ " << #x << ": " << setw(y) << x << " ]" << flush
#define printE(a,x) "[ " << #a << "[" << x << "]: " << a[x] << " ]" << flush
#define printV(v)   cout << "[ " << #v << ": "; for(auto _i : v) cout << _i << ' '; cout << "]" << flush << endl
#define printA(a,n) cout << "[ " << #a << ": "; for(int i=0;i<n;i++) cout << a[i] << ' '; cout << "]" << flush << endl
#pragma GCC target ("avx2")
#pragma GCC optimization ("O2")
#pragma GCC optimization ("unroll-loops")
const LD ERR = 1e-9; const int MOD = 1e9 + 7; bool areEqual( LD _n1, LD _n2 ){ return fabs( _n1 - _n2 ) < ERR; } bool smallerEqual( LD _n1, LD _n2 ){ return _n1 < _n2 || areEqual( _n1, _n2 ); }
int modSum( LL _n1, LL _n2 ){ return ( _n1 + _n2 ) % MOD; } int modProd( LL _n1, LL _n2 ){ return ( _n1 * _n2 ) % MOD; }
#ifdef debug
    const bool DEBUG = true;
#else
    const bool DEBUG = false;
#endif

int rankRank( char c ){
    if( c >= '2' && c <= '9' ) return int(c-'2'+2); if( c == 'T' ) return 10; if( c == 'J' ) return 11; if( c == 'Q' ) return 12; if( c == 'K' ) return 13; if( c == 'A' ) return 14; return -1;
}
int suitRank( char c ){
    if( c == 'C' ) return 0; if( c == 'D' ) return 1; if( c == 'H' ) return 2; if( c == 'S' ) return 3; return -1;
}

struct Card{
    string card;
    char suit, rank;
    Card(){}
    Card( char _s, char _r ) : suit(_s), rank(_r) { card += suit; card += rank; }
    Card( string _c ) : card(_c) { suit = card[0]; rank = card[1]; swapSuitRank(); }
    void swapSuitRank(){ swap( suit, rank ); }
    bool operator <( const Card &o ) {
        if( rank != o.rank ) return rankRank(rank) < rankRank(o.rank);
        return suitRank(suit) < suitRank(o.suit);
    } 
};

const char Ranks[] = { '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A' };
const char Suits[] = { 'C', 'D', 'H', 'S' };
vector <Card> deck[2];
vector < function<bool(int)> > isPresent(9);
vector < function<int()> > winner(9);

void init();
bool inputDecks();
int solve();

int main(){ ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    
    init();

    while( inputDecks() ){
        
        for( int i = 0 ; i <= 1 ; i++ ) sort( deck[i].begin(), deck[i].end() );

        int RESULT = solve();

        if( RESULT == -1 ) cout << "Tie." << endl;
        else cout << ( RESULT == 0 ? "Black wins." : "White wins." ) << endl;

    }

}

function < int(int,char) > countSameRank = []( int p, char v ) -> int{
    vector <Card> &d = deck[p];
    int rv = 0;
    for( Card c : d ) rv += c.rank == v;
    return rv;
};

function < int(int,char) > countSameSuit = []( int p, char s ) -> int{
    vector <Card> &d = deck[p];
    int rv = 0;
    for( Card c : d ) rv += c.suit == s;
    return rv;
};

void init(){

    // 0 - Straight flush
    isPresent[0] = [](int p) -> bool{
        return isPresent[3](p) && isPresent[4](p);
    };
    winner[0] = []() -> int{
        int r0 = rankRank( deck[0][4].rank );
        int r1 = rankRank( deck[1][4].rank );
        if( r0 == r1 ) return -1;
        return r1 > r0; 
    };

    // 1 - Four of a kind.
    isPresent[1] = [](int p) -> bool{
        for( char r : Ranks ) if( countSameRank( p, r ) == 4 ) return true;
        return false;
    };
    winner[1] = []() -> int{
        int r0, r1;
        for( char r : Ranks ){
            if( countSameRank( 0, r ) == 4 ) r0 = rankRank( r );
            if( countSameRank( 1, r ) == 4 ) r1 = rankRank( r );
        }
        if( r0 == r1 ) return -1;
        return r1 > r0; 
    };

    // 2 - Full House
    isPresent[2] = [](int p) -> bool{
        vector <Card> potentialPair;
        bool hasThree = false;
        for( Card c : deck[p] ){
            if( countSameRank( p, c.rank ) == 3 ){ hasThree = true; continue; }
            potentialPair.push_back(c);
        }
        if( !hasThree ) return false;
        return potentialPair[0].rank == potentialPair[1].rank;
    };
    winner[2] = []() -> int{
        int r0, r1;
        for( char r : Ranks ){
            if( countSameRank( 0, r ) == 3 ) r0 = rankRank( r );
            if( countSameRank( 1, r ) == 3 ) r1 = rankRank( r );
        }
        if( r0 == r1 ) return -1;
        return r1 > r0; 
    };

    // 3 - Flush
    isPresent[3] = [](int p) -> bool{
        return countSameSuit( p, deck[p][0].suit ) == 5;
    }; 
    winner[3] = []() -> int{
        int r0 = rankRank( deck[0][4].rank );
        int r1 = rankRank( deck[1][4].rank );
        if( r0 == r1 ) return -1;
        return r1 > r0; 
    };

    // 4 - Straight
    isPresent[4] = [](int p) -> bool{
        for( int i = 1 ; i <= 4 ; i++ ){
            if( rankRank( deck[p][i].rank ) - 1 != rankRank( deck[p][i-1].rank ) ) return false;
        }
        return true;
    };
    winner[4] = []() -> int{
        int r0 = rankRank( deck[0][4].rank );
        int r1 = rankRank( deck[1][4].rank );
        if( r0 == r1 ) return -1;
        return r1 > r0; 
    };

    // 5 - Three of a Kind
    isPresent[5] = [](int p) -> bool{
        for( char r : Ranks ) if( countSameRank( p, r ) == 3 ) return true;
        return false;
    };
    winner[5] = []() -> int{
        int r0, r1;
        for( char r : Ranks ){
            if( countSameRank( 0, r ) == 3 ) r0 = rankRank( r );
            if( countSameRank( 1, r ) == 3 ) r1 = rankRank( r );
        }
        if( r0 == r1 ) return -1;
        return r1 > r0; 
    };

    // 6 - Two Pairs
    isPresent[6] = [](int p) -> bool{
        int pairCount = 0;
        for( char r : Ranks ){
            if( countSameRank( p, r ) == 2 ) pairCount++;
        }
        return pairCount == 2;
    };
    winner[6] = []() -> int{
        vector <int> v0, v1;
        int rem0, rem1;
        for( char r : Ranks ){
            if( countSameRank( 0, r ) == 2 ) v0.push_back( rankRank(r) );
            else if( countSameRank( 0, r ) == 1 ) rem0 = rankRank(r);
            if( countSameRank( 1, r ) == 2 ) v1.push_back( rankRank(r) );
            else if( countSameRank( 1, r ) == 1 ) rem1 = rankRank(r);
        }
        sort( v0.begin(), v0.end() ); sort( v1.begin(), v1.end() );
        if( v0[1] != v1[1] ) return v1[1] > v0[1];
        if( v0[0] != v1[0] ) return v1[0] > v0[0];
        if( rem0 == rem1 ) return -1;
        return rem1 > rem0; 
    };

    // 7 - Pair
    isPresent[7] = [](int p) -> bool{
        for( char r : Ranks ){
            if( countSameRank( p, r ) == 2 ) return true;
        }
        return false;
    };
    winner[7] = []() -> int{
        int p0, p1;
        vector <int> r0, r1;
        for( char r : Ranks ){
            if( countSameRank( 0, r ) == 2 ) p0 = rankRank(r);
            else if( countSameRank( 0, r ) == 1 ) r0.push_back(rankRank(r));
            if( countSameRank( 1, r ) == 2 ) p1 = rankRank(r);
            else if( countSameRank( 1, r ) == 1 ) r1.push_back(rankRank(r));
        }
        if( p0 != p1 ) return p1 > p0;
        sort( r0.begin(), r0.end() ); sort( r1.begin(), r1.end() );
        for( int i = 2 ; i >= 0 ; i-- ){
            if( r0[i] != r1[i] ) return r1[i] > r0[i];
        }
        return -1;
    };

    isPresent[8] = [](int p) -> bool{
        return true;
    };
    winner[8] = []() -> int{
        for( int i = 4 ; i >= 0 ; i-- ){
            if( rankRank( deck[0][i].rank ) != rankRank( deck[1][i].rank ) ) return rankRank( deck[1][i].rank ) > rankRank( deck[0][i].rank );
        }
        return -1;
    };

}

int solve(){

    for( int i = 0 ; i < 9 ; i++ ){

        bool hasBlack = isPresent[i](0);
        bool hasWhite = isPresent[i](1);
        if( !hasBlack && !hasWhite ) continue;
        if( hasBlack != hasWhite ) return hasWhite;

        int roundWinner = winner[i]();
        if( roundWinner == -1 ) continue;
        return roundWinner;

    }

    return -1;

}

bool inputDecks(){

    for( int d = 0 ; d < 2 ; d++ ){
        deck[d].clear();
        for( int i = 0 ; i < 5 ; i++ ){
            string CARD;
            if( !( cin >> CARD ) ) return false;
            deck[d].push_back(CARD);
        }
    }

    return true;

}
