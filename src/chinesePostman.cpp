#include "network_manager.h"
#include "path.h"
#include "dataStruct.h"

void readFile(const char *);
bool findOddDegreeNodes();
void calculateShortestPath();
int	minWeightMatching(int, int);
void allShortestPaths();
vector<string> findShortestPath(string, string, Vertex *);
void addFakeEdges();
vector<VERTEX *> hierholzer();
void tracePath();

NetworkManager *nm, *nm2;
Vertex *nm2_head;

vector<VERTEX *> vlist, ovlist, euler_cycle;
vector<EDGE *> elist;
vector<vector<Edge *>> avail_paths, shortpath;
vector<vector<int>> val_of_shortpath;

int **length_matrix;
VERTEX **optimal_list;
int min_weight = INT_MAX;

int main(int argc, char *argv[])
{
	nm = new NetworkManager();
	nm->interpret(argv[1]);
	
	Path *path;
	path = new Path();
	path->append(nm->elist);

	readFile(argv[1]);
	allShortestPaths();

	if (!findOddDegreeNodes()) {
		optimal_list = new VERTEX*[ovlist.size()];
		calculateShortestPath();
		minWeightMatching(0, 0);
		addFakeEdges();
	}

	euler_cycle = hierholzer();
	tracePath();
	return 0;
}

void readFile(const char *filename)
{
	ifstream in_file(filename, ios::in);

	if (!in_file) {
		cout << "The file cannot be opened." << endl;
		exit(1);
	}

	VERTEX *v1, *v2;
	EDGE *edge;
	string line, s1, s2;
	const char *delimiters = " \t";
	map< string, VERTEX * > hash_tb;
	map< string, VERTEX * >::iterator iter_v1, iter_v2;

	while (!in_file.eof()) {
		getline(in_file, line);
		if (line.length() == 0) continue;
		
		s1 = strtok(strdup(line.c_str()), delimiters);
		s2 = strtok(NULL, delimiters);
		
		iter_v1 = hash_tb.find(s1);
		if (iter_v1 == hash_tb.end()) {
			v1 = new VERTEX(s1);
			hash_tb[s1] = v1;
			vlist.push_back(v1);
		}
		else
			v1 = iter_v1->second;
		
		iter_v2 = hash_tb.find(s2);
		if (iter_v2 == hash_tb.end()) {
			v2 = new VERTEX(s2);
			hash_tb[s2] = v2;
			vlist.push_back(v2);
		}
		else
			v2 = iter_v2->second;
		
		edge = new EDGE(v1, v2, false);
		v1->connected_edges.push_back(edge);
		if(v1->name != v2->name)
			v2->connected_edges.push_back(edge);
		elist.push_back(edge);
	}
	
	in_file.close();

	/*for(unsigned int i = 0; i < vlist.size(); ++i) {
		cout << vlist[i]->name << ":" << endl;
		for(unsigned int j = 0; j < vlist[i]->connected_edges.size(); ++j)
			cout << vlist[i]->connected_edges[j]->v1->name << "->" << vlist[i]->connected_edges[j]->v2->name << endl;
	}
	cout << elist.size() << endl;*/
}

bool findOddDegreeNodes()
{
	int ov_index = 0;
	for (unsigned int i = 0; i < vlist.size(); ++i) {
	 	int degree = 0;
		for (unsigned int j = 0; j < vlist[i]->connected_edges.size(); ++j)
			if (vlist[i]->connected_edges[j]->v1->name == vlist[i]->connected_edges[j]->v2->name)
				degree++;
		
		if ((vlist[i]->connected_edges.size() + degree) % 2 != 0) {
			vlist[i]->ov_index = ov_index;
			ov_index++;
			ovlist.push_back(vlist[i]);
		}
	}
	if (ov_index == 0)
		return true;

	return false;
}

