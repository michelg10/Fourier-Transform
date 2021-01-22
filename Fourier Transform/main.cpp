#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;
typedef long long ll;
typedef long double ld;
class pt {
public:
    ld x,y;
    pt() { };
    pt(ld const dx,ld const dy) {
        x=dx;
        y=dy;
    }
    pt operator +(const pt &dis) {
        pt rturn=*this;
        rturn.x+=dis.x;
        rturn.y+=dis.y;
        return rturn;
    }
    pt operator *(const ld d) {
        pt rturn=*this;
        rturn.x*=d;
        rturn.y*=d;
        return rturn;
    }
};
inline pt operator *(const ld d,const pt dis) {
    pt rturn=dis;
    rturn.x*=d;
    rturn.y*=d;
    return rturn;
}
ll classifyChar(char c) {
    if (c>='0'&&c<='9') return 1;
    if (c>='A'&&c<='Z') return c+100;
    if (c>='a'&&c<='z') return c+100; //just return something random idc
    if (c=='.'||c=='-') return 1;
    return 0;
}
string nxtToken(string str,ll &ptr) {
    ll iniPos=ptr;
    char initial=str[ptr]; //groups: letters, space, numbers
    for (;ptr<str.size()&&(classifyChar(initial)==classifyChar(str[ptr]));ptr++);
    return str.substr(iniPos,ptr-iniPos);
}
ld toLd(string s) {
    return atof(s.c_str());
}
class cmpx {
public:
    ld real,im;
    ld mag() {
        return real*real+im*im;
    }
    cmpx() { };
    cmpx(ld r,ld i) {
        real=r;
        im=i;
    }
    cmpx operator *(const cmpx &dis) {
        return cmpx(this->real*dis.real-this->im*dis.im,this->real*dis.im+this->im*dis.real);
    }
    cmpx operator *(const ld d) {
        return cmpx(this->real*d,this->im*d);
    }
    cmpx operator +(const cmpx &dis) {
        cmpx rturn=*this;
        rturn.im+=dis.im;
        rturn.real+=dis.real;
        return rturn;
    }
    cmpx operator +(const ld d) {
        return cmpx(this->real+d,this->im);
    }
    void operator +=(const cmpx &dis) {
        real+=dis.real;
        im+=dis.im;
    }
};
inline cmpx epow(ld n) {
    return cmpx(cos(n),sin(n));
}
// ### Parameters ###
ll const ctrlp=5000; //up this to 20000 in production
ll const curveQuant=5000; //200000
ll const dirCirc=30;
ll const dispLim=10;
// ### Parameters ###
/*
inline ld logistic(ld x) {
    return 1.0/(1.0+exp(-spdRmp*(x)));
}
inline ld splgr(ld x) {
    if (spdRmp==0) {
        return x;
    }
    return (logistic(x-0.5)-logistic(-0.5))*(1.0/(logistic(0.5)-logistic(-0.5)));
}
// find inverse of a function
ld* findInv(ld (*numF)(ld),vector<ld>lst,ld l,ld r,ld tol=0.001) { //find x values of y values in lst for monotonically increasing func numf
    ld *res=new ld[lst.size()];
    ld nxtl=l;
    for (ll i=0;i<lst.size();i++) {
        ld sl=nxtl,sr=r;
        while (sr-sl>tol) {
            ld mid=(sl+sr)/2;
            if (numF(mid)>=lst[i]) { //mid is too much
                sr=mid;
            } else {
                sl=mid;
            }
        }
        nxtl=l;
        res[i]=(sl+sr)/2;
    }
    return res;
}
 ld integ(ld (*numF)(ld),ld l,ld r,ll keyp) {
     if (keyp%2) cerr<<"Integration must be done with an even number!"<<endl;
     ld rturn=numF(l);
     for (ll i=1;i<keyp;i++) {
         rturn+=(i%2?4:2)*numF(l+(r-l)*((ld)i/keyp));
     }
     rturn+=numF(r);
     rturn*=((r-l)/keyp)/3.0;
     return rturn;
 }
 */ //Code Graveyard
