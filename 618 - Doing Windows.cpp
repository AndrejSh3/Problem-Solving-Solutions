// Problem Statement: https://onlinejudge.org/external/6/618.pdf
#include<bits/stdc++.h>
using namespace std;
#define LL long long
#define INF (((1LL<<61)+(1LL<<30)-1)/2)
#define LD long double
#define endl '\n'
#define print(x) "[ " << #x << ": " << x << " ]" << flush
#define printW(x,y) "[ " << #x << ": " << setw(y) << x << " ]" << flush
#define printE(a,x) "[ " << #a << "[" << x << "]: " << a[x] << " ]" << flush
#define printV(v)   if(1){cout << "[ " << #v << ": "; for(auto _i : v) cout << _i << ' '; cout << "]" << flush << endl;}
#define printA(a,n) if(1){cout << "[ " << #a << ": "; for(int i=0;i<n;i++) cout << a[i] << ' '; cout << "]" << flush << endl;}
#pragma GCC target ("avx2")
#pragma GCC optimization ("O2")
#pragma GCC optimization ("unroll-loops")
const LD ERR = 1e-7; const int MOD = 1e9 + 7; bool areEqual( LD _n1, LD _n2 ){ return fabs( _n1 - _n2 ) < ERR; } bool smallerEqual( LD _n1, LD _n2 ){ return _n1 < _n2 || areEqual( _n1, _n2 ); }
int modSum( LL _n1, LL _n2 ){ return ( _n1 + _n2 ) % MOD; } int modProd( LL _n1, LL _n2 ){ return ( _n1 * _n2 ) % MOD; }
#ifdef debug
    const bool DEBUG = true;
#else
    const bool DEBUG = false;
#endif

struct window{
    int x, y;
    int w, h;
    window(){}
    window( int _x, int _y ) : x(_x), y(_y) {}
    void reduce();
    int area( int );
    const bool operator < ( const window &o ){
        if( x != o.x ) return x < o.x;
        return y < o.y;
    }
};

bool arrangmentPossible();
int findGCD( vector<int> );
bool arrangeRectangles( vector <int>, int, int );
bool makeArrangment( int, int&, int& );
bool rectanglesIntersect( window, window );

const int maxW = 4;
int W, H;
window windowList[maxW], win[maxW];

int main(){ ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

    int TC = 0;
    while( cin >> W >> H && (W|H) ){ TC++;

        for( int i = 0 ; i < 4 ; i++ ) { cin >> windowList[i].x >> windowList[i].y; windowList[i].reduce(); }

        cout << "Set " << TC << ": " << ( arrangmentPossible() ? "Yes" : "No" ) << endl;

    }

}



bool arrangmentPossible(){

    if( DEBUG ) cout << endl;

    int AREA = W * H;

    for( int r0 = 1 ; r0*windowList[0].x <= W && r0*windowList[0].y <= H && windowList[0].area(r0) < AREA ; r0++ )
    for( int r1 = 1 ; r1*windowList[1].x <= W && r1*windowList[1].y <= H && windowList[1].area(r1) + windowList[0].area(r0) < AREA ; r1++ )
    for( int r2 = 1 ; r2*windowList[2].x <= W && r2*windowList[2].y <= H && windowList[2].area(r2) + windowList[1].area(r1) + windowList[0].area(r0) < AREA ; r2++ ){
        int remArea = AREA - (windowList[2].area(r2) + windowList[1].area(r1) + windowList[0].area(r0));
        if( remArea % (windowList[3].x*windowList[3].y) ) continue;
        int r3 = remArea / ( windowList[3].x*windowList[3].y );
        if( int(sqrt(r3)+ERR)*int(sqrt(r3)+ERR) != r3 ) continue;
        r3 = sqrt(r3+ERR);
        if( !( r3*windowList[3].x <= W && r3*windowList[3].y <= H ) ) continue;

        int _GCD = findGCD( {r0, r1, r2, r3, AREA} );
        int g0 = r0 / _GCD;
        int g1 = r1 / _GCD;
        int g2 = r2 / _GCD;
        int g3 = r3 / _GCD;
        int w = W / _GCD;
        int h = H / _GCD;

        if( DEBUG ) cout << print(g0) << print(g1) << print(g2) << print(g3) << print(w) << print(h) << endl;

        if( arrangeRectangles( {g0, g1, g2, g3}, w, h ) ) return true;

    }

    return false;

}

int findGCD( vector <int> V ){

    int rv = V[0];
    for( int i = 1 ; i < V.size() ; i++ ){
        rv = __gcd( rv, V[i] );
    }
    return rv;

}

bool windowUsed[maxW];
vector <window> usedWindowList, criticalPoints;
bool arrangeRectangles( vector <int> gv, int w, int h ){

    memset( windowUsed, 0, sizeof windowUsed );
    usedWindowList.clear();
    criticalPoints.clear();
    criticalPoints.push_back({0,0});
    for( int i = 0 ; i < 4 ; i++ ) win[i] = { windowList[i].x * gv[i], windowList[i].y * gv[i] };

    sort( win, win+4 );

    return makeArrangment( 4, w, h );

}

bool makeArrangment( int windowsRemaining, int &w, int &h ){

    if( !windowsRemaining ) return true;
    if( criticalPoints.empty() ) return false;

    window candidateWindow;
    for( window point : criticalPoints ){

        for( int i = 0 ; i < 4 ; i++ ){

            if( windowUsed[i] ) continue;

            candidateWindow = point;
            candidateWindow.w = win[i].x, candidateWindow.h = win[i].y;
            
            bool hasConflict = false;
            for( window uw : usedWindowList ) if( rectanglesIntersect( candidateWindow, uw ) ) hasConflict = true;
            if( candidateWindow.x + candidateWindow.w > w ) continue;
            if( candidateWindow.y + candidateWindow.h > h ) continue;
            if( hasConflict ) continue;

            windowUsed[i] = true;
            usedWindowList.push_back(candidateWindow);
            criticalPoints.push_back({point.x + candidateWindow.w, point.y});
            criticalPoints.push_back({point.x, point.y + candidateWindow.h});
            if( makeArrangment( windowsRemaining-1, w, h ) ) return true;
            criticalPoints.pop_back();
            criticalPoints.pop_back();
            usedWindowList.pop_back();
            windowUsed[i] = false;

        }

    }

    return false;

}

bool rectanglesIntersect( window A, window B ){
    return A.x < B.x + B.w && A.x + A.w > B.x && A.y < B.y + B.h && A.y + A.h > B.y;
}



void window::reduce(){
    int GCD = __gcd( x, y );
    x /= GCD;
    y /= GCD;
}

int window::area( int r ){
    return r*r*x*y;
}
