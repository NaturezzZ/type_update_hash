#include<bits/stdc++.h>
#include "sketch.h"
using namespace std;
#define fi first
#define se second
#define mp make_pair
#define pb push_back
#define SZ(x) (int((x).size()))
#define All(x) (x).begin(), (x).end()
typedef pair<string, int> psi;
const int typebit = 2, hashnum = 4, bucknum = 50000;
const int TRACE_LEN = 26;
const int TUPLE_LEN = 13;
const int WIN = 50000;
#define rep(i,a,b) for(int (i)=(a);(i)<=(b);(i)++)
#define rep2(i,a,b) for(int (i)=(a);(i)<(b);(i)++)

string strrep[WIN];
map<string, int> strmap;
Sketch<TUPLE_LEN> * sketch = NULL;
CMSketch<TUPLE_LEN> * cmsketch = NULL;
void measure(){

}

int main() {
	ifstream fin("./data/20.dat", ios::binary);
	ofstream fout("log.csv");

	uint8_t key[TRACE_LEN];
	rep2 (i, 0, 20){
		cout << i << endl;
		sketch = new Sketch<TUPLE_LEN>(typebit, hashnum, bucknum, TRACE_LEN);
		cmsketch = new CMSketch<TUPLE_LEN>(hashnum, bucknum, TRACE_LEN);
		rep2(j, 0, WIN) strrep[j].clear();
		strmap.clear();

		rep2(pkt, 0, WIN){

			fin.read((char *)key, sizeof(char) * TRACE_LEN);

			string tuple;
			for(int j = 0; j < TUPLE_LEN; j++){
				tuple += (char)key[j];
			}
			strmap[tuple] += 1;
			strrep[pkt] = tuple;
			//cout << "key" ;
			//for(int t = 0; t < 26; t++)cout << (unsigned int)key[t];
			//cout << endl;
			sketch->insert(key);
			cmsketch->insert(key);
			memset(key,0,sizeof(key));
		}

		map<string, int>::iterator it;
		float cnt = 0;
		float sum = 0;
		float cmsum = 0;
		for(it = strmap.begin(); it != strmap.end(); it++){
			cnt += 1;
			int skepre = (int) sketch->query((uint8_t *)it->fi.c_str());
			//cout << skepre << endl;
			int cmpre = (int) cmsketch->query((uint8_t *) it->fi.c_str());
			//cout << cmpre << endl;
			int gt = it->se;
			sum += (float) abs(gt-skepre)/(float) gt;
			cmsum += (float) abs(gt-cmpre)/(float) gt;
		}

		cout << sum/cnt << ',' << cmsum/cnt << endl;
		fout << sum/cnt << ',' << cmsum/cnt << endl;
		
		if (sketch) delete sketch, sketch = NULL;
		if (cmsketch) delete cmsketch, cmsketch = NULL;
	}
	return 0;
}
