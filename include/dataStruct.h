#ifndef DATASTRUCT
#define DATASTRUCT

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include <map>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

class EDGE;

class VERTEX {
	public:
		VERTEX(string s) {
			name = s;
			ov_index = -1;
		};
		string name;
		vector< EDGE * > connected_edges;
		int ov_index;
};

class EDGE {
	public:
		EDGE() {};
		EDGE(VERTEX *a, VERTEX *b, bool f) {
			v1 = a;
			v2 = b;
			fake = f;
			flag = false;
		};
		VERTEX *v1, *v2;
		bool flag;
		bool fake;
};

#endif