inline ld dst(pt x,pt y) {
    return sqrt((x.x-y.x)*(x.x-y.x)+(x.y-y.y)*(x.y-y.y));
}
bool eq(ld x,ld y) {
    return abs(x-y)<=0.0000001;
}
cmpx *cfPass;
bool effCmp(const ll &a,const ll &b) {
    return cfPass[a].mag()>cfPass[b].mag();
}
int main() {
    cout<<"Parameters"<<endl<<"Control Points:"<<ctrlp<<endl<<"Curve Quantization Points:"<<curveQuant<<endl<<"Circle range for every curve:"<<-dirCirc<<"..."<<dirCirc<<endl<<"Circle display:"<<dispLim<<" circles"<<endl;
    chrono::steady_clock::time_point grs=chrono::steady_clock::now();
    ifstream in("Swift.txt");
    string name=to_string(time(0));
    ofstream pathF(name+"-path.txt");
    ofstream circF(name+"-circ.txt");
    if (!in.good()) cerr<<"File error!"<<endl;
    string shape;
    getline(in,shape);
    in.close();
    pt cur(0,0);
    pt nul(0,0);
    ll ptr=0;
    //MARK: Load from svg
    pt lst(0,0);
    
    string lstMd="";
    pt lstMem(0,0);
    ld ttlCurLen=0;
    struct ele {
        char md;
        pt arg1,arg2,arg3,arg4;
        ld len;
    };
    vector<vector<ele>>grph;
    grph.push_back(vector<ele>());
    vector<ld>curveLen;
    ll curves=0;
    bool first=true;
    while (ptr!=shape.size()) {
        //supported modes: Q,q
        //does not support: T,A,a
        string md=nxtToken(shape,ptr);
        if (md=="M"||md=="m") {
            pt tmp;
            //MARK: Jump
            tmp.x=toLd(nxtToken(shape,ptr));
            if (nxtToken(shape,ptr)!=",") cerr<<"unexpected error while parsing (1)"<<endl;
            tmp.y=toLd(nxtToken(shape,ptr));
            cur=(md=="M"?tmp:tmp+cur);
            if (grph[grph.size()-1].size()!=0) {
                grph.push_back(vector<ele>());
            }
            lst=cur;
        } else if (md=="L"||md=="l"||md=="Z"||md=="z"||md=="H"||md=="h"||md=="V"||md=="v") { //draw a line
            pt toPt;
            if (md=="L"||md=="l") {
                ld ix,iy;
                ix=toLd(nxtToken(shape,ptr));
                if (nxtToken(shape,ptr)!=",") cerr<<"unexpected error while parsing (3)"<<endl;
                iy=toLd(nxtToken(shape,ptr));
                toPt=(md=="L"?pt(ix, iy):cur+pt(ix, iy));
            }
            if (md=="Z"||md=="z") toPt=lst;
            if (md=="H"||md=="h") {
                //draw a horizontal line to point x
                ld ix;
                ix=toLd(nxtToken(shape,ptr));
                toPt=(md=="H"?pt(ix,cur.y):cur+pt(ix,0));
            }
            if (md=="V"||md=="v") {
                //draw a horizontal line to point x
                ld iy;
                iy=toLd(nxtToken(shape,ptr));
                toPt=(md=="V"?pt(cur.x,iy):cur+pt(0,iy));
            }
            if (eq(cur.x,toPt.x)&&eq(cur.y,toPt.y)) continue;
            // ----- DEAL WITH IT -----
            grph[grph.size()-1].push_back((ele){'l',cur,toPt,nul,nul,dst(cur,toPt)});
            // ----- DEAL WITH IT -----
            
            cur=toPt;
        } else if (md=="C"||md=="c"||md=="S"||md=="s") {
            pt ctrl1,ctrl2,endpt;
            ld ix,iy;
            if (md=="C"||md=="c") {
                ix=toLd(nxtToken(shape,ptr));
                if (nxtToken(shape,ptr)!=",") cerr<<"unexpected error while parsing (4)"<<endl;
                iy=toLd(nxtToken(shape,ptr));
                ctrl1=((md=="C"||md=="S")?pt(ix, iy):cur+pt(ix, iy));
                
                if (nxtToken(shape,ptr)!=" ") cerr<<"unexpected error while parsing (5)"<<endl;
            } else {
                //reflect the last ctrl2 across the current point to be the current ctrl1
                if (lstMd=="S"||lstMd=="s"||lstMd=="C"||lstMd=="c") {
                    ctrl1=pt(cur.x+cur.x-lstMem.x,cur.y+cur.y-lstMem.y);
                } else {
                    ctrl1=cur;
                }
            }
            ix=toLd(nxtToken(shape,ptr));
            if (nxtToken(shape,ptr)!=",") cerr<<"unexpected error while parsing (6)"<<endl;
            iy=toLd(nxtToken(shape,ptr));
            ctrl2=((md=="C"||md=="S")?pt(ix, iy):cur+pt(ix, iy));
            
            if (nxtToken(shape,ptr)!=" ") cerr<<"unexpected error while parsing (7)"<<endl;
            
            ix=toLd(nxtToken(shape,ptr));
            if (nxtToken(shape,ptr)!=",") cerr<<"unexpected error while parsing (8)"<<endl;
            iy=toLd(nxtToken(shape,ptr));
            endpt=((md=="C"||md=="S")?pt(ix, iy):cur+pt(ix, iy));
            // ----- DEAL WITH IT -----
            
//            for (ld i=0;i<1;i+=0.1) {
//                pt got=pow(1-i,3)*cur+3*pow(1-i,2)*i*ctrl1+3*(1-i)*i*i*ctrl2+pow(i,3)*endpt;
//                cout<<"("<<got.x<<','<<0-got.y<<")"<<endl;
//            }
            pt lst=pow(1-0,3)*cur+pow(0,3)*endpt;
            ld disLen=0;
            for (ld k=0;k<=1;k+=0.001) {
                pt got=pow(1-k,3)*cur+3*pow(1-k,2)*k*ctrl1+3*(1-k)*k*k*ctrl2+pow(k,3)*endpt;
                disLen+=dst(got,lst);
                lst=got;
            }
            curveLen.push_back(disLen);
            ttlCurLen+=disLen;
            grph[grph.size()-1].push_back((ele){'c',cur,ctrl1,ctrl2,endpt,disLen});
            curves++;
            // ----- DEAL WITH IT -----
            
            cur=endpt;
            lstMem=ctrl2;
        } else {
            cerr<<"Unsupported mode!"<<endl;
        }
        lstMd=md;
    }
    ll qntPt[curves];
    for (ll i=0;i<curves;i++) {
        qntPt[i]=curveQuant*curveLen[i]/ttlCurLen;
    }
    curveLen.clear();
    cout<<grph.size()<<" discrete areas, "<<ctrlp<<" control points each."<<endl<<"Curves:"<<curves<<endl;
    //the point of this is to find areas where len might match up to provide control points
    ll curCurve=0;
    for (ll i=0;i<grph.size();i++) {
        ll begcurCurve=curCurve; //just in case i want to access number of qpoints after calculations
        vector<pt *>qntz;
        vector<ld *>dsts;
        ld curveNugLen=0;
        for (ll j=0;j<grph[i].size();j++) {
            if (grph[i][j].md=='c') {
                pt *pts=new pt[qntPt[curCurve]+1];
                ld *ptdst=new ld[qntPt[curCurve]]; //ptdst[i] is the distance between point i and i+1
                ele current=grph[i][j];
                pt lst=pow(1-0,3)*current.arg1+pow(0,3)*current.arg4;
                pts[0]=lst;
                ld k=0;
                //each curve is [ )
                for (ll cnt=1;cnt<=qntPt[curCurve];cnt++,k=(ld)cnt/qntPt[curCurve]) {
                    pt got=pow(1-k,3)*current.arg1+3*pow(1-k,2)*k*current.arg2+3*(1-k)*k*k*current.arg3+pow(k,3)*current.arg4;
                    pts[cnt]=got;
                    ptdst[cnt-1]=dst(lst,got);
                    lst=got;
                    curveNugLen+=ptdst[cnt-1];
                }
//                for (ll cnt=0;cnt<qntPt[curCurve];cnt++) {
//                    cout<<"("<<pts[cnt].x<<','<<pts[cnt].y<<")"<<endl;
//                }
                curCurve++;
                qntz.push_back(pts);
                dsts.push_back(ptdst);
            } else if (grph[i][j].md=='l') {
                curveNugLen+=dst(grph[i][j].arg1,grph[i][j].arg2);
            }
        }
        //always use curveNugLen divided by ctrlp
        pt fnctrl[ctrlp+1];
        fnctrl[0]=grph[i][0].arg1;
        fnctrl[ctrlp]=(grph[i][grph[i].size()-1].md=='l'?grph[i][grph[i].size()-1].arg2:grph[i][grph[i].size()-1].arg4);
        ll curLookFor=1;
        ld cumDst=0;
        ll curveOn=0;
        for (ll j=0;j<grph[i].size()&&curLookFor<ctrlp;j++) {
            if (grph[i][j].md=='c') {
                for (ll k=0;k<qntPt[begcurCurve+curveOn];k++) {
                    if (cumDst+dsts[curveOn][k]>curLookFor*curveNugLen/ctrlp) {
                        //extrapolate
                        pt extrpl1=qntz[curveOn][k];
                        pt extrpl2=qntz[curveOn][k+1];
                        ld scale=(curLookFor*curveNugLen/ctrlp-cumDst)/dst(extrpl1,extrpl2);
                        fnctrl[curLookFor]=extrpl1+pt((extrpl2.x-extrpl1.x)*scale,(extrpl2.y-extrpl1.y)*scale);
                        curLookFor++;
                        k--;
                        continue;
                    }
                    cumDst+=dsts[curveOn][k];
                }
                curveOn++;
            } else if (grph[i][j].md=='l') {
                if (cumDst+dst(grph[i][j].arg1,grph[i][j].arg2)>curLookFor*curveNugLen/ctrlp) {
                    ld scale=(curLookFor*curveNugLen/ctrlp-cumDst)/dst(grph[i][j].arg1,grph[i][j].arg2);
                    fnctrl[curLookFor]=grph[i][j].arg1+pt((grph[i][j].arg2.x-grph[i][j].arg1.x)*scale,(grph[i][j].arg2.y-grph[i][j].arg1.y)*scale);
                    curLookFor++;
                    j--;
                    continue;
                }
                cumDst+=dst(grph[i][j].arg1,grph[i][j].arg2);
            }
        }
        if (curLookFor!=ctrlp) cout<<"Wtf"<<endl;
        for (ll j=0;j<=ctrlp;j++) fnctrl[j].y*=-1; //weird svg coords lmao
//        for (ll j=0;j<=ctrlp;j++) {
//            cout<<fixed<<"("<<fnctrl[j].x<<","<<fnctrl[j].y<<")"<<endl;
//        }
//        cin.get();
        //control points: function relationship from (0...ctrlp
        cmpx coef[2*dirCirc+1];
        for (ll j=0;j<2*dirCirc+1;j++) coef[j]=cmpx(0,0);
        for (ll j=0;j<ctrlp;j++) {
            cmpx drta=cmpx(fnctrl[j].x,fnctrl[j].y);
            for (ll k=-dirCirc;k<=dirCirc;k++) {
                coef[k+dirCirc]+=epow(-2*M_PI*k*(j/(ld)ctrlp))*drta*(1/(ld)ctrlp);
            }
        }
//        ll show=500;
//        for (ll j=0;j<show;j++) {
//            ld curX=j/(ld)show;
//            cmpx fin(0,0);
//            for (ll k=-dirCirc;k<=dirCirc;k++) {
//                fin+=coef[k+dirCirc]*epow(2*M_PI*k*curX);
//            }
//            cout<<"("<<fixed<<fin.real<<','<<fin.im<<")"<<endl;
//        }
        // ----- outputting paths -----
        if (first) {
            first=false;
        } else pathF<<endl;
        pathF<<"(";
        bool virgin=true;
        for (ll k=-dirCirc;k<=dirCirc;k++) {
            if (virgin) {
                virgin=false;
            } else pathF<<"+";
            pathF<<fixed<<coef[k+dirCirc].real<<"cos("<<2*M_PI*k<<"t)-"<<coef[k+dirCirc].im<<"sin("<<2*M_PI*k<<"t)";
        }
        pathF<<",";
        virgin=true;
        for (ll k=-dirCirc;k<=dirCirc;k++) {
            if (virgin) {
                virgin=false;
            } else pathF<<"+";
            pathF<<fixed<<coef[k+dirCirc].real<<"sin("<<2*M_PI*k<<"t)+"<<coef[k+dirCirc].im<<"cos("<<2*M_PI*k<<"t)";
        }
        pathF<<")";
        
        pathF<<endl<<"(";
        virgin=true;
        for (ll k=-dirCirc;k<=dirCirc;k++) {
            if (virgin) {
                virgin=false;
            } else pathF<<"+";
            pathF<<fixed<<coef[k+dirCirc].real<<"cos("<<2*M_PI*k<<"a)-"<<coef[k+dirCirc].im<<"sin("<<2*M_PI*k<<"a)";
        }
        pathF<<",";
        virgin=true;
        for (ll k=-dirCirc;k<=dirCirc;k++) {
            if (virgin) {
                virgin=false;
            } else pathF<<"+";
            pathF<<fixed<<coef[k+dirCirc].real<<"sin("<<2*M_PI*k<<"a)+"<<coef[k+dirCirc].im<<"cos("<<2*M_PI*k<<"a)";
        }
        pathF<<")";
        // ----- outputting paths -----
        // ----- outputting circs -----
        ll circEff[dirCirc*2+1];
        for (ll j=0;j<dirCirc*2+1;j++) circEff[j]=j;
        cfPass=coef;
        sort(circEff,circEff+dirCirc*2+1,effCmp);
        stringstream curX,curY;
        for (ll k=0;k<(dispLim==-1?2*dirCirc+1:dispLim);k++) {
            if (curX.str().size()==0) {
                circF<<"x^2+y^2="<<fixed<<coef[circEff[k]].mag()<<endl;
            } else {
                circF<<"(x-("<<curX.str()<<"))^2+(y-("<<curY.str()<<"))^2="<<fixed<<coef[circEff[k]].mag()<<endl;
                curX<<"+";
                curY<<"+";
            }
            curX<<fixed<<coef[circEff[k]].real<<"cos("<<2*M_PI*(circEff[k]-dirCirc)<<"a)-"<<coef[circEff[k]].im<<"sin("<<2*M_PI*(circEff[k]-dirCirc)<<"a)";
            curY<<fixed<<coef[circEff[k]].real<<"sin("<<2*M_PI*(circEff[k]-dirCirc)<<"a)+"<<coef[circEff[k]].im<<"cos("<<2*M_PI*(circEff[k]-dirCirc)<<"a)";
        }
    }
    pathF.close();
    circF.close();
    system(("open "+name+"-path.txt").c_str());
    system(("open "+name+"-circ.txt").c_str());
    cout<<"Program finished in "<<chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - grs).count()<<" ms"<<endl;
}

//feature: speed ramp?
//speed is a logistic curve
//inverse of a logistic curve
//feature: multithreading
//TODO: integrate with simpson's rule instead?
