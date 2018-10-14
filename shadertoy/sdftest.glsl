// the base scene is from iq  //https://www.shadertoy.com/view/Xds3zN
// i just want to test SDF functions
// jiepengtan@gmail.com

#define ITERATIONS 4

#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)
#define HASHSCALE4 vec4(.1031, .1030, .0973, .1099)


//----------------------------------------------------------------------------------------
//  1 out, 2 in...
float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}


#define AA 4   // make this 1 is your machine is too slow

//------------------------------------------------------------------

float sdPlane( vec3 p )
{
	return p.y;
}

float sdSphere( vec3 p, float s )
{
    return length(p)-s;
}

float sdBox( vec3 p, vec3 b )
{
    vec3 d = abs(p) - b;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}


float udRoundBox( vec3 p, vec3 b, float r )
{
    return length(max(abs(p)-b,0.0))-r;
}

float sdTorus( vec3 p, vec2 t )
{
    return length( vec2(length(p.xz)-t.x,p.y) )-t.y;
}



float sdCylinder( vec3 p, vec2 h )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}


//------------------------------------------------------------------

float opS( float d1, float d2 )
{
    return max(-d2,d1);
}

vec2 opU( vec2 d1, vec2 d2 )
{
	return (d1.x<d2.x) ? d1 : d2;
}

vec3 opRep( vec3 p, vec3 c )
{
    return mod(p,c)-0.5*c;
}

vec2 opRep( vec2 p, vec2 c )
{
    return mod(p,c)-0.5*c;
}

vec3 opTwist( vec3 p )
{
    float  c = cos(10.0*p.y+10.0);
    float  s = sin(10.0*p.y+10.0);
    mat2   m = mat2(c,-s,s,c);
    return vec3(m*p.xz,p.y);
}
vec3 rX(const in vec3 v, const in float cs, const in float sn) {return mat3(1.0,0.0,0.0,0.0,cs,sn,0.0,-sn,cs)*v;}
vec3 rY(const in vec3 v, const in float cs, const in float sn) {return mat3(cs,0.0,-sn,0.0,1.0,0.0,sn,0.0,cs)*v;}
vec3 rZ(const in vec3 v, const in float cs, const in float sn) {return mat3(cs,sn,0.0,-sn,cs,0.0,0.0,0.0,1.0)*v;}

