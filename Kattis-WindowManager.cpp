// Problem Statement: https://open.kattis.com/problems/windows
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
int modSum( int _n1, int _n2 ){ return ( _n1 + _n2 ) % MOD; } int modProd( int _n1, int _n2 ){ return ( _n1 * _n2 ) % MOD; }
#ifdef debug
    const bool DEBUG = true;
#else
    const bool DEBUG = false;
#endif

struct rect{
    int openedIdx = -1;
    int x, y, w, h;
    rect(){}
    rect( int _x, int _y, int _w, int _h ) : x(_x), y(_y), w(_w), h(_h) {}
    const bool operator < ( const rect &o ) const{
        if( x != o.x ) return x < o.x;
        return y < o.y;
    }
    const bool operator == ( const rect &o ) const{
        return x == o.x && y == o.y && w == o.w && h == o.h;
    }
};

struct point{
    int x, y;
    point(){}
    point( int _x, int _y ) : x(_x), y(_y) {}
};

void openWindow( int, int, int, int );
void closeWindow( int, int );
void resizeWindow( int, int, int, int );
void moveWindow( int, int, int, int );

bool windowsOverlap( rect, rect );
bool containsPoint( rect, point );
int findWindow( int, int );

bool openedIdxSort( rect, rect );
void printWindows();
void clearWindows();
void rotateEverything(int&,int&);
void flipEverything(int&,int&);

int attemptMove( int );
void makeMove( int, int );

const int maxN = 300;
int WIDTH, HEIGHT;
vector <rect> windowList;
int CMD_IDX = 0, NEXT_OPEN_IDX = 0;
int memo[maxN];

int main(){ ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

    cin >> WIDTH >> HEIGHT;
    
    string inputString;
    while( cin >> inputString ){ CMD_IDX++;

        if( isdigit(inputString[0]) ){ WIDTH = stoi(inputString); cin >> HEIGHT; printWindows(); clearWindows(); continue; }

        if( inputString == "OPEN" ){
            int x, y, w, h;
            cin >> x >> y >> w >> h;
            openWindow( x, y, w, h );
        }

        if( inputString == "CLOSE" ){
            int x, y;
            cin >> x >> y;
            closeWindow( x, y );
        }

        if( inputString == "RESIZE" ){
            int x, y, w, h;
            cin >> x >> y >> w >> h;
            resizeWindow( x, y, w, h );
        }

        if( inputString == "MOVE" ){
            int x, y, dx, dy;
            cin >> x >> y >> dx >> dy;
            moveWindow( x, y, dx, dy );
        }

        if( !DEBUG ) continue;

        cout << print(CMD_IDX) << endl;
        printWindows();
        cout << endl;
    
    }

    printWindows();

}



void openWindow( int x, int y, int w, int h ){

    if( x + w > WIDTH || y + h > HEIGHT ){ cout << "Command " << CMD_IDX << ": OPEN - window does not fit" << endl; return; }

    rect newWindow( x, y, w, h );

    for( rect w : windowList ){

        if( windowsOverlap( newWindow, w ) ){
            cout << "Command " << CMD_IDX << ": OPEN - window does not fit" << endl; return;
        }

    }
    
    newWindow.openedIdx = NEXT_OPEN_IDX++;
    windowList.push_back(newWindow);

}

void closeWindow( int x, int y ){

    int idx = findWindow( x, y );

    if( idx == -1 ){ cout << "Command " << CMD_IDX << ": CLOSE - no window at given position" << endl; return; }

    swap( windowList[idx], windowList[windowList.size()-1] );
    windowList.pop_back();

}

void resizeWindow( int x, int y, int w, int h ){

    int idx = findWindow( x, y );

    if( idx == -1 ){ cout << "Command " << CMD_IDX << ": RESIZE - no window at given position" << endl; return; }

    if( windowList[idx].x + w > WIDTH || windowList[idx].y + h > HEIGHT ){ cout << "Command " << CMD_IDX << ": RESIZE - window does not fit" << endl; return; }

    rect newWindow( windowList[idx].x, windowList[idx].y, w, h );

    for( rect w : windowList ){

        if( w == windowList[idx] ) continue;

        if( windowsOverlap( newWindow, w ) ){
            cout << "Command " << CMD_IDX << ": RESIZE - window does not fit" << endl; return;
        }

    }

    newWindow.openedIdx = windowList[idx].openedIdx;
    windowList[idx] = newWindow;

}

