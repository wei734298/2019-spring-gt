# 中國郵差問題

=====無向圖演算法=====

1. 先判斷整張圖是否為一個強連通分量，否則無解。
2. 找出圖上所有奇點，一定是偶數個。
3. 找出所有奇點點對之間的最短路徑長度。
4. 把這些奇點做最小權匹配，權重採用剛才算的最短路徑長度。
5. 把匹配邊加在原圖上，再找歐拉環，即得中國郵差路徑之權重。
6. 將匹配邊改成其代表的最短路徑，即得中國郵差路徑。

=====資料結構=====

1. VERTEX : 紀錄Graph裡的vertex資訊，包括name及連接的所有edges。
2. EDGE : 紀錄Graph裡的edge資訊，包括edge的head及tail。

=====程式碼解釋=====

1. void readFile(const char *) : 讀取testbench檔案，並將topology data儲存成network。
2. bool findOddDegreeNodes() : 找出圖上所有的奇點，並儲存在ovlist的vector裡。
3. void calculateShortestPath() : 計算出所有奇點點對之間的最短路徑長度。
4. void allShortestPaths() : 計算所有點到點的最短路徑長度及路徑。
5. vector<string> findShortestPath(string, string, Vertex *) : 回傳點和點之間的最短路徑。
6. int	minWeightMatching(int, int) : 將奇點做最小權重匹配，權重是最短路徑的長度。
7. void addFakeEdges() : 將匹配的edge加在graph上。
8. vector<VERTEX *> hierholzer() : 執行hierholzer's algorithm來找歐拉環。
9. void tracePath() : 將歐拉環的路徑打印出來，並輸出result檔。

=====程式操作說明=====
1. 先執行 make 指令。
	make
2. 執行執行檔。
	./CP_N26074914 <測資>
3. 產生"result.txt"輸出檔。
        a->c->d->f->e->f->g->d->c->b->c->b->a