float smin( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

#define clamp01(a) clamp(a,0.0,1.0)
#define opS(d1,d2) max(-d1,d2)
// union 2 objects carrying material info
#define opU(a,b) ((a.x < b.x) ? a : b)

#define PI 3.14159

// implicitly specifies shading rules
#define WOOD_MAT 0.
#define STRAW_MAT 1.
#define VILLAGE_MAT 2.
float sdCappedCone( in vec3 p, in vec3 c ) // c=vec3(bottom radius, angle , height)
{
    vec2 q = vec2( length(p.xz), p.y );
    vec2 v = vec2( c.z*c.y/c.x, -c.z );
    vec2 w = v - q;
    vec2 vv = vec2( dot(v,v), v.x*v.x );
    vec2 qv = vec2( dot(v,w), v.x*w.x );
    vec2 d = max(qv,0.0)*qv/vv;
    return sqrt( dot(w,w) - max(d.x,d.y) )* sign(max(q.y*v.x-q.x*v.y,w.y));
}



float sdBounceBalls(vec3 pos){
    float SIZE = 2.;
    vec2 gridSize = vec2(SIZE,SIZE);
    float rv = hash12( floor((pos.xz) / gridSize));
    pos.xz = opRep(pos.xz,gridSize);
    float bollSize = 0.1;
    float bounceH = 1.;
    return sdSphere(pos- vec3(0.,(bollSize+bounceH+sin(iTime*3.14 + rv*6.24)*bounceH),0.),bollSize);
}

float sdBoatHull( vec3 vPos )
{
    vPos.y-=0.4;
    vec3 vBoatDomain = vPos;
    vBoatDomain.x = abs(vBoatDomain.x);
    
    float boatDist = length( vBoatDomain.xyz - vec3(-3.9,3.2,0.)) - 5.6;
	
    float bs = 14.5;
    float gap = 0.3;
    vec2 offset=vec2(0.3,bs+gap);
    float cylinder= length(vBoatDomain.xy-offset)-bs;
    boatDist=opS(cylinder,boatDist);
    float sSize = 15.0;
    float sOff = 0.05;
    float sphere = length( vBoatDomain.xyz - vec3(0,sSize+sOff,0)) - sSize;
    boatDist=opS(sphere,boatDist);
    
    float bx = vPos.y+0.4;
    boatDist=opS(bx,boatDist);
    
    return boatDist;
}


float sdHalfCylinder(vec3 pos , vec3 h){
    float cy=opS(
        sdCylinder(pos.yzx,h.xy),
        sdCylinder(pos.yzx,h.xy + vec2(0.03,-0.03))
    );
    float bx = sdBox(pos-vec3(0.,-h.x*h.z*2.,0.),vec3(h.x+0.3,h.x,h.y+0.3));
    return opS(bx,cy);
}
float sdPin( vec3 pos, vec3 h)
{
    
   pos.x += h.x*h.z*2. - h.x;
   float cy = sdCylinder(pos,h.xy);
   float bx = sdBox(pos-vec3(h.x*(1.+2.*h.z),0.,0.),vec3(h.x*2.,h.y+0.3,h.x*2.));
   return opS(bx,cy);
}

float sdQuant(vec3 pos){
    pos -= vec3(-0.,.3,0.);
    float barLen = 0.3;
    float quantLen = 0.2;
	float cy = sdCylinder(pos-vec3(0.,0.,0.),vec2(0.008,barLen));
    float bx = udRoundBox(pos-vec3(0.,barLen+quantLen,0.),vec3(0.025,quantLen,0.002),0.003);
    return min(cy,bx);
}

float sdBoat(vec3 pos){
    float ret = 10000.;
    //body
    float vb = sdBoatHull(pos);
    pos.y -=0.07;
    float vs = sdBoatHull(pos);
    float boatBody =  opS(vs,vb);
    
    ret = min(ret , boatBody);
    //quant
    vec3 quantPos = pos ;
    quantPos.x = abs(pos.x);
    quantPos = quantPos- vec3(0.05,.72,1.5);
    
    float degZ= PI*-0.15;
    quantPos = rZ(quantPos,sin(degZ),cos(degZ));
    float degX= PI*0.65;
    quantPos = rX(quantPos,sin(degX),cos(degX));
    float quant = sdQuant(quantPos*0.5);
    ret = min(ret , quant);
    
    //quant Ring
    
    vec3 ringPos = pos ;
    ringPos.x = abs(pos.x);
    ringPos = ringPos- vec3(0.44,.49,1.72);
    
    degZ= PI*-0.05;
    ringPos = rZ(ringPos,sin(degZ),cos(degZ));
    degZ= PI*-0.45;
    ringPos = rX(ringPos,sin(degZ),cos(degZ));
    float ringd = sdTorus(ringPos,vec2(0.05,0.005));
    ret = min(ret , ringd);
    //bar
    vec3 bpos = pos;
    bpos.z = abs(bpos.z);
    float bar1 = udRoundBox(bpos-vec3(0.,0.4,1.5),vec3(.46,0.01,0.04),0.01);
    ret = min(ret , bar1);
    
    // roof
   	vec3 q1=pos-vec3(0.,0.65,0.);
    vec3 roofSize = vec3(0.65,1.0,0.7);
    float roof = sdHalfCylinder(q1,roofSize);
    ret = min(ret , roof);
    
    //pos.x= abs(pos.x);//!! it is strange that i cann't use pos.x= abs(pos.x) to simplify the code
    vec3 padPos = pos;
    //padPos.x = abs(pos.x);
    float paddingT = sdPin( padPos - vec3(-0.65,0.42,0.),vec3(8,0.02,0.003));
    ret = min(ret , paddingT);
    padPos.x = -pos.x;
    paddingT = sdPin( padPos - vec3(-0.65,0.42,0.),vec3(8,0.02,0.003));
    ret = min(ret , paddingT);
    
    return ret;
}


#define BridgeL 40.
#define BridgeW 2.
#define BridgeH 4.

#define BridgeL1 50.


#define LegGap (BridgeW*0.45)
#define LegWid 0.1
#define LegLenGap LegGap * 4.

#define BoardLen  0.2
float BridgeSeg1(float mindist,in vec3 pos){
    float dis= 0.;
	
    // bridge leg
    vec3 legPos = pos;
    legPos.z -= LegLenGap * 0.5;
    if( legPos.z < BridgeL-LegLenGap && legPos.z >0.){
    	legPos.z = mod(legPos.z- LegLenGap * 0.5,LegLenGap) - LegLenGap * 0.5;
    }
    
   	dis = sdCylinder( vec3(abs(legPos.x),legPos.yz)-vec3(LegGap,0.,0.0), vec2(LegWid,BridgeH) );
    mindist = min( dis, mindist );
    
    dis = sdBox( legPos-vec3(0.,BridgeH*0.8,0.0), vec3(LegGap*0.95,LegWid*0.7,LegWid*0.7) );
    mindist = min( dis, mindist );
    
    // bridge face
    vec3 facePos = pos;
    dis = sdBox( facePos-vec3(0.,BridgeH,BridgeL*0.5 ), vec3(BridgeW,LegWid*0.7,BridgeL*0.5) );
    mindist = min( dis, mindist );
    
    vec3 boardPos = pos;
    if( boardPos.z < BridgeL && boardPos.z >0.){
    	boardPos.z = mod(boardPos.z,BoardLen)- BoardLen * 0.5;
    }
    dis = sdBox( boardPos-vec3(0.,BridgeH + .1,0.), vec3(BridgeW,0.1,BoardLen*0.3));
    //dis = sdSphere( boardPos-vec3(0.,BridgeH + 4.,BridgeL*0.5),vec4(0.,0.,0.,0.5));
    //dis = sdSphere( boardPos, vec4(0.,BridgeH,0.,0.5) );
    mindist = min( dis, mindist );
   
    return mindist;  
}

float BridgeSeg2(float mindist,in vec3 pos){
    float dis= 0.;
	pos.z -= BridgeL;
    pos.xz = pos.zx;
    pos.z = -pos.z;
    pos.z += BridgeW;
    
    return BridgeSeg1(mindist,pos);
}
float sdBridge( in vec3 pos )
{
    pos*=3.;
	float mindist = 10000000.0;
	
    float f = 0.5;//-0.5*cos(3.14159*pos.z/20.0);
	mindist = BridgeSeg1(mindist,pos);
  	mindist = BridgeSeg2(mindist,pos);
	return 0.25*mindist;
}



vec2 map( in vec3 pos )
{
    vec2 res = vec2( sdPlane(     pos), 1.0 );
    res = opU( res, vec2( sdBounceBalls( pos),1.) );
    res = opU( res, vec2( sdBridge( pos),1.) );
    pos -=vec3(-1,0.,-.5);
    
    float deg = 1.*3.14159/180.;
    pos = rY(pos,sin(deg),cos(deg));
    res = opU( res, vec2( sdBoat( pos),1.) );
    return res;
}

vec2 castRay( in vec3 ro, in vec3 rd )
{
    float tmin = 1.0;
    float tmax = 20.0;
   

    float t = tmin;
    float m = -1.0;
    for( int i=0; i<256; i++ )
    {
	    float precis = 0.0005*t;
	    vec2 res = map( ro+rd*t );
        if( res.x<precis || t>tmax ) break;
        t += res.x;
	    m = res.y;
    }

    if( t>tmax ) m=-1.0;
    return vec2( t, m );
}


float calcSoftshadow( in vec3 ro, in vec3 rd, in float mint, in float tmax )
{
	float res = 1.0;
    float t = mint;
    for( int i=0; i<40; i++ )
    {
		float h = map( ro + rd*t ).x;
        res = min( res, 8.0*h/t );
        t += clamp( h, 0.02, 0.10 );
        if( h<0.001 || t>tmax ) break;
    }
    return clamp( res, 0.0, 1.0 );
}

vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*map( pos + e.xyy ).x + 
					  e.yyx*map( pos + e.yyx ).x + 
					  e.yxy*map( pos + e.yxy ).x + 
					  e.xxx*map( pos + e.xxx ).x );
}

