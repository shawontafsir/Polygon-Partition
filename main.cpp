#include <bits/stdc++.h>
#include <windows.h>
#include <glut.h>
#define PI 3.14159265

using namespace std;

struct point{
    int index;
    int helperIndex;
    bool isTaken;
    double x;
    double y;

    bool operator < (const point& rhs) const
    {
        if(y==rhs.y) return (x>rhs.x);
        else return (y < rhs.y);
    }
};

vector<point> glarr;
vector<point> gldiagonal;
vector<vector<point> > gltriangles;
int glarrsize,gldiagonalsize,gltrianglesize;

void drawAxes(){
    glColor3f(0,0,0);
	glBegin(GL_LINES);
    {
        glVertex2f(200*.09, 0*.09);
        glVertex2f(-200*.09, 0*.09);
    }
    glEnd();
    glBegin(GL_LINES);
    {
        glVertex2f(0,200*.09);
        glVertex2f(0,-200*.09);
    }
    glEnd();
}

double ccw(point p2, point p1, point p3)
{
    return (p2.x - p1.x)*(p3.y - p1.y) - (p2.y - p1.y)*(p3.x - p1.x);
}

int findIndex(vector<int> a, int num){
    for(unsigned int i=0;i<a.size();i++){
        if(a[i]==num) return i;
    }
    return 99999;
}

string vertexType(vector<point> arr, int index){
    int V=arr.size();
    double k = ccw(arr[(index-1+V)%V], arr[index], arr[(index+1)%V]);

    if(k<0){
        if(arr[index].y>arr[(index+1)%V].y && arr[index].y>arr[(index-1+V)%V].y) return "start";
        else if(arr[index].y<arr[(index+1)%V].y && arr[index].y<arr[(index-1+V)%V].y) return "end";
    }
    else if(k>0){
        if(arr[index].y>arr[(index+1)%V].y && arr[index].y>arr[(index-1+V)%V].y) return "split";
        else if(arr[index].y<arr[(index+1)%V].y && arr[index].y<arr[(index-1+V)%V].y) return "merge";
    }

    return "regular";
}