void calculateShortestPath()
{
	length_matrix = new int*[ovlist.size()];
	for (unsigned int i = 0; i < ovlist.size(); ++i) {
		length_matrix[i] = new int[ovlist.size()];
		for (unsigned int j = 0; j < ovlist.size(); ++j)
			length_matrix[i][j] = 0;
	}
	for (unsigned int i = 0; i < ovlist.size() - 1; ++i) {
		for (unsigned int j = i + 1; j < ovlist.size(); ++j) {
			vector<string> vertex_path = findShortestPath(ovlist[i]->name, ovlist[j]->name, nm2_head);
			int shortest_path_length = vertex_path.size() - 1;
			length_matrix[i][j] = shortest_path_length;
			length_matrix[j][i] = shortest_path_length;
		}
	}
}

int	minWeightMatching(int w, int start)
{
	int min_w = INT_MAX;

	if (start == ovlist.size() - 2) {
		int weight = length_matrix[ovlist[start]->ov_index][ovlist[start + 1]->ov_index];
		if (w + weight < min_weight) {
			min_weight = w + weight;
			for (unsigned int i = 0; i < ovlist.size(); ++i)
				optimal_list[i] = ovlist[i];
		}
		return weight;
	}

	for (unsigned int i = start + 1; i < ovlist.size(); ++i) {
		int weight = length_matrix[ovlist[start]->ov_index][ovlist[i]->ov_index];
		VERTEX *tmp = ovlist[start + 1];
		ovlist[start + 1] = ovlist[i];
		ovlist[i] = tmp;
		weight += minWeightMatching(w + weight, start + 2);
		tmp = ovlist[start + 1];
		ovlist[start + 1] = ovlist[i];
		ovlist[i] = tmp;
		if (weight < min_w)
			min_w = weight;
	}
	return min_w;
}

void allShortestPaths()
{
	nm2 = new NetworkManager();
	int num_v = 0;
	Vertex *ver0 = nm->get_all_nodes();
	Vertex *ver_head = ver0;

	while (ver0 != NULL) {
		nm2->add_switch(ver0->name);
		num_v++;
		ver0 = ver0->next;
	}
	
	Vertex *ver1 = ver_head;
	Vertex *ver2 = ver1;
	Edge *edge1 = nm->elist;

	while (edge1 != NULL) {
		if (nm2->connected(edge1->head->name, edge1->tail->name) == 1)
			nm2->connect_r(edge1->head->name, edge1->tail->name);
		edge1 = edge1->next;
	}

	Path *path;
	path = new Path();
	path->append(nm2->elist);
	Vertex *v = nm2->get_all_nodes();
	Vertex *v2 = v;
	nm2_head = v;
	Vertex *v_tail;

	while (v2 != NULL) {
		if (v2->next == NULL)
			v_tail = v2;
		v2 = v2->next;
	}

	v2 = v;

	while (v != NULL) {
		while (v2 != NULL) {
			if (v2 == v)
				v2 = v2->next;

			if (v2 != NULL) {
				avail_paths = path->find_paths(v->name, v2->name);
				v2 = v2->next;
				int shorter = 0;
				for (int i = 0; i < avail_paths.size(); i++)
					if (avail_paths[i].size() < avail_paths[shorter].size())
						shorter = i;
				shortpath.push_back(avail_paths[shorter]);
			}
		}
		v = v->next;
		v2 = nm2_head;
	}

	vector<int> val_of_line;
	int count = 0;
	for (int i = 0; i<shortpath.size(); i++) {
		count++;
		val_of_line.push_back(shortpath[i].size());
		if (count == num_v - 1) {
			int pos = (i + 1) / count - 1;
			val_of_line.insert(val_of_line.begin() + pos, 0);
			val_of_shortpath.push_back(val_of_line);
			val_of_line.clear();
			count = 0;
		}
	}
}