void moveWindow( int x, int y, int dx, int dy ){

    int idx = findWindow( x, y );

    if( idx == -1 ){ cout << "Command " << CMD_IDX << ": MOVE - no window at given position" << endl; return; }

    int moveDist = ( dx ? dx : dy );

    if( !dx ) rotateEverything(x,y);

    if( moveDist < 0 ) flipEverything(x,y);

    sort( windowList.begin(), windowList.end() );
    idx = findWindow(x,y);

    memset( memo, -1, sizeof memo );
    
    int maxPossibleMove = min( abs(moveDist), attemptMove( idx ) );

    if( maxPossibleMove != abs(moveDist) ){
        cout << "Command " << CMD_IDX << ": MOVE - moved " << maxPossibleMove << " instead of " << abs(moveDist) << endl;
    }

    makeMove( idx, maxPossibleMove );

    if( moveDist < 0 ) flipEverything(x,y);

    if( !dx ) rotateEverything(x,y);

}



bool windowsOverlap( rect A, rect B ){
    return ( A.x < B.x+B.w ) && ( A.x+A.w > B.x ) && ( A.y < B.y+B.h ) && ( A.y+A.h > B.y );
}

bool containsPoint( rect R, point p ){
    return R.x <= p.x && p.x < R.x+R.w && R.y <= p.y && p.y < R.y+R.h;
}

int findWindow( int x, int y ){
    for( int i = 0 ; i < windowList.size() ; i++ ){
        if( containsPoint( windowList[i], {x,y} ) ) return i;
    }
    return -1;
}



bool openedIdxSort( rect p, rect q ){
    return p.openedIdx < q.openedIdx;
} 

void printWindows(){
    sort( windowList.begin(), windowList.end(), openedIdxSort );
    cout << windowList.size() << " window(s):" << endl;
    for( rect w : windowList ){
        cout << w.x << ' ' << w.y << ' ' << w.w << ' ' << w.h << endl;
    }
}

void clearWindows(){
    CMD_IDX = NEXT_OPEN_IDX = 0;
    windowList.clear();
}

void rotateEverything( int &x, int &y ){

    swap( x, y );
    swap( WIDTH, HEIGHT );
    for( rect &r : windowList ){
        swap( r.x, r.y );
        swap( r.w, r.h );
    }

}

void flipEverything( int &x, int &y ){

    x = WIDTH - x - 1;
    for( rect &r : windowList ){
        r.x = WIDTH - r.x - r.w;
    }

}



int attemptMove( int idx ){ // Returns the maximum amount of pixels the window can move to the right

    if( memo[idx] != -1 ) return memo[idx];

    rect currentWindow = windowList[idx];
    int rv = WIDTH - currentWindow.x - currentWindow.w;
    rect newWindow( currentWindow.x, currentWindow.y, currentWindow.w+rv, currentWindow.h );

    if( DEBUG ) cout << print(idx) << ' ' << currentWindow.x << ' ' << currentWindow.y << ' ' << currentWindow.w << ' ' << currentWindow.h << endl;

    for( int j = idx+1 ; j < windowList.size() ; j++ ){

        if( !windowsOverlap( newWindow, windowList[j] ) ) continue;

        int currentLimit = attemptMove(j) + ( windowList[j].x - currentWindow.x - currentWindow.w );

        if( rv <= currentLimit ) continue;

        rv = currentLimit;
        newWindow.w = currentWindow.w + rv;

    }

    return memo[idx] = rv;

}

void makeMove( int idx, int moveDist ){

    if( moveDist <= 0 ) return;

    rect currentWindow = windowList[idx];
    rect affectedArea( currentWindow.x, currentWindow.y, currentWindow.w+moveDist, currentWindow.h );
    rect newWindow( currentWindow.x+moveDist, currentWindow.y, currentWindow.w, currentWindow.h );

    for( int j = idx+1 ; j < windowList.size() ; j++ ){

        if( !windowsOverlap( affectedArea, windowList[j] ) ) continue;

        makeMove( j, moveDist - abs( windowList[j].x - ( currentWindow.x+currentWindow.w ) ) );

    }

    newWindow.openedIdx = windowList[idx].openedIdx;
    windowList[idx] = newWindow;

}