vector<point> makeMonotone(priority_queue<point> priorityQ, vector<point> arr, vector<vector<int> > &pieces){
    vector<point> result;
    vector<int> T;
    int V = arr.size();
    int c = 0;
    while(!priorityQ.empty()){
        point p = priorityQ.top();
        priorityQ.pop();
        string s = vertexType(arr, p.index);

        if(s == "start"){

            arr[p.index].helperIndex = p.index;
            int ind=0;

            for(unsigned int i=0;i<T.size();i++){
                point t1 = arr[T[i]];
                point t2 = arr[(T[i]+1)%V];
                double x = (p.y-t1.y)*(t1.x-t2.x)/(t1.y-t2.y) + t1.x;
                if(x>p.x) break;
                ind++;
            }
            T.insert(T.begin()+ind, p.index);
        }
        else if(s == "end"){
            //delete E[i-1]
            point temp = arr[(p.index-1+V)%V];
            if(vertexType(arr, temp.helperIndex) == "merge") {
                    vector<int> piece;
                    int a=0;
                    if(p.index>temp.helperIndex) a=p.index-temp.helperIndex-1;
                    else a=V-temp.helperIndex+p.index;

                    if(a<V-a-c-2){
                        for(int i=temp.helperIndex;i<=p.index;i++){
                            if(arr[i].isTaken==false) piece.push_back(i);
                            if(i!=temp.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                        }
                    }
                    else{
                        for(int i=p.index;i!=(temp.helperIndex+1)%V;i=(i+1)%V){
                            if(arr[i].isTaken==false) piece.push_back(i);
                            if(i!=temp.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                        }
                    }
                    pieces.push_back(piece);

                    result.push_back(arr[p.index]);
                    result.push_back(arr[temp.helperIndex]);
            }

            for(unsigned int i=0;i<T.size();i++){
                if(temp.index == T[i]){
                    T.erase(T.begin()+i);
                    break;
                }
            }
        }
        else if(s == "split"){
            //update E[j]
            for(int i=T.size()-1;i>=0;i--){
                point t1 = arr[T[i]];
                point t2 = arr[(T[i]+1)%V];

                if(t1.x<p.x && t2.x<p.x && t1.y>=p.y && t2.y<p.y){
                    vector<int> piece;
                    int a=0;
                    if(p.index>t1.helperIndex) a=p.index-t1.helperIndex-1;
                    else a=V-t1.helperIndex+p.index;

                    if(a<V-a-c-2){
                        for(int i=t1.helperIndex;i<=p.index;i++){
                            if(arr[i].isTaken==false) piece.push_back(i);
                            if(i!=t1.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                        }
                    }
                    else{
                        for(int i=p.index;i!=(t1.helperIndex+1)%V;i=(i+1)%V){
                            if(arr[i].isTaken==false) piece.push_back(i);
                            if(i!=t1.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                        }
                    }
                    pieces.push_back(piece);

                    result.push_back(arr[p.index]);
                    result.push_back(arr[t1.helperIndex]);

                    arr[T[i]].helperIndex = p.index;
                    break;
                }
            }

            //insert E[i]
            arr[p.index].helperIndex = p.index;
            int ind=0;
            for(unsigned int i=0;i<T.size();i++){
                point t1 = arr[T[i]];
                point t2 = arr[(T[i]+1)%V];
                double x = (p.y-t1.y)*(t1.x-t2.x)/(t1.y-t2.y) + t1.x;
                if(x>p.x) break;
                ind++;
            }
            T.insert(T.begin()+ind, p.index);
        }
        else if(s == "merge"){
            //UPdate E[j]
            for(int i=T.size()-1;i>=0;i--){
                point t1 = arr[T[i]];
                point t2 = arr[(T[i]+1)%V];

                if(t1.x<p.x && t2.x<p.x && t1.y>=p.y && p.y>t2.y){
                    if(vertexType(arr,t1.helperIndex) == "merge"){
                        vector<int> piece;
                        int a=0;
                        if(p.index>t1.helperIndex) a=p.index-t1.helperIndex-1;
                        else a=V-t1.helperIndex+p.index;

                        if(a<V-a-c-2){
                            for(int i=t1.helperIndex;i<=p.index;i++){
                                if(arr[i].isTaken==false) piece.push_back(i);
                                if(i!=t1.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                            }
                        }
                        else{
                            for(int i=p.index;i!=(t1.helperIndex+1)%V;i=(i+1)%V){
                                if(arr[i].isTaken==false) piece.push_back(i);
                                if(i!=t1.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                            }
                        }
                        pieces.push_back(piece);

                        result.push_back(arr[p.index]);
                        result.push_back(arr[t1.helperIndex]);
                    }

                    arr[T[i]].helperIndex = p.index;
                    break;
                }
            }

            //delete E[i-1]
            point temp = arr[(p.index-1+V)%V];
            if(vertexType(arr, temp.helperIndex) == "merge"){
                vector<int> piece;
                int a=0;
                if(p.index>temp.helperIndex) a=p.index-temp.helperIndex-1;
                else a=V-temp.helperIndex+p.index;

                if(a<V-a-c-2){
                    for(int i=temp.helperIndex;i<=p.index;i++){
                        if(arr[i].isTaken==false) piece.push_back(i);
                        if(i!=temp.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                    }
                }
                else{

                    for(int i=p.index;i!=(temp.helperIndex+1)%V;i=(i+1)%V){
                        if(arr[i].isTaken==false) piece.push_back(i);
                        if(i!=temp.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                    }
                }
                pieces.push_back(piece);

                result.push_back(arr[p.index]);
                result.push_back(arr[temp.helperIndex]);
            }

            for(unsigned int i=0;i<T.size();i++){
                if(temp.index == T[i]){
                    T.erase(T.begin()+i);
                    break;
                }
            }

        }
        else if(s == "regular"){
            point t1 = arr[(p.index-1+V)%V];
            point t2 = arr[(p.index+1)%V];
            if(t1.y-t2.y >= 0){
                //delete E[i-1]
                if(vertexType(arr, t1.helperIndex) == "merge") {
                    vector<int> piece;
                    int a=0;
                    if(p.index>t1.helperIndex) a=p.index-t1.helperIndex-1;
                    else a=V-t1.helperIndex+p.index;

                    if(a<V-a-c-2){
                        for(int i=t1.helperIndex;i<=p.index;i++){
                            if(arr[i].isTaken==false) piece.push_back(i);
                            if(i!=t1.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                        }
                    }
                    else{
                        for(int i=p.index;i!=(t1.helperIndex+1)%V;i=(i+1)%V){
                            if(arr[i].isTaken==false) piece.push_back(i);
                            if(i!=t1.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                        }
                    }
                    pieces.push_back(piece);

                    result.push_back(arr[p.index]);
                    result.push_back(arr[t1.helperIndex]);
                }

                for(unsigned int i=0;i<T.size();i++){
                    if(t1.index == T[i]){
                        T.erase(T.begin()+i);
                        break;
                    }
                }

                //insert E[i]
                arr[p.index].helperIndex = p.index;
                int ind=0;
                for(unsigned int i=0;i<T.size();i++){
                    point t3 = arr[T[i]];
                    point t4 = arr[(T[i]+1)%V];
                    double x = (p.y-t3.y)*(t3.x-t4.x)/(t3.y-t4.y) + t3.x;
                    if(x>p.x) break;
                    ind++;
                }
                T.insert(T.begin()+ind, p.index);
            }
            else{
                //UPdate E[j]
                for(int i=T.size()-1;i>=0;i--){
                    point t1 = arr[T[i]];
                    point t2 = arr[(T[i]+1)%V];

                    if(t1.x<p.x && t2.x<p.x && t1.y>=p.y && p.y>t2.y){
                        if(vertexType(arr,t1.helperIndex) == "merge"){
                            vector<int> piece;
                            int a=0;
                            if(p.index>t1.helperIndex) a=p.index-t1.helperIndex-1;
                            else a=V-t1.helperIndex+p.index;

                            if(a<V-a-c-2){
                                for(int i=t1.helperIndex;i<=p.index;i++){
                                    if(arr[i].isTaken==false) piece.push_back(i);
                                    if(i!=t1.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                                }
                            }
                            else{
                                for(int i=p.index;i!=(t1.helperIndex+1)%V;i=(i+1)%V){
                                    if(arr[i].isTaken==false) piece.push_back(i);
                                    if(i!=t1.helperIndex && i!=p.index) {arr[i].isTaken = true;c++;}
                                }
                            }
                            pieces.push_back(piece);

                            result.push_back(arr[p.index]);
                            result.push_back(arr[t1.helperIndex]);
                        }

                        arr[T[i]].helperIndex = p.index;
                        break;
                    }
                }
            }
        }
    }
    vector<int> piece;
    for(int i=0;i<V;i++){
        if(arr[i].isTaken==false) piece.push_back(i);
    }
    pieces.push_back(piece);

    return result;
}

vector<point> makeTriangle(vector<point> arr, vector<int> piece){
    priority_queue<point> Q;
    vector<point> S;
    vector<point> result;
    int V = piece.size();
    for(unsigned int i=0;i<piece.size();i++) Q.push(arr[piece[i]]);

    S.push_back(Q.top()); Q.pop();
    S.push_back(Q.top()); Q.pop();

    while(Q.size()>1){
        point p = Q.top();
        Q.pop();

        int ind = findIndex(piece, p.index);
        point t1 = arr[piece[(ind-1+V)%V]];
        point t2 = arr[piece[(ind+1)%V]];
        int ind2 = findIndex(piece, S[S.size()-1].index);
        point t3 = arr[piece[(ind2-1+V)%V]];
        point t4 = arr[piece[(ind2+1)%V]];

        if((t1.y-t2.y>0 && t3.y-t4.y>0) || (t1.y-t2.y<=0 && t3.y-t4.y<=0)){
            while(true){
                if(t1.y-t2.y>0 && t3.y-t4.y>0){
                    if(ccw(S[S.size()-2], S[S.size()-1], p)>=0) break;
                }
                else if(t1.y-t2.y<=0 && t3.y-t4.y<=0){
                    if(ccw(p, S[S.size()-1], S[S.size()-2])>=0) break;
                }

                result.push_back(p);
                result.push_back(S[S.size()-2]);
                S.pop_back();
                if(S.size()==1) break;
            }
            S.push_back(p);
        }
        else{
            point temp = S[S.size()-1];
            while(true){
                point t = S[S.size()-1];
                S.pop_back();
                if(S.empty()) break;
                result.push_back(p);
                result.push_back(t);
            }
            S.push_back(temp);
            S.push_back(p);
        }
    }

    S.pop_back();
    while(S.size()>1){
        point t = S[S.size()-1];
        S.pop_back();

        result.push_back(Q.top());
        result.push_back(t);
    }

    return result;
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1,1,1,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	drawAxes();

	glPointSize(5);

	for(int i=0;i<glarrsize;i++){
        glColor3f(1,0,0);

        glBegin(GL_POINTS);
        {
            glVertex2f(glarr[i].x*.09, glarr[i].y*.09);
        }
        glEnd();

        glBegin(GL_LINES);
        {
            glVertex2f(glarr[i%glarrsize].x*.09, glarr[i%glarrsize].y*.09);
            glVertex2f(glarr[(i+1)%glarrsize].x*.09, glarr[(i+1)%glarrsize].y*.09);
        }
        glEnd();
    }

    for(int i=0;i<gldiagonalsize;i+=2){
        glColor3f(0,0,1);
        glBegin(GL_LINES);
        {
            glVertex2f(gldiagonal[i].x*.09, gldiagonal[i].y*.09);
            glVertex2f(gldiagonal[i+1].x*.09, gldiagonal[i+1].y*.09);
        }
        glEnd();
    }
    for(int i=0;i<gltrianglesize;i++){
        glColor3f(0,0,1);
        for(unsigned int j=0;j<gltriangles[i].size();j+=2){
            glBegin(GL_LINES);
            {
                glVertex2f(gltriangles[i][j].x*.09, gltriangles[i][j].y*.09);
                glVertex2f(gltriangles[i][j+1].x*.09, gltriangles[i][j+1].y*.09);
            }
            glEnd();
        }
    }

	glutSwapBuffers();
}

int main(int argc, char **argv)
{
    int V;
    vector<point> arr;
    vector<vector<int> > pieces;
    priority_queue<point> priorityQ;
    ifstream in("C:\\Users\\USER\\Favorites\\Desktop\\1305072_PolygonPartition\\1305072_input2.txt");
    if (in.is_open())
    {
        string line;
        getline(in,line);
        V=atoi(line.c_str());
        int i=0;
        while(getline(in,line))
        {
            stringstream ss(line);
            string s;
            point p;
            ss>>s;
            p.x = atof(s.c_str());
            ss>>s;
            p.y = atof(s.c_str());
            p.index = i;
            p.isTaken = false;

            arr.push_back(p);
            priorityQ.push(p);
            i++;
        }
        in.close();
    }
    for(unsigned int j=0;j<arr.size();j++)
        cout<<arr[j].x<<" "<<arr[j].y<<endl;
    cout<<"type12----"<<vertexType(arr, 12)<<endl;

    vector<point> result = makeMonotone(priorityQ, arr, pieces);

    for(unsigned int i=0;i<pieces.size();i++){
            cout<<i<<"---->";
        for(unsigned int j=0;j<pieces[i].size();j++) cout<<pieces[i][j]<<" ";
        cout<<endl;
    }

    vector<vector<point> > triangles;
    for(unsigned int i=0;i<pieces.size();i++)
        triangles.push_back(makeTriangle(arr, pieces[i]));

    for(unsigned int i=0;i<triangles.size();i++){
            cout<<i<<"---->"<<endl;
        for(unsigned int j=0;j<triangles[i].size();j+=2) cout<<triangles[i][j].index<<" "<<triangles[i][j+1].index<<endl;
        cout<<endl;
    }

    glarr = arr;
    glarrsize = V;
    gldiagonal = result;
    gldiagonalsize = result.size();
    gltriangles = triangles;
    gltrianglesize = 0;//triangles.size();

    glutInit(&argc,argv);
	glutInitWindowSize(600, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("My OpenGL Program");
	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(display);
	glutMainLoop();

    return 0;
}