vector< string > findShortestPath(string m, string n, Vertex *current)
{
	vector<string> vertex_path;
	int order_m = 0, order_n = 0, count = 0, num = 0;

	while (current != NULL) {
		num++;
		if (n == current->name)
			order_n = count;
		if (m == current->name)
			order_m = count;
		current = current->next;
		count++;
	}
	int order_in_path = 0;

	if (order_m > order_n)
		order_in_path = order_m*(num - 1) + order_n;
	if (order_m < order_n)
		order_in_path = order_m*(num - 1) + order_n - 1;
	vertex_path.push_back(m);
	for (int i = 0; i < shortpath[order_in_path].size(); i++)
		vertex_path.push_back(shortpath[order_in_path][i]->tail->name);

	return vertex_path;
}

void addFakeEdges()
{
	for (unsigned int i = 0; i < elist.size(); ++i)
		elist[i]->fake = false;

	EDGE *extra_edge;
	for(unsigned int i = 0; i < ovlist.size(); i += 2) {
		extra_edge = new EDGE(ovlist[i], ovlist[i + 1], true);
		ovlist[i]->connected_edges.push_back(extra_edge);
		ovlist[i + 1]->connected_edges.push_back(extra_edge);
		elist.push_back(extra_edge);
	}
}

vector< VERTEX * > hierholzer()
{
	VERTEX *top;
	stack< VERTEX * > tmp_path;
	vector< VERTEX * > final_path;

	for (unsigned int i = 0; i < elist.size(); ++i)
		elist[i]->flag = false;

	if (!vlist.empty())
		tmp_path.push(vlist.front());

	while (!tmp_path.empty()) {
		top = tmp_path.top();
		unsigned int i;
		for (i = 0; i < top->connected_edges.size(); ++i) {
			if (!top->connected_edges[i]->flag) {
				top->connected_edges[i]->flag = true;
				if (top->connected_edges[i]->v1->name != top->name)
					tmp_path.push(top->connected_edges[i]->v1);
				else
					tmp_path.push(top->connected_edges[i]->v2);
				break;
			}
		}
		if (i == top->connected_edges.size()) {
			tmp_path.pop();
			final_path.push_back(top);
		}
	}

	return final_path;
}

void tracePath()
{
	ofstream out_file("result.txt", ios::out);
	
	if (!out_file) {
		cout << "The file cannot be opened." << endl;
		exit(1);
	}

	/*for(unsigned int i = 0; i < euler_cycle.size(); ++i)
		cout << euler_cycle[i]->name << " ";

	cout << endl;
	*/
	vector< string > vertex_path;
	for (unsigned int i = 0; i < euler_cycle.size() - 1; ++i) {
		for (unsigned int j = 0; j < euler_cycle[i]->connected_edges.size(); ++j) {
			if (euler_cycle[i]->connected_edges[j]->v1->name == euler_cycle[i + 1]->name ||
				euler_cycle[i]->connected_edges[j]->v2->name == euler_cycle[i + 1]->name) {
				if (!euler_cycle[i]->connected_edges[j]->fake) {
					//cout << " * " << euler_cycle[i]->connected_edges[j]->v1->name << " " << euler_cycle[i]->connected_edges[j]->v2->name << endl;
					cout << euler_cycle[i]->name << "->";
					out_file << euler_cycle[i]->name << "->";
				}
				else {
					//cout << " * " << euler_cycle[i]->connected_edges[j]->v1->name << " " << euler_cycle[i]->connected_edges[j]->v2->name << endl;
					vertex_path = findShortestPath(euler_cycle[i]->name, euler_cycle[i + 1]->name, nm2_head);
					//cout << vertex_path.size() << endl;
					for (unsigned int k = 0; k < vertex_path.size() - 1; ++k) {
						cout << vertex_path[k] << "->";
						out_file << vertex_path[k] << "->";
					}
				}
				break;
			}
		}
	}

	cout << euler_cycle.back()->name << endl;
	out_file << euler_cycle.back()->name << endl;
	out_file.close();
}
