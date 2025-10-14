#include<iostream> 
#include<iomanip>
#define ex -1

using namespace std;

double tinh(int l, int r, double p, double k)
{
    double kq=0;
    if(k < l) return kq;
    else
    {
        if(k < r || r == ex) kq = (k-l)*p;
        else kq = (r-l)*p;
    }
    return kq;
}


int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    double n;
    cin >> n;
    n = n / 1000000.0;
    double s = tinh(4, 6, 0.05, n) + tinh(6, 9, 0.1, n) + tinh(9, 14, 0.15, n) + tinh(14, 24, 0.2, n) + tinh(24, 44, 0.25, n) + tinh(44, 84, 0.3, n) + tinh(84, ex, 0.35, n);
    cout << "Tien thue la: " << fixed << setprecision(0) <<  s*1000000.0;
}