float calcAO( in vec3 pos, in vec3 nor )
{
	float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<5; i++ )
    {
        float hr = 0.01 + 0.12*float(i)/4.0;
        vec3 aopos =  nor * hr + pos;
        float dd = map( aopos ).x;
        occ += -(dd-hr)*sca;
        sca *= 0.95;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 );    
}

// http://iquilezles.org/www/articles/checkerfiltering/checkerfiltering.htm
float checkersGradBox( in vec2 p )
{
    // filter kernel
    vec2 w = fwidth(p) + 0.001;
    // analytical integral (box filter)
    vec2 i = 2.0*(abs(fract((p-0.5*w)*0.5)-0.5)-abs(fract((p+0.5*w)*0.5)-0.5))/w;
    // xor pattern
    return 0.5 - 0.5*i.x*i.y;                  
}

vec3 render( in vec3 ro, in vec3 rd )
{ 
    vec3 col = vec3(0.7, 0.9, 1.0) +rd.y*0.8;
    vec2 res = castRay(ro,rd);
    float t = res.x;
	float m = res.y;
    if( m>-0.5 )
    {
        vec3 pos = ro + t*rd;
        vec3 nor = calcNormal( pos );
        vec3 ref = reflect( rd, nor );
        
        // material        
		col = 0.45 + 0.35*sin( vec3(0.05,0.08,0.10)*(m-1.0) );
        if( m<1.5 )
        {
            
            float f = checkersGradBox( 5.0*pos.xz );
            col = 0.3 + f*vec3(0.1);
        }

        // lighitng        
        float occ = calcAO( pos, nor );
		vec3  lig = normalize( vec3(-0.4, 0.7, -0.6) );
        vec3  hal = normalize( lig-rd );
		float amb = clamp( 0.5+0.5*nor.y, 0.0, 1.0 );
        float dif = clamp( dot( nor, lig ), 0.0, 1.0 );
        float bac = clamp( dot( nor, normalize(vec3(-lig.x,0.0,-lig.z))), 0.0, 1.0 )*clamp( 1.0-pos.y,0.0,1.0);
        float dom = smoothstep( -0.1, 0.1, ref.y );
        float fre = pow( clamp(1.0+dot(nor,rd),0.0,1.0), 2.0 );
        
        dif *= calcSoftshadow( pos, lig, 0.02, 2.5 );
        dom *= calcSoftshadow( pos, ref, 0.02, 2.5 );

		float spe = pow( clamp( dot( nor, hal ), 0.0, 1.0 ),16.0)*
                    dif *
                    (0.04 + 0.96*pow( clamp(1.0+dot(hal,rd),0.0,1.0), 5.0 ));

		vec3 lin = vec3(0.0);
        lin += 1.30*dif*vec3(1.00,0.80,0.55);
        lin += 0.40*amb*vec3(0.40,0.60,1.00)*occ;
        lin += 0.50*dom*vec3(0.40,0.60,1.00)*occ;
        lin += 0.50*bac*vec3(0.25,0.25,0.25)*occ;
        lin += 0.25*fre*vec3(1.00,1.00,1.00)*occ;
		col = col*lin;
		col += 10.00*spe*vec3(1.00,0.90,0.70);

    	col = mix( col, vec3(0.8,0.9,1.0), 1.0-exp( -0.0002*t*t*t ) );
    }

	return vec3( clamp(col,0.0,1.0) );
}

