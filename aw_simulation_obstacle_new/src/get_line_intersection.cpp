int get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
    s10_x = p1_x - p0_x;
    s10_y = p1_y - p0_y;
    s32_x = p3_x - p2_x;
    s32_y = p3_y - p2_y;

    denom = s10_x * s32_y - s32_x * s10_y;
    if (denom == 0)//平行或共线
        return 0; // Collinear
    bool denomPositive = denom > 0;

    s02_x = p0_x - p2_x;
    s02_y = p0_y - p2_y;
    s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0) == denomPositive)//参数是大于等于0且小于等于1的，分子分母必须同号且分子小于等于分母
        return 0; // No collision

    t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0) == denomPositive)
        return 0; // No collision

    if (fabs(s_numer) > fabs(denom) || fabs(t_numer) > fabs(denom))
        return 0; // No collision
    // Collision detected
    t = t_numer / denom;
    if (i_x != NULL)
        *i_x = p0_x + (t * s10_x);
    if (i_y != NULL)
        *i_y = p0_y + (t * s10_y);

    return 1;
}


class line{
public:
	int xa;
	int ya;
	int xb;
	int yb;
	line(){}
	line(int xa, int ya, int xb, int yb){
		this->xa = xa;
		this->ya = ya;
		this->xb = xb;
		this->yb = yb;
	}
	int get_max_x(){
		return xa > xb ? xa : xb;
	}
	int get_min_x(){
		return xa > xb ? xb : xa;
	}
	int get_max_y(){
		return ya > yb ? ya : yb;
	}
	int get_min_y(){
		return ya > yb ? yb : ya;
	}
};

bool is_intersect(line myline1, line myline2){
	if(myline1.get_max_x() < myline2.get_min_x() || 
	   myline2.get_max_x() < myline1.get_min_x() ||
	   myline1.get_max_y() < myline2.get_min_y() || 
	   myline2.get_max_y() < myline1.get_min_y())   return false;
	int res1 = (myline1.xa - myline1.xb) * (myline2.ya - myline1.yb) - (myline1.ya - myline1.yb) * (myline2.xa - myline1.xb);
	int res2 = (myline1.xa - myline1.xb) * (myline2.yb - myline1.yb) - (myline1.ya - myline1.yb) * (myline2.xb - myline1.xb);
	
	int res3 = (myline2.xa - myline2.xb) * (myline1.ya - myline2.yb) - (myline2.ya - myline2.yb) * (myline1.xa - myline2.xb);
	int res4 = (myline2.xa - myline2.xb) * (myline1.yb - myline2.yb) - (myline2.ya - myline2.yb) * (myline1.xb - myline2.xb);
	if(res1 * res2 <= 0 && res3 * res4 <= 0) return true;
	else return false;
}