mat3 setCamera( in vec3 ro, in vec3 ta, float cr )
{
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(sin(cr), cos(cr),0.0);
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

float remap(float a,float b ,float c, float d,float v){
	return ((v-a)/(b-a))*(d-c)+ c;
}
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 mo = iMouse.xy/iResolution.xy;
	float time = 15.0 + iTime;

    
    vec3 tot = vec3(0.0);
    vec2 p = (-iResolution.xy + 2.0*fragCoord)/iResolution.y;

    #define Deg2Rad (3.14159/180.)
    #define Rad2Deg (180./3.14159)
	
    float deg = 0.1*time + 6.0*mo.x;
    deg = remap(0.,360.,-230.,40.,mod(deg,6.28318) * Rad2Deg) * Deg2Rad;
    
    // -230 ~40
    //deg = Deg2Rad*40.;
    float radius = 4.8;
    // camera	
    vec3 ro = vec3( radius*cos(deg), 1. + 2.0*mo.y, radius*sin(deg) );
    vec3 ta = vec3( -0.5, .1, 0.5 );
    // camera-to-world transformation
    mat3 ca = setCamera( ro, ta, 0.0 );
    // ray direction
    vec3 rd = ca * normalize( vec3(p.xy,2.0) );

    // render	
    vec3 col = render( ro, rd );

    // gamma
    col = pow( col, vec3(0.4545) );

    tot += col;

    
    fragColor = vec4( tot, 1.0 );
}
